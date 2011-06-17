#include "StdAfx.h"


//================================================================
// Вычисление местоимения oneself по граммемам числа, лица и одушевленности
//================================================================
struct CEngPronounParadigm {
	long m_EngGrammems;
	char m_SelfLemma[30];
	char m_Pronoun[30];
};

const CEngPronounParadigm g_OnePronounParadigm[] =
{
	{_QM(eSingular) | _QM(eFirstPerson),									"myself",	"my"},
	{_QM(eSingular) | _QM(eSecondPerson),									"yourself", "your"},
	{_QM(ePlural)	| _QM(eSecondPerson),									"yourselves", "your"},
	{_QM(eSingular) | _QM(eThirdPerson) | _QM(eFeminum) | _QM(eAnimative),	"herself",	"her"},
	{_QM(eSingular) | _QM(eThirdPerson) | _QM(eMasculinum) | _QM(eAnimative),"himself", "his"},
	{_QM(eSingular) | _QM(eThirdPerson),									"itself",	"its"},
	{_QM(ePlural) | _QM(eFirstPerson),										"ourselves","our"},
	{_QM(ePlural) | _QM(eSecondPerson),										"yourselves","your"},
	{_QM(ePlural) | _QM(eThirdPerson),										"themselves","their"},
	{0,																		"oneself",	 "one's"}
};


string GetPronounEnglishFormByGrammems(QWORD Grammems, bool self_form)
{
	for (long i=0; i < sizeof(g_OnePronounParadigm) / sizeof (CEngPronounParadigm); i++)
		if  (   (Grammems & g_OnePronounParadigm[i].m_EngGrammems)  == g_OnePronounParadigm[i].m_EngGrammems)
			if (self_form) 
			    return g_OnePronounParadigm[i].m_SelfLemma;
			else
				return g_OnePronounParadigm[i].m_Pronoun;
	return "";
};


// строит по русским граммемам набор английских граммем, содержащий только граммемы времени
void TransferTimeGrammems (const CSemNode& RusNode, CEngSemNode& EngNode, const string& CallingFunc)
{
	if (EngNode.m_MainWordNo == -1) return;
	CEngSemWord& EngWord = EngNode.m_Words[EngNode.m_MainWordNo];
	EngWord.SetFormGrammems( EngWord.GetFormGrammems() & ~eAllVerbForms);
	string name = "TransferTimeGrammems called by "+ CallingFunc;

	if (RusNode.HasPOS (PARTICIPLE))
	   if (RusNode.HasOneGrammem(rActiveVoice) ) // сделавший
		   EngWord.SetTense(gerund_tn, name);
       else
		   EngWord.SetTense(pp_tn, name);  // сделанный
    else
	
	if( RusNode.HasPOS (PARTICIPLE_SHORT) )
	{
	  if (RusNode.HasOneGrammem(rPastTense) )  // прошедшее время
	  {
			EngWord.SetTense(past_smp_tn, name);
			EngWord.m_bMorphologicalPassiveForm = true;
	  }
	  if (RusNode.HasOneGrammem(rFutureTense) )  // будущее время
	  {
			EngWord.SetTense(future_smp_tn, name);
			EngWord.m_bMorphologicalPassiveForm = true;
	  }
  	  if (RusNode.HasOneGrammem(rPresentTense) )  // настоящее время
	  {
			EngWord.SetTense(present_smp_tn, name);
			EngWord.m_bMorphologicalPassiveForm = true;
	  }

	}
	
	else
	if (RusNode.HasPOS (ADVERB_PARTICIPLE))
	  if (RusNode.HasOneGrammem(rNonPerfective))  // несовершенный вид
		  // будущего времени у деепричастий  нет
				EngWord.SetTense(gerund_tn, name);
       else // совершенный вид
			{	
			    EngWord.SetTense(gerund_prf_tn, name);
			}
    else
  		if (RusNode.HasPOS (INFINITIVE))
			EngWord.SetTense(present_inf_tn, name);
		else
		if (RusNode.HasOneGrammem(rFutureTense) )
			EngWord.SetTense(future_smp_tn, name);
		else
			if (RusNode.HasOneGrammem( rPastTense) )
			EngWord.SetTense(past_smp_tn, name);
		else
			if (RusNode.HasOneGrammem(rPresentTense) )
			EngWord.SetTense(present_smp_tn, name);


    if (RusNode.HasOneGrammem(rImperative))
	{
	    EngWord.SetTense(present_smp_tn, name);
		EngWord.m_bImperative = true;
	};

};

// строит по русским граммемам набор английских граммем, содержащий только граммемы числа
void TransferNumberGrammems (const CSemNode& RusNode, CEngSemWord& EngWord)
{
	EngWord.SetFormGrammems(EngWord.GetFormGrammems() & ~eAllNumbers);

	// к английским прилагательным категория числа не применима
	if (   RusNode.HasPOS(ADJ_FULL) 
		|| RusNode.HasPOS(ADJ_SHORT)
	   ) 
	   return;

	if (RusNode.HasOneGrammem(rPlural) )
		EngWord.AddFormGrammem(ePlural);
	
	if (RusNode.HasOneGrammem( rSingular) )
		   EngWord.AddFormGrammem(eSingular);
};





// строит по русским граммемам набор английских граммем, содержащий только граммемы числа
void TransferPersonGrammems (const CSemNode& RusNode,  CEngSemWord& EngWord)
{
	EngWord.SetFormGrammems(EngWord.GetFormGrammems() & ~eAllPersons);

	if (RusNode.GetGrammems() & _QM( rFirstPerson) )
		EngWord.AddFormGrammem( eFirstPerson);
	else
	if (RusNode.GetGrammems() & _QM( rSecondPerson) )
		EngWord.AddFormGrammem( eSecondPerson);
	else
	   if (RusNode.GetGrammems() & _QM( rThirdPerson) )
		EngWord.AddFormGrammem( eThirdPerson);
};


void TransferGrammems (const CSemNode& RusNode, CEngSemNode& EngNode, string CallingFunc)
{
	TransferTimeGrammems(RusNode, EngNode, CallingFunc);
	if (EngNode.m_MainWordNo != -1)
	{
	  TransferNumberGrammems(RusNode, EngNode.m_Words[EngNode.m_MainWordNo]);
	  TransferPersonGrammems (RusNode, EngNode.m_Words[EngNode.m_MainWordNo]);
  
	};
	// установка сравнительной  степени
	if(RusNode.GetGrammems() & _QM( rComparative))
		EngNode.AddOneGrammemRich(eComparativ);

	// установка превосходной степени
	if(RusNode.GetGrammems() & _QM( rSuperlative))
		EngNode.AddOneGrammemRich(eSupremum);

   // установка одушевленности 
	if (RusNode.HasOneGrammem( rAnimative)  && !RusNode.HasOneGrammem( rNonAnimative))
			EngNode.AddOneGrammemRich( eAnimative);
	else
		if (!RusNode.HasOneGrammem( rAnimative)  && RusNode.HasOneGrammem( rNonAnimative))
			EngNode.DeleteGrammemsRich( _QM( eAnimative)  );

   if ( RusNode.GetGrammems() & ( _QM( rSurName) |  _QM( rName) | _QM( rPatronymic)) )
   {
	   EngNode.AddOneGrammemRich( eProper);
	  
   };
   if ( RusNode.HasOneGrammem(rToponym) )
   {
	   EngNode.AddOneGrammemRich( eGeographics );
	  
   };

   EngNode.DeleteGrammemsRich( eAllGenders );

   if (RusNode.HasOneGrammem(rMasculinum) )
		EngNode.AddOneGrammemRich(eMasculinum);
		else
			if (RusNode.HasOneGrammem(rFeminum) )
			EngNode.AddOneGrammemRich( eFeminum);

   


	
   if ( RusNode.HasPOS(PRONOUN) )// если это русское местоимение
	  {

		// установка падежа 
		  EngNode.DeleteGrammemsRich(  _QM(eNominative) |  _QM(eObjectCase) );
		if (RusNode.HasOneGrammem(rNominativ) )
			EngNode.AddOneGrammemRich( eNominative);
		else
			 EngNode.m_Words[EngNode.m_MainWordNo].AddFormGrammem( eObjectCase);

	  };

};


long ValuePosition (string Position)
{
	if (Position == "begin")
		return -4;
	else
	if (Position == "<")
		return -3;
	else
	if (Position == "!<")
		return -2;
	else
	if (Position == "<<")
		return -1;
    else
	if (Position == ">>>")
		return 1;
    else
	if (Position == ">>")
		return 2;
	else
	if (Position == ">!")
		return 3;
	else
	if (Position == ">")
		return 4;
	else
	if (Position == "end")
		return 5;
	else 
		return 0;

};


string  GetDualPosition (string Position)
{

	if (Position == "begin") // это абсолютное положенние
		return "begin";
	else
	if (Position == "<")
		return ">";
	else
	if (Position == "!<")
		return ">!";
	else
	if (Position == "<<")
		return ">>";
    else
	if (Position == ">>")
		return "<<";
	else
	if (Position == ">!")
		return "!<";
	else
	if (Position == ">")
		return "<";
	else
	if (Position == "end") // Это абсолютное положение 
		return "end";
	else 
		return Position;

};


bool is_past(EngVerbTenseEnum t)
{
	switch(t){
	case past_smp_tn: 
	case past_cnt_tn:
	case past_prf_tn:
	case past_prf_cnt_tn:
		return true;
	}
	return false;
}

//------------------------------------------------------------------------

bool is_future(EngVerbTenseEnum t)
{
	switch(t){
	case future_smp_tn: 
	case future_cnt_tn:
	case future_prf_tn:
	case future_prf_cnt_tn:
		return true;
	}
	return false;
}

bool is_present(EngVerbTenseEnum t)
{
	switch(t){
	case present_smp_tn: 
	case present_cnt_tn:
	case present_prf_tn:
	case present_prf_cnt_tn:
		return true;
	}
	return false;
}

bool is_would(EngVerbTenseEnum t)
{
	switch(t){
	case would_smp_tn:
	case would_prf_tn:
	case would_cnt_tn:
	case would_prf_cnt_tn:
		return true;
	}
	return false;

}

bool is_infinitive(EngVerbTenseEnum t)
{
	switch(t){
		case present_inf_tn :
		case present_cont_inf_tn:
		case prf_inf_tn:
		case prf_cont_inf_tn:
		return true;
	}
	return false;

}

bool is_continous(EngVerbTenseEnum t)
{
	switch(t){
		case present_cnt_tn:
		case past_cnt_tn:
		case future_cnt_tn:
		case present_prf_cnt_tn:
		case past_prf_cnt_tn:
		case future_prf_cnt_tn:
		case would_cnt_tn:
		case would_prf_cnt_tn:
		return true;
	}
	return false;

}

bool is_finite_continous(EngVerbTenseEnum t)
{
	switch(t){
		case present_cnt_tn:
		case past_cnt_tn:
		case future_cnt_tn:
		case present_prf_cnt_tn:
		case past_prf_cnt_tn:
		case future_prf_cnt_tn:
		case would_cnt_tn:
		case would_prf_cnt_tn:
		return true;
	}
	return false;

}

bool is_finite_perfect(EngVerbTenseEnum t)
{
	switch(t){
		case present_prf_tn:
		case past_prf_tn:
		case future_prf_tn:
		case present_prf_cnt_tn:
		case past_prf_cnt_tn:
		case future_prf_cnt_tn:
		case would_prf_tn:
		case would_prf_cnt_tn:
		return true;
	}
	return false;

}


bool is_gerund(EngVerbTenseEnum t)
{
	switch(t){
		case gerund_tn :
		case gerund_prf_tn:
		return true;
	}
	return false;

}

bool is_not_finite_form(EngVerbTenseEnum t)
{
	return		is_infinitive(t) 
			||  is_gerund(t)
			||  (t == pp_tn);
};

EngVerbTenseEnum convert_finite_form_to_infinitive(EngVerbTenseEnum t)
{
	if (is_not_finite_form(t)) return t;

	if (   is_finite_continous(t) 
		&& is_finite_perfect(t)
	   )
	   return  prf_cont_inf_tn;
	else
		if (   is_finite_perfect(t)    )
		  return prf_inf_tn;
		else
			if (   is_finite_continous(t)    )
				return present_cont_inf_tn;
			else
				return present_inf_tn;
};


bool node_is_adv(const CEngSemNode& Node) 
{
	return (Node.GetPos() == eADV); 
	
}

bool _node_is_adj(const CEngSemNode& Node) 
{
	return      (Node.GetPos() == eADJ)
		     ||  (Node.GetPos() == ePN_ADJ);
	
}

void set_possessive (CEngSemWord& EngWord)
{
   if (EngWord.m_Word.size() == 0) return;
   if  (EngWord.m_Poses == (1 << eNOUN))
   {
	   if (    (EngWord.m_Word[EngWord.m_Word.size() - 1] == 's')
			|| (EngWord.m_Word[EngWord.m_Word.size() - 1] == 'S')
		  )
		  EngWord.m_Word += "'";
	   else
		  EngWord.m_Word += "'s";
   }
   else
   {
	   if (EngWord.HasOneGrammem(ePersonalPronoun) )
		   EngWord.m_Word = GetPronounEnglishFormByGrammems(EngWord.GetAllGrammems(), false);
   };
};



