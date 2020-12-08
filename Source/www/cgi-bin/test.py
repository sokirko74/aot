#!/usr/local/bin/python2.7
# -*- coding: utf-8 -*-

# this script is for Zenon hosting that uses Freebsd and python 2.7

import cgitb

if __name__ == '__main__':
    cgitb.enable(format="plain")
    print ("Content-Type: application/json; charset=utf8".encode('utf8'))
    print ("Access-Control-Allow-Origin: *".encode('utf8'))
    print ("".encode('utf8'))
    print ("aaaa".encode('utf8'))
