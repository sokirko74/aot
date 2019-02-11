import { SynanDaemonUrl } from './common.js';

var TopClauses = [];
var cursor;

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

class CMorphVariant {
    constructor(synUnits, arcs, subjArcs) {
        this.synUnits = synUnits;
        this.arcs = arcs;
        this.subjArcs = subjArcs;
        this.compareTo = function(var2){
            for (var i = 0; i < var2.synUnits.length; i++) {
                if (this.synUnits[i].homonymNo < var2.synUnits[i].homonymNo)
                    return -1;
                if (this.synUnits[i].homonymNo > var2.synUnits[i].homonymNo)
                    return 1;
            }
            return 0;
        };

        this.equals = function(Var) {
            return (this.compareTo(Var) == 0);
        };
    };
}

class WordArc {
    constructor(group) {
        this.childArcs = [];
        this.firstWord = 0;
        this.lastWord = 0;
        this.height = 0;
        this.groupArc = true;
        this.isSubj = false;
        this.parentGroupLeg = {};
        this.firstWord = group.start;
        this.lastWord = group.last;
        this.strName = group.descr;
        this.groupArc = group.isGroup;
        this.isSubj = group.isSubj;
        this.getHeight = function() {
            if(this.height == 0)
                this.height = this.calculateHeight();
            return this.height;
        };

        this.calculateHeight = function() {
            var height = 0;
            for(var i in this.childArcs) {
                var ii = this.childArcs[i].calculateHeight();
                if(ii > height)
                    height = ii;
            }
            return height + ARC_HEIGHT;
        };

        this.drawOneLineArc = function(Clause, leftPoint, rightPoint) {
            var WordPannelLeft = Clause.WordPanels[this.firstWord];
            ctx.beginPath();
            ctx.moveTo(leftPoint.x, leftPoint.y);
            ctx.lineTo(leftPoint.x, WordPannelLeft.y - this.height);
            ctx.lineTo(rightPoint.x, WordPannelLeft.y - this.height);
            ctx.lineTo(rightPoint.x, rightPoint.y);
            ctx.stroke();
            this.parentGroupLeg.x = leftPoint.x + (rightPoint.x - leftPoint.x)/2;
            this.parentGroupLeg.y = WordPannelLeft.y - this.height;
        };

        this.draw = function(Clause) {
            for (var i in this.childArcs)
                this.childArcs[i].draw(Clause);

            this.height = this.getHeight();
            var leftPoint = this.getLeftLegPoint(Clause);
            var rightPoint = this.getRightLegPoint(Clause);

            if(this.groupArc)
                ctx.strokeStyle = GROUP_ARC_COLOR;
            else
                ctx.strokeStyle = NONGROUP_ARC_COLOR;

            this.drawOneLineArc(Clause, leftPoint, rightPoint);

            ctx.font = ARC_FONT_SIZE + 'px ' + ARC_FONT;
            ctx.fillText(this.strName, this.parentGroupLeg.x - ctx.measureText(this.strName).width / 2, this.parentGroupLeg.y - ARC_FONT_SIZE*0.3);
        };

        this.getLeftLegPoint = function(Clause) {
            var WordPannelLeft = Clause.WordPanels[this.firstWord];

            var leftPoint = {};
            var bSet = false;
            if(this.childArcs.length > 0) {
                var wordArcLeft = this.childArcs[0];
                //Магическое какое-то условие
                if ( (wordArcLeft.firstWord == this.firstWord) &&
                    ((wordArcLeft.groupArc && this.groupArc) ||
                    (!wordArcLeft.groupArc && !this.groupArc)||
                    (!wordArcLeft.groupArc && this.groupArc)))
                {
                    leftPoint = wordArcLeft.parentGroupLeg;
                    bSet = true;
                }
            }

            if (!bSet) {
                if(this.groupArc) {
                    leftPoint.x = WordPannelLeft.x + WordPannelLeft.width/2;
                    leftPoint.y = WordPannelLeft.y;
                } else {
                    leftPoint.x = WordPannelLeft.x;
                    leftPoint.y = WordPannelLeft.y;
                }
            }
            return leftPoint;
        };

        this.getRightLegPoint = function(Clause) {
            var rightPoint = {};

            var WordPannelRight = Clause.WordPanels[this.lastWord];

            var bSet = false;
            if(this.childArcs.length >= 1) {
                var wordArcRight = this.childArcs[this.childArcs.length - 1];
                if ( (wordArcRight.lastWord == this.lastWord) &&
                    ((wordArcRight.groupArc && this.groupArc) ||
                    (!wordArcRight.groupArc && !this.groupArc)||
                    ( !wordArcRight.groupArc && this.groupArc)))
                {
                    rightPoint = wordArcRight.parentGroupLeg;
                    bSet = true;
                }
            }

            if(!bSet) {
                if(this.groupArc) {
                    rightPoint.x = WordPannelRight.x + WordPannelRight.width/2;
                    rightPoint.y = WordPannelRight.y;
                } else {
                    rightPoint.x = WordPannelRight.x + WordPannelRight.width;
                    rightPoint.y = WordPannelRight.y;
                }
            }
            return rightPoint;
        }
    };
};

class CSynUnit {
    constructor (str) {
        if (str != 'emtpy'){
            this.homonymNo = str.homNo;
            this.strGram = str.grm;
        }
    };
}

class Homonym {
    constructor(str) {
        this.lemma = str;
        this.strCurrentGram = '';
    };
};

class WordPanel {
    constructor(word) {
        this.x = 0;
        this.y = 0;
        this.width = 0;
        this.activeHomonym = 0;
        this.word = word.str;
        this.homonyms = [];
        for (var i in word.homonyms) {
            this.homonyms.push(new Homonym(word.homonyms[i]));
        }
    };
}  

class TopClause {
    constructor(Info) {
        this.currentMorphVariant = 0;
        this.WordPanels = [];
        this.MorphVariants = [];
        this.parseWords(Info.words);
        this.parseVariants(Info.variants);

        this.getCurArcs = function() {
            var CurrVar = this.getActiveHomonymNumbers();
            for(var i = 0; i < this.MorphVariants.length; i++) {
                var Var = this.MorphVariants[i];
                if( Var.equals(CurrVar) ) {
                    this.currentMorphVariant = i;
                    this.setActiveHomonyms(i);
                    return Var.arcs;
                }
            }
            this.currentMorphVariant = -1;
            return [];
        };

        this.getActiveHomonymNumbers = function() {
            var WordsCount = this.WordPanels.length;
            var arr = [];
            for (var i = 0; i < WordsCount; i++) {
                var Panel = this.WordPanels[i];
                var U = new CSynUnit('empty');
                U.homonymNo = Panel.activeHomonym;
                arr.push(U);
            }
            return new CMorphVariant(arr, [], []);
        };

        this.drawWordPanels = function(height) {
            ctx.fillStyle = FONT_COLOR;
            for (var i in this.WordPanels) {
                var panel = this.WordPanels[i];
                if (panel.homonyms.length > 1)  ctx.font = 'bold ' + FONT_SIZE + 'px ' + FONT;
                else                            ctx.font = FONT_SIZE + 'px ' + FONT;
                panel.width = ctx.measureText(panel.word).width;
                var prevLineNo = Math.floor(cursor / ctxMain.canvas.width);
                var lineNo = Math.floor((panel.width + cursor) / ctxMain.canvas.width);
                if (lineNo > prevLineNo) {
                    cursor = ctxMain.canvas.width * lineNo + LEFT_SPACE;
                }
                panel.x = cursor;
                panel.y = height;
                panel.outerX = cursor - ctxMain.canvas.width * lineNo;
                panel.outerY = height + ctx.canvas.height * lineNo;
                ctx.fillText(panel.word, panel.x, panel.y + FONT_SIZE);
                cursor += panel.width + SPACE_SIZE;
            }
        };

        this.drawSubjPredic = function () {
            if (this.currentMorphVariant < 0) return;
            var homs = this.MorphVariants[this.currentMorphVariant];
            ctx.strokeStyle = FONT_COLOR;
            for(var i = 0; i < homs.subjArcs.length; i++) {
                var arc = homs.subjArcs[i];
                var panel1 = this.WordPanels[arc.firstWord];
                var panel2 = this.WordPanels[arc.lastWord];
                this.drawSubj(panel1);
                this.drawPredic(panel2);
            }
        };

        this.drawSubj = function(panel) {
            ctx.beginPath();
            ctx.moveTo(panel.x, panel.y + FONT_SIZE*1.2);
            ctx.lineTo(panel.x + panel.width, panel.y + FONT_SIZE*1.2);
            ctx.stroke();
        };

        this.drawPredic = function(panel) {
            ctx.beginPath();
            ctx.moveTo(panel.x, panel.y + FONT_SIZE*1.2);
            ctx.lineTo(panel.x + panel.width, panel.y + FONT_SIZE*1.2);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(panel.x, panel.y + FONT_SIZE*1.4);
            ctx.lineTo(panel.x + panel.width, panel.y + FONT_SIZE*1.4);
            ctx.stroke();
        };

        this.drawArcs = function() {
            var arcs = this.getCurArcs();
            for (var i in arcs) {
                arcs[i].draw(this);
            }
        };

        this.addPopups = function() {
            for (var i in this.WordPanels) {
                var panel = this.WordPanels[i];
                var popDiv = document.createElement("div");
                popDiv.style.position = 'absolute';
                popDiv.style.top = panel.outerY;
                popDiv.style.left = panel.outerX;
                popDiv.style.height = FONT_SIZE*1.2 + 'px';
                popDiv.style.width = panel.width + 'px';
                popDiv.title = panel.homonyms[panel.activeHomonym].lemma + ' ' + panel.homonyms[panel.activeHomonym].strCurrentGram;
                popDiv.className = 'synanWordPanel';
                if (panel.homonyms.length > 1) {
                    var menuDiv = document.createElement("div");
                    menuDiv.className = 'synanDropdownContent';
                    for (var j in panel.homonyms) {
                        var menuEl = document.createElement("a");
                        menuEl.innerHTML = panel.homonyms[j].lemma;
                        menuEl.setAttribute('ActionCommand', j);
                        menuEl.setAttribute('wordPanelNo', i);
                        var Clause = this;
                        menuEl.addEventListener("click", function(){
                            var homNum = this.getAttribute('ActionCommand');
                            var panelNo = this.getAttribute('wordPanelNo');
                            Clause.WordPanels[panelNo].activeHomonym = homNum;
                            drawAll();
                        });
                        menuDiv.appendChild(menuEl);
                    }
                    popDiv.appendChild(menuDiv);
                    popDiv.style.cursor = 'pointer';
                    popDiv.addEventListener("click", function(){
                        this.getElementsByClassName('synanDropdownContent')[0].style.display = 'block';
                    });
                    popDiv.className += ' panelDroppable';
                }
                var canWrap = document.getElementById('canvasWrapper');
                canWrap.appendChild(popDiv);
            }
        };
    };
}

var protoClause = TopClause.prototype;

protoClause.parseWords = function(words) {
    for (var i in words) {
        this.WordPanels.push(new WordPanel(words[i]));
    }
};

protoClause.parseVariants = function(variants) {
    for (var i in variants) {
        this.parseOneVariant(variants[i]);
    }
    if( this.MorphVariants.length > 0 )
        this.setActiveHomonyms(0);
};

protoClause.setActiveHomonyms = function(VarNo) {
    var hom = this.MorphVariants[VarNo];
    for(var i = 0 ; i < hom.synUnits.length ; i++ )
    {
        var panel = this.WordPanels[i];
        panel.activeHomonym = hom.synUnits[i].homonymNo;
        panel.homonyms[panel.activeHomonym].strCurrentGram = hom.synUnits[i].strGram;
    }
};

protoClause.parseOneVariant = function(variant) {
    var homs = this.readUnits(variant.units),
        arcs = [],
        subjArcs = [];
    for (var i in variant.groups) {
        var arc = new WordArc(variant.groups[i]);
        if (arc.isSubj) {
            subjArcs.push(arc);
        } else {
            arcs.push(arc);
        }
    }
    arcs = this.orderArcs(arcs);
    this.MorphVariants.push(new CMorphVariant(homs, arcs, subjArcs));
};

protoClause.readUnits = function(str) {
    var wordsCount = this.WordPanels.length,
        ii = 0,
        arr = [];
    while ((ii < wordsCount) && (str[ii])) {
        arr.push(new CSynUnit(str[ii]));
        ii++;
    }
    return arr;
};

protoClause.orderArcsRec = function(arcs, parentArc, iCur) {
    for(var i = iCur; i < arcs.length;) {
        var arc = arcs[i];
        if(+arc.firstWord > +parentArc.lastWord)
            return i;
        i = this.orderArcsRec(arcs, arc, i + 1);
        parentArc.childArcs.push(arc);
    }
    return arcs.length;
};

protoClause.orderArcs = function(arcs) {
    var ordered = [];
    for(var i = 0; i < arcs.length;) {
        var arc = arcs[i];
        i = this.orderArcsRec(arcs, arc, i + 1);
        ordered.push(arc);
    }
    return ordered;
};

function parseSynanJson(synanJson) {
    TopClauses = []
    for (var s in synanJson) {
        for (var c in synanJson[s]) {
            TopClauses.push(new TopClause(synanJson[s][c]));
        }
    }
}


function calcMaxArcHeight() {
    var height = 0;
    for (var ClauseNo in TopClauses) {
        var arcs = TopClauses[ClauseNo].getCurArcs();
        arcs = arcs.sort(function(a,b){return b.childArcs.length - a.childArcs.length});
        for(var i in arcs) {
            var curHeight = arcs[i].getHeight();
            if(curHeight > height)
                height = curHeight;
        }
    }
    return height;
};

function calcWordsLength() {
    var length = LEFT_SPACE;
    for (var i in TopClauses) {
        var Clause = TopClauses[i];
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

function wrapAll() {
    var linesNo = Math.ceil(ctx.canvas.width / ctxMain.canvas.width);
    ctxMain.canvas.height = linesNo * ctx.canvas.height;
    ctxMain.clearRect(0,0,ctxMain.canvas.width,ctxMain.canvas.height);
    for (var i = 0; i < linesNo; i++) {
        ctxMain.drawImage(ctx.canvas, ctxMain.canvas.width*i, 0, ctxMain.canvas.width, ctx.canvas.height, 0, ctx.canvas.height * i, ctxMain.canvas.width, ctx.canvas.height);
    }
};

function removePopups() {
    var wrapper = document.getElementById('canvasWrapper');
    var popDiv = wrapper.getElementsByClassName("synanWordPanel");
    while (popDiv.length > 0){                                      //Именно так, иначе не работает
        for (var i = 0; i < popDiv.length; i++)
            popDiv[i].parentNode.removeChild(popDiv[i]);
        popDiv = wrapper.getElementsByClassName("synanWordPanel");
    }
};

function addPopups() {
    for (var i in TopClauses)
        TopClauses[i].addPopups();
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
    for (var i in TopClauses) {
        TopClauses[i].drawWordPanels(height);
        TopClauses[i].drawArcs();
        TopClauses[i].drawSubjPredic();
    }
    wrapAll();
    addPopups();
}



function syntax_request() {
    var langua = document.getElementById("Language").value;
    var query = document.getElementById("InputText").value;

    var url = SynanDaemonUrl + "?action=syntax&langua=" + langua;
    url +=  "&query=" + query;

    fetch(url)
        .then(function(response) {
            return response.json();
        })
        .then(function(synanJson) {
            console.log (JSON.stringify(synanJson, null));
            parseSynanJson(synanJson);
            drawAll();
        })
        .catch( alert );
}

window.syntax_request = syntax_request
