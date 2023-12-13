#include "dict_type.h"

bool IsThesRoss(DictTypeEnum dict_type)
{
	return (dict_type == LocRoss) || (dict_type == CompRoss) || (dict_type == FinRoss) || (dict_type == OmniRoss);
};


int GetThesIdByRossId(DictTypeEnum dict_type)
{
	assert(IsThesRoss(dict_type));
	switch (dict_type) {
	case LocRoss: return LocThes;
	case CompRoss: return CompThes;
	case FinRoss: return FinThes;
	case OmniRoss: return OmniThes;
	default: return OmniThes;
	};
};

DictTypeEnum GetRossIdByThesId(int ThesId)
{
	switch (ThesId) {
	case LocThes: return LocRoss;
	case CompThes: return CompRoss;
	case FinThes: return FinRoss;
	case OmniThes: return OmniRoss;
	default: return OmniRoss;
	};
};

std::string GetRmlThesNameThesId(int ThesId)
{
	switch (ThesId) {
	case LocThes: return  "RML_THES_LOC";
	case OmniThes: return   "RML_THES_OMNI";
	case CompThes: return   "RML_THES_COMP";
	case FinThes: return  "RML_THES_FIN";
	default: return "RML_THES_FIN";
	};
};

extern MorphLanguageEnum GetDictLanguage(DictTypeEnum dict_type) {
	switch (dict_type) {
	case Ross:	
	case TimeRoss:	
	case OborRoss:	
	case CollocRoss:	return morphRussian;

	case Aoss:	
	case EngCollocRoss:	
	case EngObor:	return morphEnglish;
	}
	return morphUnknown;

}

static std::string  GetThesRossPath(DictTypeEnum Type)
{
	int ThesId = GetThesIdByRossId(Type);
	std::string Name = GetRmlThesNameThesId(ThesId);
	std::string  R = Format("Software\\Dialing\\%s\\DictPath", Name.c_str());
	std::string  strPath = GetRegistryString(R.c_str());;
	DictTypeEnum dict_type = GetRossIdByThesId(ThesId);
	strPath += "/Ross";
	return strPath;
}

const char g_strRegEngRossDicPath[] = "Software\\Dialing\\Aross\\DictPath";
const char g_strRegRusRossDicPath[] = "Software\\Dialing\\Ross\\DictPath";
const char g_strRegRusTimeRossDicPath[] = "Software\\Dialing\\TimeRoss\\DictPath";
const char g_strRegEngCollocsDicPath[] = "Software\\Dialing\\EngCollocs\\DictPath";
const char g_strRegRusCollocsDicPath[] = "Software\\Dialing\\Collocs\\DictPath";
const char g_strRegEngOborDicPath[] = "Software\\Dialing\\EngObor\\DictPath";
const char g_strRegRusOborDicPath[] = "Software\\Dialing\\Obor\\DictPath";




std::string  GetRossPath(DictTypeEnum dict_type)
{
	if (IsThesRoss(dict_type))
		return  GetThesRossPath(dict_type);
	else
	{
		switch (dict_type) {
		case Ross:	return GetRegistryString(g_strRegRusRossDicPath);;
		case TimeRoss:	return GetRegistryString(g_strRegRusTimeRossDicPath);
		case OborRoss:	return GetRegistryString(g_strRegRusOborDicPath);
		case CollocRoss:	return GetRegistryString(g_strRegRusCollocsDicPath);
		case Aoss:	return GetRegistryString(g_strRegEngRossDicPath);
		case EngCollocRoss:	return GetRegistryString(g_strRegEngCollocsDicPath);
		case EngObor:	return GetRegistryString(g_strRegEngOborDicPath);
		}

	};

	throw CExpc("Unknown Ross");
}

