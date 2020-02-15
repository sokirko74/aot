///var host = "www.aot.ru";
//var host = "localhost";
export var SynanDaemonUrl = 'http://185.237.97.32:8080?dummy=1';
export var SemanDaemonUrl = 'http://185.237.97.32:8081?dummy=1';


export const TableBorderStyle  = '1px solid black';

export function AddCell (tr, text) {
    var td = tr.insertCell()
    td.appendChild(document.createTextNode(text));
    td.style.border = TableBorderStyle;
    td.cellSpacing = "0";
    td.cellPadding = "10";
}

export function CreateTable() {
    var tbl  = document.createElement('table');
    tbl.style.width  = '500px';
    tbl.style.border = TableBorderStyle;
    tbl.style.borderCollapse = "collapse";
    return tbl;
}
                                               
