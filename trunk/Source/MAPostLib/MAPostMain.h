// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)


#ifndef __MAPOST_MAIN_H_
#define __MAPOST_MAIN_H_



#include "../common/util_classes.h"
#include "../common/PlmLine.h"       
#include "../MAPostLib/PostMorphInterface.h" 
#include "../MAPostLib/PostLemWord.h" 


#include "../LemmatizerLib/PLMLineCollection.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../AgramtabLib/rus_consts.h"
#include "../TrigramLib/TrigramModel.h"
#include "list"       

const char g_strAgramTabRegPath[] = "Software\\Dialing\\Lemmatizer\\Russian\\Agramtab";


struct CFixedColloc
{
	string		   m_LemmaStr;
	struct CLemmaAndPOS {
		string m_Lemma;
		BYTE   m_POS;
	};
	vector<CLemmaAndPOS> m_Lemmas;
	long		   m_MainWordNo;
	string		   m_InterfaceString;
	CFixedColloc(string LemmaStr,	long MainWordNo,	string InterfaceString,  const CAgramtab* m_piRusGramTab);
	CFixedColloc()
	{
		m_MainWordNo = -1;
	};

};


struct CSurnameSuffix {
	string m_Suffix;
	string m_GramCode;
	int    m_ParadigmNo;
};

struct CLemmaAndCodes {
	string m_Lemma;
	string m_GramCodes;
};



typedef list<CPostLemWord> LineCollection ;

typedef LineCollection::iterator CLineIter;
typedef LineCollection::const_iterator CConstLineIter;

typedef CSmallVector<CLemmaAndCodes, 20> SurnameHypotsVec;


/////////////////////////////////////////////////////////////////////////////
// CCOMMAPost
class CMAPost  : public CPostMorphInteface
{
public:
	CMAPost();
	~CMAPost();
public:
	//грамматический код ДУРНОВО (неизменяеммое существительное всех родов)
	string          m_DURNOVOGramCode;
	list<CPostLemWord>	m_Words;
	

    bool    LoadWords(const CPlmLineCollection *piInTextItems);
	bool	ProcessData(const CPlmLineCollection *piInTextItems, CPlmLineCollection& piOutTextItems);
	bool	Init(const CLemmatizer* RusLemmatizer, const CAgramtab* RusGramTab);
	CLineIter Remove(CLineIter it, bool bRemoveSpaceAfterDeletedWord);
    CLineIter PassSpaces(CLineIter it);
    CLineIter BackSpaces(CLineIter it);
    CLineIter NextNotSpace(CLineIter it);

	

protected:
	const CLemmatizer*			m_pRusLemmatizer;
	const CAgramtab*			m_pRusGramTab;
	vector<CFixedColloc>        m_FixedCollocs;
    CTrigramModel               m_TrigramModel;
	bool                        m_bUseTrigrams;

	

	string			m_LogFileName;

	

	void RunRules();
	int Count() const	{ return m_Words.size(); };

	string		GetSimilarNumAncode (const string&  Lemma, const string&  Flexia, bool IsNoun);
	bool		HasParadigmOfFormAndPoses(string WordForm, size_t Poses) const;
	bool		NounHasObviousPluralContext(CLineIter it);
    

	

protected: 
	//rules
	void Odnobuk();
	void Cifrdef();
	void ILeDefLe();
	void Memlem();
	void ParticipleAndVerbInOneForm();
	void PronounP_Pronoun_Homonymy();
	void FixedCollocations();
	bool ReadCollocs();
	void CorrectOborots();
	void SemiAdjectives();
	void SemiNouns();
	void Rule_UZHE();
	void Rule_Ideclinable();
	void Rule_DeadPlurals();
	void Rule_RelationalAdjective();
	
	string GetNormSurnameSuffix(long ParadigmNo) const;
	void Rule_QuoteMarks();
	void Rule_ILE();
	void Rule_KAK_MOZHNO();
	void Rule_Redublication();
	void Rule_CHTO_ZA();
	void Rule_VOT_UZHE();
	void Rule_Fio();

	bool CanBeDubleDelimiter(CLineIter it);
	void InsertComma(CLineIter it);
	void Rule_UnknownNames();
	void Rule_SOROK();
	void Rule_Abbreviation();
	// по-восточному
	void Rule_AdverbFromAdjectives();
	void log(string s);
	void SaveToFile(string s);
	CLineIter AddCollocation(CLineIter start_it, CLineIter end_it, CLineIter main_it, bool HasEndtSent,  int CollocNo);
	void Rule_FilterProperName();
	void Rule_ChangePatronymicLemmas();
	void Rule_Interjections();
    bool FilterPostMorphWords();
    bool FilterOnePostLemWord(CPostLemWord& W, WORD tagid, WORD tagid2) const;
    void Rule_TwoPredicates();
    void SolveAmbiguityUsingRuleForTwoPredicates(CLineIter start, CLineIter end);

};






#endif //__MAPOST_H_



