#ifndef TrigramModel_h
#define TrigramModel_h


#include "../common/util_classes.h"
#include "list"
#include "limits.h"

#ifdef  USE_TRIGRAM_LEMMATIZER
#include "TagSet.h"
#include "../common/PlmLine.h"
#include "../GraphanLib/GraphmatFile.h"
#include "../LemmatizerLib/Lemmatizers.h"
#endif

#include "../common/MorphXmlToken.h"


#include <float.h> 

//typedef double prob_t;
//#define MAXPROB DBL_MAX

typedef float prob_t;
#define MAXPROB FLT_MAX

const WORD UnknownTag = 0xffff;

typedef QWORD trigram_integer_t;
//typedef int trigram_integer_t;

struct CLexProb 
{
	WORD	m_PrevTag;
	WORD	m_Tag;
	DWORD	m_Count;
	CLexProb()
	{
		m_Tag = UnknownTag;
		m_PrevTag = UnknownTag;
		m_Count = 0;
	};
	CLexProb(WORD PrevTag, WORD Tag, DWORD Count)
	{
		m_PrevTag = PrevTag;
		m_Tag = Tag ;
		m_Count = Count;
	};
    bool operator == (const CLexProb& X) const 
    {
        return      m_PrevTag == X.m_PrevTag
                &&  m_Tag == X.m_Tag
                &&  m_Count == X.m_Count;
    }

};


struct CTrigramWord
{
    string m_WordStr; 
    size_t m_StartOffset;
    size_t m_Length;
    bool operator == (const CTrigramWord& X) const 
    {
        return      m_WordStr == X.m_WordStr
                &&  m_StartOffset == X.m_StartOffset
                &&  m_Length == X.m_Length;
    }
};



struct CWordIntepretation 
{	
	WORD	m_TagId1;
	WORD	m_TagId2;
	CWordIntepretation()
	{
		m_TagId1 =  UnknownTag;
		m_TagId2 = UnknownTag;
	}
};
class CLemmatizer;
class CAgramtab;
struct CLambdas{
		prob_t m_lambda[3];   /* lambda_1 - _3 for trigram interpolation */
};



struct CDictionarySearch 
{
	const CTrigramWord*     m_pFoundWord;
	set<WORD>		        m_PossibleWordTags;
};


/*
Структура CViterbiInfo  используется только в самом Viterbi-алгоритме. Один экземпляр 
этой структуры создается для каждого входного слова.  В этой структуре сохраняются 
вероятности переходов в данный тэг с учетом текущего симовола входной цепочки (m_Probs).
Слот m_TagRefs хранит указатель на наиболее  вероятный первый элемент n1  для тройки тэгов 
(n1, n2, n3). Т.е  m_TagRefs[n2][n3] = n1, если в качестве тэгов выбраны n2 и n3, то слову, которое идет 
до n2 надо приписывать тэг n1.
Слот m_pLexicalProbs содержит унарные (=бесконтекстные=лексические)вероятности приписывания тэгов данном слову.
*/
class CViterbiInfo
{
private:
	
    map<pair<WORD,WORD>, prob_t>			m_Probs;
    map<pair<WORD,WORD>, WORD>			m_TagRefs;
    //set< pair<prob_t, WORD> >		m_Maximums;
    pair<prob_t, WORD> 		m_FirstMaximum;
    pair<prob_t, WORD> 		m_SecondMaximum;

public:
	set<WORD>						m_LexicalProbs;
    
    
	

	CViterbiInfo ();
    void SetProb(WORD i,  WORD j, prob_t  value);
	prob_t GetProb(WORD i,  WORD j) const;
	void SetTagRef(WORD i,  WORD j, WORD TagRef);
    WORD GetTagRef(WORD i,  WORD j) const;
	const map<pair<WORD,WORD>, prob_t>&  GetAllProbs() const;
    void  UpdateMaximum(prob_t prob, WORD tag);
    bool  IsAmbig()  const;
    pair<prob_t, WORD>  GetFirstMaximum()  const;
    pair<prob_t, WORD>  GetSecondMaximum()  const;
};

const 	int	START_AT_TAG  = 1;
const  int MaxSmallTagSetSize = 150;
struct CTransProbForSmallTagSet{
	prob_t m_Probs[MaxSmallTagSetSize][MaxSmallTagSetSize][MaxSmallTagSetSize];
};

const	size_t	m_MaxCacheSize = 1000000;

class CTrigramModel
{

	// структура используется для построения бакетов 
	struct CSingleHistory
	{
		WORD i;
		WORD j;
		int  m_TrigramsCount;
	};
	typedef map<int, vector< CSingleHistory >  >  PMap;
	typedef map<size_t, prob_t>  BMap;

    // serializable
	WORD					m_TagsCount; // == m_RegisteredTags.size()
	vector<int>			m_Unigrams;      /* uni-, bi- and trigram counts */
	vector<WORD>		m_TagsOrderedByUnigrams;      /* uni-, bi- and trigram counts */
	vector<int>			m_Bigrams;      /* uni-, bi- and trigram counts */
	map<trigram_integer_t,int>		m_Trigrams;
    int					m_TypeCounts[3];        /* uni-, bi- and trigram type counts */
	int					m_TokenCounts[3];       /*	m_TokenCounts[0] - число слов во входном массиве  
									m_TokenCounts[1] - число биграмм во входном массиве (NB! деление на предложения )
									m_TokenCounts[2] - число триграмм во входном массиве (NB! деление на предложения )
								*/
    mutable map<string, const vector<CXmlMorphAnnot>*> m_CurrentSentenceWords2Annots;
    vector<CLexProb >   m_LexProbs;
    vector<CTrigramWord> m_Dictionary; 
    // ... 
	

	mutable	size_t                          m_CachedSmoothedSize;	
	mutable vector< map<size_t, prob_t>  >  m_CachedSmoothedTransProb;         /* smoothed transition probs */
	bool					                m_bHasSmallTagset;
	CTransProbForSmallTagSet*               m_pSmoothedProbsForSmallTagsets;

	
	vector<int>			m_Buckets;
	vector< CLambdas >	m_BucketsLambdas;
	bool				m_bDebugViterbi;
	set<string>			m_TestLexicon;

	
	

	
	void	set_ngram(size_t n, WORD t1, WORD t2, WORD t3, int value);
	int ngram_index(WORD t1) const;
	int ngram_index(WORD t1, WORD t2) const;
	trigram_integer_t ngram_index(WORD t1, WORD t2, WORD t3) const;

	WORD find_tag(const string &t) const; 
	WORD register_tag(const string& t);
	bool CheckLemma(string Word, string TagStr1, string TagStr2) const;
	int				compute_bucket_denominator(WORD i, WORD j, int& TrigramsCount);
	CLambdas		compute_lambdas_for_one_bucket(PMap::const_iterator start_it, PMap::const_iterator end_it);
	prob_t			GetSmoothedProb(WORD PrevPrevTag, WORD PrevTag, WORD CurrTag) const;
	void			ViterbiForward(const vector<string>& words, vector<CViterbiInfo>& Triplet, const vector<CViterbiInfo>& RevTriplet) const;
	void			ViterbiBackward(const vector<string>& words, const vector<CViterbiInfo>& Triplet, vector<CWordIntepretation>& tags) const;
	size_t			GetTrigram(WORD t1, WORD t2, WORD t3)  const;
	void			get_tags_from_lemmatizer_but_not_preps(const string& WordStr, set<WORD>& tags) const;
	CDictionarySearch		find_word(const string& WordStr) const;
	prob_t					GetSmoothedLexProb(const CDictionarySearch& DS, WORD PrevTag, WORD CurrTag) const;
	bool					LoadSynanForThisSentence(const vector<string>& words) const;
	set<string>				GetLemmaSetByTagAndWordStr(string Word, string TagStr, bool bOnlyWithMaxWeight) const;
	
	prob_t					BuildSmoothedProb(WORD PrevPrevTag, WORD PrevTag, WORD CurrTag) const;
	bool					TagRawTexts(string FileName);
	bool					TagRawText(string FileName);
	
    
public:
	bool				m_bQuiet;
    

#ifdef  USE_TRIGRAM_LEMMATIZER
   	CLemmatizer*	m_pLemmatizerPrivate;
	const CLemmatizer*	m_pLemmatizer;

    CAgramtab*	m_pAgramtabPrivate;
	const CAgramtab*	m_pAgramtab;

   	CGraphmatFile		m_Graphan;
    CTagSet				m_TagSet;
#endif
	CTrigramModel*       m_pReverseModel;
	vector<string>			m_RegisteredTags;      /* tags */
	bool				m_bUseSecondLocalMax;
    bool				m_bReverseModel;
	MorphLanguageEnum	m_Language;
	
	size_t				m_MinBucketSize;
	size_t				m_SecondLocalCoef;
	bool				m_bCheckOnlyAmbiguosWords;
	
	bool				m_bRawTexts;
    string              m_DictionaryFile;
    string              m_NgramFile;
	

	CTrigramModel ();
	~CTrigramModel ();

	bool InitDicts ();
	bool read_ngram_file();
    bool register_tags_from_ngram_file();
	void compute_counts_for_boundary();
	void enter_rare_word_tag_counts(void *key, CTrigramWord& wd,  void *d2);
	void compute_lambdas();
	void load_lambdas(string FileName);
	
	bool compute_bucketed_lambdas();
	void compute_transition_probs();
    bool read_dictionary_text_file(map<string,  vector<CLexProb> >& Dictionary) const;
	
	void dump_transition_probs();
	bool tag_sentence(char* SentenceStr) const;
	bool tagging(string FileName)  ;
	bool testing(string FileName)  const;
	bool read_1test_lexicon(string FileName);
	bool adjusting_homonyms_coef(string FileName)  const;
	void print_tag_set() const;
	bool save_lambdas(string FileName) const;
	bool lemmatize_sentence(vector<string> words, vector<string>& lemmas) const;
	bool LoadModelUsingConfig(string FileName, const CLemmatizer* Lemmatizer=0, const CAgramtab* GramTab=0);
    void InitModelFromConfigAndBuildTagset(string FileName, const CLemmatizer* Lemmatizer=0, const CAgramtab* GramTab=0,  bool LoadReverseModel=true);

#ifdef  USE_TRIGRAM_LEMMATIZER
	bool tagging_string_with_end_of_sents(string InputStr, string& Result) ;
	vector<string> get_tokens_from_graphan(string InputStr);
	size_t GetLemmasCount(string Word) const;
	bool print_disamb_lemmas(vector<string> words) const;
	bool lemmatize_file(string FileName) ;
	bool CheckTagsForFormInfo(const vector<CTag>& Tags, const CFormInfo& F) const;
	string GetParticipleLemma(const CFormInfo& F) const;
    bool FindGramTabLineInTags(const vector<CTag>& Tags, size_t Poses, QWORD AllGrammems) const;
#endif

	bool write_dictionary_binary(const map<string, vector<CLexProb> >& Dictionary) const;
    bool read_dictionary_binary();
    bool convert_to_lex_binary() ;
    const CTrigramWord* lookup_word(const string& s) const;
    bool SaveBinary();
    bool ReadBinary();
    bool DisambiguateRusCorpXml(vector<CXmlToken>& Words) const;
    void get_tags_from_annots(const vector<CXmlMorphAnnot>& Annots, set<WORD>& tags, const string& WordStr) const;
    
	bool viterbi(const vector<string>& words, vector<CWordIntepretation>& tags) const;
    void BuildReverseLexProb(const vector<string>& words, vector<map<WORD,prob_t> >& RevLexProbs) const;
	
}; 



#endif
