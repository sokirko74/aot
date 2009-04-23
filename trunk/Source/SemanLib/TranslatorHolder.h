#ifndef TRANSLATERHOLDER_H
#define TRANSLATERHOLDER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "assert.h"
#include "../common/util_classes.h"
#include  "../StructDictLib/Ross.h"
#include  "../StructDictLib/StructDictConsts.h"
#include  "../BinaryDictsLib/AspPairDict.h"
#include  "../BinaryDictsLib/FreqDict.h"
#include  "../BinaryDictsLib/DerivDict.h"
#include  "../BinaryDictsLib/SynDictionary.h"
#include  "../BinaryDictsLib/BinaryDictionary.h"

#include  "../common/SyntaxHolder.h"


enum LanguageEnum { Russian, English};
class CRossHolder;

const char g_strRegRusOborDicPath[]    = "Software\\Dialing\\Obor\\DictPath"; 	 
const char g_strAgramTabRegPath[]      = "Software\\Dialing\\Lemmatizer\\Russian\\Rgramtab";
const long UnknownParadigmId = -1;
const BYTE UnknownPOS = 0xff;

/*анадлогичное перечисление в seman.idl*/
enum DictTypeEnum   { Ross=0, LocRoss=1, CollocRoss=2, TimeRoss=3, OborRoss=4, Aoss=5, EngCollocRoss=6, EngObor=7, FinRoss=8, CompRoss=9, OmniRoss=10, NoneRoss=11 };

inline bool IsThesRoss(DictTypeEnum RossId)  
{
  return (RossId == LocRoss) ||  (RossId == CompRoss) || (RossId == FinRoss) ||  (RossId == OmniRoss);
};

inline int GetThesIdByRossId(DictTypeEnum RossId) 
{ 
		assert (IsThesRoss(RossId));
		switch  (RossId)  {
		   case LocRoss  : return LocThes;
		   case CompRoss : return CompThes;
		   case FinRoss  : return FinThes;
		   case OmniRoss : return OmniThes;
		   default       : return OmniThes; 
		};
};

inline DictTypeEnum GetRossIdByThesId(int ThesId) 
{ 
		switch  (ThesId)  {
		   case LocThes  : return LocRoss;
		   case CompThes : return CompRoss;
		   case FinThes  : return FinRoss;
		   case OmniThes : return OmniRoss;
		   default       : return OmniRoss; 
		};
};

inline string GetRmlThesNameThesId(int ThesId) 
{ 
  switch (ThesId) {
	 case LocThes  : return  "RML_THES_LOC"; 
	 case OmniThes  : return   "RML_THES_OMNI";
	 case CompThes : return   "RML_THES_COMP"; 
	 case FinThes  : return  "RML_THES_FIN"; 
     default : return "RML_THES_FIN"; 
	};
};
inline int GetThesIdByRmlThesName(const char* Name) 
{ 
  if (!strcmp(Name,"RML_THES_LOC")) return LocThes;
  if (!strcmp(Name,"RML_THES_OMNI")) return OmniThes;
  if (!strcmp(Name,"RML_THES_COMP")) return CompThes;
  if (!strcmp(Name,"RML_THES_FIN")) return FinThes ;
  return -1;
};

class CTranslatorHolder  
{
	CMorphologyHolder		m_EngHolder;
	CSyntaxHolder			m_RusHolder;

public:
	

	CTranslatorHolder();
	virtual ~CTranslatorHolder();

	
	
	CBinaryDictionary		m_BinaryDict;
	CFreqDict				m_HudFreq;
	CFreqDict				m_CompFreq;
	CFreqDict				m_FinFreq;
	CFreqDict				m_AdjNounDualFreq;
	//CDerivDictonary			m_piSupAdjDict;
	CSynDictionary			m_pSynonymsDict;
	CAspPairDict			m_AspDict;
	bool                    m_AspDictEnabled;
	long					m_LastUpdateTime;
	bool					m_bSilentMode;
	string					m_LastError;

	

	bool                    m_BinaryDictEnabled;
	const CLemmatizer* GetRusLemmatizer () const;
	const CLemmatizer* GetEngLemmatizer () const;
	const CLemmatizer* GetLemmatizer (MorphLanguageEnum langua) const;
	const CRusGramTab* GetRusGramTab () const;
	const CEngGramTab* GetEngGramTab () const;
	const CAgramtab* GetGramTab (MorphLanguageEnum langua) const;
	CSentencesCollection*	GetSynan ();
	const CThesaurus*       GetThes(int ThesId)  const;
	string					GetThesStr (int ThesId) const ;
	
	void InitThesaurus(int ThesId);
	bool Init();
	bool InitAspDict();
	bool InitBinaryDictionary();
	
	

	StringVector GetAspVerb (long ParadigmId, bool IsPerfective);
    StringVector GetAspVerb (string Verb, bool IsPerfective);
	void ErrorMessage (string Mess);
	bool MakeSyntaxStr(const char* text, CMyTimeSpanHolder& GlobalSpan);
	DWORD GetFirstParadigmIdAndLemma (const MorphLanguageEnum langua, string WordForm, long Poses, string& ResultLemma) const;
	DWORD  GetFirstParadigmId (const MorphLanguageEnum langua, string WordForm,  long Poses) const;
	string GetThesPath(int ThesId) const;
	const CDictionary* GetRusOborots() const;

};



#endif 
