#include "stdafx.h"
#include "AllRossesHolder.h"
#pragma warning (disable : 4355)


const char g_strRegEngRossDicPath[]    = "Software\\Dialing\\Aross\\DictPath"; 	 
const char g_strRegRusRossDicPath[]    = "Software\\Dialing\\Ross\\DictPath"; 	 
const char g_strRegRusTimeRossDicPath[] = "Software\\Dialing\\TimeRoss\\DictPath"; 	 
const char g_strRegEngCollocsDicPath[] = "Software\\Dialing\\EngCollocs\\DictPath";
const char g_strRegRusCollocsDicPath[] = "Software\\Dialing\\Collocs\\DictPath";
const char g_strRegEngOborDicPath[] = "Software\\Dialing\\EngObor\\DictPath";




CAllRossesHolder::CAllRossesHolder() :
		m_RossDoc(this),
		m_LocRossDoc(this),
		m_TimeRossDoc(this),
		m_RusOborDoc(this),
		m_RusCollocsDoc(this),
		m_EngRossDoc(this),
		m_EngCollocsRossDoc(this),
		m_EngOborRossDoc(this),
		m_FinThesRossDoc(this),
		m_LocThesRossDoc(this),
		m_OmniThesRossDoc(this),
		m_CompThesRossDoc(this)
{
};



bool  CAllRossesHolder::InitHierarchies() 
{
	m_HierarchySemRelDoc.m_pRossDoc = &m_RossDoc;
	m_HierarchySemRelDoc.m_Type	= SemRel;
	m_HierarchySemRelDoc.MySerialize(false,	true);
	m_HierarchySemRelDoc.SetHierarchyTransitiveClosure();

	m_HierarchySemFetDoc.m_pRossDoc = &m_RossDoc;
	m_HierarchySemFetDoc.m_Type =	SemFet;
	m_HierarchySemFetDoc.MySerialize(false, true);
	m_HierarchySemFetDoc.SetHierarchyTransitiveClosure();

	return true;
};

CDictionary* CAllRossesHolder::GetRoss (DictTypeEnum Type)     
{
		 return GetRossHolder(Type)->GetRoss();
};




CRossHolder* CAllRossesHolder:: GetRossHolder (DictTypeEnum Type)  
{ 
	switch (Type) {
		case Ross			:	return &m_RossDoc;
		case TimeRoss		:	return &m_TimeRossDoc;
		case OborRoss		:	return &m_RusOborDoc;
		case CollocRoss		:	return &m_RusCollocsDoc;
		case Aoss			:	return &m_EngRossDoc;
		case EngCollocRoss 	:	return &m_EngCollocsRossDoc;
		case EngObor		:	return &m_EngOborRossDoc;
		case FinRoss		:	return &m_FinThesRossDoc;
		case OmniRoss		:	return &m_OmniThesRossDoc;
		case CompRoss		:	return &m_CompThesRossDoc;
		case LocRoss		:	return &m_LocThesRossDoc;
	};

	throw CExpc ("Unknown Ross");
};


string  GetThesRossPath  (DictTypeEnum Type)
{
	int ThesId = GetThesIdByRossId(Type);
	string Name = GetRmlThesNameThesId( ThesId );
	string  R = Format("Software\\Dialing\\%s\\DictPath",Name.c_str());
	string  strPath = GetRegistryString( R.c_str());;
	DictTypeEnum RossId =  GetRossIdByThesId(ThesId);
	strPath += "/Ross/ross.rcf";
	return strPath;
}

const CRossHolder* CAllRossesHolder::GetRossHolder (DictTypeEnum Type)  const
{
	switch (Type) {
		case Ross			:	return &m_RossDoc;
		case TimeRoss		:	return &m_TimeRossDoc;
		case OborRoss		:	return &m_RusOborDoc;
		case CollocRoss		:	return &m_RusCollocsDoc;
		case Aoss			:	return &m_EngRossDoc;
		case EngCollocRoss 	:	return &m_EngCollocsRossDoc;
		case EngObor		:	return &m_EngOborRossDoc;
		case FinRoss		:	return &m_FinThesRossDoc;
		case OmniRoss		:	return &m_OmniThesRossDoc;
		case CompRoss		:	return &m_CompThesRossDoc;
		case LocRoss		:	return &m_LocThesRossDoc;

	};

	throw CExpc ("Unknown Ross");
};

void setbackslash(string& s)
{
	for (size_t i=0; i < s.length(); i++)
		if (s[i] == '\\')
			s[i] = '/';
}

DictTypeEnum  CAllRossesHolder::GetRegisteredRossId(string FileName)  const
{
	setbackslash(FileName);
	EngRusMakeLower(FileName);
	for (int i=0; i < NoneRoss; i++)
	{
		string path = GetRossPath((DictTypeEnum)i);
		setbackslash(path);
		EngRusMakeLower(path);
		if ( path == FileName)
			return (DictTypeEnum)i;
	}
	return NoneRoss;
}



string  CAllRossesHolder::GetRossPath(DictTypeEnum RossId)  const
{
	if (IsThesRoss(RossId))
		return  GetThesRossPath(RossId);
	else
	{
		switch (RossId) {
			case Ross			:	return GetRegistryString( g_strRegRusRossDicPath )+ "/ross.rcf";;
			case TimeRoss		:	return GetRegistryString( g_strRegRusTimeRossDicPath )+"/timeross.rcf";
			case OborRoss		:	return GetRegistryString( g_strRegRusOborDicPath )+"/obor.rcf";
			case CollocRoss		:	return GetRegistryString( g_strRegRusCollocsDicPath )+"/collocs.rcf";;
			case Aoss			:	return GetRegistryString( g_strRegEngRossDicPath ) + "/aross.rcf";;
			case EngCollocRoss 	:	return GetRegistryString( g_strRegEngCollocsDicPath )+"/eng_collocs.rcf";
			case EngObor		:	return GetRegistryString( g_strRegEngOborDicPath )+"/aobor.rcf";;
		}
	};

	throw CExpc ("Unknown Ross");
}


DictTypeEnum CAllRossesHolder::GetTypeByRossHolder (const CRossHolder* pHolder)  const
{ 
	if (&m_RossDoc  == pHolder)
		return Ross;
	else
	if (&m_TimeRossDoc == pHolder)
		return TimeRoss;
	else
	if (&m_RusOborDoc == pHolder)
		return OborRoss;
	else
	if (&m_RusCollocsDoc == pHolder)
		return CollocRoss;
	else
	if (&m_EngRossDoc == pHolder)
		return Aoss;
	else
	if (&m_EngCollocsRossDoc == pHolder)
		return EngCollocRoss;
	else
	if (&m_EngOborRossDoc == pHolder)
		return EngObor;
	else
	if (&m_FinThesRossDoc == pHolder)
		return FinRoss;
	else
	if (&m_OmniThesRossDoc == pHolder)
		return OmniRoss;
	else
	if (&m_CompThesRossDoc == pHolder)
		return CompRoss;
	else
	if (&m_LocThesRossDoc == pHolder)
		return LocRoss;
	else
	{
		assert (false); 
		return NoneRoss;
	};
};


DictTypeEnum CAllRossesHolder::GetTypeByStr (string DictName)
{
	if (DictName == m_RossDoc.m_DictName)
		return Ross;
	else
	if (DictName == m_TimeRossDoc.m_DictName)
		return TimeRoss;
	else
	if (DictName == m_RusOborDoc.m_DictName)
		return OborRoss;
	else
	if (DictName == m_RusCollocsDoc.m_DictName)
		return CollocRoss;
	else
	if (DictName == m_EngRossDoc.m_DictName)
		return Aoss;
	else
	if (DictName == m_EngCollocsRossDoc.m_DictName)
		return EngCollocRoss;
	else
	if (DictName == m_EngOborRossDoc.m_DictName)
		return EngObor;
	else
	if (DictName == m_FinThesRossDoc.m_DictName)
		return FinRoss;
	else
	if (DictName == m_OmniThesRossDoc.m_DictName)
		return OmniRoss;
	else
	if (DictName == m_CompThesRossDoc.m_DictName)
		return CompRoss;
	else
	if (DictName == m_LocThesRossDoc.m_DictName)
		return LocRoss;
	else
		return NoneRoss;
};


void  CAllRossesHolder::Initialize() 
{
	for (int i=0; i < NoneRoss; i++)
	{
		DictTypeEnum RossId = (DictTypeEnum)i;
		CRossHolder* RossHolder = GetRossHolder(RossId);
		if (RossHolder->m_DictPath.empty())
			if (!RossHolder->OpenRossHolder(GetRossPath(RossId), m_bDontLoadExamples))
				throw CExpc("Cannot load " + GetRossPath(RossId));
	}

	m_LastUpdateTime = 1;
}



