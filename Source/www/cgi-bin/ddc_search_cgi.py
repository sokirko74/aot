#!/usr/bin/python3

import cgi
import socket
import cgitb
import json
import os
from urllib.parse import urlencode

DDC_SERVER_ADDRESS_HOST = "195.70.213.239"
DDC_SERVER_ADDRESS_PORT = 80
DDC_CORPUS_NAME = "server"
DDC_TEXT_ENCODING = 'windows-1251'
HITS_PER_PAGE = 10
TEMPLATE_FILE = os.path.join(os.path.dirname(__file__), "../wwwroot/search_ddc.html")
if not os.path.exists(TEMPLATE_FILE):
    TEMPLATE_FILE = os.path.join(os.path.dirname(__file__), "../www/search_ddc.html")
SELF_WEB_LINK = "/cgi-bin/" + os.path.basename(__file__)
BYTEORDER = 'little'


def print_error(err):
    print (err + "\n")


def send_query (socket_conn, query, start_hit_no, result_limit):
    s = "run_query {}\x01{}\x01json\x01{} {} {}".format(DDC_CORPUS_NAME, query,
                                                        start_hit_no, result_limit, 100)
    encoded_s = s.encode(DDC_TEXT_ENCODING)                                                    
    len_s = len(encoded_s)
    socket_conn.sendall(len_s.to_bytes(4, BYTEORDER))
    socket_conn.sendall(encoded_s)


def decode_bytes(s):
    if DDC_TEXT_ENCODING.lower() == "utf8":
        return s.decode('utf8')
    result = ""
    i = 0
    s = s.decode('latin')
    while i < len(s):
        if s[i:].startswith('\\u00'):
            one_char = int(s[i+4:i+6], 16)
            result += one_char.to_bytes(1,'little').decode(DDC_TEXT_ENCODING)
            i += 6
        else:
            result += s[i]
            i += 1
    return result


def recieve_hits (socket_conn):
    len_bytes = socket_conn.recv(4)
    packet_length = int.from_bytes(len_bytes, BYTEORDER)
    res = b""
    while len(res) < packet_length:
        res += socket_conn.recv(4096)
    try:
        encoded_res = decode_bytes(res)
        answer = json.loads(encoded_res)
        if answer['nstatus_'] != 0:
            print_error("network error id={}".format(answer['nstatus_']))
            return
        if answer['istatus_'] != 0:
            print_error("internal_error={}".format(answer['istatus_']))
            return
        return answer
    except Exception as exp:
        print_error("server answer is in a bad format, exception= {}".format(exp))
        return


PUNCTUATION_WITHOUT_SPACES_BEFORE = {'.', ',', ';', ':', '!', '?'}


def print_ddc_result(template_file_name, query, answer, start_hit_no):
    with open (template_file_name, "r") as inp:
        template_str = inp.read()

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
        url_encoded_query = urlencode({'search_text': query, 'start_hit_no': answer['end_']})
        hits_html += u'<br/><br/><a href={}?{}> more </a>\n'.format(SELF_WEB_LINK, url_encoded_query)

    template_str = template_str.replace ("<!--result_text-->", hits_html)
    query = query.replace('"', '&quot;')
    template_str = template_str.replace ('name="search_text">', u'name="search_text" value="{}">'.format(query))
    print (template_str)


if __name__ == '__main__':
    cgitb.enable()
    print ("Content-Type: text/html")
    print ("")

    try:
        form = cgi.FieldStorage()
        query = form.getvalue('search_text').strip()
        start_hit_no = form.getvalue('start_hit_no', 0)

        #query = u"мама"
        #start_hit_no = 0

        socket_conn = socket.create_connection((DDC_SERVER_ADDRESS_HOST, DDC_SERVER_ADDRESS_PORT), 100)
        send_query(socket_conn, query, start_hit_no, HITS_PER_PAGE)
        answer = recieve_hits(socket_conn)
        if answer is not None:
            print_ddc_result(TEMPLATE_FILE, query, answer, start_hit_no)
    except Exception as exp:
        print(exp)
