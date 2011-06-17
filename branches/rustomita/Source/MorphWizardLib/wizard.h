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


typedef multimap<string, CParadigmInfo>		LemmaMap;
typedef LemmaMap::iterator lemma_iterator_t;
typedef LemmaMap::const_iterator const_lemma_iterator_t;



//----------------------------------------------------------------------------
struct CPredictSuffix
{
	WORD	m_FlexiaModelNo;
	string	m_Suffix;

	//  grammatical code of the lemma
	string	m_SourceLemmaAncode;

	//  common gramcode of the lemma
	string	m_SourceCommonAncode;

	string	m_SourceLemma;
	mutable size_t	m_Frequence;	
	string  m_PrefixSetStr;

	bool operator  < (const  CPredictSuffix& X) const 
	{
		if (m_FlexiaModelNo != X.m_FlexiaModelNo)
			return m_FlexiaModelNo < X.m_FlexiaModelNo;

		if (m_SourceLemmaAncode != X.m_SourceLemmaAncode)
			m_SourceLemmaAncode < X.m_SourceLemmaAncode;

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
	string		m_UserName;
	string		m_SessionStart;
	string		m_LastSessionSave;

	bool operator  == (const  CMorphSession& X) const;
	bool		ReadFromString(const string& s);
	string		ToString() const;
	void		SetEmpty();
	bool		IsEmpty() const;
};

//----------------------------------------------------------------------------
class MorphWizardMeter;

const int MinPredictSuffixLength = 2;
const int MaxPredictSuffixLength = 5;

typedef set<CPredictSuffix> predict_container_t;

struct CDumpParadigm 
{
	string			m_TypeGrammemsStr;
	string			m_PrefixesStr;
	string			m_SlfStr;
	string			m_AuthorStr;
	CMorphSession	m_Session;
	int				m_FirstSlfLineNo;


	void	SetEmpty();
	bool	ReadFromFile(FILE* fp, int& line_no, bool& bError, string& Errors);
	bool	SaveToFile(FILE* fp) const;
	bool	SaveHeaderToFile(FILE* fp) const;
};

//----------------------------------------------------------------------------
class MorphoWizard
{
	
	bool			m_bWasChanged;

	StringVector  m_PosesList;
	StringVector  m_GrammemsList;
	StringVector  m_TypeGrammemsList;


	predict_container_t	m_PredictIndex[MaxPredictSuffixLength-MinPredictSuffixLength+1];

	
	StringVector			m_Users;
	size_t					m_SessionNo;
	vector<CMorphSession>	m_Sessions;

	friend	class MorphWizardMeter;
	MorphWizardMeter* m_pMeter;	// Nick 30.11.2003

	void load_gramtab();
	void ReadSessions (FILE* fp);
public:
	struct AncodeLess
	{
		const CAgramtab*			m_pGramTab;
		void init(const CAgramtab*			pGramTab);
		bool operator()(const string &s1, const string &s2)const ;
	};
	AncodeLess ancode_less;

	//  a vector of all Paradigms 
	vector<CFlexiaModel>	m_FlexiaModels;

	// all accent models
	vector<CAccentModel>	m_AccentModels;
	


	vector<set<string> >	m_PrefixSets;

	// the multimap from lemma to paradigms (the most largest list)
	LemmaMap				m_LemmaToParadigm;


	// the keys from .mwz file (project file)
	map<string, string>		m_ProjectFileKeys;

	string											m_CurrentNewLemma;
	vector< predict_container_t::const_iterator>				m_CurrentPredictedParadigms;

	
	MorphLanguageEnum		m_Language;

	//! a table of character properties for regular expressions which depend on CConcIndexator::m_Language
	vector<BYTE>			m_PcreCharacterTables;

	bool					m_bLoaded;

	const CAgramtab*			m_pGramTab;
	bool						m_ReadOnly;
	string						m_LanguageStr;
	string						m_MrdPath;
	bool						m_bFullTrace;
	
	MorphoWizard();
	~MorphoWizard();

	//=================  general: loading, saving, logging ======================
	bool	load_wizard(const char *path,const char *user_name, bool bCreatePrediction= true);
	void	load_mrd(bool guest, bool bCreatePrediction);
	bool	load_static(MorphLanguageEnum langua);
	void	load_string_vector(const string &name, StringVector &res);
	string&	get_value(const string &key);
	void	log(const string &messg);
	void	log(const string &lemm, const CFlexiaModel &p, bool is_added);
	bool	is_changed(){return m_bWasChanged;}
	void	save_mrd();
	string	get_lock_file_name()   const;
	string	get_log_file_name()   const;
	string	get_predict_src_file_path(int mode) const;
	void	MakeReadOnly();
	void	CreatePredictIndex();
	void	pack();
	WORD	GetCurrentSessionNo() const;
	size_t	del_dup_lemm();

	//===============  simple  primitives for ancode  and lemma iterator =========
	string					get_pos_string	(const lemma_iterator_t it) const ;
	string					get_pos_string	(const string &code) const;
	string					get_lemm_string	(const_lemma_iterator_t it)   const;
	string					get_lemm_string_with_accents	(const_lemma_iterator_t it)   const;
	string					get_base_string	(const_lemma_iterator_t it)   const;
	string					get_grammem_string  (const string &code) const;
	string					get_grammem_string  (lemma_iterator_t it)  const;
	QWORD					get_all_lemma_grammems	(const_lemma_iterator_t it) const;
	string					get_common_grammems_string(const_lemma_iterator_t it) const;
	string					get_prefix_set(const_lemma_iterator_t it) const;
	string					get_pos_string_and_grammems	(const string &code) const;
	const CMorphSession&	get_session	(int SessionNo) const;
	bool					IsGerman () const  { return m_Language == morphGerman;};
	const StringVector&	get_poses(){return m_PosesList;}
	const StringVector&	get_grammems(){return m_GrammemsList;}
	const StringVector&	get_type_grammems(){return m_TypeGrammemsList;}

	//  ===========   find procedures ================
	void find_lemm_by_grammem(const string &pos_and_grammems, vector<lemma_iterator_t> &res);
	void find_lemm(string lemm, bool bCheckLemmaPrefix, vector<lemma_iterator_t> &res);
	void find_lemm_by_user(string username, vector<lemma_iterator_t> &res);
	void find_wordforms(string lemm, vector<lemma_iterator_t> &res);
	void find_ancodes(const string &ancode, vector<lemma_iterator_t> &res);
	void find_lemm_by_prdno(WORD no, vector<lemma_iterator_t> &res);
	void find_lemm_by_accent_model(int no, vector<lemma_iterator_t> &res);
	
	void find_lemm_by_prd_info( const CParadigmInfo& info, vector<lemma_iterator_t> &res);

	//============ Main functions for editing dictionary ====================
	// Mrd -> Slf (Converting from dictionary to text representation)
	string	mrd_to_slf(const string &lemm, const CFlexiaModel&p, WORD AccentModelNo, BYTE AuxAccent, int line_size) const;
	// Slf -> Mrd (Converting from text to dictionary representation)
	void	slf_to_mrd(const string &s, string &lemm, CFlexiaModel& FlexiaModel, CAccentModel& AccentModel, BYTE& AuxAccent, int& line_no_err) const;
	void	check_paradigm(long line_no);
	void	remove_lemm(lemma_iterator_t it);
	void	predict_lemm(const string &lemm, const int preffer_suf_len, int minimal_frequence, bool bOnlyMainPartOfSpeeches);
	string	get_slf_string	(lemma_iterator_t it, string &dict, string& Prefixes, int line_size = 79);
	void	get_wordforms(const_lemma_iterator_t it, StringVector& forms) const;
	string	create_slf_from_predicted(int PredictParadigmNo,  string &dict, int line_size = 79) const;
	CParadigmInfo	add_lemma(const string &slf, string common_grammems, const string& prefixes, int& line_no_err, WORD SessionNo = UnknownSessionNo); 
	void	set_to_delete_false();
	void	delete_checked_lemms();
	void	clear_predicted_paradigms();
	bool	change_prd_info(CParadigmInfo& I, const string& Lemma, WORD NewParadigmNo, WORD newAccentModelNo, bool keepOldAccents );
	string	show_differences_in_two_paradigms(WORD FlexiaModelNo1, WORD FlexiaModelNo2) const;
	
	bool	slf2ancode(const string slf_line, string& gramcode) const;
	bool	check_common_grammems(string common_grammems) const;
	bool	check_prefixes(string prefixes) const;
	bool	attach_form_prefixes_to_bases();
	bool	prepare_for_RML();

	bool				HasMeter() const		{ return  !!m_pMeter; }
	MorphWizardMeter*	GetMeter()				{ return  m_pMeter; }
	bool	HasUnknownAccents( lemma_iterator_t it ) const;
	bool	IsPartialAccented( lemma_iterator_t it ) const;
	BYTE	GetLemmaAccent( const_lemma_iterator_t it ) const;
	bool	ReadNextParadigmFromFile(FILE* fp, CDumpParadigm& P, int& line_no, bool& bError, string& Errors) const;
	bool	StartSession(string user_name);
	void	EndSession();
	string  GetUserName() const;
	void	StartLastSessionOfUser(string user_name);
	WORD	RegisterSession(const CMorphSession& S);
private:
	BYTE	_GetReverseVowelNo( const string& form, WORD accentModelNo, WORD formInd ) const;
	void	SetAccent(WORD AccentModelNo, BYTE AuxAccent, int FormNo, string& form) const;
	string	get_prefix_set_str(WORD PrefixSetNo) const;
	void	ReadOnePrefixSet(string PrefixSet, set<string>& Result) const;
	void	ReadPrefixSets (FILE* fp);
	WORD	AddPrefixSet(string PrefixSetStr);

	
	
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




extern BYTE  TransferReverseVowelNoToCharNo( const string& form, BYTE AccentCharNo, MorphLanguageEnum Language);

#endif // _INCL_WIZARD_H
