#pragma once

#include "morph_dict/common/utilit.h"

enum DictTypeEnum { 
	Ross = 0, 
	LocRoss = 1, 
	CollocRoss = 2, 
	TimeRoss = 3, 
	OborRoss = 4, 
	Aoss = 5, 
	EngCollocRoss = 6, 
	EngObor = 7, 
	FinRoss = 8, 
	CompRoss = 9, 
	OmniRoss = 10, 
	NoneRoss = 11 
};

extern bool IsThesRoss(DictTypeEnum dict_type);
extern int GetThesIdByRossId(DictTypeEnum dict_type);
extern DictTypeEnum GetRossIdByThesId(int ThesId);
extern std::string GetRmlThesNameThesId(int ThesId);
extern MorphLanguageEnum GetDictLanguage(DictTypeEnum dict_type);
extern std::string  GetRossPath(DictTypeEnum dict_type);
