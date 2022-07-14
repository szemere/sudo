#include <config.h>

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# include "compat/stdbool.h"
#endif /* HAVE_STDBOOL_H */
#include "sudo_plugin.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MIN(a,b) (a<b?a:b)

static sudo_conv_t sudo_conv;
static sudo_printf_t sudo_log;

static struct http_iolog_state {
    CURL *curl;
    char *outbuf;
    size_t pos;
    size_t size;
} http_iolog_state;


static size_t
http_iolog_read_callback(char *dest, size_t size, size_t nmemb, void *userp)
{
    struct http_iolog_state *state = (struct http_iolog_state*)userp;
    size_t buffer_size = size*nmemb;

    if (state->pos < state->size)
    {
        size_t amount = MIN(state->size - state->pos, buffer_size);
        memcpy(dest, state->outbuf+state->pos, amount);
        state->pos+=amount;
        return amount;
    }

    free(state->outbuf);
    state->size = 0;
    return 0;
}

static int
http_iolog_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t sudo_printf, char * const settings[],
    char * const user_info[], char * const command_info[],
    int argc, char * const argv[], char * const user_env[],
    char * const plugin_options[])
{

    if (!sudo_conv)
	    sudo_conv = conversation;
    if (!sudo_log)
	    sudo_log = sudo_printf;

    http_iolog_state.curl = curl_easy_init();
    if (!http_iolog_state.curl)
    {
        return false;
    }
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_URL, "localhost:1234");
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_POST, 1L);
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_READFUNCTION, http_iolog_read_callback);
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_READDATA, &http_iolog_state);
    return true;
}

static void
http_iolog_close(int exit_status, int error)
{
    curl_easy_cleanup(http_iolog_state.curl);
    return;
}

static int
http_iolog_version(int verbose)
{
    sudo_log(SUDO_CONV_INFO_MSG, "HTTP I/O plugin version %s\n",
	PACKAGE_VERSION);
    return true;
}


static int
temporary_send(const char *buf, unsigned int len, const char **errstr)
{
    sudo_log(SUDO_CONV_ERROR_MSG, "Sending %d bytes.\n", len);
    http_iolog_state.outbuf = strndup(buf, len);
    if (!http_iolog_state.outbuf)
    {
        sudo_log(SUDO_CONV_ERROR_MSG, "Unable to allocate memory for I/O log.");
    };
    http_iolog_state.size = len;
    http_iolog_state.pos = 0;
    curl_easy_setopt(http_iolog_state.curl, CURLOPT_POSTFIELDSIZE, (long)len);
    CURLcode res = curl_easy_perform(http_iolog_state.curl);
    if (res != CURLE_OK)
    {
        sudo_log(SUDO_CONV_ERROR_MSG, "Unable to perform POST request. Reason: %s\n",
            curl_easy_strerror(res));
    }
    return 0;
}

static int
http_iolog_ttyin(const char *buf, unsigned int len, const char **errstr)
{
    return temporary_send(buf, len, errstr);
}

static int
http_iolog_ttyout(const char *buf, unsigned int len, const char **errstr)
{
    return temporary_send(buf, len, errstr);
}

static int
http_iolog_stdin(const char *buf, unsigned int len, const char **errstr)
{
    return temporary_send(buf, len, errstr);
}

static int
http_iolog_stdout(const char *buf, unsigned int len, const char **errstr)
{
    return temporary_send(buf, len, errstr);
}

static int
http_iolog_stderr(const char *buf, unsigned int len, const char **errstr)
{
    return temporary_send(buf, len, errstr);
}

static int
http_iolog_change_winsize(unsigned int line, unsigned int cols,
    const char **errstr)
{
    const char * message = "Screen size changed.";
    return temporary_send(message, strlen(message), errstr);
}

static int
http_iolog_log_suspend(int signo, const char **errstr)
{
    const char * message = "Log suspended";
    return temporary_send(message, strlen(message), errstr);
}

sudo_dso_public struct io_plugin http_iolog = {
    SUDO_IO_PLUGIN,
    SUDO_API_VERSION,
    http_iolog_open,
    http_iolog_close,
    http_iolog_version,
    http_iolog_ttyin,   /* tty input */
    http_iolog_ttyout,  /* tty output */
    http_iolog_stdin,   /* command stdin if not tty */
    http_iolog_stdout,  /* command stdout if not tty */
    http_iolog_stderr,  /* command stderr if not tty */
    NULL,               /* register_hooks */
    NULL,               /* deregister_hooks */
    http_iolog_change_winsize,
    http_iolog_log_suspend,
    NULL                /* event_alloc */
};
