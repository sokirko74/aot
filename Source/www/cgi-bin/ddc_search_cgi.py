#!/usr/local/bin/python2.7
# -*- coding: utf-8 -*-

# this script is for Zenon hosting that uses Freebsd and python 2.7

import cgi
import socket
import cgitb
import json
import os
import urllib

LOG_FILE_PATH = "cgi_python.log"
DDC_SERVER_ADDRESS_HOST = "195.70.213.239"
DDC_SERVER_ADDRESS_PORT = 80
DDC_CORPUS_NAME = "server"
DDC_TEXT_ENCODING = 'windows-1251'
HITS_PER_PAGE = 10
TEMPLATE_FILE = os.path.join(os.path.dirname(__file__), "../wwwroot/search_ddc.html")
SELF_WEB_LINK = "/cgi-bin/" + os.path.basename(__file__)

def print_error(err):
    print err + "\n"

def int_to_lsb(l):
    return  chr(l & 0xff) + \
            chr((l >> 8) & 0xff) + \
            chr((l >> 16) & 0xff) + \
            chr((l >> 24) & 0xff)


def lsb_to_int(buf):
    return  ord(buf[0]) | (ord(buf[1]) << 8) | (ord(buf[2]) << 16) | (ord(buf[3]) << 24)


def send_query (socket_conn, query, start_hit_no, result_limit):
    s = "run_query {}\x01{}\x01json\x01{} {} {}".format(DDC_CORPUS_NAME, query.encode(DDC_TEXT_ENCODING),
                                                        start_hit_no, result_limit, 100)
    len_int32 = int_to_lsb(len(s))
    socket_conn.sendall(len_int32)
    socket_conn.sendall(s)


def decode_string(s):
    if DDC_TEXT_ENCODING.lower() == "utf8":
        return s
    result = ""
    i = 0
    while i < len(s):
        if s[i:].startswith('\u00'):
            one_char = int(s[i+4:i+6], 16)
            result += chr(one_char).decode(DDC_TEXT_ENCODING)
            i += 6
        else:
            result += s[i]
            i += 1
    return result


def recieve_hits (socket_conn):
    packet_length = lsb_to_int(socket_conn.recv(4))
    res = ""
    while len(res) < packet_length:
        res += socket_conn.recv(4096)
    try:
        answer = json.loads(decode_string(res))
        if answer['nstatus_'] != 0:
            print_error("network error id={}".format(answer['nstatus_']))
            return
        if answer['istatus_'] != 0:
            print_error("internal_error={}".format(answer['istatus_']))
            return
        return answer
    except Exception as exp:
        print_error("server answer is in a bad format")
        return


PUNCTUATION_WITHOUT_SPACES_BEFORE = {'.', ',', ';', ':', '!', '?'}


def print_ddc_result(template_file_name, query, answer, start_hit_no):
    s = open (template_file_name, "r").read().decode('utf8')
    hits_html  = u"<br/><br/> Sentences: {}-{} out of {}".format(
        start_hit_no, answer['end_']-1, answer['nhits_'])
    for serp_item in answer['hits_']:
        hits_html += "<h4>" + serp_item.get('meta_', {}).get('orig_', "") + "</h4>\n"
        for sentence in serp_item['ctx_']:
            prev_hit = False
            for is_hit, token in sentence:
                if len(token) == 0:
                    continue
                if is_hit == 1:
                    token = "<b>" + token + "</b>"
                if len(token) > 0 and token[0] not in PUNCTUATION_WITHOUT_SPACES_BEFORE:
                    if prev_hit:
                        hits_html += '&nbsp;'
                    else:
                        hits_html += ' '
                hits_html += token
                prev_hit = is_hit
            hits_html += "\n"

    if answer['end_'] < answer['nhits_']:
        url_encoded_query = urllib.urlencode({'search_text': query.encode('utf8'), 'start_hit_no': answer['end_']})
        hits_html += u'<br/><br/><a href={}?{}> more </a>\n'.format(SELF_WEB_LINK, url_encoded_query)

    s = s.replace ("<!--result_text-->", hits_html)
    query = query.replace('"', '&quot;')
    s = s.replace ('name="search_text">', u'name="search_text" value="{}">'.format(query))
    print (s.encode('utf8'))


if __name__ == '__main__':
    cgitb.enable()
    print "Content-Type: text/html" 
    print 

    form = cgi.FieldStorage()
    query = form.getvalue('search_text').strip().decode('utf8')
    start_hit_no = form.getvalue('start_hit_no', 0)

    #query = u"мама"
    #start_hit_no = 0

    socket_conn = socket.create_connection((DDC_SERVER_ADDRESS_HOST, DDC_SERVER_ADDRESS_PORT), 100)
    send_query(socket_conn, query, start_hit_no, HITS_PER_PAGE)
    answer = recieve_hits(socket_conn)
    if answer is not None:
        print_ddc_result(TEMPLATE_FILE, query, answer, start_hit_no)

