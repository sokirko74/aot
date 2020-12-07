#!/usr/local/bin/python
# -*- coding: utf-8 -*-

import sys
import cgi
import cgitb

if __name__ == '__main__':
    cgitb.enable(format="plain")
    print ("Content-Type: application/json; charset=utf8".encode('utf8'))
    print ("Access-Control-Allow-Origin: *".encode('utf8'))
    print ("".encode('utf8'))
    print ("aaaa".encode('utf8'))
