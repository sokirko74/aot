var graph = new joint.dia.Graph;

var paper = new joint.dia.Paper({
    el: $('#myholder'),
    width: 600,
    height: 300,
    model: graph,
    gridSize: 1
});

var graphCells = [];

for (var n in Nodes) {
    graphCells.push(new joint.shapes.basic.Rect({
        position: { x: Nodes[n].x, y: Nodes[n].y },
        size: { width: Nodes[n].width, height: 25 },
        attrs: { rect: { fill: 'blue' }, text: { text: Nodes[n].label, fill: 'white' } },
    }));
}

for (var e in Edges) {
    graphCells.push(new joint.dia.Link({
        source: {id: graphCells[Edges[e].sourceNode].id},
        target: {id: graphCells[Edges[e].targetNode].id},
        attrs: {'.marker-target': { fill: '#4b4a67', stroke: '#4b4a67', d: 'M 10 0 L 0 5 L 10 10 z' }},
        labels: [
            { position: 0.5, attrs: { text: { text: Edges[e].label } } }
        ],
        //connector: {name: 'smooth'}
    }));
}

graph.addCells(graphCells);

$(document).click(function() {
    $('#popup').css('display','none');
});

$(document).ready(function(){
    paper.on('cell:pointerdblclick',
        function(cellView, evt, x, y) {
            var nodeNum = cellView.id.split('_')[1] - 1; //получаем номер элемента, по которому кликнули
            if (nodeNum >= Nodes.length)                    //если это не нод(у них первые номера) - выходим
                return;
            $('#popover-content').text(Nodes[nodeNum].morphology); //вот такая вот связность, да
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

