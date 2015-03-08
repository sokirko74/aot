#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import re
import cgi
import cgitb
import cgi_utils


def print_template_file_morph(FileName, Query, Result, bWithParadigms, Langua):
    s = open (FileName, "r").read()
    s = s.replace ('CHECKED', '')
    s = s.replace ("value=\"\"", " value=\"%s\"" % Query)
    if bWithParadigms == 1:
        s = s.replace("ID=WithParadigms", 'ID=WithParadigms CHECKED')
    s = s.replace ("value=%i" % Langua, "value=%i CHECKED" % Langua)
    s = s.replace ("</form>", Result + "</form>")
    print s

def GetLemmas(form, socket_conn):
    cgi_utils.log ("GetLemmas\n");
    WithParadigmsStr = form.getvalue("WithParadigms", "")
    if WithParadigmsStr  == "on":
        WithParadigms = 1
    else:
        WithParadigms = 0
    Langua = int(form.getvalue ("langua"))
    WordForm = form.getvalue ("SearchText", "")
    #WithParadigms = 1
    #Langua = 1
    #WordForm = u"мама".encode('windows-1251')

    S = "get_paradigm %s\x01%i %i" % (WordForm, WithParadigms, Langua)
    cgi_utils.SendString (socket_conn, S)
    Result = cgi_utils.RecieveString(socket_conn) 
    Result = Result.replace ("\n", "<br>").replace ("\t", "&nbsp&nbsp")
    #print "aaaa", Result

    TemplateFile = form.getvalue("TemplateFile")
    if not TemplateFile:
        print_error("Cannot read template file from html")
        return False
    print_template_file_morph (TemplateFile, WordForm, Result,  WithParadigms,  Langua)
    cgi_utils.log ("written  results")
    return True


if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 
    cgi_utils.init_rml("morph.log")

    try:
        FORM = cgi.FieldStorage()
        cgi_utils.log (str(FORM))
        CorpusName = FORM.getvalue('CorpusName')
        #CorpusName = "SynanDaemon"    
        socket_conn = cgi_utils.GetDaemon("synandmn.cfg", CorpusName)
        GetLemmas(FORM, socket_conn)
    except:
        cgi_utils.log ("Network error: %s" % sys.exc_info()[0])
        exit (1)


