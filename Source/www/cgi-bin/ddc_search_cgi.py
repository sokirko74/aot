#!/usr/bin/python3
"""CGI: search in a ddc3 corpus (HTTP protocol, see ddc3/doc/ddc_proto.md, command run_query).

Parameters (GET or POST): search_text (a ddc query), start_hit_no (the first hit, from 0).
"""

import html
import json
import logging
import os
import sys
import urllib.request
from urllib.parse import parse_qs, urlencode

RML = os.path.join(os.path.dirname(__file__), "../../..")

logging.basicConfig(filename=os.path.join(RML, 'logs', "ddc_search.log"),
                    level=logging.INFO, format='%(asctime)s %(message)s')

DDC_SERVER_URL = "http://127.0.0.1:17000/run_query"
DDC_TIMEOUT = 30
HITS_PER_PAGE = 10
TEMPLATE_FILE = os.path.join(RML, "Source/www/wwwroot/search_ddc.html")
if not os.path.exists(TEMPLATE_FILE):
    TEMPLATE_FILE = os.path.join(RML, "Source/www/search_ddc.html")
SELF_WEB_LINK = "/cgi-bin/" + os.path.basename(__file__)
PUNCTUATION_WITHOUT_SPACES_BEFORE = {'.', ',', ';', ':', '!', '?', ')', '»'}


def get_form():
    """The parameters of the url and of a POST body (application/x-www-form-urlencoded)."""
    qs = os.environ.get('QUERY_STRING', '')
    if os.environ.get('REQUEST_METHOD') == 'POST':
        length = int(os.environ.get('CONTENT_LENGTH') or 0)
        qs += '&' + sys.stdin.buffer.read(length).decode('utf-8', errors='replace')
    return {k: v[0] for k, v in parse_qs(qs).items()}


def run_query(query, start, limit):
    data = urlencode({'query': query, 'format': 'json', 'start': start, 'limit': limit}).encode('utf-8')
    with urllib.request.urlopen(DDC_SERVER_URL, data, timeout=DDC_TIMEOUT) as r:
        answer = json.loads(r.read().decode('utf-8'))
    if answer.get('nstatus_') or answer.get('istatus_') or answer.get('error_'):
        raise RuntimeError(answer.get('error_') or "ddc status {}/{}".format(
            answer.get('nstatus_'), answer.get('istatus_')))
    return answer


def tokens_to_html(tokens):
    """tokens: strings (the context around the hit sentence) or [match_id, word, ...]."""
    s = ""
    prev_hit = False
    for t in tokens:
        is_hit, word = (t[0], t[1]) if isinstance(t, list) else (0, t)
        if not word:
            continue
        if word[0] not in PUNCTUATION_WITHOUT_SPACES_BEFORE and s:
            s += '&nbsp;' if prev_hit and is_hit else ' '
        s += "<b>" + html.escape(word) + "</b>" if is_hit else html.escape(word)
        prev_hit = is_hit
    return s


def hit_title(meta):
    """bibl: "Author. Title" (a free Bibl field of flibusta.opt, needs OutputBibliographyOfHits 1)."""
    s = meta.get('bibl') or meta.get('orig_') or os.path.basename(meta.get('file_', ''))
    date = meta.get('date_') or ''
    if date and not date.startswith('0000'):
        s += " ({})".format(date[:4])
    return html.escape(s)


def result_html(query, answer, start):
    s = "<br/><br/> Предложения {}-{} из {}".format(start + 1, answer['end_'], answer['nhits_'])
    for hit in answer['hits_']:
        s += "<h4>" + hit_title(hit.get('meta_', {})) + "</h4>\n"
        s += " ".join(tokens_to_html(part) for part in hit['ctx_']) + "\n"
    if answer['end_'] < answer['nhits_']:
        url = SELF_WEB_LINK + "?" + urlencode({'search_text': query, 'start_hit_no': answer['end_']})
        s += '<br/><br/><a href="{}"> дальше </a>\n'.format(html.escape(url))
    return s


def print_page(query, result):
    with open(TEMPLATE_FILE, encoding="utf-8") as inp:
        page = inp.read()
    page = page.replace("<!--result_text-->", result)
    page = page.replace('name="search_text">',
                        'name="search_text" value="{}">'.format(html.escape(query)))
    sys.stdout.write(page)


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    print("Content-Type: text/html; charset=utf-8\n", flush=True)
    form = get_form()
    query = form.get('search_text', 'мама').strip()
    try:
        start = max(0, int(form.get('start_hit_no', 0)))
    except ValueError:
        start = 0
    logging.info("query={}, start_hit_no={}".format(query, start))
    try:
        result = result_html(query, run_query(query, start, HITS_PER_PAGE), start)
    except Exception as exp:
        logging.error("query={}: {}".format(query, exp))
        result = "<br/><br/>Ошибка: " + html.escape(str(exp))
    print_page(query, result)


if __name__ == '__main__':
    main()
