#!/usr/local/bin/python
import os
import sys
import re
import cgi
import socket
import cgitb
import cgi_utils

LOG_FILE_PATH = "cgi_python.log"
DDC_SERVER_ADDRESS_HOST = "195.70.213.239"
DDC_SERVER_ADDRESS_PORT = 80
DDC_CORPUS_NAME = "server"

def print_error(err):
    print ("%s\n".format(err))

def log(t):
    try:
        with open((LOG_FILE_PATH, "a") as outp:
            outp.write(t + "\n")
    except IOError as e:
        print ("Cannot write to log-file({}) I/O error({}): {}".format(LOG_FILE_PATH, e.errno, e.strerror))
    except Exception as e:
        print_error("Cannot write to log-file({}) , error message = {} ({})".format(LOG_FILE_PATH,t,str(e)))


def  send_query (socket_conn, query, start_hit_no, result_limit):
    s = "run_query {}\x01{}\x01html\x01{} {} {}".format(DDC_CORPUS_NAME, query, start_hit_no, result_limit,100)
    socket_conn.sendall("%020i" % len(s))
    socket_conn.sendall(s)


def  recieve_hits (socket_conn):
    packet_length = int(socket_conn.recv(20))
    res = ""
    while len(res) < packet_length:
        res += socket_conn.recv(4096)
    z = res.find('\x01')
    if z == -1:
        print_error("server answer is in a bad format")
        return
    hits = res[z+1:]
    internal_error, network_error,  end_hit_no, hits_count, docs_count = res[:z].split()
    if network_error != 0:
        print_error("network error id={}".format(network_error))
        return
    if internal_error != 0:
        print_error("internal_error={}".format(internal_error))
        return
    return end_hit_no, hits_count, docs_count, hits


HTML = """

"""

def print_synan_result(InputText, syn_str, LanguaStr, FileName):
    s = open (FileName, "r").read()     
    syn_str = syn_str.replace ("\n", "$$$")
    s = s.replace ('selected', '')
    s = s.replace( 'value="%s"' % LanguaStr, 'selected value="%s"' % LanguaStr)
    s = s.replace ("InputText>", "InputText>" + InputText.strip())
    s = s.replace ("$syn_struct", syn_str)
    print s
#ftp://vh53129:jpL8C4N4Mz@home.sokirko.host.ru


if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 

    try:
        FORM = cgi.FieldStorage()
        #log (str(FORM))
        #query = FORM.getvalue('InputText').strip()
        #TemplateFile = FORM.getvalue('TemplateFile')
        query = "test"
        #TemplateFile = '../wwwroot/demo/synt.html'
        socket_conn = socket.create_connection((DDC_SERVER_ADDRESS_HOST, DDC_SERVER_ADDRESS_PORT), 100)
        send_query(socket_conn, query)
        answer = recieve_hits(socket_conn)
        if answer is not None:
            end_hit_no, hits_count, docs_count, hits = answer
            print_ddc_result(end_hit_no, hits_count, docs_count, hits)
        cgi_utils.log ("SynAnAction was ok")
    except:
        cgi_utils.log ("Exception: %s" % sys.exc_info()[0])
        exit (1)


