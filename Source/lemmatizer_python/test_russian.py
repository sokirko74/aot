import os
import sys
import json

sys.path.append(".")

import aot

if __name__ == '__main__':

    print (aot.load_morphology(1, False))

    answer = json.loads(aot.lemmatize_json("мама", False))
    assert len(answer) > 0
    assert answer[0]['found'] == True

    answer = json.loads(aot.synthesize("мама", "N fem,sg,gen"))
    assert len(answer['forms']) > 0
    assert answer['forms'][0] == "МАМЫ"

    answer = json.loads(aot.synthesize("бай-тал", "N sg,gen"))
    assert len(answer['forms']) == 0 # no prediction in load_morphology
