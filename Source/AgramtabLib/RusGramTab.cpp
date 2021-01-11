// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "RusGramTab.h"
#include "rus_consts.h"


struct TMorphConstant {
	std::string Cyrillic;
	std::string Latin;
};

const static TMorphConstant RussianPartOfSpeech[] = {
	{_R("С"), "N"},  // 0
	{_R("П"), "A"}, // 1
	{_R("Г"), "V"}, // 2
	{_R("МС"),"P"}, // 3
	{_R("МС-П"),"PA"}, // 4
	{_R("МС-ПРЕДК"),"P-PRED"}, // 5
	{_R("ЧИСЛ"), "N"}, // 6
	{_R("ЧИСЛ-П"),"NA"}, // 7
	{_R("Н"),"ADV"}, // 8
	{_R("ПРЕДК"), "PRED"}, //9 
	{_R("ПРЕДЛ"), "PREP"}, // 10
	{_R("ПОСЛ"), "POSL"}, // 11
	{_R("СОЮЗ"), "CONJ"}, // 12
	{_R("МЕЖД"), "INT"}, // 13
	{_R("ВВОДН"), "INP"},// 14
	{_R("ФРАЗ"), "COLLOC"}, // 15
	{_R("ЧАСТ"), "PARTICLE"}, // 16
	{_R("КР_ПРИЛ"), "ADJ_SHORT"},  // 17
	{_R("ПРИЧАСТИЕ"), "PARTICIPLE"}, //18
	{_R("ДЕЕПРИЧАСТИЕ"),"ADV_PARTICIPLE"}, //19
	{_R("КР_ПРИЧАСТИЕ"),"PARTICIPLE_SHORT"}, // 20
	{_R("ИНФИНИТИВ"), "INFINITIVE"}  //21
};

const static TMorphConstant Grammems[] = {
	{_R("мн"), "pl"},
	{_R("ед"), "sg"},
	{_R("им"), "nom"},
	{_R("рд"), "gen"},
	{_R("дт"), "dat"},
	{_R("вн"), "acc"},
	{_R("тв"), "ins"},
	{_R("пр"), "prp"},
	{_R("зв"), "voc"},
	{_R("мр"), "mas"},
	{_R("жр"), "fem"},
	{_R("ср"), "neu"},
	{_R("мр-жр"), "mas-fem"},
	{_R("нст"), "pres"},
	{_R("буд"), "fut"},
	{_R("прш"), "past"},
	{_R("1л"), "1p"},
	{_R("2л"), "2p"},
	{_R("3л"), "3p"},
	{_R("пвл"), "imp"},
	{_R("од"), "anim"},
	{_R("но"), "inanim"},
	{_R("сравн"), "compar"},
	{_R("св"), "perf"},
	{_R("нс"), "imperf"},
	{_R("нп"), "intrans"},
	{_R("пе"), "trans"},
	{_R("дст"), "act"},
	{_R("стр"), "pass"},
	{_R("0"), "0"},
	{_R("аббр"), "abbr"},
	{_R("отч"), "patr"},
	{_R("лок"), "loc"},
	{_R("орг"), "org"},
	{_R("кач"), "qual"},
	{_R("дфст"), "sing_tant"},
	{_R("вопр"), "interrog"},
	{_R("указат"), "demon"},
	{_R("имя"), "name"},
	{_R("фам"), "surname"},
	{_R("безл"), "impers"},
	{_R("жарг"), "jargon"},
	{_R("опч"), "misspell"},
	{_R("разг"), "conv"},
	{_R("притяж"), "poss"},
	{_R("арх"), "arch"},
	{_R("2"), "2"},
	{_R("поэт"), "poet"},
	{_R("проф"), "prof"},
	{_R("прев"), "superl"},
	{_R("полож"), "positive"}
};


std::string rClauseTypes[] =
{
	_R("ГЛ_ЛИЧН"),
	_R("ДПР"),
	_R("КР_ПРЧ"),
	_R("КР_ПРИЛ"),
	_R("ПРЕДК"),
	_R("ПРЧ"),
	_R("ИНФ"),
	_R("ВВОД"),
	_R("ТИРЕ"),
	_R("НСО"),
	_R("СРАВН"),
	_R("КОПУЛ")
};


CRusGramTab::CRusGramTab()
{
	assert(sizeof(Grammems) / sizeof(TMorphConstant) == RussianGrammemsCount);
	assert(sizeof(RussianPartOfSpeech) / sizeof(TMorphConstant) == RUSSIAN_PART_OF_SPEECH_COUNT);
	assert(sizeof(rClauseTypes) / sizeof(std::string) == RUSSIAN_CLAUSE_TYPE_COUNT);

	for (size_t i = 0; i < GetMaxGrmCount(); i++) {
		GetLine(i) = nullptr;
	}
	m_Language = morphRussian;
	m_bUseNationalConstants = true;
}

CRusGramTab :: ~CRusGramTab()
{
	for (size_t i = 0; i < GetMaxGrmCount(); i++) {
		if (GetLine(i) != NULL)
			delete GetLine(i);
	}

}
const char* CRusGramTab::GetRegistryString() const
{
	return "Software\\Dialing\\Lemmatizer\\Russian\\Rgramtab";
};


part_of_speech_t	CRusGramTab::GetPartOfSpeechesCount() const
{
	return RUSSIAN_PART_OF_SPEECH_COUNT;
};

size_t CRusGramTab::GetMaxGrmCount() const
{
	return rMaxGrmCount;
}

const char* CRusGramTab::GetPartOfSpeechStr(part_of_speech_t i) const
{
	if (m_bUseNationalConstants) {
		return RussianPartOfSpeech[i].Cyrillic.c_str();
	}
	else {
		return RussianPartOfSpeech[i].Latin.c_str();
	}
};

size_t	CRusGramTab::GetGrammemsCount()  const
{
	return RussianGrammemsCount;
};

const char* CRusGramTab::GetGrammemStr(size_t i) const
{
	if (m_bUseNationalConstants) {
		return Grammems[i].Cyrillic.c_str();
	}
	else {
		return Grammems[i].Latin.c_str();
	}
};

CAgramtabLine*& CRusGramTab::GetLine(size_t LineNo)
{
	return Lines[LineNo];
}

const CAgramtabLine* CRusGramTab::GetLine(size_t LineNo) const
{
	return Lines[LineNo];
}

size_t CRusGramTab::GramcodeToLineIndex(const char* s)  const
{
	return  (unsigned char)s[0] * 0x100 + (unsigned char)s[1] - rStartUp;
};

std::string CRusGramTab::LineIndexToGramcode(uint16_t i) const
{
	i += rStartUp;
	char res[3];

	res[0] = (i >> 8);
	res[1] = (0xFF & i);
	res[2] = 0;
	return  res;
};


bool CRusGramTab::ProcessPOSAndGrammems(const char* tab_str, part_of_speech_t& PartOfSpeech, grammems_mask_t& grammems) const
{
	if (!CAgramtab::ProcessPOSAndGrammems(tab_str, PartOfSpeech, grammems)) return false;



	// неизменяемые слова как будто принадлежат всем падежам
	if ((_QM(rIndeclinable) & grammems)
		&& (PartOfSpeech != PREDK)
		)
		grammems |= rAllCases;

	if ((_QM(rIndeclinable) & grammems)
		&& (PartOfSpeech == PRONOUN_P)
		)
		grammems |= rAllGenders | rAllNumbers;


	// слова общего рода ('сирота') могут  использованы как 
	// слова м.р., так и как слова ж.р.
	if (_QM(rMascFem) & grammems)
		grammems |= _QM(rMasculinum) | _QM(rFeminum);

	// слово 'пальто' не изменяется по числам, поэтому может
	// быть использовано в обоих числах
	if (PartOfSpeech != PREDK)
		if ((_QM(rIndeclinable) & grammems) && !(_QM(rSingular) & grammems))
			grammems |= _QM(rPlural) | _QM(rSingular);

	return true;
};

// Стандартное согласование между двумя именами  по  падежу, причем первый код должен
// иметь граммему множественного числа 
inline bool CaseFirstPlural(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	return (((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0)
		&& (((1 << rPlural) & l1->m_Grammems) > 0)
		);
};


// Стандартное согласование между двумя именами  по роду и числу 
inline bool GenderNumber(const CAgramtabLine* l1, const CAgramtabLine* l2)
{

	return 	((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0)
		&& (((l1->m_Grammems & l2->m_Grammems & _QM(rPlural)) > 0)
			|| ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0)
			);
}

// Стандартное согласование по лицу и числу (для 1 и 2 лица)
inline bool PersonNumber(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	size_t t1 = (rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0;
	size_t t2 = ((rAllPersons & l1->m_Grammems & l2->m_Grammems) > 0);
	return t1 && t2;
}

// Стандартное согласование между подлежащим и сказуемым
inline bool SubjectPredicate(const CAgramtabLine* subj_l, const CAgramtabLine* verb_l)
{
	const grammems_mask_t& subj = subj_l->m_Grammems;
	const grammems_mask_t& verb = verb_l->m_Grammems;

	if (!(subj & _QM(rNominativ))
		)
		return false;


	if (((verb & _QM(rPastTense)) > 0)
		|| (verb_l->m_PartOfSpeech == ADJ_SHORT)
		|| (verb_l->m_PartOfSpeech == PARTICIPLE_SHORT)
		)
	{
		// ты вышел 
		// я вышел
		// ты был
		// я красива
		// мы шли
		// вы шли
		if (subj & (_QM(rFirstPerson) | _QM(rSecondPerson)))
			return		(verb & subj & _QM(rPlural))
			|| ((verb & (_QM(rMasculinum) | _QM(rFeminum)))
				&& (verb & subj & _QM(rSingular))
				);
		else
			// он вышел
			// поезд ушел
			// девочка красива
			// девочки красивы 	
			// мальчик красив
			return GenderNumber(subj_l, verb_l);
	}
	else
		if ((verb & _QM(rPresentTense))
			|| (verb & _QM(rFutureTense)))
		{
			//  я выйду
			//  ты выедешь
			//  мы выйдем
			if ((subj & (_QM(rFirstPerson) | _QM(rSecondPerson)))
				|| (verb & (_QM(rFirstPerson) | _QM(rSecondPerson))))
				return PersonNumber(subj_l, verb_l);
			else
				return (rAllNumbers & subj & verb) > 0;
		}
		else
			if (verb & _QM(rImperative))
				return		((subj & _QM(rSecondPerson)) > 0)
				&& ((rAllNumbers & subj & verb) > 0);

	return false;
}


// Стандартное согласование между двумя именами  по  падежу 
inline bool Case(const CAgramtabLine* l1, const CAgramtabLine* l2)
{

	return 	((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0);
}


// Стандартное согласование между существительным и прилашательнымпо роду, числу и падежу, если 
// если существительное одушевленное
bool GenderNumberCaseAnimRussian(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	const grammems_mask_t& gram_noun = l1->m_Grammems;
	const grammems_mask_t& gram_adj = l2->m_Grammems;

	return			((rAllCases & gram_noun & gram_adj) > 0)
		&& ((rAllNumbers & gram_noun & gram_adj) > 0)
		&& (((_QM(rAnimative) & gram_adj) > 0)
			|| ((rAllAnimative & gram_adj) == 0)
			)
		&& (((rAllGenders & gram_noun & gram_adj) > 0)
			|| ((rAllGenders & gram_noun) == 0)
			|| ((rAllGenders & gram_adj) == 0)
			);
};

// Стандартное согласование между существительным и прилашательнымпо роду, числу и падежу, если 
// если существительное неодушевленное
bool GenderNumberCaseNotAnimRussian(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	const grammems_mask_t& gram_noun = l1->m_Grammems;
	const grammems_mask_t& gram_adj = l2->m_Grammems;

	return			((rAllCases & gram_noun & gram_adj) > 0)
		&& ((rAllNumbers & gram_noun & gram_adj) > 0)
		&& (((_QM(rNonAnimative) & gram_adj) > 0)
			|| ((rAllAnimative & gram_adj) == 0)
			)
		&& (((rAllGenders & gram_noun & gram_adj) > 0)
			|| ((rAllGenders & gram_noun) == 0)
			|| ((rAllGenders & gram_adj) == 0)
			);
};

// Стандартное согласование между существительным и прилашательнымпо роду, числу и падежу, если 
// если существительное не неодушевленное и не одушевленное
//  (для местоимений, например, "все это было хорошо")
bool GenderNumberCaseRussian(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	const grammems_mask_t& gram_noun = l1->m_Grammems;
	const grammems_mask_t& gram_adj = l2->m_Grammems;

	return			((rAllCases & gram_noun & gram_adj) > 0)
		&& ((rAllNumbers & gram_noun & gram_adj) > 0)
		&& (((rAllGenders & gram_noun & gram_adj) > 0)
			|| ((rAllGenders & gram_noun) == 0)
			|| ((rAllGenders & gram_adj) == 0)
			);
};

bool FiniteFormCoordRussian(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	const grammems_mask_t& verb1 = l1->m_Grammems;
	const grammems_mask_t& verb2 = l2->m_Grammems;

	// жил и был
	if (verb1 & verb2 & rAllNumbers)
		if (verb1 & verb2 & _QM(rPastTense))
		{

			if ((verb1 & verb2 & rAllGenders)
				|| ((verb1 & verb2 & rAllNumbers) == _QM(rPlural))
				)
				return true;
		}
		else
			if (verb1 & verb2 & rAllPersons)
				return true;

	return false;

};

/*
Примеры работы функции GleicheGenderNumberCase:
- красивого стол;
- красивых столы;
- красивый пирата;
- красивые пиратов;
- достойных жизни;
- достойные матерей;
- длинные чудовищ
+ на этот дом;
+ красивую маму;
+ в красивый  дом;
+ в длинное облако;
+ примерить его цилиндр;
+ все это
*/
grammems_mask_t CRusGramTab::GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const
{
	if ((common_gram_code_noun == 0)
		|| !strcmp(common_gram_code_noun, "??")
		|| (strlen(common_gram_code_noun) == 0)
		)
		// нет сведений об одушевленности
		return Gleiche(GenderNumberCaseRussian, gram_code_noun, gram_code_adj);
	else
		if ((GetLine(GramcodeToLineIndex(common_gram_code_noun))->m_Grammems & _QM(rNonAnimative)) > 0)
			// неодушевленный
			return Gleiche(GenderNumberCaseNotAnimRussian, gram_code_noun, gram_code_adj);
		else
			if ((GetLine(GramcodeToLineIndex(common_gram_code_noun))->m_Grammems & _QM(rAnimative)) > 0)
				// одушевленный
				return Gleiche(GenderNumberCaseAnimRussian, gram_code_noun, gram_code_adj);
			else
				// нет сведений об одушевленности
				return Gleiche(GenderNumberCaseRussian, gram_code_noun, gram_code_adj);

}



bool CRusGramTab::GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(CaseNumber, gram_code1, gram_code2) != 0;
}

bool CRusGramTab::GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(GenderNumber, gram_code1, gram_code2) != 0;
}
//with absent grammems check, less strict than GleicheGenderNumber
bool CRusGramTab::ConflictGenderNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(GenderNumber0, gram_code1, gram_code2) == 0;
}
bool CRusGramTab::ConflictGrammems(grammems_mask_t g1, grammems_mask_t g2, grammems_mask_t breaks) const
{
	grammems_mask_t BR[] = { rAllCases, rAllNumbers, rAllGenders };
	bool R = true;
	for (int i = 0; i < (sizeof BR) / (sizeof BR[0]) && R; i++)
	{
		if (breaks & BR[i])
			R &= ((BR[i] & g1 & g2) > 0 || !(BR[i] & g1) || !(BR[i] & g2));
	}
	return  !R;
}

bool CRusGramTab::GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(SubjectPredicate, gram_code1, gram_code2) != 0;
}

bool CRusGramTab::GleicheCase(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(Case, gram_code1, gram_code2) != 0;
}


const size_t CRusGramTab::GetClauseTypesCount() const
{
	return RUSSIAN_CLAUSE_TYPE_COUNT;
};

long CRusGramTab::GetClauseTypeByName(const char* TypeName) const
{
	for (int i = 0; i < RUSSIAN_CLAUSE_TYPE_COUNT; i++)
	{
		if (rClauseTypes[i] == TypeName)
		{
			return i;
		}
	}

	return -1;
}


const char* CRusGramTab::GetClauseNameByType(long type) const
{
	if (type >= RUSSIAN_CLAUSE_TYPE_COUNT) return 0;
	if (type < 0) return 0;

	return rClauseTypes[type].c_str();
}

/*
	истина для предикативных типов клауз.
*/
bool CRusGramTab::IsStrongClauseRoot(const part_of_speech_mask_t poses) const
{
	return		(poses & (1 << VERB))
		|| (poses & (1 << ADVERB_PARTICIPLE)) // субъект деепричастия  совпадлает с субъектом
											 // главной  клаузы 
		|| (poses & (1 << PARTICIPLE_SHORT))
		|| (poses & (1 << ADJ_SHORT))
		|| (poses & (1 << PREDK));
};

const static std::string months[] = { _R("ЯНВАРЬ"),_R("ФЕВРАЛЬ"),_R("МАРТ"),
			   _R("АПРЕЛЬ"),_R("МАЙ"),_R("ИЮНЬ"),
			   _R("ИЮЛЬ"),_R("АВГУСТ"),_R("СЕНТЯБРЬ")
			   ,_R("ОКТЯБРЬ"),_R("НОЯБРЬ"),_R("ДЕКАБРЬ") };

bool CRusGramTab::is_month(const char* lemma) const
{
	if (!lemma) return false;
	for (size_t i = 0; i < 12; i++)
		if (months[i] == lemma)
			return true;
	return false;
}

const static std::string SmallNumbers[] = { _R("ДВА"),_R("ТРИ"),_R("ЧЕТЫРЕ"),_R("ОБА"),_R("ПОЛТОРА") };

bool CRusGramTab::is_small_number(const char* lemma) const
{
	if (!lemma) return false;
	for (size_t i = 0; i < 5; i++)
		if (SmallNumbers[i].compare(lemma) == 0)
			return true;
	return false;
}


bool CRusGramTab::IsMorphNoun(part_of_speech_mask_t poses)  const
{
	return  (poses & (1 << NOUN));
};


bool CRusGramTab::is_morph_adj(part_of_speech_mask_t poses) const
{
	return		(poses & (1 << ADJ_FULL))
		|| (poses & (1 << ADJ_SHORT));
};

bool CRusGramTab::is_morph_participle(part_of_speech_mask_t poses) const
{
	return  (poses & (1 << PARTICIPLE))
		|| (poses & (1 << PARTICIPLE_SHORT));
};

bool CRusGramTab::is_morph_pronoun(part_of_speech_mask_t poses) const
{
	return  (poses & (1 << PRONOUN)) != 0;
};


bool CRusGramTab::is_morph_pronoun_adjective(part_of_speech_mask_t poses) const
{
	return  (poses & (1 << PRONOUN_P)) != 0;
};


bool CRusGramTab::is_left_noun_modifier(part_of_speech_mask_t poses, grammems_mask_t grammems) const
{
	return     (poses & (1 << ADJ_FULL))
		|| (poses & (1 << NUMERAL_P))
		|| (poses & (1 << PRONOUN_P))
		|| (poses & (1 << PARTICIPLE));
}


bool CRusGramTab::is_numeral(part_of_speech_mask_t poses) const
{
	return  (poses & (1 << NUMERAL)) != 0;
};

bool CRusGramTab::is_verb_form(part_of_speech_mask_t poses) const
{
	return     is_morph_participle(poses)
		|| (poses & (1 << VERB))
		|| (poses & (1 << INFINITIVE))
		|| (poses & (1 << ADVERB_PARTICIPLE));
};



bool CRusGramTab::is_infinitive(part_of_speech_mask_t poses) const
{
	return (poses & (1 << INFINITIVE)) != 0;
}

bool CRusGramTab::is_morph_predk(part_of_speech_mask_t poses) const
{
	return (poses & (1 << PREDK)) ||
		(poses & (1 << PRONOUN_PREDK));
}

bool CRusGramTab::is_morph_adv(part_of_speech_mask_t poses) const
{
	return (poses & (1 << ADV)) != 0;
}


bool CRusGramTab::is_morph_personal_pronoun(part_of_speech_mask_t poses, grammems_mask_t grammems) const
{
	return		 (poses & (1 << PRONOUN)) != 0
		&& (grammems & (_QM(rFirstPerson) | _QM(rSecondPerson) | _QM(rThirdPerson)));
};



const size_t  ParticleCount = 8;
const static std::string Particles[ParticleCount] = { _R("ЛИ"),_R("ЖЕ"),_R("БЫ"),_R("УЖ"),_R("ТОЛЬКО"), _R("Ж"), _R("Б"), _R("ЛЬ") };

bool CRusGramTab::IsSimpleParticle(const char* lemma, part_of_speech_mask_t poses) const
{
	if (!lemma) return false;
	if (!(poses & (1 << PARTICLE))) return false;
	for (int i = 0; i < ParticleCount; i++)
		if (Particles[i] == lemma)
			return true;

	return false;
}









/*
	Есть такие  мест. прилагательные, которые могут выступать в роли существительных:
	_R("КАЖДЫЙ"), _R("ОДИН"), _R("ДРУГОЙ"),_R("ТОТ"),_R("КОТОРЫЙ").
	Для  них не имеет смысла вводит аналогичные местоимения,поскольку  все они могут
	быть использованы во всех родах. Эти прилагательные могут вести себя так же, как
	существительные. Например,
	Я купил два яблока, в каждом оказалось много червяков
	Я знаю ту, которая тебя видит
	Один пришел, другой ушел.
	Я вижу дом, который разрушился.
	Вышеуказанные мест. прилагательные отличаются, например, от местоимения _R(_R("этот")) и _R("всякий"), поскольку
	трудно себе представить что-то вроде:
	"эта пришла"
	"всякая пришла"
	Но возможно:
	"всякое бывает"
	"это бывает"
	Здесь _R("всякое") и _R("это") - простые местоимения ср рода.

	!! Таким образом, я не хочу дублировать в морфологии МС-П, если они могут использованы
	!! во всех родах в качестве существительного.

	1 марта 2001 года, Сокирко
*/
bool CRusGramTab::IsSynNoun(part_of_speech_mask_t poses, const char* lemma) const
{
	return   IsMorphNoun(poses)
		|| (poses & (1 << PRONOUN))
		|| ((poses & (1 << PRONOUN_P))
			&& (_R("КАЖДЫЙ") == lemma)
			|| (_R("ОДИН") == lemma)
			|| (_R("ДРУГОЙ") == lemma)
			|| (_R("ТОТ") == lemma)
			|| (_R("КОТОРЫЙ") == lemma)
			)
		;
};


const int StandardParamAbbrCount = 8;
const static std::string StandardParamAbbr[StandardParamAbbrCount] =
{ _R("КГ"), _R("КМ"), _R("СМ"),_R("МЛ"), _R("МБ"),_R("КБ"), _R("МГЦ"), _R("КВ") };

bool CRusGramTab::IsStandardParamAbbr(const char* WordStrUpper) const
{
	if (strlen(WordStrUpper) > 4) return false;
	for (long i = 0; i < StandardParamAbbrCount; i++)
		if (StandardParamAbbr[i] == WordStrUpper)
			return true;

	return false;
}


bool CRusGramTab::is_morph_article(part_of_speech_mask_t poses)  const
{
	return  false;
};

bool CRusGramTab::FilterNounNumeral(std::string& gcNoun, const std::string& gcNum, grammems_mask_t& grammems) const
{
	if (gcNoun.length() == 2 || !(grammems & rAllCases)) return false;
	GleicheAncode1(CaseNumberGender0, gcNum,
		FilterGramCodes(gcNum, grammems & rAllCases | ~rAllCases, 0), gcNoun);
	return true;
}


std::string RussianCaseNumberGender(const CAgramtab* pGramTab, const std::string& adj, const std::string& common_noun_grm, const std::string& noun)
{
	assert(false);
	//return pGramTab->GleicheGenderNumberCase(common_noun_grm.c_str(), noun.c_str(), adj.c_str());
	return "";
};

grammems_mask_t CRusGramTab::ChangeGleicheAncode1(GrammemCompare CompareFunc, const std::string& wordGramCodes, std::string& groupGramCodes, const grammems_mask_t wordGrammems) const
{
	groupGramCodes = GleicheAncode1(CompareFunc, FilterGramCodes(wordGramCodes, ~_QM(rIndeclinable), 0), groupGramCodes);
	if (groupGramCodes == "") { return 0; }
	const grammems_mask_t gramFilter = rAllCases | rAllGenders | rAllTimes | rAllPersons | rAllAnimative;
	return 	wordGrammems & ~(gramFilter) | GetAllGrammems(groupGramCodes.c_str());
}


bool CaseNumber(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	return ((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0) &&
		((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0);
};
bool CaseGender(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	return ((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0) &&
		((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0);
};

bool CaseNumberGender(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	return ((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0) &&
		((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0) &&
		((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0);
};

bool CaseNumberGender0(const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
{
	return ((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllCases & l1->m_Grammems) || !(rAllCases & l2->m_Grammems)) &&
		((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems)) &&
		((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllGenders & l1->m_Grammems) || !(rAllGenders & l2->m_Grammems)); ;
};
bool GenderNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
{
	return ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllGenders & l1->m_Grammems) || !(rAllGenders & l2->m_Grammems)) &&
		((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems));
};

bool CaseNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
{
	return ((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllCases & l1->m_Grammems) || !(rAllCases & l2->m_Grammems)) &&
		((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems)); ;
};
