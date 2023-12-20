#pragma once

#include "assert.h"
#include "morph_dict/common/util_classes.h"
#include  "dicts/StructDictLib/Ross.h"
#include  "dicts/StructDictLib/StructDictConsts.h"
#include  "dicts/BinaryDictsLib/AspPairDict.h"
#include  "dicts/BinaryDictsLib/FreqDict.h"
#include  "dicts/BinaryDictsLib/DerivDict.h"
#include  "dicts/BinaryDictsLib/BinaryDictionary.h"

#include  "synan/SynanLib/SyntaxHolder.h"


class CStructDictHolder;
class CRusGramTab;
class CEngGramTab;

class CTranslatorHolder  
{
	CSyntaxHolder			m_RusHolder;

public:
	

	CTranslatorHolder();
	virtual ~CTranslatorHolder();

	
	
	CBinaryDictionary		m_BinaryDict;
	CFreqDict				m_HudFreq;
	CFreqDict				m_CompFreq;
	CFreqDict				m_FinFreq;
	CFreqDict				m_AdjNounDualFreq;
	CAspPairDict			m_AspDict;
	long					m_LastUpdateTime;
	bool					m_bSilentMode;
	std::string					m_LastError;

	const CLemmatizer* GetRusLemmatizer () const;
	const CLemmatizer* GetEngLemmatizer () const;
	const CLemmatizer* GetLemmatizer (MorphLanguageEnum langua) const;
	const CRusGramTab* GetRusGramTab () const;
	const CEngGramTab* GetEngGramTab () const;
	const CAgramtab* GetGramTab (MorphLanguageEnum langua) const;
	CSentencesCollection*	GetSynan ();
	const CThesaurus*       GetThes(int ThesId)  const;
	std::string					GetThesStr (int ThesId) const ;
	
	void InitThesaurus(int ThesId);
	void Init();

	StringVector GetAspVerb (uint32_t ParadigmId, bool IsPerfective);
    StringVector GetAspVerb (std::string Verb, bool IsPerfective);
	void ErrorMessage (std::string Mess);
	bool MakeSyntaxStr(const char* text, CMyTimeSpanHolder& GlobalSpan);
	uint32_t GetFirstParadigmIdAndLemma (const MorphLanguageEnum langua, std::string WordForm, part_of_speech_mask_t Poses, std::string& ResultLemma) const;
	uint32_t  GetFirstParadigmId (const MorphLanguageEnum langua, std::string WordForm,  part_of_speech_mask_t Poses) const;
	std::string GetThesPath(int ThesId) const;

};
