import { SynanDaemonUrl, AddCell, CreateTable } from './common.js';

function ParadigmToTable(paradigms){
    var tbl  = CreateTable();

    var header = tbl.insertRow();
    AddCell(header, "Found");
    AddCell(header, "Common");
    AddCell(header, "Lemma");
    AddCell(header, "Grammems");

    for (var i in paradigms) {
        var tr = tbl.insertRow();
        AddCell(tr, paradigms[i].found ? "+" : "-");
        AddCell(tr, paradigms[i].commonGrammems);
        AddCell(tr, paradigms[i].wordForm);
        AddCell(tr, paradigms[i].morphInfo);
    }
    var holder = document.getElementById("morphholder")
    holder.innerHTML = '';
    holder.appendChild(tbl);

    for (var i in paradigms) {
        if (paradigms[i].paradigm) {
            var ptbl  = CreateTable();
            for (var m in paradigms[i].paradigm) {
                var tr = ptbl.insertRow();
                var gr1  = paradigms[i].paradigm[m];
                AddCell(tr, gr1.pos);

                for (var k in gr1.formsGroups) {
                    var gr2 = gr1.formsGroups[k];
                    tr = ptbl.insertRow();
                    AddCell(tr, gr2.grm);
                    for (var j in gr2.forms) {
                        tr = ptbl.insertRow();
                        //AddCell(tr, "");
                        AddCell(tr, "\u00A0\u00A0\u00A0\u00A0\u00A0" + gr2.forms[j].f);
                        AddCell(tr, gr2.forms[j].grm);
                    }
                }
            }
            holder.appendChild(document.createElement("br"));
            holder.appendChild(ptbl);
        }
    }
}

function morph_request() {
    var langua = document.querySelector('input[name="langua"]:checked').value;
    var query = document.getElementById("SearchText").value;
    var withParadigms = document.getElementById("WithParadigms").checked;

    var url = SynanDaemonUrl + "&action=morph&langua=" + langua + "&query=" + query;
    if (withParadigms) {
        url += "&withparadigms=1";
    }
    //fetch(url'http://127.0.0.1:17017?action=morph&langua=Russian&query=%D0%BC%D0%B0%D0%BC%D0%B0')
    fetch(url)
        .then(function(response) {
            //alert(response.headers.get('Content-Type')); // application/json; charset=utf-8
            //alert(response.status); // 200
            return response.json();
        })
        .then(function(paradigms) {
            console.log (JSON.stringify(paradigms, null));
            ParadigmToTable(paradigms);
        })
        .catch( alert );
}

window.morph_request = morph_request
