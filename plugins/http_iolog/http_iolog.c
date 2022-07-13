#include <config.h>

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# include "compat/stdbool.h"
#endif /* HAVE_STDBOOL_H */
#include "sudo_plugin.h"


static int
http_iolog_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t sudo_printf, char * const settings[],
    char * const user_info[], char * const command_info[],
    int argc, char * const argv[], char * const user_env[], char * const args[])
{
    return true;
}

static void
http_iolog_close(int exit_status, int error)
{
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
http_iolog_ttyin(const char *buf, unsigned int len)
{
    return 0;
}

static int
http_iolog_ttyout(const char *buf, unsigned int len)
{
    return 0;
}

static int
http_iolog_stdin(const char *buf, unsigned int len)
{
    return 0;
}

static int
http_iolog_stdout(const char *buf, unsigned int len)
{
    return 0;
}

static int
http_iolog_stderr(const char *buf, unsigned int len)
{
    return 0;
}


sudo_dso_public struct io_plugin sample_io = {
    SUDO_IO_PLUGIN,
    SUDO_API_VERSION,
    http_iolog_open,
    http_iolog_close,
    http_iolog_version,
    http_iolog_ttyin,   /* tty input */
    http_iolog_ttyout,  /* tty output */
    http_iolog_stdin,   /* command stdin if not tty */
    http_iolog_stdout,  /* command stdout if not tty */
    http_iolog_stderr   /* command stderr if not tty */
};
