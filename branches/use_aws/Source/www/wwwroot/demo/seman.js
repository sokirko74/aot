import { SemanDaemonUrl } from './common.js';

//test

var TheInputJson = null;
var TheJointGraph = new joint.dia.Graph;
var TheJointPaper = new joint.dia.Paper({
        el: $('#graphHolder'),
        width: 600,
        height: 300,
        model: TheJointGraph,
        gridSize: 1
});

$(document).click(function() {
        $('#popup').css('display','none');
});

$(document).ready(function(){
        TheJointPaper.on('cell:pointerdblclick',
            function(cellView, evt, x, y) {
                var nodeNum = cellView.id.split('_')[1] - 1; //получаем номер элемента, по которому кликнули
                if (nodeNum >= TheInputJson.nodes.length)                    //если это не нод(у них первые номера) - выходим
                    return;
                $('#popover-content').text(TheInputJson.nodes[nodeNum].morph); //вот такая вот связность, да
                var $popup = $('#popup');
                var targetX = event.pageX - ($popup.width() / 2);
                var targetY = event.pageY - $popup.height() - 20;
                $popup.css('left',targetX);
                $popup.css('top',targetY);
                $popup.css('display','block');
                $popup.css("position", "absolute");
            }
        );
});

function drawAll(inputJson, graph) {

    var graphCells = [];


    for (var i in inputJson.nodes) {
        var n = inputJson.nodes[i];
        var width = n.label.length > 10 ? n.label.length * 10 : 100;

        graphCells.push(new joint.shapes.basic.Rect({
            position: { x: n.x, y: n.y },
            size: { width: width, height: 25 },
            attrs: { rect: { fill: 'blue' }, text: { text: n.label, fill: 'white' } },
        }));
    }

    for (var i in inputJson.edges) {
        var e = inputJson.edges[i];
        graphCells.push(new joint.dia.Link({
            source: {id: graphCells[e.source].id},
            target: {id: graphCells[e.target].id},
            attrs: {'.marker-target': { fill: '#4b4a67', stroke: '#4b4a67', d: 'M 10 0 L 0 5 L 10 10 z' }},
            labels: [
                { position: 0.5, attrs: { text: { text: e.label } } }
            ],
        }));
    }
    graph.addCells(graphCells);
}



function graph_request() {
    var query = document.getElementById("InputText").value.trim();
    var url = SemanDaemonUrl + "&action=graph&langua=Russian";
    url +=  "&query=" + query + "&topic=общ";

    fetch(url)
        .then(function(response) {
            return response.json();
        })
        .then(function(resJson) {
            console.log (JSON.stringify(resJson, null));
            TheInputJson =  resJson;
            TheJointGraph.clear();
            drawAll (resJson, TheJointGraph);
            TheJointPaper.fitToContent();
        })
        .catch( 
            alert 
        );
}

window.graph_request = graph_request
