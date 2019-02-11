#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import re
import cgi
import cgitb
import cgi_utils

def print_synan_result(InputText, syn_str, LanguaStr, FileName):
    s = open (FileName, "r").read()     
    syn_str = syn_str.replace ("\n", "$$$")
    s = s.replace ('selected', '')
    s = s.replace( 'value="%s"' % LanguaStr, 'selected value="%s"' % LanguaStr)
    s = s.replace ("InputText>", "InputText>" + InputText.strip())
    s = s.replace ("$syn_struct", syn_str)
    print s

if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 
    cgi_utils.init_rml("synan_cgi.log")

    try:
        FORM = cgi.FieldStorage()
        cgi_utils.log (str(FORM))
        InputText = FORM.getvalue('InputText').strip()
        Langua = FORM.getvalue('Language')
        TemplateFile = FORM.getvalue('TemplateFile')
        socket_conn = cgi_utils.GetDaemon("synandmn.cfg", "SynanDaemon")
        cgi_utils.SendString(socket_conn, "%s#%s" % (Langua, InputText))
        syn_str = cgi_utils.RecieveString(socket_conn)
        print_synan_result(InputText, syn_str, Langua, TemplateFile)
        cgi_utils.log ("SynAnAction was ok")
    except:
        cgi_utils.log ("Exception: %s" % sys.exc_info()[0])
        exit (1)


