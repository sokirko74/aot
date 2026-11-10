// Search in a ddc3 corpus from the browser (ddc3/doc/ddc_protocol.md, command run_query, format=json).
// nginx proxies only /ddc-daemon/run_query to the ddc server (GET, limit <= 99), see the nginx config of aot.ru.
// The page url keeps the query and the first hit: search_ddc.html?q=QUERY&start=N

const DdcUrl = '/ddc-daemon/run_query';
const HitsPerPage = 10;
const NoSpaceBefore = new Set(['.', ',', ';', ':', '!', '?', ')', '»']);

function appendText(parent, text) {
    parent.appendChild(document.createTextNode(text));
}

// tokens: strings (the context around the hit sentence) or [match_id, word, ...]
function appendTokens(parent, tokens) {
    let first = true, prevHit = false;
    for (const t of tokens) {
        const [isHit, word] = Array.isArray(t) ? [t[0], t[1]] : [0, t];
        if (!word) continue;
        if (!first && !NoSpaceBefore.has(word[0]))
            appendText(parent, prevHit && isHit ? ' ' : ' ');
        if (isHit) {
            const b = document.createElement('b');
            b.textContent = word;
            parent.appendChild(b);
        } else {
            appendText(parent, word);
        }
        first = false;
        prevHit = isHit;
    }
}

// bibl: "Author. Title" (a free Bibl field of flibusta.opt, needs OutputBibliographyOfHits 1)
function hitTitle(meta) {
    let s = meta.bibl || meta.orig_ || (meta.file_ || '').split('/').pop();
    const date = meta.date_ || '';
    if (date && !date.startsWith('0000'))
        s += ' (' + date.substring(0, 4) + ')';
    return s;
}

function pageUrl(query, start) {
    return 'search_ddc.html?' + new URLSearchParams({q: query, start: start});
}

function showMessage(holder, text) {
    holder.replaceChildren();
    const p = document.createElement('p');
    p.textContent = text;
    holder.appendChild(p);
}

function showResult(holder, query, start, answer) {
    holder.replaceChildren();
    const nhits = answer.nhits_.toLocaleString('ru-RU');
    const head = document.createElement('p');
    head.textContent = 'Предложения ' + (start + 1) + '-' + answer.end_ + ' из '
        + (answer.nhits_estimated_ ? 'примерно ' : '') + nhits;
    holder.appendChild(head);
    for (const hit of answer.hits_) {
        const h4 = document.createElement('h4');
        h4.textContent = hitTitle(hit.meta_ || {});
        holder.appendChild(h4);
        const p = document.createElement('p');
        hit.ctx_.forEach((part, i) => {
            if (i > 0) appendText(p, ' ');
            appendTokens(p, part);
        });
        holder.appendChild(p);
    }
    if (answer.end_ < answer.nhits_ && answer.hits_.length > 0) {
        const a = document.createElement('a');
        a.href = pageUrl(query, answer.end_);
        a.textContent = 'дальше';
        a.addEventListener('click', (e) => {
            e.preventDefault();
            history.pushState(null, '', a.href);
            search(query, answer.end_);
        });
        holder.appendChild(a);
    }
}

async function search(query, start) {
    const holder = document.getElementById('ddc_result');
    document.getElementById('search_text').value = query;
    if (!query) {
        holder.replaceChildren();
        return;
    }
    showMessage(holder, 'Ищем...');
    const params = new URLSearchParams({query: query, format: 'json', start: start, limit: HitsPerPage});
    try {
        const response = await fetch(DdcUrl + '?' + params);
        if (!response.ok)
            throw new Error('http status ' + response.status);
        const answer = await response.json();
        if (answer.nstatus_ || answer.istatus_ || answer.error_)
            throw new Error(answer.error_ || ('ddc status ' + answer.nstatus_ + '/' + answer.istatus_));
        showResult(holder, query, start, answer);
        window.scrollTo(0, 0);
    } catch (e) {
        showMessage(holder, 'Ошибка: ' + e.message);
    }
}

function searchFromUrl() {
    const params = new URLSearchParams(window.location.search);
    const start = Math.max(0, parseInt(params.get('start') || '0', 10) || 0);
    search((params.get('q') || '').trim(), start);
}

document.getElementById('search_form').addEventListener('submit', (e) => {
    e.preventDefault();
    const query = document.getElementById('search_text').value.trim();
    history.pushState(null, '', pageUrl(query, 0));
    search(query, 0);
});
window.addEventListener('popstate', searchFromUrl);
searchFromUrl();
