#!/usr/bin/env python3

import time
from pprint import pformat
from flask import Flask, request
app = Flask(__name__)

@app.route('/', methods = ['POST'])
def hello_world():
   print(pformat(request.form))
   time.sleep(10)
   return ''

if __name__ == '__main__':
   app.run(port=1234)
