// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)
#pragma once 

#include "../common/utilit.h"       
#include "../AgramtabLib/agramtab_.h"
#include "../StructDictLib/Ross.h"
#include "Models.h"
#include <map>       

const char FieldDelimiter[] = "#";

struct COborot 
{
	WORD m_UnitNo;
	// m_OborotId is a unique ID  which was attached to each oborot in this  thesaurus
	// we cannot store in thesaurus OborotNo (m_UnitNo) since OborotNo changes every time when user inserts a new record to the beginning
	long m_OborotId;
	COborot ()	{};
	COborot (int OborotId)
	{
		m_OborotId = OborotId;
	};
	bool operator < (const COborot& X) const
	{
		return m_OborotId < X.m_OborotId;
	};

};

const int siLemma = 1;
const int siOb1 = 2;
const int siOb2 = 4;
const int siUpLw = 8;
const int siUpUp = 16;
const int siPlural = 32;


struct CInnerSynItem {
 WORD   m_TerminNo;
 std::string m_ItemStr;
 int    m_Flags : 8;
 WORD   m_OborotId;
 WORD   m_OborotNo;
 BYTE   m_ItemPos;
 CInnerSynItem()
 {
	 m_Flags = 0;
 };

 bool operator < (const CInnerSynItem& X) const
 {
	 return m_ItemStr < X.m_ItemStr;
 };
 
};

typedef int thesaurus_concept_t;
const thesaurus_concept_t UnknownConceptId = -1;
typedef std::unordered_set<thesaurus_concept_t> thesaurus_concept_set_t;

typedef int thesaurus_termin_t;
const thesaurus_termin_t UnknownTerminNo = -1;
typedef std::unordered_set<thesaurus_termin_t>	thesaurus_termin_set_t;

struct CInnerTermin {
	std::string m_TerminStr;
	std::string m_AbbrForm;
	int	   m_TerminId;
	short  m_ModelNo;
	std::vector<long> m_TermItems;
};



struct CSynonym {
	thesaurus_concept_t		m_ConceptId;
	int		m_TextEntryId;
	UINT    m_DomainMask;
	CSynonym ()	{};

};


enum RelationEnum 
{
  HIGHER = 10,
  WHOLE = 20,
  LOWER = 30,
  PART  = 40
};

struct CRelat {
	thesaurus_concept_t		m_Concept1Id;
	thesaurus_concept_t		m_Concept2Id;
	int		m_RelationType;
};


typedef std::map<std::string, int> String2Int;

class CThesaurus 
{
	std::unordered_map<thesaurus_concept_t, std::string>		m_ConceptId2Name;
	std::unordered_map<std::string, thesaurus_concept_t>		m_ConceptName2Id;
	void dfs(thesaurus_concept_t conceptId, RelationEnum relationType, thesaurus_concept_set_t& concepts) const;

public:
	std::string			m_TestDebug;
	bool			m_bDontLoad;
	std::string			m_Name;
	std::string			m_Directory;
	MorphLanguageEnum m_MainLanguage;
    std::vector<CInnerModel>	m_Models;
	const CAgramtab*	m_pEngGramTab;
	const CAgramtab*	m_pMainGramTab;
	const CDictionary*	m_pOborDictionary;
	
	std::vector<COborot>			m_Oborots;
	std::vector<CInnerTermin>	m_Termins;
	std::vector<CSynonym>		m_SynonymsByConcept;
	std::vector<CSynonym>		m_SynonymsByTextEntry;
	std::vector<CRelat>			m_Relats;
	std::vector<CInnerSynItem>	m_SynItems;
	String2Int				m_AbbrForms2TerminNo;
	
	

	CThesaurus();
	~CThesaurus();
	bool LoadModels (std::string FileName);
	bool LoadAtomicGroups (std::string Buff, CInnerModel& M);
	bool LoadModelRelations (std::string Buff, CInnerModel& M);
	bool LoadGroups (std::string Buff, CInnerModel& M);

	bool LoadOborots (std::string FileName);

	int	 GetModelNoByModelId (long ModelId) const;
	bool LoadTermins (std::string FileName);

	void GetConceptsByTextEntryId(long TextEntryId, std::vector<thesaurus_concept_t>& Concepts) const;
	void GetTextEntriesByConcept(thesaurus_concept_t ConceptId, std::vector<long>& TextEntries) const;
	bool LoadSynonyms (std::string FileName);

	
	thesaurus_concept_t	 GetConceptIdByConceptStr (const std::string& ConceptStr) const;
	std::string GetConceptStrById(const thesaurus_concept_t& id) const;
	bool LoadConcepts (std::string FileName);

	bool LoadRelats (std::string FileName);
	
	int  GetTerminNoByTextEntryId(long TextEntryId) const;

	bool LoadSynItems(std::string FileName);

	// main methods
	bool	ReadThesaurusFromDisk();
	bool	ReadRelationsFromDisk();
	bool	SetDatabase(std::string DatabaseName);
	bool	IsA(DWORD TextEntryId, std::string ConceptStr) const;
	void	QueryEnglishTranslations(DWORD TextEntryId, std::vector<int>& CurrentEnglishTermins) const;
	int		GetTerminIdBySingleWord(std::string WordStr) const;
	thesaurus_concept_set_t		QueryTopConcepts(UINT TextEntryId) const;
	DWORD	QueryTerminItem(const std::string& ItemStr, std::vector<int>& CurrentTerminItems) const;
	thesaurus_termin_set_t QueryLowerTermins(const std::string& conceptStr, MorphLanguageEnum lang) const;
	int		FindAbbr(const std::string& str) const;
	void	SetDicts(MorphLanguageEnum MainLanguage, const CDictionary* OborDic, const CAgramtab* MainGramtab, const CAgramtab* EngGramtab);
};
