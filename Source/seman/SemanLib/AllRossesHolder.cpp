#include "stdafx.h"
#include "AllRossesHolder.h"
#pragma warning (disable : 4355)




CAllRossesHolder::CAllRossesHolder() :
		m_RossDoc(Ross),
		m_TimeRossDoc(TimeRoss),
		m_RusOborDoc(OborRoss),
		m_RusCollocsDoc(CollocRoss),
		m_EngRossDoc(Aoss),
		m_EngCollocsRossDoc(EngCollocRoss),
		m_EngOborRossDoc(EngObor),
		m_FinThesRossDoc(FinRoss),
		m_LocThesRossDoc(LocRoss),
		m_OmniThesRossDoc(OmniRoss),
		m_CompThesRossDoc(CompRoss)
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




CStructDictHolder* CAllRossesHolder:: GetRossHolder (DictTypeEnum Type)  
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


const CStructDictHolder* CAllRossesHolder::GetRossHolder (DictTypeEnum Type)  const
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

void setbackslash(std::string& s)
{
	for (size_t i=0; i < s.length(); i++)
		if (s[i] == '\\')
			s[i] = '/';
}

DictTypeEnum  CAllRossesHolder::GetRegisteredRossId(std::string FileName)  const
{
	setbackslash(FileName);
	MakeLowerUtf8(FileName);
	for (int i=0; i < NoneRoss; i++)
	{
		std::string path = GetRossPath((DictTypeEnum)i);
		setbackslash(path);
		MakeLowerUtf8(path);
		if ( path == FileName)
			return (DictTypeEnum)i;
	}
	return NoneRoss;
}




DictTypeEnum CAllRossesHolder::GetTypeByRossHolder (const CStructDictHolder* pHolder)  const
{ 
	return pHolder->GetRossType();
};


DictTypeEnum CAllRossesHolder::GetTypeByStr (std::string DictName)
{
	if (DictName == m_RossDoc.GetDictName())
		return Ross;
	else
	if (DictName == m_TimeRossDoc.GetDictName())
		return TimeRoss;
	else
	if (DictName == m_RusOborDoc.GetDictName())
		return OborRoss;
	else
	if (DictName == m_RusCollocsDoc.GetDictName())
		return CollocRoss;
	else
	if (DictName == m_EngRossDoc.GetDictName())
		return Aoss;
	else
	if (DictName == m_EngCollocsRossDoc.GetDictName())
		return EngCollocRoss;
	else
	if (DictName == m_EngOborRossDoc.GetDictName())
		return EngObor;
	else
	if (DictName == m_FinThesRossDoc.GetDictName())
		return FinRoss;
	else
	if (DictName == m_OmniThesRossDoc.GetDictName())
		return OmniRoss;
	else
	if (DictName == m_CompThesRossDoc.GetDictName())
		return CompRoss;
	else
	if (DictName == m_LocThesRossDoc.GetDictName())
		return LocRoss;
	else
		return NoneRoss;
};


void  CAllRossesHolder::Initialize() 
{
	for (int i=Ross; i < NoneRoss; i++)
	{
		DictTypeEnum RossId = (DictTypeEnum)i;
		CStructDictHolder* RossHolder = GetRossHolder(RossId);
		if (!RossHolder->LoadStructDict(GetRossPath(RossId), m_bDontLoadExamples))
			throw CExpc("Cannot load " + GetRossPath(RossId));
	}

	m_LastUpdateTime = 1;
}



