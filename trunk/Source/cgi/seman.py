#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import re
import cgi
import cgitb
import cgi_utils

def print_result_graph(rus, graph,  TemplateFile):
    s = open (TemplateFile, "r").read()     
    graph = graph.replace ('\n',  '$$$')
    #cgi_utils.log("print_result_graph graph=%s" % graph)
    s = s.replace ('rus_text>', 'rus_text>' + rus.strip());
    s = s.replace ('$rus_graph', graph.strip())
    print s

if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 
    cgi_utils.init_rml("semgraph_cgi.log")

    try:
        FORM = cgi.FieldStorage()
        cgi_utils.log (str(FORM))
        po = u'общ'.encode ('cp1251')
        russian = FORM.getvalue('russian').strip()
        TemplateFile = FORM.getvalue('TemplateFile')
        #russian = "test"
        #TemplateFile = '../wwwroot/demo/graph.html'
        socket_conn = cgi_utils.GetDaemon("translate.cfg", "TransDaemon")
        cgi_utils.SendString(socket_conn, "graph#%s#%s" % (po, russian))
        graph_str = cgi_utils.RecieveString(socket_conn)
        print_result_graph(russian, graph_str, TemplateFile)
        cgi_utils.log ("Graph action was ok")
    except:
        cgi_utils.log ("Exception: %s" % sys.exc_info()[0])
        exit (1)

