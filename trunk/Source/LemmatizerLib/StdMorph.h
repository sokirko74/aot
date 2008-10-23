#ifndef StdMorph_h
#define StdMorph_h

#include <stdio.h>
#include "../common/util_classes.h"


#include "../GraphanLib/GraphmatFile.h"       // main symbols
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../MorphWizardLib/wizard.h"



#define HOMOWEIGHT_BIN_PATH			"homoweight.bin"		
#define WORDWEIGHT_BIN_PATH			"wordweight.bin"		
#define PREDICT_BIN_PATH			"npredict.bin"
#define MORPH_MAIN_FILES			"morph.bin"
#define OPTIONS_FILE				"morph.options"

const DWORD ErrorParadigmId			= 0xffffffff;


#endif
