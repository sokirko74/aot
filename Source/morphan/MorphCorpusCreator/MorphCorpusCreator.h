// CMorphCorpusCreator.h: interface for the CMorphCorpusCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXT_PROCESSOR_H__44D218CB_4FBD_420C_83EE_45B22FB5C27C__INCLUDED_)
#define AFX_TEXT_PROCESSOR_H__44D218CB_4FBD_420C_83EE_45B22FB5C27C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "assert.h"
#include "../../common/COM/COMSyntaxHolder.h"

class CMorphCorpusCreator 
{
	MorphLanguageEnum				m_CurrentLanguage;
	LEMMATIZERLib::ILemmatizerPtr	m_piLemmatizer;
	AGRAMTABLib::IGramTabPtr		m_pGramTab;
public:
	CMorphCorpusCreator();
	std::string get_xml_result(const CCOMSyntaxHolder& Holder);
	bool m_b_show_clause;	
	bool m_bArtificialCases;
	
protected:
	void add_open_clause_tag(std::string& result, std::string clause_type);
	void add_close_clause_tag(std::string& result);
	void get_top_clauses(SYNANLib::ISentencePtr piSent, std::vector<long>& topClauses);
	void process_sent(SYNANLib::ISentencePtr piSent, std::string& result);
	void process_clause(SYNANLib::ISentencePtr piSent, SYNANLib::IClausePtr piClause, std::string& result);
	long process_word(SYNANLib::ISentencePtr piSent, long iUnit, SYNANLib::IClausePtr piClause, std::string& result, bool bCheckOborot);
	void process_homonym(SYNANLib::IHomonymPtr piHom, SYNANLib::ISyntaxUnitPtr piUnit, SYNANLib::IWordPtr piWord, std::set<std::string>& result_inters);
	std::string process_gram_homonym(std::string lemma, long paradigm_id, const std::string& ancode, SYNANLib::IWordPtr piWord);
	long  process_oborot(SYNANLib::ISentencePtr piSent, SYNANLib::IClausePtr piClause, long iUnit, SYNANLib::IClauseVariantPtr piVar, std::string& result);
	std::string GetLemma(std::string lemma, BYTE pos, long ParadigmID, SYNANLib::IWordPtr piWord, uint64_t lexema_grammems, std::string ancode);
	bool try_to_process_hyphen_word(SYNANLib::IWordPtr piWord, std::string& sRes);
	bool hasSecondCase(long paradigm_id, std::string strForm, BYTE eCase);
	int get_index_in_paradigm(SYNANLib::IWordPtr piWord, LEMMATIZERLib::IParadigmPtr piPard, const std::string& str_ancode);
	void AddAccent(std::string& lemma, int ParadigmID);
	void get_all_morph_interps(SYNANLib::IWordPtr piWord , std::set<std::string>& all_morph_interps);
		
	
};

#endif // !defined(AFX_TEXT_PROCESSOR_H__44D218CB_4FBD_420C_83EE_45B22FB5C27C__INCLUDED_)
