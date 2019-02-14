import { SemanDaemonUrl } from './common.js';

function translate_request() {
    var query = document.getElementById("InputText").value.trim();
    var url = SemanDaemonUrl + "&action=translate&langua=Russian";
    var topic = document.getElementById("TextTopic").value;
    url +=  "&query=" + query + "&topic=" + topic;

    fetch(url)
        .then(function(response) {
            return response.json();
        })
        .then(function(resJson) {
            console.log (JSON.stringify(resJson, null));
            document.getElementById("OutputText").value  = resJson["translation"];
        })
        .catch( 
            alert 
        );
}

window.translate_request = translate_request
