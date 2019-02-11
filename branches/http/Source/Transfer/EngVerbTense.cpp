#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "StdAfx.h"
#include "EngVerbTense.h"


long CEngVerbTense::GetPersonGrammemsIfCan(CEngSemNode& EngNode)
{
	if (EngNode.m_MainWordNo == -1) return 0;
	CEngSemWord& EngWord = EngNode.m_Words[EngNode.m_MainWordNo];
	if  (   (EngWord.m_Lemma == "there") 
		 || (EngWord.m_Lemma == "which")
		 || (EngWord.m_Lemma == "who")
		 || (EngWord.m_Lemma == "that")
		 || (EngWord.m_Lemma == "one")
		 || (EngWord.m_Lemma == "what")
		 || (EngWord.m_Lemma == "this")
		)
		return _QM(eThirdPerson);

	// ������������� ����� ������� ������ ���, �����   �� ����������� 
	// ��� ����, ����� �������� ��������  ����
    S.helper.synthesize(EngWord);
	if (EngWord.m_ParadigmId == -1) 
	{
		// ��� ����, ������� ��� � ���������� ���������� (��������, "����") ����� ����������
		// ������ ����
		if (EngNode.GetGrammemsRich() & _QM(eProper)) 
			return _QM(eThirdPerson);

	    if  (EngNode.GetPos() == eNOUN)
			return _QM(eThirdPerson);
		return 0;
	};

	CFormInfo p;
	bool bRes = helper.GetEngLemmatizer()->CreateParadigmFromID(EngWord.m_ParadigmId, p);
	assert (bRes);
	QWORD gr;
	helper.GetEngGramTab()->GetGrammems(p.GetAncode(0).c_str(), gr);
	if ((gr & eAllPersons) == 0)
		if (    (helper.GetEngGramTab()->GetPartOfSpeech(p.GetAncode(0).c_str())  == eNOUN)
			||  (helper.GetEngGramTab()->GetPartOfSpeech(p.GetAncode(0).c_str())  == ePN)
			||  (EngNode.GetPos() == eNOUN)
			|| (EngNode.GetPos() == ePN)
			|| (EngNode.GetPos() == ePRON)
		   )
		 return _QM(eThirdPerson);


	return gr & eAllPersons;
};


void CEngVerbTense::make_verb_form()
{
	assert (m_VerbNodeNo != -1);
	m_pVerbNode = &E.m_Nodes[m_VerbNodeNo];
	assert(m_pVerbNode->m_MainWordNo != -1);
	m_pVerbMainWord = &m_pVerbNode->m_Words[m_pVerbNode->m_MainWordNo];



	S.helper.synthesize(*m_pVerbMainWord);

	dir_obj_postfix = post_verb_advs = " ";

	string posle_log_str;
	if(!m_pVerbMainWord->m_PosleLog.m_PosleLog.empty())
		posle_log_str = m_pVerbMainWord->m_PosleLog.m_PosleLog;
	else if(m_pVerbMainWord->m_PosleLog.m_PosleLogPrep.m_DictType != NoneRoss){
		const CRossInterp &prep = m_pVerbMainWord->m_PosleLog.m_PosleLogPrep;
		if(prep.m_DictType == EngObor)
			posle_log_str = 
				E.m_pData->GetEngOborStr(prep.m_UnitNo);
		else
			posle_log_str = E.GetRoss(prep.m_DictType)->GetEntryStr(prep.m_UnitNo);
	}

	if(m_pVerbMainWord->m_PosleLog.m_Position == "dir_obj _")
		dir_obj_postfix += posle_log_str;
	if(m_pVerbMainWord->m_PosleLog.m_Position == ">>")
		post_verb_advs += posle_log_str;


	result = S.Res(m_VerbNodeNo);

	m_SubjNodeNo = result.subject_node;

	m_Grammems = m_pVerbNode->GetGrammemsRich();
	m_Tense = m_pVerbMainWord->GetTense();

	// ����� ����� �� �����������, ���� ���� ���������� � ����� � ���� 
	if(m_SubjNodeNo != -1) 
	  if (E.m_Nodes[m_SubjNodeNo].IsLemma ("both") )
	  {
		  m_Grammems &= ~eAllNumbers;
		  m_Grammems |= _QM(ePlural);
	  }
	  else
	  if (eAllNumbers & E.m_Nodes[m_SubjNodeNo].GetGrammemsRich())
		{
			m_Grammems &= ~eAllNumbers;
			m_Grammems |= (eAllNumbers & E.m_Nodes[m_SubjNodeNo].GetGrammemsRich());
		};

   // ���e� ���� �� �����������, ���� ������ �������������� ������ to be
	if(    
		   (m_SubjNodeNo != -1) 
		&& is_be_verb() 
	  )
	{
		long SubjGrams = GetPersonGrammemsIfCan(E.m_Nodes[m_SubjNodeNo]);
		m_Grammems &= ~eAllPersons;
		m_Grammems |= SubjGrams;
	};


	// check for plural numeral
	if(GetNumber() == 1 && m_SubjNodeNo != -1 && S.E.has_plural_rel(m_SubjNodeNo))
		SetNumber(2);

	// ���� ���������� �������� ���������� �����, �� ����� ��������� ������ �� �������������
	// �����
	if(    (GetNumber() == 1) 
		&& (m_SubjNodeNo != -1) 
		&& (E.m_Nodes[m_SubjNodeNo].m_NodeType == MNA)
	)
		SetNumber(2);


	if(GetNumber() == 1 && S.FieldContainsValue(E.m_Nodes[m_VerbNodeNo], "RESTR", "�� !"))
	  SetNumber(1);

	// ���� ���������� - ����������� ��. �., � �������� ��������� ���� =/= 3�.
	// ����� ����� ������ ������ ���������� ����� � ����   �� �������,
	// ��������� � ��������� ������ ���������� 3�, sg, ��� �������
	if (    (GetNumber() == 1) 
		&&  (m_SubjNodeNo != -1)
	   )
      if (! is_be_verb() )
	  if (	GetPersonGrammemsIfCan(E.m_Nodes[m_SubjNodeNo]) & ( _QM(eFirstPerson)|_QM(eSecondPerson) ) ) 
	  {
		 m_Grammems &= ~eAllNumbers;
		 m_Grammems &= ~eAllPersons;
	  }
	  else 
		 // ���� ���������� - ����������� ��. �. 3�. ��� ������ �� ��������� ����, �� ����� ��������� 3� 
		 m_Grammems |= _QM(eThirdPerson);



	make_string();

	S.Res(m_VerbNodeNo) = result;
}




// ���������, ��� ���� ���� �������� �������, ������� ������������� i-� �����������
// � GFi=inf (��������� ��� to)

bool CEngSynthes::IsInfinitivePatternWithoutTo (long NodeNo) const
{
	vector<long> Rels;
	E.GetIncomingRelations(NodeNo, Rels, false);
	if (Rels.size() != 1) return false;

	if (E.m_Nodes[E.m_Relations[Rels[0]].m_SourceNodeNo].m_NodeType == MNA)
		E.GetIncomingRelations(E.m_Relations[Rels[0]].m_SourceNodeNo, Rels, false);


	for (long i=0; i <Rels.size(); i++)
 	  if (E.m_Relations[Rels[i]].m_bInterpreted)
		if (E.IsGramFet(E.m_Relations[Rels[i]],"inf"))
			return true;

	// ����� ����������� ������, ������� ������� ����������, �������� "in_order_to"
	for (int i=0; i <Rels.size(); i++)
	{
		if( E.m_Relations[Rels[i]].m_SynReal.m_Conj.m_DictType != EngObor )
			continue;

		if( E.m_Relations[Rels[i]].m_SynReal.m_Conj.m_UnitNo == ErrUnitNo )
			continue;

		CSemPattern P;
		// ��������� GFi �� ������� ��������
		P.InitSemPattern(E.GetRossHolder(EngObor),E.m_Relations[Rels[i]].m_SynReal.m_Conj.m_UnitNo, 2, 0);
		P.LoadGramFromDict();
		if (!P.m_GramCorteges.empty() )
		  if (P.GetSynRelStr(0) == "inf")
			  return true;
	};
    return false;
};




//------------------------------------------------------------------------
void CEngVerbTense::make_string()
{
	
	// ��������� ����� �� ����������, �� ����� ��������������� �������
	// ��������: "not to go", �� "we do not go".

	bool has_not_before = has_not() 
					&& (   is_infinitive (m_Tense) 
						|| is_gerund(m_Tense));
	
	// ����� ����� �������� �������������� ������
	string res;

	S.handle_rel_operators(m_VerbNodeNo, false);
	rel_operators = S.Res(m_VerbNodeNo).rel_operator;
	S.Res(m_VerbNodeNo).rel_operator = "";


	UINT grammems;
	if(m_pVerbMainWord->m_bDoNotChangeForm)
		m_Tense = zero_tn;

	if (m_Tense  != zero_tn)
			CreateAuxVerbsThatStandBeforeMain(m_Tense, res, grammems);
	else
			grammems = m_Grammems;

	// ������� ������ ���� ���������
	// "even not to think"
	res += rel_operators + " ";

	// ����� ���������, ���� ��������� ��� �������� 
	
	if (has_not_before)
		res += " not  ";
	/*
	 �����  ������ ���� ������� to, ���� ��� �����
	*/
	if (     is_infinitive(m_pVerbNode->GetTense())
		 && !S.IsInfinitivePatternWithoutTo(m_VerbNodeNo)
		 && !m_pVerbNode->m_bNotUseTo
	  )
	res  += " to ";

	/*
	 ����� ���� ��� ������
	*/
	if(m_pVerbMainWord->m_bDoNotChangeForm)
	{
			// ����� "���������" ��������� � ����� "3-legged" (pp)
			// "3-legged" ����������� ������ ���� �������� m_bDoNotChangeForm
			string Word = m_pVerbMainWord->m_Word;
			if (m_pVerbMainWord->m_NumeralPrefix != "")
				Word =   m_pVerbMainWord->m_NumeralPrefix+"-"+ Word;
			res += Word;
	}
	else 
		if(m_pVerbMainWord->m_ParadigmId != -1)
			if (!m_pVerbMainWord->m_bMorphologicalPassiveForm)
			  res += S.helper.create_form_by_id(m_pVerbMainWord->m_ParadigmId, grammems);
			else
			{
			  // ��� ������� ������� ��������� ������ to be	
			  long BeParadigmId = 	helper.GetParadigmIdByLemma(morphEnglish, "be", eVBE);
			  res += S.helper.create_form_by_id(BeParadigmId, grammems);
			  res += " ";
			  // � ����� ��� ������ 
			  res += S.helper.create_form_by_id(m_pVerbMainWord->m_ParadigmId, _QM(ePastParticiple));
			}
		else 
			res += m_pVerbMainWord->m_Word;


	// ������ ��������� ����� ��������������� �������
	// ��������: "we do not go".
  	if (has_not() && !has_not_before)
		 res +=  " not";

	m_DirObjNodeNo = E.FindDirObj(m_VerbNodeNo);
	if(m_DirObjNodeNo != -1 && S.is_conj_word(m_VerbNodeNo, m_DirObjNodeNo))
		m_DirObjNodeNo = -1;

	if(m_DirObjNodeNo != -1)
		S.handle_prep(m_DirObjNodeNo);

	
	result.m_WordForms.push_back(res);
	result.postfix += " " + post_verb_advs;

	S.handle_colloc_words(m_VerbNodeNo, result);

	

}

//------------------------------------------------------------------------
// ������� �� ������� Tense, ������ ����� ��������������� �������, ������� ������ 
// ������ �� ��������� �������. ��� ������� ������� ��������, � ������� ����� ���������
// �������� ������.
void CEngVerbTense::CreateAuxVerbsThatStandBeforeMain(EngVerbTenseEnum Tense, string& AuxVerbs, UINT& MainVerbGrammems) const
{
	MainVerbGrammems = 0;

	switch(Tense){
	case present_smp_tn: //inf (person) / do (person) + not + inf
		 if(GetPerson() == 3 && GetNumber() == 1)
			MainVerbGrammems = _QM(eThirdPerson) | _QM(eSingular) | _QM(ePresentIndef); // "does", "is"
		 else
		  if (is_be_verb()) 
		   if ( (GetPerson() == 1) && (GetNumber() == 1))
			   MainVerbGrammems = _QM(eFirstPerson) | _QM(eSingular) | _QM(ePresentIndef); //"am";
		   else
			   MainVerbGrammems =  _QM(ePlural) | _QM(ePresentIndef); // "are"

		  else
			 MainVerbGrammems = _QM(eInfinitive); // do
		break;
	case past_smp_tn: //pasa 
		MainVerbGrammems |= _QM(ePastIndef);
		if (is_be_verb()) // was =/= were
			if (GetPerson() == 2) // ���� ���������� - �ou, ����� ����� ������� were
				MainVerbGrammems |= _QM(ePlural);
			else
			    MainVerbGrammems |= eAllNumbers & m_Grammems;
		break;
	case future_smp_tn: 
		MainVerbGrammems = _QM(eFuturum); // future ������ ������ � to be
		MainVerbGrammems |= eAllNumbers & m_Grammems; // shall != will
		break;
	case prf_inf_tn: //have + pp,
		AuxVerbs =  "have ";
		MainVerbGrammems |= _QM(ePastParticiple);
		break;
	case prf_cont_inf_tn: //have been + ing,
		AuxVerbs = "have been ";
		MainVerbGrammems |= _QM(eGerund);
		break;
	case present_cont_inf_tn: //be + ing,
		AuxVerbs = "be ";
		MainVerbGrammems |= _QM(eGerund);		
		break;
	case pp_cont_tn : //have been sitting
		AuxVerbs = "been ";
		MainVerbGrammems |= _QM(eGerund);		
		break;
	case gerund_tn: // ing,
	case ing_tn: 
		MainVerbGrammems |= _QM(eGerund);		
		break;
    case pp_tn: // ed,
		MainVerbGrammems |= _QM(ePastParticiple);		
		break;
    case present_inf_tn: // ed,
		MainVerbGrammems |= _QM(eInfinitive);		
		break;

	}
}






bool		CEngVerbTense::is_be_verb() const
{
	return m_pVerbMainWord->m_Lemma == "be";
}




bool		CEngVerbTense::has_not()
{
	return S.E.m_Nodes[m_VerbNodeNo].HasRelOperator("��");
}






