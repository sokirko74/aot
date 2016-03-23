#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import re
import cgi
import cgitb
import cgi_utils

def print_result_graph(rus, eng,  po, TemplateFile):
    s = open (TemplateFile, "r").read()     
    #cgi_utils.log("print_result_graph graph=%s" % graph)
    s = s.replace ('selected', '');
    s = s.replace ('rus_text>', 'rus_text>' + rus.strip());
    s = s.replace ('eng_text>', 'eng_text>' + eng.strip());
    s = s.replace ('VALUE="%s"' % po, 'VALUE="%s" selected' % po);
    print s

if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 
    cgi_utils.init_rml("translate_cgi.log")

    try:
        FORM = cgi.FieldStorage()
        cgi_utils.log (str(FORM))
        default_po = u'общ'.encode ('cp1251')
        po = FORM.getvalue('PO', default_po).strip()
        russian = FORM.getvalue('russian').strip()
        TemplateFile = FORM.getvalue('TemplateFile')
        #russian = "test"
        #TemplateFile = '../wwwroot/demo/graph.html'
        socket_conn = cgi_utils.GetDaemon("translate.cfg", "TransDaemon")
        cgi_utils.SendString(socket_conn, "trans#%s#%s" % (po, russian))
        english = cgi_utils.RecieveString(socket_conn)
        print_result_graph(russian, english, po, TemplateFile)
        cgi_utils.log ("Translate action was ok")
    except:
        cgi_utils.log ("Exception: %s" % sys.exc_info()[0])
        exit (1)

