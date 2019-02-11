#!/usr/bin/python
# -*- coding: utf-8 -*-
# python synan_cli.py --input "мама мыла раму"
# #words$мама@homonyms@МАМА$мыла@homonyms@МЫТЬ$раму@homonyms@РАМА#groups$0=С жр,им,ед,; ^0=Г дст,прш,жр,ед,; ^0=С жр,вн,ед,; @0^2^гл_личн^cl@1^2^прям_доп^gr@0^1^sp^sp#topclause
import os
import sys
import re
import cgi
import cgitb
import cgi_utils
import argparse

def parse_args():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument('--input', default="test",  help='input text', required=False)
    parser.add_argument('--language', default="Russian",  help='language', required=False)
    return parser.parse_args() 


if __name__ == '__main__':
    cgi_utils.init_rml("synan_cgi.log")

    try:
        args = parse_args()
        InputText = args.input.decode('utf8').encode('windows-1251')
        Langua = "_" + args.language
        socket_conn = cgi_utils.GetDaemon("synandmn.cfg", "SynanDaemon")
        print >> sys.stderr, "use socket {}".format(socket_conn)

        cgi_utils.SendString(socket_conn, "%s#%s" % (Langua, InputText))
        syn_str = cgi_utils.RecieveString(socket_conn)
        print  syn_str.decode('windows-1251').encode('utf-8')
    except:
        print >> sys.stderr, "Exception: %s" % sys.exc_info()[0]
        exit (1)


