#ifndef _INCL_WIZARD_H
#define _INCL_WIZARD_H

#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "../common/utilit.h"
#include "../AgramtabLib/agramtab_.h"
#include "FormInfo.h"
#include "OperationMeter.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
const WORD UnknownSessionNo = 0xffff-1;
const WORD UnknownPrefixSetNo = 0xffff-1;
const BYTE UnknownAccent = 0xff;	// не менять - уже проставлено в mrd

// Nick [17/Dec/2003]
const WORD AnyParadigmNo = 0xffff;
const WORD AnyAccentModelNo = 0xffff;
const WORD AnySessionNo = 0xffff;
const WORD AnyPrefixSetNo = 0xffff;
const BYTE AnyAccent = 0xff-1;
extern const char* AnyCommonAncode;


//----------------------------------------------------------------------------
//	CParadigmInfo is a special class, which is used only in Morphwizard 
//----------------------------------------------------------------------------
struct CParadigmInfo : public CLemmaInfo
{
	WORD	m_SessionNo;
	WORD	m_PrefixSetNo;
	BYTE	m_AuxAccent;
	bool	m_bToDelete;

	CParadigmInfo();
	CParadigmInfo(WORD ParadigmNo, WORD AccentModelNo, WORD SessionNo, BYTE AuxAccent, const char* CommonAncode, WORD PrefixSetNo);
	bool operator == (const CParadigmInfo& X) const;

	static CParadigmInfo	AnyParadigmInfo();				// Nick [17/Dec/2003]
	bool	IsAnyEqual( const CParadigmInfo& X ) const;	// Nick [17/Dec/2003]
};


typedef std::multimap<std::string, CParadigmInfo>		LemmaMap;
typedef LemmaMap::iterator lemma_iterator_t;
typedef LemmaMap::const_iterator const_lemma_iterator_t;



//----------------------------------------------------------------------------
struct CPredictSuffix
{
	WORD	m_FlexiaModelNo;
	std::string	m_Suffix;

	//  grammatical code of the lemma
	std::string	m_SourceLemmaAncode;

	//  common gramcode of the lemma
	std::string	m_SourceCommonAncode;

	std::string	m_SourceLemma;
	mutable size_t	m_Frequence;	
	std::string  m_PrefixSetStr;

	bool operator  < (const  CPredictSuffix& X) const 
	{
		if (m_FlexiaModelNo != X.m_FlexiaModelNo)
			return m_FlexiaModelNo < X.m_FlexiaModelNo;

		if (m_SourceLemmaAncode != X.m_SourceLemmaAncode)
			return m_SourceLemmaAncode < X.m_SourceLemmaAncode;

		return m_Suffix < X.m_Suffix;
	};
	bool operator  == (const  CPredictSuffix& X) const 
	{
		return			(m_FlexiaModelNo == X.m_FlexiaModelNo)
					&&	(m_Suffix == X.m_Suffix)
					&&	(m_SourceLemmaAncode == X.m_SourceLemmaAncode);
	};
};

//----------------------------------------------------------------------------
struct CMorphSession
{
	std::string		m_UserName;
	std::string		m_SessionStart;
	std::string		m_LastSessionSave;

	bool operator  == (const  CMorphSession& X) const;
	bool		ReadFromString(const std::string& s);
	std::string		ToString() const;
	void		SetEmpty();
	bool		IsEmpty() const;
};

//----------------------------------------------------------------------------
class MorphWizardMeter;

const int MinPredictSuffixLength = 2;
const int MaxPredictSuffixLength = 5;

typedef std::set<CPredictSuffix> predict_container_t;

struct CDumpParadigm 
{
	std::string			m_TypeGrammemsStr;
	std::string			m_PrefixesStr;
	std::string			m_SlfStr;
	std::string			m_AuthorStr;
	CMorphSession	m_Session;
	int				m_FirstSlfLineNo;


	void	SetEmpty();
	bool	ReadFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors);
	bool	SaveToFile(FILE* fp) const;
	bool	SaveHeaderToFile(FILE* fp) const;
};

//----------------------------------------------------------------------------
class MorphoWizard : public CMorphWizardBase
{
	
	bool			m_bWasChanged;

	StringVector  m_PosesList;
	StringVector  m_GrammemsList;
	StringVector  m_TypeGrammemsList;


	predict_container_t	m_PredictIndex[MaxPredictSuffixLength-MinPredictSuffixLength+1];

	
	StringVector			m_Users;
	size_t					m_SessionNo;
	std::vector<CMorphSession>	m_Sessions;

	friend	class MorphWizardMeter;
	MorphWizardMeter* m_pMeter;	// Nick 30.11.2003

	void load_gramtab();
	void ReadSessions (std::ifstream& mrdFile);
	void ReadLemmas(std::ifstream& mrdFile);
public:
	struct AncodeLess
	{
		const CAgramtab*			m_pGramTab;
		void init(const CAgramtab*			pGramTab);
		bool operator()(const std::string &s1, const std::string &s2)const ;
	};
	AncodeLess ancode_less;



	std::vector<std::set<std::string> >	m_PrefixSets;

	// the multimap from lemma to paradigms (the most largest list)
	LemmaMap				m_LemmaToParadigm;


	// the keys from .mwz file (project file)
	std::map<std::string, std::string>		m_ProjectFileKeys;

	std::string											m_CurrentNewLemma;
	std::vector< predict_container_t::const_iterator>				m_CurrentPredictedParadigms;

	

	//! a table of character properties for regular expressions which depend on CConcIndexator::m_Language
	std::vector<BYTE>			m_PcreCharacterTables;

	bool					m_bLoaded;

	const CAgramtab*			m_pGramTab;
	bool						m_ReadOnly;
	std::string						m_LanguageStr;
	std::string						m_MrdPath;
	bool						m_bFullTrace;
	
	MorphoWizard();
	~MorphoWizard();

	//=================  general: loading, saving, logging ======================
	void	load_wizard(const char *path,const char *user_name, bool bCreatePrediction= true);
	void	load_mrd(bool guest, bool bCreatePrediction);
	bool	load_static(MorphLanguageEnum langua);
	void	load_string_vector(const std::string &name, StringVector &res);
	std::string&	get_value(const std::string &key);
	void	log(const std::string &messg);
	void	log(const std::string &lemm, const CFlexiaModel &p, bool is_added);
	bool	is_changed(){return m_bWasChanged;}
	void	save_mrd();
	std::string	get_lock_file_name()   const;
	std::string	get_log_file_name()   const;
	std::string	get_predict_src_file_path(int mode) const;
	void	MakeReadOnly();
	void	CreatePredictIndex();
	void	pack();
	WORD	GetCurrentSessionNo() const;
	size_t	del_dup_lemm();

	//===============  simple  primitives for ancode  and lemma iterator =========
	std::string					get_pos_string	(const lemma_iterator_t it) const ;
	std::string					get_pos_string	(const std::string &code) const;
	std::string					get_lemm_string	(const_lemma_iterator_t it)   const;
	std::string					get_lemm_string_with_accents	(const_lemma_iterator_t it)   const;
	std::string					get_base_string	(const_lemma_iterator_t it)   const;
	std::string					get_grammem_string  (const std::string &code) const;
	std::string					get_grammem_string  (lemma_iterator_t it)  const;
	QWORD					get_all_lemma_grammems	(const_lemma_iterator_t it) const;
	std::string					get_common_grammems_string(const_lemma_iterator_t it) const;
	std::string					get_prefix_set(const_lemma_iterator_t it) const;
	std::string					get_pos_string_and_grammems	(const std::string &code) const;
	const CMorphSession&	get_session	(int SessionNo) const;
	bool					IsGerman () const  { return m_Language == morphGerman;};
	const StringVector&	get_poses(){return m_PosesList;}
	const StringVector&	get_grammems(){return m_GrammemsList;}
	const StringVector&	get_type_grammems(){return m_TypeGrammemsList;}

	//  ===========   find procedures ================
	void find_lemm_by_grammem(const std::string &pos_and_grammems, std::vector<lemma_iterator_t> &res);
	void find_lemm(std::string lemm, bool bCheckLemmaPrefix, std::vector<lemma_iterator_t> &res);
	void find_lemm_by_user(std::string username, std::vector<lemma_iterator_t> &res);
	void find_wordforms(std::string lemm, std::vector<lemma_iterator_t> &res);
	void find_ancodes(const std::string &ancode, std::vector<lemma_iterator_t> &res);
	void find_lemm_by_prdno(WORD no, std::vector<lemma_iterator_t> &res);
	void find_lemm_by_accent_model(int no, std::vector<lemma_iterator_t> &res);
	
	void find_lemm_by_prd_info( const CParadigmInfo& info, std::vector<lemma_iterator_t> &res);

	//============ Main functions for editing dictionary ====================
	// Mrd -> Slf (Converting from dictionary to text representation)
	std::string	mrd_to_slf(const std::string &lemm, const CFlexiaModel&p, WORD AccentModelNo, BYTE AuxAccent, int line_size) const;
	// Slf -> Mrd (Converting from text to dictionary representation)
	void	slf_to_mrd(const std::string &s, std::string &lemm, CFlexiaModel& FlexiaModel, CAccentModel& AccentModel, BYTE& AuxAccent, int& line_no_err) const;
	void	check_paradigm(long line_no);
	void	remove_lemm(lemma_iterator_t it);
	void	predict_lemm(const std::string &lemm, const int preffer_suf_len, int minimal_frequence, bool bOnlyMainPartOfSpeeches);
	std::string	get_slf_string	(lemma_iterator_t it, std::string &dict, std::string& Prefixes, int line_size = 79);
	void	get_wordforms(const_lemma_iterator_t it, StringVector& forms) const;
	std::string	create_slf_from_predicted(int PredictParadigmNo,  std::string &dict, int line_size = 79) const;
	CParadigmInfo	add_lemma(const std::string &slf, std::string common_grammems, const std::string& prefixes, int& line_no_err, WORD SessionNo = UnknownSessionNo); 
	void	set_to_delete_false();
	void	delete_checked_lemms();
	void	clear_predicted_paradigms();
	bool	change_prd_info(CParadigmInfo& I, const std::string& Lemma, WORD NewParadigmNo, WORD newAccentModelNo, bool keepOldAccents );
	std::string	show_differences_in_two_paradigms(WORD FlexiaModelNo1, WORD FlexiaModelNo2) const;
	
	bool	slf2ancode(const std::string slf_line, std::string& gramcode) const;
	bool	check_common_grammems(std::string common_grammems) const;
	bool	check_prefixes(std::string prefixes) const;
	bool	attach_form_prefixes_to_bases();
	bool	prepare_for_RML();
	bool	prepare_for_RML2();

	bool				HasMeter() const		{ return  !!m_pMeter; }
	MorphWizardMeter*	GetMeter()				{ return  m_pMeter; }
	bool	HasUnknownAccents( lemma_iterator_t it ) const;
	bool	IsPartialAccented( lemma_iterator_t it ) const;
	BYTE	GetLemmaAccent( const_lemma_iterator_t it ) const;
	bool	ReadNextParadigmFromFile(FILE* fp, CDumpParadigm& P, int& line_no, bool& bError, std::string& Errors) const;
	bool	StartSession(std::string user_name);
	void	EndSession();
	std::string  GetUserName() const;
	void	StartLastSessionOfUser(std::string user_name);
	WORD	RegisterSession(const CMorphSession& S);
    bool    Filter(std::string flt_str, std::vector<lemma_iterator_t>& found_paradigms) const;
private:
	BYTE	_GetReverseVowelNo( const std::string& form, WORD accentModelNo, WORD formInd ) const;
	void	SetAccent(WORD AccentModelNo, BYTE AuxAccent, int FormNo, std::string& form) const;
	std::string	get_prefix_set_str(WORD PrefixSetNo) const;
	void	ReadOnePrefixSet(std::string PrefixSet, std::set<std::string>& Result) const;
	void	ReadPrefixSets (std::ifstream& mrdFile);
	WORD	AddPrefixSet(std::string PrefixSetStr);

	
	
};

//----------------------------------------------------------------------------
class MorphWizardMeter: public CFileMeterRML
{
public:
	MorphWizardMeter( MorphoWizard& wizard )	: m_pWizard(&wizard) { wizard.m_pMeter=this; }
	virtual ~MorphWizardMeter()					{ m_pWizard->m_pMeter=NULL; }
private:
	MorphoWizard*	m_pWizard;
};




extern BYTE  TransferReverseVowelNoToCharNo( const std::string& form, BYTE AccentCharNo, MorphLanguageEnum Language);

#endif // _INCL_WIZARD_H
