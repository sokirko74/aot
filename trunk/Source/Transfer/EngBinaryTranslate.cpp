
#include "StdAfx.h"
#include "EngBinaryTranslate.h"
#include "../BinaryDictsLib/SetOfWordPairs.h"
#include "../AprRead/all_labels.h"


int find_label(const char *label)
{
	for(int i = 0;  i < GetTransLabelsCount(); i++){
		if(!strcmp(label, GetTransLabels()[i].str)) return i;
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
	m_bad_flags_arr.push_back(find_label("��"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("�����"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("�������"));
	m_bad_flags_arr.push_back(find_label("�����"));
	m_bad_flags_arr.push_back(find_label("�����"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("����"));
	m_bad_flags_arr.push_back(find_label("���"));
	m_bad_flags_arr.push_back(find_label("�����"));
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





// ������� ����� �� ParadigmId � eng_grammems, ���� eng_grammems == 0, ���������� �����
string translate_helper::create_form_by_id(long Id, QWORD eng_grammems) const
{
	int j;
	CFormInfo Paradigm;
	bool bRes = GetEngLemmatizer()->CreateParadigmFromID(Id, Paradigm);
	assert (bRes);


	int count = Paradigm.GetCount();
	for(j = 0; j < count; j++)
	{
		UINT POS =  GetEngGramTab()->GetPartOfSpeech(Paradigm.GetAncode(j).c_str());
		if(GetEngGramTab()->FindGrammems(Paradigm.GetAncode(j).c_str(), eng_grammems))
			break;
	}
	if(j == count) j = 0;
	string res = Paradigm.GetWordForm(j);
	EngRusMakeLower(res);

	return res;
}



// ������� ���������� ����� �� RusId
string translate_helper::create_norm_by_id(long EngId) const
{
	if (EngId ==-1) return "";
	CFormInfo p;
	bool bRes =  GetEngLemmatizer()->CreateParadigmFromID(EngId, p);
	if (bRes && p.GetCount() > 0)
		return p.GetWordForm(0);
	else
		return "";
}




// �������������  ������ �������
string fix_case(const CEngSemWord& EngWord)
{
	string s = EngWord.m_Word;
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

	  // �������� �������������� �����
	  if ( EngWord.HasOneGrammem(ePossessive) )
		set_possessive(EngWord);

  	// ����� "�����������" ��������� � ����� "3-fenestral"
	  if (EngWord.m_NumeralPrefix != "")
		EngWord.m_Word =   EngWord.m_NumeralPrefix+"-"+ EngWord.m_Word;


   	 // ������� ��������� "-s" ��� "-es" � ���������������� 
	  if (   EngWord.m_Poses == _QM(eNOUN) )
		  if (		EngWord.HasOneGrammem(ePlural) // ���� �������������
				&&	!EngWord.HasOneGrammem(eSingular) // �� ��� �������������
				&&  !EngWord.m_Word.empty()
		 )
	  {
		if (   (EngWord.m_Word[EngWord.m_Word.size() - 1] == 'S')
			|| (EngWord.m_Word[EngWord.m_Word.size() - 1] == 's')
		   )// ���� ������������� �� "s", ����� ������� "es"
		   EngWord.m_Word += "es";
		else
		  EngWord.m_Word += "s";
	  };


	  return;
	};


	QWORD eng_grammems = EngWord.GetAllGrammems();
	string prefix;

	if (!EngWord.m_bDoNotChangeForm)
	if(    (EngWord.m_Poses == _QM(eADJ) )
		&& has_not_english_comparative(EngWord.m_ParadigmId)
	  )
		if (eng_grammems & _QM( eSupremum))
		{
			prefix = "most ";
			// �������� �������� comp �  sup
			eng_grammems &= ~( _QM( eSupremum) |  _QM( eComparativ)) ;
		}
		else
		if (eng_grammems & _QM( eComparativ))
		{
			prefix = "more ";
			// �������� �������� comp �  sup
			eng_grammems &= ~( _QM( eSupremum) |  _QM( eComparativ)) ;
		};



	// ���������� ���� � ����� � �����������, ��������� � ���� ����. ���� � ����� �� ��������
	if (EngWord.m_Poses == _QM(ePRON) )
			eng_grammems &= ~(eAllPersons|eAllNumbers);

	// ��������� ��� (��� �������������������� ��������)
	eng_grammems &= ~eAllGenders;

	// ���������� ���������� ������ � �����, ����� �����������
	if (EngWord.m_Poses != 1<<ePRON)
			eng_grammems &= ~(_QM(eObjectCase));

	// ���������� ���������������� � �����, ����� �����������
	if (EngWord.m_Poses != _QM(ePRON))
			eng_grammems &= ~(_QM(ePossessive));

	eng_grammems &= ~(_QM(eGeographics));


	// �������e� � �������������� � ������� ���, ����� �������� ��������� 
	if  (   (EngWord.m_Poses == 1<<eADJ) 
		  || (EngWord.m_Poses == 1<<eADV)   
		)
		eng_grammems &= eAllDegrees;

	// ��������� � ���� ��������������
	eng_grammems &= ~(_QM(eAnimative));



	try{
		if (!EngWord.m_bDoNotChangeForm)
		{
			// �������� ����� �� Id �  ���������
			EngWord.m_Word = create_form_by_id(EngWord.m_ParadigmId, eng_grammems);

			// �������� �������������� �����
			if (EngWord.HasOneGrammem(ePossessive) )     
				set_possessive(EngWord);

			// ����� "�����������" ��������� � ����� "3-fenestral"
			if (EngWord.m_NumeralPrefix != "")
				EngWord.m_Word =   EngWord.m_NumeralPrefix+"-"+ EngWord.m_Word;

			// ��������� ������� �����
			EngWord.m_Word = prefix + fix_case(EngWord);
		};
	}catch(...){
	}

	

}


void add_rel_operators(CEngSemNode& Node)
{

	assert (Node.m_MainWordNo != -1);
	long WordNo =  Node.m_MainWordNo;
		// ���������� ����������� ���������
	if (Node.m_RelOperators.size() > 0)
	 for (long i=Node.m_RelOperators.size()-1;  i>=0; i--)
 	  if (Node.m_RelOperators[i] == "����")
		  Node.m_Words[WordNo].m_Word = "aero"+Node.m_Words[WordNo].m_Word;
	  else
	  if (Node.m_RelOperators[i] == "����-")
		  Node.m_Words[WordNo].m_Word = "vice-"+Node.m_Words[WordNo].m_Word;
	  else
  	  if (Node.m_RelOperators[i] == "���-")
		  Node.m_Words[WordNo].m_Word = "ex-"+Node.m_Words[WordNo].m_Word;
	  else
		  if (Node.m_RelOperators[i] == "����")
		  {
			if (Node.GetPos () == eADJ)
		      Node.m_Words[WordNo].m_Word = "semi"+Node.m_Words[WordNo].m_Word;
			else
			  Node.m_Words[WordNo].m_Word = "half "+Node.m_Words[WordNo].m_Word;
		  }
	  else
		  /* ��������� ����� "��� ������ �����"-> "more and more people"*/
		  if (   (Node.m_RelOperators[i] == "�������")
			  && ( Node.HasGrammemRich(eComparativ) )
			 )
		  {
			  string s = Node.m_Words[WordNo].m_Word;
			  EngRusMakeLower(s);
			  Trim(s);
			  Node.m_Words[WordNo].m_Word += " and  " + s + " ";
		  };
};

// ���������� ������� ����� � ���������  �������� �� ������� m_Reloperators 
void translate_helper::synthesize_by_node(CEngSemNode& Node) const
{
	assert (Node.m_MainWordNo != -1);

	long WordNo =  Node.m_MainWordNo;



	if (Node.m_Words[WordNo].m_bDoNotChangeForm) 
	{
		/*
		 ���� ��������� �������������� ����� ��� ��������������� ����� 
		 (m_bDoNotChangeForm),��������� �������� ���������������� ����� ���������� 
		 ����� �� �������.
		 ��������, ������� ������������� ����� ���� �����������,
		 � ���������� ���������������� - ��� ���� �� ����������.
		*/
		if (Node.HasGrammemRich(ePossessive) )
			set_possessive(Node.m_Words[0]);

		add_rel_operators(Node);

		return;
	};

	if (Node.m_bProper) return;

	// �������� ������������� ����� � ����������� ���� half an hour, �����
	// �� ���������� half hours.
	if (Node.GetPos() == eNOUN)
	 if (Node.HasRelOperator("����"))
	 {
		Node.AddOneGrammemRich(eSingular);
		Node.DeleteGrammemsRich( _QM(ePlural) );
	 };

		
		
	
	synthesize(Node.m_Words[WordNo]);

	add_rel_operators(Node);

	// ����� ���������� �������, ���������, ��������, ���� ��������� ���������
	if (!Node.m_Words[WordNo].m_bDoNotChangeForm)
	  Node.m_Words[WordNo].m_Word = fix_case(Node.m_Words[WordNo]);

};



// ���������, ��� ID - ���������� ������������� �������
bool translate_helper::has_not_english_comparative(long Id) const
{
	CFormInfo p;
	bool bRes = GetEngLemmatizer()->CreateParadigmFromID(Id, p);
	assert (bRes);
	for(int j = 0; j < p.GetCount(); j++)
	{
		QWORD gr;
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



// ���������� ��������� ���� �� ������� �������� �������� �����  � ������ ���������� ������� (m_TextKind)
// ���� � ����������� ������� m_TextKind ������� �����, �� ������������ ������� ������
// ���������� ��������, ����� ��������� �� ��������   ����. ParadigmId
struct FreqDictLess{
	// ������� ParadigmId
	long						m_RusParadigmId;
	// ��������� ������� ��� ������ ���������� �������
	vector<const CFreqDict*>&	 m_FreqDicts;
	// ������� ���������� �������
	int							m_TextKind;

	FreqDictLess(long RusParadigmId, vector<const CFreqDict*> &FreqDicts, int TextKind)
		:m_RusParadigmId(RusParadigmId),
		m_FreqDicts(FreqDicts),
		m_TextKind(TextKind)
	{}


	bool operator() (long id_eng1, long id_eng2) const
	{
		int i;
		if(id_eng1 == id_eng2) return false;
		// main freqs are equal
		vector<long> vec1(m_FreqDicts.size());
		vector<long> vec2(m_FreqDicts.size());
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


void translate_helper::translate_id(long Id, vector<long> &res, UINT RusPoses) const
{
	
	// ��������� ���� ������� ��� ������� �����, ���� ��� ������,
	vector<long> rus_vec;
	rus_vec.push_back(Id);
	int i;
	vector<DWORD> ResVector;
	m_pData->m_AspDict.nonperf2perf(Id, ResVector);
	for(i = 0; i < ResVector.size(); i++)
		rus_vec.push_back(ResVector[i]);

	m_pData->m_AspDict.perf2nonperf(Id, ResVector);
	for(i = 0; i < ResVector.size(); i++)
		rus_vec.push_back(ResVector[i]);
		



	for(int r = 0; r < rus_vec.size(); r++){
		Id = rus_vec[r];

		// ��������� � ������� ��������� �������
		CSetOfWordPairs p = m_pData->m_BinaryDict.TranslateIndirect(Id);
		int count = p.get_Count();
		// ���� �� ���� ������������
		for(i = 0; i < count; i++)
		{
			bool ok = true;
			string b;
			// ���� ������ ����, ������� ���������  "������", �� 
			// ���������� ���� �������
			for(int j = 4; j <= 7 && j < m_pData->m_BinaryDict.GetFlagCount(); j++)
			{
				int flag = p.GetFlag(i, j, b);
				if(binary_search(m_bad_flags_arr.begin(), m_bad_flags_arr.end(), flag))
				{
					ok = false;
					break;
				}
			}
			// ���������, ��� ���� �� ����� ���� � ��������, �� 
			// ������� ������ ������� ���������� ������� (1)
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
        // ���� ��� �������� ���� ����������, �� �������, ��� ��� �������� ��������
		if(count > 0 && res.empty()){
			for(int i = 0; i < count; i++)
				res.push_back(p.GetId(i));
		}

		if(res.size()) break;
	}

   // ��������� �������� �� ������� �������, ���� ������� ������� ��� ���������
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
	// ���� ���� ������� ����� � ������� �������, �� ������� ��� ��������� 
	// �����������, ������� ���� �����, ��� ������� �� ���������� ��������
	if(max_dual_freq_ind >= 0){
		res[0] = res[max_dual_freq_ind];
		res.resize(1);
		return;

	}

	// ��������� �� �������� �������� � ���������� ��������
	// ������ ����� � ��������������� ������� - ����� ��������� �������
	vector<const CFreqDict*> freq_vec(3);
	freq_vec[0] = &m_pData->m_CompFreq;
	freq_vec[1] = &m_pData->m_FinFreq;
	freq_vec[2] = &m_pData->m_HudFreq;
	sort(res.begin(), res.end(), FreqDictLess(Id, freq_vec, text_kind));
}






//--------------------------------------------------------------------------------
// ������ �� ������ ��� ����� �� ���������������� ������� ������ ����� ����
// ���� pos=-1, ����� ����� ���� �� �����������
long  translate_helper::GetParadigmIdByLemma(MorphLanguageEnum langua, string norm, UINT pos, bool bProper) const
{

	vector<CFormInfo> ParadigmCollection;
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

		QWORD g;
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
// ������ ����������� �� ����� �������� ��� �����

QWORD  translate_helper::GetFixedGrammemsByLemma(MorphLanguageEnum langua,  string norm,  UINT pos, bool bProper) const
{
	long ParadigmID = GetParadigmIdByLemma(langua, norm, pos, bProper);

	if( ParadigmID==-1 )
		return 0;
	
	CFormInfo Paradigm;
	bool bRes = GetLemmatizer(langua)->CreateParadigmFromID(ParadigmID, Paradigm);
	if (!bRes)
		throw CExpc ("Cannot create paradigm from Id");

	string ancode = Paradigm.GetAncode(0);
	const CAgramtab* pGramTab = GetGramTab(langua);

	QWORD grammems = pGramTab->GetAllGrammems(ancode.c_str());

	for( int i=1; i<Paradigm.GetCount(); i++ )
	{
		string ancode = Paradigm.GetAncode(i);
		grammems &= pGramTab->GetAllGrammems(ancode.c_str());
	}

	return( grammems );
}


// ������ ������� a/an � �����������, � ���� ����������  �����  _str
string translate_helper::an_article_before(const string &_str) const
{
	string str(_str);

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

// ���������, ��� ������ s ���������� � ������ �� ��������� starts
bool translate_helper::starts_with(const string &s,const StringVector &starts) const 
{
	string lower_s = s;
	EngRusMakeLower(lower_s);
	for(int i = 0; i < starts.size(); i++)
	{
		int size = min(starts[i].size(), s.size());
		if(lower_s.substr(0,size) ==  starts[i])
			return true;
	}
	return false;
}

// �������������� ������� ������ _str
void translate_helper::transliterate(CEngSemWord& W ) 
{
	string str;
	if (!W.m_Lemma.empty()) 
		str = W.m_Lemma;
	else
		str = W.m_Word;
	EngRusMakeUpper(str);
	string res;
	StringVector vec;
	vec = StringVector(256);
	for(int i = 0; i < 256; i++) vec[i] = (char)(unsigned char)i;
#define PAIR(ch, s) vec[(int)(unsigned char)ch] = s;
	PAIR('�', "A");
	PAIR('�', "B");
	PAIR('�', "V");
	PAIR('�', "G");
	PAIR('�', "D");
//	PAIR('�', "");
	PAIR('�', "E");
	PAIR('�', "Zh");
	PAIR('�', "Z");
	PAIR('�', "I");
	PAIR('�', "Y");
	PAIR('�', "K");
	PAIR('�', "L");
	PAIR('�', "M");
	PAIR('�', "N");
	PAIR('�', "O");
	PAIR('�', "P");
	PAIR('�', "R");
	PAIR('�', "S");
	PAIR('�', "T");
	PAIR('�', "U");
	PAIR('�', "F");
	PAIR('�', "Kh");
	PAIR('�', "Ts");
	PAIR('�', "Ch");
	PAIR('�', "Sh");
	PAIR('�', "Shch");
	PAIR('�', "\"");
	PAIR('�', "Y");
	PAIR('�', "'");
	PAIR('�', "E");
	PAIR('�', "Yu");
	PAIR('�', "Ya");
#undef PAIR

	unsigned char before_e[] = "�����������";
	sort(before_e, before_e + sizeof(before_e));

	for(int i = 0; str[i]; i++)
		if(str[i] == '�')
			if(    i == 0 
				|| binary_search(before_e, before_e + sizeof(before_e), (unsigned char)str[i-1])
			  )
				res += "Ye";
			else
				res += "E";
		else
		  res += vec[(unsigned char)str[i]];
   W.m_Lemma = res;
   W.m_Word = res;
   W.m_Word = fix_case(W);
}




// ��������� �������� ���� �� ����
void translate_helper::init_list_from_ross(const CRossHolder* RossHolder, const string &list_name, StringVector &res)
{
	WORD UnitNo = RossHolder->GetRoss()->LocateUnit(list_name.c_str(), 1);

	assert(UnitNo != ErrUnitNo);

	res.clear();

	if (!RossHolder->GetRoss()->IsEmptyArticle(UnitNo))
	 for (size_t i = RossHolder->GetRoss()->GetUnitStartPos(UnitNo); i<= RossHolder->GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (     (RossHolder->GetRoss()->GetCortegeFieldNo(i) == RossHolder->LexFetFieldNo) 
		 )
	  {
         string Item = (const char*)RossHolder->GetRoss()->GetDomItemStr(RossHolder->GetRoss()->GetCortegeItem(i,0));
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
	 	 && m_Nodes[NodeNo].m_Colloc.m_Type != ThesType 
	   )
		return false;

// ����������� ����, ������� ���������������� ����������� ��������
	const CSemWord& W = RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWord(0);
	long ParadigmId = W.m_ParadigmId;

	vector<long> EngIds;
	helper.translate_id(ParadigmId, EngIds, W.m_Poses);

	if (EngIds.empty() || W.HasOneGrammem(rPatronymic) )
	{
		
		if (	isdigit((unsigned char)W.m_Word[0])
				&& W.HasPOS(ADV) // "������", "������"
			)
		{
			m_Nodes[NodeNo].m_Words[0].m_Lemma = "together";
			m_Nodes[NodeNo].m_Words[0].m_Word = "together";
		}
		else
		// ���������������
		for (long i=0; i < m_Nodes[NodeNo].m_Words.size(); i++)
		{
			// ��������������� �����
			assert (i < RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWordsSize());
			if( i < RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWordsSize() )
				m_Nodes[NodeNo].m_Words[i].m_Lemma = RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetWord(i).m_Lemma;

			helper.transliterate(m_Nodes[NodeNo].m_Words[i]);
		};
		
	}

	else
	{
		string Lemma = helper.create_norm_by_id (EngIds[0]);
		m_Nodes[NodeNo].m_Words[0].m_Lemma = Lemma;
		m_Nodes[NodeNo].m_Words[0].m_Word = Lemma;
		m_Nodes[NodeNo].m_Words[0].m_ParadigmId = EngIds[0];
		if (W.m_AdditParadigmId != -1)
		{
			helper.translate_id(W.m_AdditParadigmId, EngIds, W.m_Poses);
			if (EngIds.size() > 0)
			{
				string Lemma = helper.create_norm_by_id (EngIds[0]);
				m_Nodes[NodeNo].m_Words[0].m_Lemma += "-"+Lemma;
				m_Nodes[NodeNo].m_Words[0].m_Word += "-"+Lemma;
				m_Nodes[NodeNo].m_Words[0].m_AdditParadigmId = EngIds[0];
			};

		};
	}


	return true;
}





