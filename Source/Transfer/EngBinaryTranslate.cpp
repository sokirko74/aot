
#include "StdAfx.h"
#include "EngBinaryTranslate.h"
#include "../BinaryDictsLib/SetOfWordPairs.h"
#include "../AprRead/all_labels.h"


int find_label(const std::string& label)
{
	for(int i = 0;  i < GetTransLabelsCount(); i++){
		if (GetTransLabels()[i].str == label) {
			return i;
		}
	}
	return -1;
}


translate_helper::translate_helper()
{
	flush();
	
}

void translate_helper::init_private_lists()
{
	m_bad_flags_arr.clear();
	m_bad_flags_arr.push_back(find_label(_R("сл")));
	m_bad_flags_arr.push_back(find_label(_R("бран")));
	m_bad_flags_arr.push_back(find_label(_R("вульг")));
	m_bad_flags_arr.push_back(find_label(_R("груб")));
	m_bad_flags_arr.push_back(find_label(_R("жарг")));
	m_bad_flags_arr.push_back(find_label(_R("ирон")));
	m_bad_flags_arr.push_back(find_label(_R("неприст")));
	m_bad_flags_arr.push_back(find_label(_R("презр")));
	m_bad_flags_arr.push_back(find_label(_R("прост")));
	m_bad_flags_arr.push_back(find_label(_R("редк")));
	m_bad_flags_arr.push_back(find_label(_R("разг")));
	m_bad_flags_arr.push_back(find_label(_R("уст")));
	m_bad_flags_arr.push_back(find_label(_R("фольк")));
    sort(m_bad_flags_arr.begin(), m_bad_flags_arr.end());

	init_list_from_ross(GetRossHolder(Aoss), "_an_article_exception" , except_vec);
};

void translate_helper::SetData(const CSemanticsHolder* pData)
{
	m_pData = pData;
}


const CRossHolder* translate_helper::GetRossHolder (DictTypeEnum type) const
{
	return m_pData->GetRossHolder(type);
}



const CLemmatizer* translate_helper::GetRusLemmatizer () const
{
	return m_pData->GetRusLemmatizer();
}

const CLemmatizer* translate_helper::GetEngLemmatizer () const
{
	return m_pData->GetEngLemmatizer();
}
const CLemmatizer* translate_helper::GetLemmatizer (MorphLanguageEnum langua) const
{
	return m_pData->GetLemmatizer(langua);
}
const CRusGramTab* translate_helper::GetRusGramTab () const
{
	return m_pData->GetRusGramTab();
}

const CEngGramTab* translate_helper::GetEngGramTab () const
{
	return m_pData->GetEngGramTab();
}

const CAgramtab* translate_helper::GetGramTab (MorphLanguageEnum langua) const
{
	return m_pData->GetGramTab(langua);
}





// создаем форму по ParadigmId и eng_grammems, если eng_grammems == 0, возвращает лемму
std::string translate_helper::create_form_by_id(long Id, uint64_t eng_grammems) const
{
	int j;
	CFormInfo Paradigm;
	bool bRes = GetEngLemmatizer()->CreateParadigmFromID(Id, Paradigm);
	assert (bRes);


	int count = Paradigm.GetCount();
	rml_TRACE("lemma %s try to find a word form with grammems %s\n", Paradigm.GetSrcNorm().c_str(), GetEngGramTab()->GrammemsToStr(eng_grammems).c_str());
	for(j = 0; j < count; j++)
	{
		UINT POS =  GetEngGramTab()->GetPartOfSpeech(Paradigm.GetAncode(j).c_str());
		if(GetEngGramTab()->FindGrammems(Paradigm.GetAncode(j).c_str(), eng_grammems))
			break;
	}
	if(j == count) j = 0;
	std::string res = Paradigm.GetWordForm(j);
	EngRusMakeLower(res);

	return res;
}



// создаем возвращает лемму по RusId
std::string translate_helper::create_norm_by_id(long EngId) const
{
	if (EngId ==-1) return "";
	CFormInfo p;
	bool bRes =  GetEngLemmatizer()->CreateParadigmFromID(EngId, p);
	if (bRes && p.GetCount() > 0)
		return p.GetWordForm(0);
	else
		return "";
}




// устанавливает  нужный регистр
std::string fix_case(const CEngSemWord& EngWord)
{
	std::string s = EngWord.m_Word;
	if(s.empty()) return s;
	if (!EngWord.m_bDoNotChangeForm)
	{
		EngRusMakeLower(s);
		if(  (    (EngWord.m_CharCase == UpLow)
				|| EngWord.HasOneGrammem(eProper)
			 )
		   )
		s[0] = toupper((unsigned char)s[0]);
		if(EngWord.m_CharCase == UpUp) EngRusMakeUpper(s);
	};
	return s;
}


void translate_helper::synthesize(CEngSemWord& EngWord) const
{
	EngWord.m_ParadigmId = GetParadigmIdByLemma(morphEnglish, EngWord.m_Lemma,GetOnePOS(EngWord.m_Poses));
	if(EngWord.m_ParadigmId == -1)		
	{  
	  EngWord.m_Word =  fix_case(EngWord);

	  // получаем притяжательную форму
	  if ( EngWord.HasOneGrammem(ePossessive) )
		set_possessive(EngWord);

  	// слово "трехоконный" переходит в слово "3-fenestral"
	  if (EngWord.m_NumeralPrefix != "")
		EngWord.m_Word =   EngWord.m_NumeralPrefix+"-"+ EngWord.m_Word;


   	 // добавим окончание "-s" или "-es" к существительному 
	  if (   EngWord.m_Poses == _QM(eNOUN) )
		  if (		EngWord.HasOneGrammem(ePlural) // есть множественное
				&&	!EngWord.HasOneGrammem(eSingular) // но нет единственного
				&&  !EngWord.m_Word.empty()
		 )
	  {
		if (   (EngWord.m_Word[EngWord.m_Word.size() - 1] == 'S')
			|| (EngWord.m_Word[EngWord.m_Word.size() - 1] == 's')
		   )// если заканчивается на "s", тогда добавим "es"
		   EngWord.m_Word += "es";
		else
		  EngWord.m_Word += "s";
	  };


	  return;
	};


	uint64_t eng_grammems = EngWord.GetAllGrammems();
	std::string prefix;

	if (!EngWord.m_bDoNotChangeForm)
	if(    (EngWord.m_Poses == _QM(eADJ) )
		&& has_not_english_comparative(EngWord.m_ParadigmId)
	  )
		if (eng_grammems & _QM( eSupremum))
		{
			prefix = "most ";
			// отрубаем граммемы comp и  sup
			eng_grammems &= ~( _QM( eSupremum) |  _QM( eComparativ)) ;
		}
		else
		if (eng_grammems & _QM( eComparativ))
		{
			prefix = "more ";
			// отрубаем граммемы comp и  sup
			eng_grammems &= ~( _QM( eSupremum) |  _QM( eComparativ)) ;
		};



	// отключение лица и числа у местоимений, поскольку в англ морф. лицо и число не припсано
	if (EngWord.m_Poses == _QM(ePRON) )
			eng_grammems &= ~(eAllPersons|eAllNumbers);

	// отключаем род (это словообразовательная граммема)
	eng_grammems &= ~eAllGenders;

	// отключение объектного падежа у всего, кроме местоимений
	if (EngWord.m_Poses != 1<<ePRON)
			eng_grammems &= ~(_QM(eObjectCase));

	// отключение притяжательность у всего, кроме местоимений
	if (EngWord.m_Poses != _QM(ePRON))
			eng_grammems &= ~(_QM(ePossessive));

	eng_grammems &= ~(_QM(eGeographics));


	// отключаeм у прилагательных и наречий все, кроме степеней сравнения 
	if  (   (EngWord.m_Poses == 1<<eADJ) 
		  || (EngWord.m_Poses == 1<<eADV)   
		)
		eng_grammems &= eAllDegrees;

	// отключаем у всех одушевленность
	eng_grammems &= ~(_QM(eAnimative));



	try{
		if (!EngWord.m_bDoNotChangeForm)
		{
			// получаем форму по Id и  граммемам
			EngWord.m_Word = create_form_by_id(EngWord.m_ParadigmId, eng_grammems);

			// получаем притяжательную форму
			if (EngWord.HasOneGrammem(ePossessive) )     
				set_possessive(EngWord);

			// слово "трехоконный" переходит в слово "3-fenestral"
			if (EngWord.m_NumeralPrefix != "")
				EngWord.m_Word =   EngWord.m_NumeralPrefix+"-"+ EngWord.m_Word;

			// выправлем регистр слова
			EngWord.m_Word = prefix + fix_case(EngWord);
		};
	}catch(...){
	}

	

}


void add_rel_operators(CEngSemNode& Node)
{

	assert (Node.m_MainWordNo != -1);
	long WordNo =  Node.m_MainWordNo;
		// добавление стандартных префиксов
	if (Node.m_RelOperators.size() > 0)
	 for (long i=Node.m_RelOperators.size()-1;  i>=0; i--)
 	  if (Node.m_RelOperators[i] == _R("АВИА"))
		  Node.m_Words[WordNo].m_Word = "aero"+Node.m_Words[WordNo].m_Word;
	  else
	  if (Node.m_RelOperators[i] == _R("ВИЦЕ-"))
		  Node.m_Words[WordNo].m_Word = "vice-"+Node.m_Words[WordNo].m_Word;
	  else
  	  if (Node.m_RelOperators[i] == _R("ЭКС-"))
		  Node.m_Words[WordNo].m_Word = "ex-"+Node.m_Words[WordNo].m_Word;
	  else
		  if (Node.m_RelOperators[i] == _R("ПОЛУ"))
		  {
			if (Node.GetPos () == eADJ)
		      Node.m_Words[WordNo].m_Word = "semi"+Node.m_Words[WordNo].m_Word;
			else
			  Node.m_Words[WordNo].m_Word = "half "+Node.m_Words[WordNo].m_Word;
		  }
	  else
		  /* переводим фразу "все больше людей"-> "more and more people"*/
		  if (   (Node.m_RelOperators[i] == _R("ПРОДОЛЖ"))
			  && ( Node.HasGrammemRich(eComparativ) )
			 )
		  {
			  std::string s = Node.m_Words[WordNo].m_Word;
			  EngRusMakeLower(s);
			  Trim(s);
			  Node.m_Words[WordNo].m_Word += " and  " + s + " ";
		  };
};

// синтезирет главное слово и добавляет  префиксы из перечня m_Reloperators 
void translate_helper::synthesize_by_node(CEngSemNode& Node) const
{
	assert (Node.m_MainWordNo != -1);

	long WordNo =  Node.m_MainWordNo;



	if (Node.m_Words[WordNo].m_bDoNotChangeForm) 
	{
		/*
		 надо поставить притяжательную форму уже синтезированным узлам 
		 (m_bDoNotChangeForm),поскольку граммема притяжательности могла возникнуть 
		 после их синтеза.
		 Например, термины синтезируются перед всем алгоритмами,
		 а постановка притяжательности - это один из алгоритмов.
		*/
		if (Node.HasGrammemRich(ePossessive) )
			set_possessive(Node.m_Words[0]);

		add_rel_operators(Node);

		return;
	};

	if (Node.m_bProper) return;

	// отрубаем множественное число у конструкций типа half an hour, чтобы
	// не получилось half hours.
	if (Node.GetPos() == eNOUN)
	 if (Node.HasRelOperator(_R("ПОЛУ")))
	 {
		Node.AddOneGrammemRich(eSingular);
		Node.DeleteGrammemsRich( _QM(ePlural) );
	 };

		
		
	
	synthesize(Node.m_Words[WordNo]);

	add_rel_operators(Node);

	// снова выправляем регистр, поскольку, возможно, были добавлены приставки
	if (!Node.m_Words[WordNo].m_bDoNotChangeForm)
	  Node.m_Words[WordNo].m_Word = fix_case(Node.m_Words[WordNo]);

};



// проверяет, что ID - английское сравнительная степень
bool translate_helper::has_not_english_comparative(long Id) const
{
	CFormInfo p;
	bool bRes = GetEngLemmatizer()->CreateParadigmFromID(Id, p);
	assert (bRes);
	for(int j = 0; j < p.GetCount(); j++)
	{
		uint64_t gr;
		GetEngGramTab()->GetGrammems(p.GetAncode(j).c_str(), gr );
		if(gr & _QM( eComparativ))
		{
			return false;
		}
	}
	return true;
}



void translate_helper::flush()
{

	result_vec.clear();
}



// сортировка анлийских слов по частоте перевода русского слова  в данной предметной области (m_TextKind)
// если в предментной области m_TextKind частоты равны, то используются частоты других
// предметных областей, иначе сортируем по значению   англ. ParadigmId
struct FreqDictLess{
	// русская ParadigmId
	long						m_RusParadigmId;
	// частотыне словари для каждой предметной области
	std::vector<const CFreqDict*>&	 m_FreqDicts;
	// текущая предметная область
	int							m_TextKind;

	FreqDictLess(long RusParadigmId, std::vector<const CFreqDict*> &FreqDicts, int TextKind)
		:m_RusParadigmId(RusParadigmId),
		m_FreqDicts(FreqDicts),
		m_TextKind(TextKind)
	{}


	bool operator() (long id_eng1, long id_eng2) const
	{
		int i;
		if(id_eng1 == id_eng2) return false;
		// main freqs are equal
		std::vector<long> vec1(m_FreqDicts.size());
		std::vector<long> vec2(m_FreqDicts.size());
		for(i = 0; i < m_FreqDicts.size(); i++){
			vec1[i] = m_FreqDicts[i]->GetFreq(id_eng1, m_RusParadigmId);
			vec2[i] = m_FreqDicts[i]->GetFreq(id_eng2, m_RusParadigmId);
		}
		// maximal freqs
		int max1 = *max_element(vec1.begin(), vec1.end());
		int max2 = *max_element(vec2.begin(), vec2.end());

		if(vec1[m_TextKind] != vec2[m_TextKind])
			return vec1[m_TextKind] > vec2[m_TextKind];

		for(i = 0; i < m_FreqDicts.size(); i++){
			if(vec1[i] != vec2[i])
				return vec1[i] > vec2[i];
		}
		// all freqs are equal...
		return id_eng1 < id_eng2;
	}	
};

const CThesaurus*   translate_helper::GetThes(int ThesId)  const
{
	return m_pData->GetThes(ThesId);
}


void translate_helper::translate_id(long Id, std::vector<long> &res, poses_mask_t RusPoses) const
{
	
	// получение всех видовых пар данного слова, если это глагол,
	std::vector<long> rus_vec;
	rus_vec.push_back(Id);
	int i;
	std::vector<uint32_t> ResVector;
	m_pData->m_AspDict.nonperf2perf(Id, ResVector);
	for(i = 0; i < ResVector.size(); i++)
		rus_vec.push_back(ResVector[i]);

	m_pData->m_AspDict.perf2nonperf(Id, ResVector);
	for(i = 0; i < ResVector.size(); i++)
		rus_vec.push_back(ResVector[i]);
		



	for(int r = 0; r < rus_vec.size(); r++){
		Id = rus_vec[r];

		// переводим с помощью бинарного словаря
		CSetOfWordPairs p = m_pData->m_BinaryDict.TranslateIndirect(Id);
		int count = p.get_Count();
		// идем по всем эквивалентам
		for(i = 0; i < count; i++)
		{
			bool ok = true;
			std::string b;
			// если найден флаг, который считается  "плохим", то 
			// пропускаем этот перевод
			for(int j = 4; j <= 7 && j < m_pData->m_BinaryDict.GetFlagCount(); j++)
			{
				int flag = p.GetFlag(i, j, b);
				if(binary_search(m_bad_flags_arr.begin(), m_bad_flags_arr.end(), flag))
				{
					ok = false;
					break;
				}
			}
			// проверяем, что если мы имеем дело с глаголом, то 
			// перевод должен помечен глагольной пометой (1)
			bool has_verb_flag = p.GetFlag(i, 0, b) == 1;
			if(  (    (RusPoses & (1<<VERB))
			 	   || (RusPoses & (1<<INFINITIVE))
				 )
			   && !has_verb_flag
			  )
			 ok = false;

			if(ok)
				res.push_back(p.GetId(i));
		}
        // если все переводы были отвергнуты, то считаем, что все переводы подходят
		if(count > 0 && res.empty()){
			for(int i = 0; i < count; i++)
				res.push_back(p.GetId(i));
		}

		if(res.size()) break;
	}

   // сортируем переводы по частоте биграмм, если словарь биграмм был определен
   const CFreqDict* dual_freq_dict	= m_option.dual_freq_dict;

   int max_dual_freq = 0, max_dual_freq_ind = -1;
   for(i = 0; (dual_freq_dict!=NULL) && i < res.size(); i++){
	   int freq = m_option.dual_id1 == -1
		   ? dual_freq_dict->GetFreq(res[i], m_option.dual_id2)
		   : dual_freq_dict->GetFreq(m_option.dual_id2, res[i]);
	   if(freq > max_dual_freq){
		   max_dual_freq = freq;
		   max_dual_freq_ind = i;
	   }
   }
	// если было найдено слово в словаре биграмм, то считаем это иделаьным 
	// результатом, который даже лучше, чем частоты по предметным областям
	if(max_dual_freq_ind >= 0){
		res[0] = res[max_dual_freq_ind];
		res.resize(1);
		return;

	}

	// сортируем по частотам перевода в предметных областях
	// первое слово в отсортированном массиве - самый вероятный перевод
	std::vector<const CFreqDict*> freq_vec(3);
	freq_vec[0] = &m_pData->m_CompFreq;
	freq_vec[1] = &m_pData->m_FinFreq;
	freq_vec[2] = &m_pData->m_HudFreq;
	sort(res.begin(), res.end(), FreqDictLess(Id, freq_vec, text_kind));
}






//--------------------------------------------------------------------------------
// выдает по строке все леммы из морфологического словаря нужной части речи
// если pos=-1, тогда часть речи не проверяется
long  translate_helper::GetParadigmIdByLemma(MorphLanguageEnum langua, std::string norm, UINT pos, bool bProper) const
{

	std::vector<CFormInfo> ParadigmCollection;
	Trim(norm);
	const CLemmatizer*  lemmatizer = GetLemmatizer(langua);
	const CAgramtab*	GramTab = GetGramTab(langua);
	RmlMakeUpper(norm, langua);

	if (!lemmatizer->CheckABC(norm.c_str()))
		return -1;

	try
	{
		lemmatizer->CreateParadigmCollection(true, norm, true, false, ParadigmCollection);
	}
	catch(...)
	{
		return - 1;
	}
	int count = ParadigmCollection.size();
	int AgreedWithProper = -1;
	int FirstGoodId = -1;
	for(int i = 0; i < count; i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];

		if (pos != -1)
			if( GramTab->GetPartOfSpeech(Paradigm.GetAncode(0).c_str()) != pos) continue;

		uint64_t g;
		GramTab->GetGrammems(Paradigm.GetAncode(0).c_str(), g);
		if ( (( g& _QM(eProper))>0) == bProper)
			AgreedWithProper = Paradigm.GetParadigmId();

		if (FirstGoodId == -1)
			FirstGoodId = Paradigm.GetParadigmId();
	}

	if (AgreedWithProper != -1)
		return AgreedWithProper;
	else
    	return FirstGoodId;
}

//--------------------------------------------------------------------------------
// выдает независимые от формы граммемы для слова

uint64_t  translate_helper::GetFixedGrammemsByLemma(MorphLanguageEnum langua,  std::string norm,  UINT pos, bool bProper) const
{
	long ParadigmID = GetParadigmIdByLemma(langua, norm, pos, bProper);

	if( ParadigmID==-1 )
		return 0;
	
	CFormInfo Paradigm;
	bool bRes = GetLemmatizer(langua)->CreateParadigmFromID(ParadigmID, Paradigm);
	if (!bRes)
		throw CExpc ("Cannot create paradigm from Id");

	std::string ancode = Paradigm.GetAncode(0);
	const CAgramtab* pGramTab = GetGramTab(langua);

	uint64_t grammems = pGramTab->GetAllGrammems(ancode.c_str());

	for( int i=1; i<Paradigm.GetCount(); i++ )
	{
		std::string ancode = Paradigm.GetAncode(i);
		grammems &= pGramTab->GetAllGrammems(ancode.c_str());
	}

	return( grammems );
}


// выдает артикль a/an в зависимотси, с чего начинается  слово  _str
std::string translate_helper::an_article_before(const std::string &_str) const
{
	std::string str(_str);

	while(str.size() && str[0] == ' ') str = str.substr(1);

	if(str.empty()) return "a ";

	bool is_except = starts_with(str, except_vec);
	
	if(tolower(str[0]) == 'a' || tolower(str[0]) == 'i' || tolower(str[0]) == 'o')
		return "an ";
	if(tolower((unsigned char)str[0]) == 'h' && is_except)
		return "an ";
	if(tolower((unsigned char)str[0]) == 'e' && !is_except)
		return "an ";
	if(tolower((unsigned char)str[0]) == 'u' && !is_except)
		return "an ";
	return "a ";
}

// проверяет, что строка s начинается с одного из префиксов starts
bool translate_helper::starts_with(const std::string &s,const StringVector &starts) const 
{
	std::string lower_s = s;
	EngRusMakeLower(lower_s);
	for(int i = 0; i < starts.size(); i++)
	{
		int size = std::min(starts[i].size(), s.size());
		if(lower_s.substr(0,size) ==  starts[i])
			return true;
	}
	return false;
}

struct CTransliteration {
	StringVector Rus2Eng;

	void init_char(const std::string& rus, const std::string& eng) {
		BYTE rus_char = _R(rus.c_str())[0];
		Rus2Eng[rus_char] = eng;
	}

	CTransliteration() {
		Rus2Eng.resize(256);
		for (int i = 0; i < 256; i++) {
			Rus2Eng[i] = (char)(unsigned char)i;
		}
		init_char("А", "A");
		init_char("Б", "B");
		init_char("В", "V");
		init_char("Г", "G");
		init_char("Д", "D");
//		init_char("Е", "Е");
		init_char("Ё", "E");
		init_char("Ж", "Zh");
		init_char("З", "Z");
		init_char("И", "I");
		init_char("Й", "Y");
		init_char("К", "K");
		init_char("Л", "L");
		init_char("М", "M");
		init_char("Н", "N");
		init_char("О", "O");
		init_char("П", "P");
		init_char("Р", "R");
		init_char("С", "S");
		init_char("Т", "T");
		init_char("У", "U");
		init_char("Ф", "F");
		init_char("Х", "Kh");
		init_char("Ц", "Ts");
		init_char("Ч", "Ch");
		init_char("Ш", "Sh");
		init_char("Щ", "Shch");
		init_char("Ъ", "\"");
		init_char("Ы", "Y");
		init_char("Ь", "'");
		init_char("Э", "E");
		init_char("Ю", "Yu");
		init_char("Я", "Ya");
	}
	std::string transliterate(std::string str) const {
		EngRusMakeUpper(str);
		const std::string before_e = _R("УЕЪЫАОЭЯИЬЮ");
		std::string res;
		for (size_t i = 0; i < str.length(); i++)
			if (str[i] == _R("Е")[0])
				if (i == 0
					|| before_e.find((unsigned char)str[i - 1]) != std::string::npos
					)
					res += "Ye";
				else
					res += "E";
			else
				res += Rus2Eng[(unsigned char)str[i]];
		return res;
	}
};

const static CTransliteration RusEngTransliteration = CTransliteration();

// транслитериует русскую строку _str
void translate_helper::transliterate(CEngSemWord& W ) 
{
	std::string str;
	if (!W.m_Lemma.empty()) 
		str = W.m_Lemma;
	else
		str = W.m_Word;
	
	std::string res = RusEngTransliteration.transliterate(str);
	W.m_Lemma = res;
	W.m_Word = res;
	W.m_Word = fix_case(W);
}




// загружает перечень слов из РОСС
void translate_helper::init_list_from_ross(const CRossHolder* RossHolder, const std::string &list_name, StringVector &res)
{
	WORD UnitNo = RossHolder->GetRoss()->LocateUnit(list_name.c_str(), 1);

	assert(UnitNo != ErrUnitNo);

	res.clear();

	if (!RossHolder->GetRoss()->IsEmptyArticle(UnitNo))
	 for (size_t i = RossHolder->GetRoss()->GetUnitStartPos(UnitNo); i<= RossHolder->GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (     (RossHolder->GetRoss()->GetCortegeFieldNo(i) == RossHolder->LexFetFieldNo) 
		 )
	  {
         std::string Item = (const char*)RossHolder->GetRoss()->GetDomItemStr(RossHolder->GetRoss()->GetCortegeItem(i,0));
		 EngRusMakeLower(Item);
		 res.push_back (Item);
	  };
}


bool CEngSemStructure::translate_binary(long NodeNo) 
{
	if( !m_Nodes[NodeNo].IsWordContainer() )
		return false;
	if( m_Nodes[NodeNo].m_Words[0].m_bDoNotChangeForm )
		return false;
	if ( m_Nodes[NodeNo].GetType() != Aoss )
		return false;
	if (    !IsPlugArticle(GetRossHolder(Aoss), m_Nodes[NodeNo].GetUnitNo()) 
	 	 && m_Nodes[NodeNo].m_Colloc.m_CollocSource != ThesType
	   )
		return false;

// примитивные узлы, которые интерпретированы добавочными статьями
	const CSemWord& W = RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWord(0);
	long ParadigmId = W.m_ParadigmId;

	std::vector<long> EngIds;
	helper.translate_id(ParadigmId, EngIds, W.m_Poses);

	if (EngIds.empty() || W.HasOneGrammem(rPatronymic) )
	{
		
		if (	isdigit((unsigned char)W.m_Word[0])
				&& W.HasPOS(ADV) // "вдвоем", "втроем"
			)
		{
			m_Nodes[NodeNo].m_Words[0].m_Lemma = "together";
			m_Nodes[NodeNo].m_Words[0].m_Word = "together";
		}
		else
		// транслитерируем
		for (long i=0; i < m_Nodes[NodeNo].m_Words.size(); i++)
		{
			// восстанавливаем лемму
			assert (i < RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWordsSize());
			if( i < RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWordsSize() )
				m_Nodes[NodeNo].m_Words[i].m_Lemma = RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWord(i).m_Lemma;

			helper.transliterate(m_Nodes[NodeNo].m_Words[i]);
		};
		
	}

	else
	{
		std::string Lemma = helper.create_norm_by_id (EngIds[0]);
		m_Nodes[NodeNo].m_Words[0].m_Lemma = Lemma;
		m_Nodes[NodeNo].m_Words[0].m_Word = Lemma;
		m_Nodes[NodeNo].m_Words[0].m_ParadigmId = EngIds[0];
		if (W.m_AdditParadigmId != -1)
		{
			helper.translate_id(W.m_AdditParadigmId, EngIds, W.m_Poses);
			if (EngIds.size() > 0)
			{
				std::string Lemma = helper.create_norm_by_id (EngIds[0]);
				m_Nodes[NodeNo].m_Words[0].m_Lemma += "-"+Lemma;
				m_Nodes[NodeNo].m_Words[0].m_Word += "-"+Lemma;
				m_Nodes[NodeNo].m_Words[0].m_AdditParadigmId = EngIds[0];
			};

		};
	}


	return true;
}





