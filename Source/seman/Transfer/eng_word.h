#pragma once
#include "../SemanLib/sem_word.h"
#include "verb_tense.h"

struct SPosleLog
{
	SPosleLog()
	{
		m_Position = ">>";
	};

	CRossInterp   m_PosleLogPrep;
	std::string        m_PosleLog;
	// домен D_POSITION (<,<<,>>,>,^, "dir_obj _")
	std::string	      m_Position;

};


class CEngSemWord : public CSemWord {
	// время для английской глагольной формы, если узел не является гл. формой, то
	// m_Tense = zero_tn;
	EngVerbTenseEnum	m_Tense;
	std::string m_WordStr; // case sensitive word str
	void fix_case();
public:

	SPosleLog		 m_PosleLog;
	// флаг для синтеза, чтобы он не пытался поставить  слово в форму, указананную в граммемах
	bool            m_bDoNotChangeForm;


	//  использует для порождение всп. глагола be перед основным глаголом и конвертации глагола  в P2
	// вспомогательный  глагол будет добавлен функцией BuildAuxiliaryVerbs
	bool			 m_bMorphologicalPassiveForm;
	bool			 m_bImperative;
	std::string			 m_OneselfStr;



	std::vector<CTenseHistory>  m_TenseHistory;

	void SetTense(EngVerbTenseEnum	NewTense, std::string Maker);
	EngVerbTenseEnum GetTense() const;
	void Init();
	CEngSemWord();
	CEngSemWord(const CSemWord& X);
	// принадлежит ли данная часть речи набору частей речи, который приписан слову?
	bool HasPOS(part_of_speech_t POS) const override;
	const std::string& GetWord() const override;
	void SetWord(const std::string& s, bool set_case=false);
	void AddPostfix(const std::string& suffix);
	void AddPrefix(const std::string& prefix, bool set_case=false);
};
