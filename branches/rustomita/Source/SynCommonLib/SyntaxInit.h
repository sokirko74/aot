// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef syntax_init_h
#define syntax_init_h

#pragma warning(disable:4786)


#include "stdafx.h"
#include  "ThesaurusForSyntax.h"
#include "../SimpleGrammarLib/SimpleGrammar.h"

#define MAX_DESC_LEN                    400
#define MAX_GRAM_CODES_LEN              100


#include "oborot.h"



class CFormatCaller;
class COborDic;
class CAgramtab;
class CSentence;




struct SDatItems
{
	DWORD			m_Poses;
	StringVector	m_vectorDatItems;
	
	SDatItems(DWORD Poses)
	{ m_Poses = Poses; };

	void Sort()
	{
		sort(m_vectorDatItems.begin(),m_vectorDatItems.end());
	};

};


class CSyntaxOpt {
	_share_pointer_t<const CAgramtab*>			m_piGramTab;
	_share_pointer_t<const CLemmatizer*>		m_piLemmatizer;
	_share_pointer_t<const CDictionary*>		m_piOborDictionary;
protected:
    
public:
	// special constants 
	QWORD	m_PluralMask;
	QWORD	m_SingularMask;
	int		m_SimilarNPGroupType;
	int		m_GenNounGroupType;
	int		m_PrepNounGroupType;
	int		m_DirObjGroupType;
	int		m_NPGroupType;
	int		m_NounAdjGroupType;
	int		m_NameGroupType;
	int		m_DisruptConjRelation;
	int		m_DisruptConjGroupType;
	int		m_OborotGroupType;
	int		m_WWWGroupType;
	int		m_KEYBGroupType;
	int		m_SubjRelation;
	int		m_RusParenthesis;
	int		m_Preposition;
	int		m_Conjunction;
	QWORD	m_IndeclinableMask;
	int		m_InfintiveClauseType;
	

	MorphLanguageEnum						m_Language;
	bool									m_bEnableCompThesaurus;
	bool									m_bEnableFinThesaurus;
	bool									m_bEnableOmniThesaurus;
	bool									m_bEnableLocThesaurus;
	KillHomonymsEnum						m_KillHomonymsMode;
	bool									m_bSilentMode;

	size_t									m_EmptyGroupType;
	size_t									m_EmptyClauseType;
	_share_pointer_t<const CThesaurus*>		m_FinThes;
	_share_pointer_t<const CThesaurus*>		m_CompThes;
	_share_pointer_t<const CThesaurus*>		m_LocThes;
	_share_pointer_t<const CThesaurus*>		m_OmniThes;
    

	
	SDatItems*                              m_pProfessions;
    COborDic*								m_pOborDic;
	mutable vector<string>					m_SyntaxGroupTypes;

	CWorkGrammar							m_FormatsGrammar;


    CSyntaxOpt (MorphLanguageEnum langua);
	virtual ~CSyntaxOpt() {};
	virtual void DestroyOptions ();
	bool IsValid() const;

    bool  ReadListFile(const string& FileName, StringVector& C);

	_share_pointer_t<const CThesaurus*>& GetThesPointerByThesId (UINT ThesId);
	const CThesaurus* GetThesByThesId (UINT ThesId);

	void OpenLogFile();
	void CloseLogFile();
	void OutputErrorString(string strMsg) const;
	bool LoadTermins(const CDictionary* piOborDic);
	bool LoadTerminsForOneThesaurus(const char* ThesName);

	const CThesaurusForSyntax& GetThesaurus() const  { 	return *m_pThesaurus; 	}	

	void SetGramTab (const CAgramtab*A , bool bOwnPointer)	{		m_piGramTab.SetPointer(A, bOwnPointer); 	};
	const CAgramtab* GetGramTab () const 	{ 		return m_piGramTab.m_Pointer; 	};

	void SetLemmatizer (const CLemmatizer* L, bool bOwnPointer)		{m_piLemmatizer.SetPointer(L, bOwnPointer);		};
	const CLemmatizer* GetLemmatizer () const 	{		return m_piLemmatizer.m_Pointer;	};

	void SetOborDic (const CDictionary* L, bool bOwnPointer)		{m_piOborDictionary.SetPointer(L, bOwnPointer);		};
	const CDictionary* GetOborDic () const 	{		return m_piOborDictionary.m_Pointer;	};

	bool InitializeOptions();

	virtual bool InitOptionsLanguageSpecific() = 0;	
	virtual CAgramtab* GetNewGramTab () const = 0;

	
	CSentence* NewSentence () const;

	const char*	GetGroupNameByIndex(long lType) const;
	const size_t GetGroupTypesCount() const 
	{
		return m_SyntaxGroupTypes.size();
	};
	int GetGroupTypebyName(const char* TypeName) const;
	virtual bool is_firm_group(int GroupType) const = 0 ;
	virtual bool IsGroupWithoutWeight(int GroupType, const char* cause) const = 0;
	virtual bool IsSimilarGroup (int type) const = 0;
	int GetSyntaxGroupOrRegister(const char* TypeName) const;

protected:
	FILE*		m_SynAnLogFile;
	CThesaurusForSyntax*	m_pThesaurus;

};



bool GetRegString(string key, string& value);





extern bool has_item (const StringVector* C, const char* item);


#endif
