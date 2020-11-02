import os, sys
sys.path.append(".")

import aot

if __name__ == '__main__':
    print (aot.load_morphology(1))

#    print (aot.lemmatize_json("мама", True))
    print (aot.synthesize("мама", "N fem,sg,gen"))
