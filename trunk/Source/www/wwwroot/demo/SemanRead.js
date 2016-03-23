//var graphString = "nodes#(#x 310#y 65#label Одиннадцать#Morphology Морфология одиннадцать#)#(#x 50#y 150#label Санса#)#(#x 200#y 150#label Арья#)#(#x 350#y 150#label Робб#)#(#x 500#y 150#label Бранн#)#edges#(#source_id 0#target_id 1#label Дочь#)#(#source_id 1#target_id 2#label Дочь#)#(#source_id 2#target_id 3#label Сын#)#(#source_id 3#target_id 4#label Сын#)#(#source_id 4#target_id 0#label Лошадь#)#";
//var graphString = 'nodes#(#x 170#y 60#label МАМА#Morphology МАМА =   С од,жр,им,ед,$$$SynWordNo = 0$$$WordWeight = 1480#)#edges#'
console.log(graphString); 
for (var i = 0;  i < graphString.length-3; i++)
if (graphString.substring(i, i+3) == "$$$")
{
    graphString = graphString.substring(0,i) + "\n" + graphString.substring(i+3);
}

function Node(x, y, width, label, morphology) {
    this.x = x;
    this.y = y;
    this.width = width;
    //this.height = height;
    //this.fixed = fixed;
    this.label = label;
    this.morphology = morphology;
}

function Edge(sourceNode, targetNode, label) {
    this.sourceNode = sourceNode;
    this.targetNode = targetNode;
    this.label = label;
}

var Nodes = [];
var Edges = [];

var IsZoneNodes  = true;
var InUnit  = false;
var Curr_x = 0;
var Curr_y = 0;
var Curr_source = 0;
var Curr_target = 0;
var m_Width = 0;
var m_Height = 0;
var  Curr_label = "";
var  Curr_Morphology = "";

var graphTokens = graphString.split('#');

for (var t in graphTokens) {

    var str = graphTokens[t];

    if (str.charAt(0) == 'x') {
        Curr_x = parseInt(str.substring(1));
        if (Curr_x > m_Width){
            m_Width = Curr_x;
        }
        continue;
    }

    if (str.charAt(0) == 'y') {
        Curr_y = parseInt(str.substring(1));
        if (Curr_y > m_Height){
            m_Height = Curr_y;
        }
        continue;
    }

    if (str.length > 10)
        if (str.substring(0, 10) == "Morphology") {
            Curr_Morphology = str.substring(11);
            continue;
        }

    if (str.length > 5)
        if (str.substring(0, 5) == "label") {
            Curr_label = str.substring(6);
            continue;
        }

    if (str.length > 9) {
        var s = str.substring(10);
        if (str.substring(0, 9) == "source_id") {
            Curr_source = parseInt(s);
            continue;
        }
        if (str.substring(0, 9) == "target_id") {
            Curr_target = parseInt(s);
            continue;
        }
    }

    if (str == "(") {
        InUnit = true;
        continue;
    }

    if (str == "edges") {
        IsZoneNodes = false;
        continue;
    }

    if (InUnit)
        var Curr_width = Curr_label.length > 10 ? Curr_label.length * 10 : 100;
        if (str == ")") {
            InUnit = false;
            if (IsZoneNodes)
                Nodes.push(new Node(Curr_x, Curr_y, Curr_width, Curr_label, Curr_Morphology));
            else
                Edges.push(new Edge(Curr_source, Curr_target, Curr_label));
            Curr_Morphology = "";
        }
}