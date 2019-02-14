#!/Users/sokirko/AppData/Local/Programs/Python/Python37-32/python3
# -*- coding: utf-8 -*-
##!/usr/bin/python3

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
    #port = "17018"
    
    #contents = urllib.request.urlopen('http://localhost:' + port + '/?' + urlencode(fields)).read()
    url = 'http://localhost:' + port + '/?' + urlencode(fields)
    contents = urllib.request.urlopen(url).read()
    #contents = urllib.request.urlopen(r'http://localhost:17018?action=translate&langua=Russian&query=test&topic=test').read();
    contents = contents.decode('utf8')
    #sys.stdout.write(contents) 
    print(contents)

    #dump = open ("dump.txt", "w")
    #print (url, file=dump)
    #print (contents, file=dump)
    #dump.close()