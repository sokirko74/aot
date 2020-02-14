#!/usr/bin/python3.7
# -*- coding: utf-8 -*-

##!/Users/sokirko/AppData/Local/Programs/Python/Python37-32/python3


import urllib
import urllib.parse
import urllib.request
import sys
import cgi
import cgitb
from urllib.parse import urlencode, quote_plus


if __name__ == '__main__':
    cgitb.enable(format="plain")
    sys.stdout.reconfigure(encoding='utf-8')
    print ("Content-Type: application/json; charset=utf8")
    print ("Access-Control-Allow-Origin: *")
    print ("")

    fields = dict( (key, cgi.FieldStorage().getvalue(key)) for key in cgi.FieldStorage())
    port = fields['port']
    url = 'http://localhost:' + port + '/?' + urlencode(fields)
    contents = urllib.request.urlopen(url).read()
    contents = contents.decode('utf8')
    print(contents)
