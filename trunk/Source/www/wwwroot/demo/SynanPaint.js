var FONT_COLOR = 'black',
    BG_COLOR = 'white',
    GROUP_ARC_COLOR = 'blue',
    NONGROUP_ARC_COLOR = 'red',
    FONT = 'Arial',
    ARC_FONT = 'Arial',
    FONT_SIZE = 20,
    ARC_FONT_SIZE = 14,
    SPACE_SIZE = 10,
    LEFT_SPACE = 2,
    TOP_SPACE = 30,
    ARC_HEIGHT = 40;

var mainCanvas = document.getElementById("synanCanvas"),
    longCanvas = document.createElement('canvas'),
    ctx = longCanvas.getContext("2d"),
    ctxMain = mainCanvas.getContext("2d");

calcMaxArcHeight = function() {
    var height = 0;
    for (var ClauseNo in topClauses) {
        var arcs = topClauses[ClauseNo].getCurArcs();
        arcs = arcs.sort(function(a,b){return b.childArcs.length - a.childArcs.length});
        for(var i in arcs) {
            var curHeight = arcs[i].getHeight();
            if(curHeight > height)
                height = curHeight;
        }
    }
    return height;
};

calcWordsLength = function() {
    var length = LEFT_SPACE;
    for (var i in topClauses) {
        var Clause = topClauses[i];
        for (var j in Clause.WordPanels) {
            if (Clause.WordPanels[j].homonyms.length > 1)  ctx.font = 'bold ' + FONT_SIZE + 'px ' + FONT;
            else                                            ctx.font = FONT_SIZE + 'px ' + FONT;
            var prevLineNo = Math.floor(length / ctxMain.canvas.width);
            var lineNo = Math.floor((ctx.measureText(Clause.WordPanels[j].word).width + length) / ctxMain.canvas.width);
            if (lineNo > prevLineNo) {
                length = ctxMain.canvas.width * lineNo + LEFT_SPACE;
            }
            length += ctx.measureText(Clause.WordPanels[j].word).width + SPACE_SIZE;
        }
    }
    return length;
};

wrapAll = function() {
    var linesNo = Math.ceil(ctx.canvas.width / ctxMain.canvas.width);
    ctxMain.canvas.height = linesNo * ctx.canvas.height;
    ctxMain.clearRect(0,0,ctxMain.canvas.width,ctxMain.canvas.height);
    for (var i = 0; i < linesNo; i++) {
        ctxMain.drawImage(ctx.canvas, ctxMain.canvas.width*i, 0, ctxMain.canvas.width, ctx.canvas.height, 0, ctx.canvas.height * i, ctxMain.canvas.width, ctx.canvas.height);
    }
};

removePopups = function() {
    var wrapper = document.getElementById('canvasWrapper');
    var popDiv = wrapper.getElementsByClassName("synanWordPanel");
    while (popDiv.length > 0){                                      //Именно так, иначе не работает
        for (var i = 0; i < popDiv.length; i++)
            popDiv[i].parentNode.removeChild(popDiv[i]);
        popDiv = wrapper.getElementsByClassName("synanWordPanel");
    }
};

addPopups = function() {
    for (var i in topClauses)
        topClauses[i].addPopups();
    window.onclick = function(event) {
        if (!event.target.matches('.panelDroppable')) {
            var dropdowns = document.getElementsByClassName("synanDropdownContent");
            for (var i = 0; i < dropdowns.length; i++)
                dropdowns[i].style.display = 'none';
        }
    }
};

function drawAll() {
    removePopups();
    var height = calcMaxArcHeight() + TOP_SPACE;
    var width = calcWordsLength();
    ctx.canvas.height = height + FONT_SIZE*1.6;
    ctx.canvas.width = width;
    ctx.clearRect(0,0,ctx.canvas.width,ctx.canvas.height);
    cursor = LEFT_SPACE;
    for (var i in topClauses) {
        topClauses[i].drawWordPanels(height);
        topClauses[i].drawArcs();
        topClauses[i].drawSubjPredic();
    }
    wrapAll();
    addPopups();
}

drawAll();