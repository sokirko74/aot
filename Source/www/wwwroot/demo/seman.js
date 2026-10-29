import { SemanDaemonUrl } from './common.js';

// draws the semantic graph as SVG: {nodes: [{x, y, label, morph}], edges: [{source, target, label}]}

const SvgNs = 'http://www.w3.org/2000/svg';
const NodeHeight = 25;
const Margin = 20;

function svgElement(name, attrs, parent) {
    var e = document.createElementNS(SvgNs, name);
    for (var a in attrs) {
        e.setAttribute(a, attrs[a]);
    }
    if (parent) {
        parent.appendChild(e);
    }
    return e;
}

// the point of the rectangle border on the line from its center to (x, y)
function borderPoint(n, x, y) {
    var cx = n.x + n.width / 2, cy = n.y + NodeHeight / 2;
    var dx = x - cx, dy = y - cy;
    if (dx === 0 && dy === 0) {
        return {x: cx, y: cy};
    }
    var t = Math.min(dx !== 0 ? (n.width / 2) / Math.abs(dx) : Infinity, dy !== 0 ? (NodeHeight / 2) / Math.abs(dy) : Infinity);
    return {x: cx + dx * t, y: cy + dy * t};
}

function center(n) {
    return {x: n.x + n.width / 2, y: n.y + NodeHeight / 2};
}

function showPopup(text, pageX, pageY) {
    document.getElementById('popover-content').textContent = text;
    var popup = document.getElementById('popup');
    popup.style.display = 'block';
    popup.style.position = 'absolute';
    popup.style.left = (pageX - popup.offsetWidth / 2) + 'px';
    popup.style.top = (pageY - popup.offsetHeight - 20) + 'px';
}

document.addEventListener('click', function() {
    document.getElementById('popup').style.display = 'none';
});

function drawAll(inputJson, holder) {
    holder.textContent = '';
    var svg = svgElement('svg', {}, holder);
    var defs = svgElement('defs', {}, svg);
    var marker = svgElement('marker', {id: 'arrow', viewBox: '0 0 10 10', refX: '10', refY: '5',
        markerWidth: '10', markerHeight: '10', orient: 'auto-start-reverse'}, defs);
    svgElement('path', {d: 'M 10 0 L 0 5 L 10 10 z', fill: '#4b4a67', transform: 'rotate(180 5 5)'}, marker);

    var nodes = inputJson.nodes.map(function(n) {
        return {x: n.x, y: n.y, width: n.label.length > 10 ? n.label.length * 10 : 100, label: n.label, morph: n.morph};
    });
    var edgesGroup = svgElement('g', {}, svg);
    var nodesGroup = svgElement('g', {}, svg);

    var edges = inputJson.edges.map(function(e) {
        var line = svgElement('line', {stroke: '#4b4a67', 'stroke-width': 1, 'marker-end': 'url(#arrow)'}, edgesGroup);
        var labelBox = svgElement('rect', {fill: 'white'}, edgesGroup);
        var label = svgElement('text', {'text-anchor': 'middle', 'dominant-baseline': 'middle', 'font-size': 14}, edgesGroup);
        label.textContent = e.label;
        return {source: nodes[e.source], target: nodes[e.target], line: line, label: label, labelBox: labelBox};
    });

    function updateEdge(e) {
        var from = borderPoint(e.source, center(e.target).x, center(e.target).y);
        var to = borderPoint(e.target, center(e.source).x, center(e.source).y);
        e.line.setAttribute('x1', from.x);
        e.line.setAttribute('y1', from.y);
        e.line.setAttribute('x2', to.x);
        e.line.setAttribute('y2', to.y);
        e.label.setAttribute('x', (from.x + to.x) / 2);
        e.label.setAttribute('y', (from.y + to.y) / 2);
        var b = e.label.getBBox();
        e.labelBox.setAttribute('x', b.x - 2);
        e.labelBox.setAttribute('y', b.y - 1);
        e.labelBox.setAttribute('width', b.width + 4);
        e.labelBox.setAttribute('height', b.height + 2);
    }

    // the size of the picture by its content (fitToContent in JointJS)
    function fitToContent() {
        var maxX = 0, maxY = 0;
        nodes.forEach(function(n) {
            maxX = Math.max(maxX, n.x + n.width);
            maxY = Math.max(maxY, n.y + NodeHeight);
        });
        svg.setAttribute('width', maxX + Margin);
        svg.setAttribute('height', maxY + Margin);
    }

    nodes.forEach(function(n) {
        n.group = svgElement('g', {cursor: 'move'}, nodesGroup);
        svgElement('rect', {width: n.width, height: NodeHeight, fill: 'blue'}, n.group);
        var text = svgElement('text', {x: n.width / 2, y: NodeHeight / 2, fill: 'white', 'text-anchor': 'middle',
            'dominant-baseline': 'middle', 'font-size': 14}, n.group);
        text.textContent = n.label;
        n.move = function() {
            n.group.setAttribute('transform', 'translate(' + n.x + ',' + n.y + ')');
        };
        n.move();

        // nodes can be dragged by the mouse
        n.group.addEventListener('pointerdown', function(evt) {
            var startX = evt.clientX, startY = evt.clientY, nodeX = n.x, nodeY = n.y;
            n.group.setPointerCapture(evt.pointerId);
            function onMove(e) {
                n.x = Math.max(0, nodeX + e.clientX - startX);
                n.y = Math.max(0, nodeY + e.clientY - startY);
                n.move();
                edges.forEach(function(edge) {
                    if (edge.source === n || edge.target === n) {
                        updateEdge(edge);
                    }
                });
                fitToContent();
            }
            function onUp() {
                n.group.removeEventListener('pointermove', onMove);
                n.group.removeEventListener('pointerup', onUp);
            }
            n.group.addEventListener('pointermove', onMove);
            n.group.addEventListener('pointerup', onUp);
        });
        n.group.addEventListener('dblclick', function(evt) {
            evt.stopPropagation();
            showPopup(n.morph, evt.pageX, evt.pageY);
        });
    });
    fitToContent();
    edges.forEach(updateEdge);
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
            drawAll(resJson, document.getElementById('graphHolder'));
        })
        .catch(
            alert
        );
}

window.graph_request = graph_request
window.drawAll = drawAll
