#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import re
import cgi
import cgitb
import socket
import datetime


def print_error(err):
    print "%s\n" % err

REGISTRY={}
LOG_FILE_PATH = None

def init_rml(log_file_name):
    global  LOG_FILE_PATH,  REGISTRY
    rml_ini_file = os.path.join( os.environ['RML'], "Bin", "rml.ini" )
    for x in open (rml_ini_file, "r"):
        a = x.strip().split(' ')
        REGISTRY[a[0]] = ' '.join(a[1:])        
    
    log_path  = get_registry_string ("Software\\Dialing\\Logs\\Main" );
    if not log_path:
        print_error("Cannot get log file name from Registry");
        return
    
    LOG_FILE_PATH = os.path.join( log_path, log_file_name )
    log ("enter cgiMain %s" % str(datetime.datetime.now().time()));


def get_registry_string(registry_key):
    v  = REGISTRY.get( registry_key );
    if not v:
        return v
    else:
        return v.replace ('$RML', os.environ['RML'])

    
def log(t):
    try:
        print >>open(LOG_FILE_PATH, "a"), t
    except IOError as e:
        print "Cannot write to log-file({0}) I/O error({1}): {2}".format(LOG_FILE_PATH, e.errno, e.strerror)    
    except:
        print_error("Cannot write to log-file(%s) , error message = %s (%s)" % (LOG_FILE_PATH,t,sys.exc_info()[0]));


def GetDaemon (cfg_file_name, ServerName):
    CfgFile = os.path.join(os.environ['RML'], "Bin", cfg_file_name)
    for x in open (CfgFile, "r"):
        x = re.sub('//.*', '', x).strip()
        if x:
            (name, host, port, path)  = x.strip().split()
            if name == ServerName:
                log ("try to  create connection to %s:%s" % (host, port)) 
                return socket.create_connection ( (host, int(port)), 100) 

    log ("Cannot find server \"%s\" in %s" % (ServerName, CfgFile))
    return None


def  SendString (socket_conn, s):
    socket_conn.sendall("%020i" % len(s))
    socket_conn.sendall(s)

def  RecieveString (socket_conn):
    PacketLength = int(socket_conn.recv(20))
    res = ""
    while len(res) < PacketLength:
        res += socket_conn.recv(4096)
    return res
        

if not os.environ.get('RML'):
    print_error("Set environment variable RML")
    exit (1)
