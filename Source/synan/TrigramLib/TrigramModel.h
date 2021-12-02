#ifndef TrigramModel_h
#define TrigramModel_h


#include "morph_dict/common/util_classes.h"
#include "list"
#include "limits.h"

#ifdef  USE_TRIGRAM_LEMMATIZER
#include "TagSet.h"
#include "common/PlmLine.h"
#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/LemmatizerBaseLib/Lemmatizers.h"
#endif

#include "common/MorphXmlToken.h"


#include <float.h> 


typedef float prob_t;
#define MAXPROB FLT_MAX

const uint16_t UnknownTag = 0xffff;

typedef uint64_t trigram_integer_t;
//typedef int trigram_integer_t;

struct CLexProb 
{
	uint16_t	m_PrevTag;
	uint16_t	m_Tag;
	uint32_t	m_Count;
	CLexProb()
	{
		m_Tag = UnknownTag;
		m_PrevTag = UnknownTag;
		m_Count = 0;
	};
	CLexProb(uint16_t PrevTag, uint16_t Tag, uint32_t Count)
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
    std::string m_WordStr; 
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
	uint16_t	m_TagId1;
	uint16_t	m_TagId2;
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
	std::set<uint16_t>		        m_PossibleWordTags;
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
	
    std::map<std::pair<uint16_t,uint16_t>, prob_t>			m_Probs;
	std::map<std::pair<uint16_t,uint16_t>, uint16_t>			m_TagRefs;
	std::pair<prob_t, uint16_t> 		m_FirstMaximum;
	std::pair<prob_t, uint16_t> 		m_SecondMaximum;

public:
	std::set<uint16_t>						m_LexicalProbs;
    
    
	

	CViterbiInfo ();
    void SetProb(uint16_t i,  uint16_t j, prob_t  value);
	prob_t GetProb(uint16_t i,  uint16_t j) const;
	void SetTagRef(uint16_t i,  uint16_t j, uint16_t TagRef);
    uint16_t GetTagRef(uint16_t i,  uint16_t j) const;
	const std::map<std::pair<uint16_t,uint16_t>, prob_t>&  GetAllProbs() const;
    void  UpdateMaximum(prob_t prob, uint16_t tag);
    bool  IsAmbig()  const;
	std::pair<prob_t, uint16_t>  GetFirstMaximum()  const;
	std::pair<prob_t, uint16_t>  GetSecondMaximum()  const;
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
		uint16_t i;
		uint16_t j;
		int  m_TrigramsCount;
	};
	typedef std::map<int, std::vector< CSingleHistory >  >  PMap;
	typedef std::map<size_t, prob_t>  BMap;

    // serializable
	uint16_t					m_TagsCount; // == m_RegisteredTags.size()
	std::vector<int>			m_Unigrams;      /* uni-, bi- and trigram counts */
	std::vector<uint16_t>		m_TagsOrderedByUnigrams;      /* uni-, bi- and trigram counts */
	std::vector<int>			m_Bigrams;      /* uni-, bi- and trigram counts */
	std::map<trigram_integer_t,int>		m_Trigrams;
    int					m_TypeCounts[3];        /* uni-, bi- and trigram type counts */
	int					m_TokenCounts[3];       /*	m_TokenCounts[0] - число слов во входном массиве  
									m_TokenCounts[1] - число биграмм во входном массиве (NB! деление на предложения )
									m_TokenCounts[2] - число триграмм во входном массиве (NB! деление на предложения )
								*/
    mutable std::map<std::string, const std::vector<CXmlMorphAnnot>*> m_CurrentSentenceWords2Annots;
    std::vector<CLexProb >   m_LexProbs;
    std::vector<CTrigramWord> m_Dictionary; 
    // ... 
	

	mutable	size_t                          m_CachedSmoothedSize;	
	mutable std::vector< std::map<size_t, prob_t>  >  m_CachedSmoothedTransProb;         /* smoothed transition probs */
	bool					                m_bHasSmallTagset;
	CTransProbForSmallTagSet*               m_pSmoothedProbsForSmallTagsets;

	
	std::vector<int>			m_Buckets;
	std::vector< CLambdas >	m_BucketsLambdas;
	bool				m_bDebugViterbi;
	std::set<std::string>			m_TestLexicon;

	
	

	
	void	set_ngram(size_t n, uint16_t t1, uint16_t t2, uint16_t t3, int value);
	int ngram_index(uint16_t t1) const;
	int ngram_index(uint16_t t1, uint16_t t2) const;
	trigram_integer_t ngram_index(uint16_t t1, uint16_t t2, uint16_t t3) const;

	uint16_t find_tag(const std::string &t) const; 
	uint16_t register_tag(const std::string& t);
	bool CheckLemma(std::string Word, std::string TagStr1, std::string TagStr2) const;
	int				compute_bucket_denominator(uint16_t i, uint16_t j, int& TrigramsCount);
	CLambdas		compute_lambdas_for_one_bucket(PMap::const_iterator start_it, PMap::const_iterator end_it);
	prob_t			GetSmoothedProb(uint16_t PrevPrevTag, uint16_t PrevTag, uint16_t CurrTag) const;
	void			ViterbiForward(const std::vector<std::string>& words, std::vector<CViterbiInfo>& Triplet, const std::vector<CViterbiInfo>& RevTriplet) const;
	void			ViterbiBackward(const std::vector<std::string>& words, const std::vector<CViterbiInfo>& Triplet, std::vector<CWordIntepretation>& tags) const;
	size_t			GetTrigram(uint16_t t1, uint16_t t2, uint16_t t3)  const;
	void			get_tags_from_lemmatizer_but_not_preps(const std::string& WordStr, std::set<uint16_t>& tags) const;
	CDictionarySearch		find_word(const std::string& WordStr) const;
	prob_t					GetSmoothedLexProb(const CDictionarySearch& DS, uint16_t PrevTag, uint16_t CurrTag) const;
	bool					LoadSynanForThisSentence(const std::vector<std::string>& words) const;
	std::set<std::string>				GetLemmaSetByTagAndWordStr(std::string Word, std::string TagStr, bool bOnlyWithMaxWeight) const;
	
	prob_t					BuildSmoothedProb(uint16_t PrevPrevTag, uint16_t PrevTag, uint16_t CurrTag) const;
	bool					TagRawTexts(std::string FileName);
	bool					TagRawText(std::string FileName);
	
    
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
	std::vector<std::string>			m_RegisteredTags;      /* tags */
	bool				m_bUseSecondLocalMax;
    bool				m_bReverseModel;
	MorphLanguageEnum	m_Language;
	
	size_t				m_MinBucketSize;
	size_t				m_SecondLocalCoef;
	bool				m_bCheckOnlyAmbiguosWords;
	
	bool				m_bRawTexts;
    std::string              m_DictionaryFile;
    std::string              m_NgramFile;
	

	CTrigramModel ();
	~CTrigramModel ();

	bool InitDicts ();
	bool read_ngram_file();
    bool register_tags_from_ngram_file();
	void compute_counts_for_boundary();
	void enter_rare_word_tag_counts(void *key, CTrigramWord& wd,  void *d2);
	void compute_lambdas();
	void load_lambdas(std::string FileName);
	
	bool compute_bucketed_lambdas();
	void compute_transition_probs();
    bool read_dictionary_text_file(std::map<std::string,  std::vector<CLexProb> >& Dictionary) const;
	
	void dump_transition_probs();
	bool tag_sentence(char* SentenceStr) const;
	bool tagging(std::string FileName)  ;
	bool testing(std::string FileName)  const;
	bool read_1test_lexicon(std::string FileName);
	bool adjusting_homonyms_coef(std::string FileName)  const;
	void print_tag_set() const;
	bool save_lambdas(std::string FileName) const;
	bool lemmatize_sentence(std::vector<std::string> words, std::vector<std::string>& lemmas) const;
    void InitModelFromConfigAndBuildTagset(std::string FileName, const CLemmatizer* Lemmatizer=0, const CAgramtab* GramTab=0,  bool LoadReverseModel=true);

#ifdef  USE_TRIGRAM_LEMMATIZER
	bool tagging_string_with_end_of_sents(std::string InputStr, std::string& Result) ;
	std::vector<std::string> get_tokens_from_graphan(std::string InputStr);
	size_t GetLemmasCount(std::string Word) const;
	bool print_disamb_lemmas(std::vector<std::string> words) const;
	bool lemmatize_file(std::string FileName) ;
	bool CheckTagsForFormInfo(const std::vector<CTag>& Tags, const CFormInfo& F) const;
	std::string GetParticipleLemma(const CFormInfo& F) const;
    bool FindGramTabLineInTags(const std::vector<CTag>& Tags, part_of_speech_mask_t Poses, uint64_t AllGrammems) const;
#endif

	bool write_dictionary_binary(const std::map<std::string, std::vector<CLexProb> >& Dictionary) const;
    bool read_dictionary_binary();
    bool convert_to_lex_binary() ;
    const CTrigramWord* lookup_word(const std::string& s) const;
    bool SaveBinary();
    bool ReadBinary();
    bool DisambiguateRusCorpXml(std::vector<CXmlToken>& Words) const;
    void get_tags_from_annots(const std::vector<CXmlMorphAnnot>& Annots, std::set<uint16_t>& tags, const std::string& WordStr) const;
    
	bool viterbi(const std::vector<std::string>& words, std::vector<CWordIntepretation>& tags) const;
    void BuildReverseLexProb(const std::vector<std::string>& words, std::vector<std::map<uint16_t,prob_t> >& RevLexProbs) const;
	
}; 



#endif
