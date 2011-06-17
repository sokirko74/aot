#pragma warning(disable:4786)
#include  "GerSynan.h"
#include  "GerSentence.h"

const size_t TrennbarePraefixeCount = 31;
const string TrennbarePraefixe[TrennbarePraefixeCount] = {
"AB",		"AN",		"AUF",		"AUS",		"BEI",		"DAR",		"DURCH",	"EIN",		
"FEHL",		"FEST",		"HEIM",		"HINZU",	"IRRE",		"LOS",		"MIT",		"NACH",			
"PREIS",	"STAND",	"STATT",	"TEIL",		"UEBER",	"UM",		"UNTER",	"VOR",		
"WETT",		"WIDER",	"WUNDER",	"ZU",		"ZURECHT",	"ZURÜCK",	"ZUSAMMEN"};




void DeleteAllTrennbarePraefixeHomonyms(CGerSentence& C)
{
	for (int WordNo = 0;  WordNo < C.m_Words.size(); WordNo++)
	{
		CSynWord& W = C.m_Words[WordNo];
		if (W.m_Homonyms.size() <= 1) continue;

		int ZUSHomonymNo = W.GetHomonymByPOS(gZUS);
		if (ZUSHomonymNo == -1) continue;

		W.EraseHomonym(ZUSHomonymNo);
		
	};

	
};


struct CTrennbarPrefixHyp 
{
	CWordAndHomonym	m_MainWord;
	int				m_PrefixWordNo;
	bool	operator < (const CTrennbarPrefixHyp& X) const
	{
		return Distance() < X.Distance();
	};

	int Distance() const
	{
		assert (m_PrefixWordNo > m_MainWord.m_WordNo);
		return m_PrefixWordNo - m_MainWord.m_WordNo;
	};
};

void CGerSentence::BuildTrennbarePraefixe()
{
	vector<CTrennbarPrefixHyp> Hypots;

	for (int PrefixWordNo = m_Words.size()-1;  PrefixWordNo >= 0; PrefixWordNo--)
		if	(		binary_search(TrennbarePraefixe, TrennbarePraefixe+TrennbarePraefixeCount, m_Words[PrefixWordNo].m_strUpperWord)
				&&	(		(PrefixWordNo == m_Words.size()-1)
                        ||	m_Words[PrefixWordNo+1].HasDes(OPun)
						||	(m_Words[PrefixWordNo+1].GetHomonymByPOS(gPRP) != -1)
					)
			)
		{
			string Praefix = m_Words[PrefixWordNo].m_strWord;

			// going to the left, searching for the main verb
			
			for (int MainWordNo = PrefixWordNo-1;  MainWordNo >= 0; MainWordNo--)
			{
				CSynWord& MainWord = m_Words[MainWordNo];
				
				for (int j = 0;  j < MainWord.m_Homonyms.size(); j++)
					if (	MainWord.m_Homonyms[j].HasPos(gVER)	)
					{
						string  NewWord = Praefix+MainWord.m_strWord;
						// checking, if morphology has such a verb
						vector<CFormInfo> Paradigms;
						GetOpt()->GetLemmatizer()->CreateParadigmCollection(false,NewWord, true, false, Paradigms);
						if (Paradigms.empty()) continue;

						if (		(MainWordNo != 0) // to prohibit "dem auswartigen Dienst aus"
								&&	(MainWord.m_Homonyms.size() > 1)
								&&	(MainWord.m_Register != LowLow)
							)
							continue;

	
						CTrennbarPrefixHyp H;
						H.m_MainWord.m_WordNo = MainWordNo;
						H.m_MainWord.m_HomonymNo = j;
						H.m_PrefixWordNo = PrefixWordNo;
						Hypots.push_back(H);
						
					};
			};

		};


	sort (Hypots.begin(), Hypots.end());

	for (size_t i=0; i<Hypots.size(); i++)
	{
		const CTrennbarPrefixHyp& H = Hypots[i];
		CSynWord& MainWord = m_Words[H.m_MainWord.m_WordNo];

		string Praefix = m_Words[H.m_PrefixWordNo].m_strWord;
		MainWord.m_TrennbarePraefixWordNo = H.m_PrefixWordNo;
		MainWord.m_Homonyms[H.m_MainWord.m_HomonymNo].SetLemma(Praefix+MainWord.m_Homonyms[H.m_MainWord.m_HomonymNo].m_strLemma);
        MainWord.SetWordStr(Praefix + "-" +MainWord.m_strWord, GetOpt()->m_Language);

		MainWord.SetAllOtherHomsDel(H.m_MainWord.m_HomonymNo);
		MainWord.DeleteMarkedHomonymsBeforeClauses();

		MarkWordAsDeleted(H.m_PrefixWordNo);

		for (size_t k=i+1; k<Hypots.size(); k++)
		{
			if	(		(H.m_PrefixWordNo ==  Hypots[k].m_PrefixWordNo)
					||	(H.m_MainWord.m_WordNo ==  Hypots[k].m_MainWord.m_WordNo)	
				)
			{
				Hypots.erase(Hypots.begin() +k);
				k--;
				continue;
			}
			if (Hypots[k].m_PrefixWordNo >  H.m_PrefixWordNo)
				Hypots[k].m_PrefixWordNo--;

			if (Hypots[k].m_MainWord.m_WordNo >  H.m_MainWord.m_WordNo)
				Hypots[k].m_MainWord.m_WordNo--;

		};
	};



	// we delete all homonyms of trennbare Praefixe, for which we cannot find
	// the main verb
	DeleteAllTrennbarePraefixeHomonyms(*this);
};


// finds alls Right Analytical Parts (RAP) of the sentence:
// machen
// gemacht
// gemacht haben
// gefahren sein
void FindAllRAPs(const CGerSentence& C, vector< CPeriod >&  RAPs)
{
	RAPs.clear();

	for (int WordNo=0; WordNo<C.m_Words.size(); WordNo++)
	{
		const CSynWord& W = C.m_Words[WordNo];
		if (C.FindFirstAuxVerb(WordNo) != -1) continue;
		// if it is an infintive 
		int InfinitiveHomonymNo = W.GetHomonymByPOSandGrammem(gVER, gInfinitiv);
		if (InfinitiveHomonymNo != -1)
		{
			//  all infinitives terminate with "N"
			assert(W.m_strUpperWord[W.m_strUpperWord.length()-1] == 'N');
		};
		// if it is a participle
		int PA2HomonymNo = W.GetHomonymByPOSandGrammem(gPA2, gPredikBenutz);

		// neither participle nor infinitive, go further
		if	(		(PA2HomonymNo == -1) 
				&&	(InfinitiveHomonymNo == -1)
			)
			continue;

		int last_word =  WordNo;
		if (PA2HomonymNo != -1)
			if (WordNo + 1 < C.m_Words.size())
				if	(		C.m_Words[WordNo+1].IsWordUpper("HABEN") 
						||	C.m_Words[WordNo+1].IsWordUpper("SEIN")
						||	C.m_Words[WordNo+1].IsWordUpper("WORDEN")
					)
					last_word++;

		// if there is an aux. infinitive after the last word of RAP, then it it NOT
		// a RAP for example:
		// ich kann geschadet worden sein. 
		if (last_word + 1 < C.m_Words.size())
			if	(		C.m_Words[last_word+1].IsWordUpper("HABEN") 
					||	C.m_Words[last_word+1].IsWordUpper("SEIN")
				)
			{
				WordNo = last_word;
				continue;
			};

		if (!C.CheckLastPredicatePosition(C.m_Words.size() - 1, last_word))
		{
			// A RAP should be at the end of a clause
			// "jeder würde eine Summe ausgezahlt erhalten."
			continue;
		};

		RAPs.push_back(CPeriod(WordNo, last_word));
		WordNo = last_word;
	};

};

struct CDividedUnionInfo 
{
	int		m_LeftPartWordNo;
	int		m_LeftPartHomonymNo;
	int		m_RightPartHomonymNo;
	bool	m_bPassive;
	CPeriod m_RAP;

	bool operator < (const CDividedUnionInfo& X) const 
	{
		return m_LeftPartWordNo < X.m_LeftPartWordNo;
	};
	int Distance () const 
	{
		return m_RAP.m_iFirstWord - m_LeftPartWordNo;
	};
};

bool IsLessByDistance (const CDividedUnionInfo& _X1,  const CDividedUnionInfo& _X2)
{
	return _X1.Distance() < _X2.Distance();
};

bool CheckLeftAndRightPart(const CGerSentence& C, CDividedUnionInfo& Info)
{
	const CSynHomonym& AuxVerb = C.m_Words[Info.m_LeftPartWordNo].m_Homonyms[Info.m_LeftPartHomonymNo];


	const CSynWord& W = C.m_Words[Info.m_RAP.m_iFirstWord];

	int InfinitiveHomonymNo = W.GetHomonymByPOSandGrammem(gVER, gInfinitiv);
	int PA2HomonymNo = W.GetHomonymByPOSandGrammem(gPA2, gPredikBenutz);
	assert (InfinitiveHomonymNo != -1 || PA2HomonymNo != -1);
	Info.m_bPassive = false;

	if (PA2HomonymNo == -1)
	{
		// ich werde gehen

		//  the following example should be preprocessed by BuildCompactAnalyticalForms:
		// "Er wird geschadet haben" 
		// "Er wird gefahren sein"

		if (!AuxVerb.IsLemma("WERDEN")) return false;

		//  prohibit a second uniting with "werden", if there is a comma between them
		if (		W.m_Homonyms[InfinitiveHomonymNo].IsLemma("WERDEN") 
				&&	!W.m_MainVerbs.empty()
			) 
		{
			for (size_t i=Info.m_LeftPartWordNo+1; i< Info.m_RAP.m_iFirstWord; i++)
				if (C.m_Words[i].m_bComma)
					return false;

			if (W.m_Homonyms[InfinitiveHomonymNo].m_bPassive)
				Info.m_bPassive = true;
		};

		
		Info.m_RightPartHomonymNo = InfinitiveHomonymNo;
		return true;
	}
	else
		if (InfinitiveHomonymNo == -1)
		{
			

			if (Info.m_RAP.size() == 2)
			{
				if (	C.m_Words[Info.m_RAP.m_iLastWord].IsWordUpper("WORDEN") )
				{	
					//  "ich bin gefragt worden"
					//  "ich war gefragt worden"
					if (!AuxVerb.IsLemma("SEIN")) return false;
					Info.m_bPassive = true;
				};
			}
			else
			{
				assert (Info.m_RAP.size() == 1);
				// ich bin gefahren
				// ich habe geweint
				// ich werde gefragt

				if (C.m_Words[Info.m_LeftPartWordNo].m_strUpperWord == "SEIN")
				{
					//  we should not connect wordform "sein" to PA2
					// since otherwise there are too many errors:
					// "er hat für sein Konzept geworben."
					return false;
				};

				if (AuxVerb.IsLemma("WERDEN"))
					Info.m_bPassive = true;

			}

			Info.m_RightPartHomonymNo = PA2HomonymNo;
			return true;
		}
		else 
		{
			//  there are homonyms for PA2 and for infinitives.
			//  Such cases are nor processed, for us it is too difficult, for example:
			// "Ich werde erhalten"
			return false; 
		};

	
}
bool UniteLeftAndRightPart(CGerSentence& C, const CDividedUnionInfo Info)
{

	assert (Info.m_RAP.size() <= 2);
	CSynHomonym& AuxVerb = C.m_Words[Info.m_LeftPartWordNo].m_Homonyms[Info.m_LeftPartHomonymNo];
	CSynWord& W = C.m_Words[Info.m_RAP.m_iFirstWord];
	
	//  deleting all homonyms except auxiliary verb
	CSynWord& AuxWord = C.m_Words[Info.m_LeftPartWordNo];
	AuxWord.SetAllOtherHomsDel(Info.m_LeftPartHomonymNo);
	AuxWord.DeleteMarkedHomonymsBeforeClauses();
	assert(AuxWord.m_Homonyms.size() == 1);
	AuxWord.m_MainVerbs.push_back( Info.m_RAP.m_iLastWord );	
	if (Info.m_RAP.size() == 2)
		C.m_Words[Info.m_RAP.m_iLastWord].m_MainVerbs.push_back( Info.m_RAP.m_iFirstWord);


	W.SetAllOtherHomsDel(Info.m_RightPartHomonymNo);
	W.DeleteMarkedHomonymsBeforeClauses();
	W.m_Homonyms[0].m_bPassive = Info.m_bPassive;

	string dump = AuxWord.m_strWord + " " +W.m_strWord;
	if (Info.m_RAP.size() == 2)
		dump += " "+C.m_Words[Info.m_RAP.m_iLastWord].m_strWord;
	rml_TRACE("divided analytical form \"%s\" was created\n", dump.c_str());

	return true;
};


int GetAuxVerbHomonym(const CSynWord& W)
{
	if (!W.m_MainVerbs.empty())
		return -1;

	for (int i=0; i<W.m_Homonyms.size(); i++)
	{
		const CSynHomonym& H = W.m_Homonyms[i];
		if (H.HasPos(gVER))	
		if (		H.IsLemma("SEIN") 
				||	H.IsLemma("WERDEN") 
				||	H.IsLemma("HABEN") 
			)
			return i;
	};

	return -1;
};

/*
This procedure finds all analytical forms in the sentence and moves the right part of the analyytical
form to the left, for example:

Ich habe das gemacht => Ich habe-gemacht das.
Ich werde das machen => Ich werde-machen das.
The procedure goes from the right to the left and finds all infinitives without "zu" or particple II.    
Such sequence of words are calles Right Analytical Part (RAP). Then it goes to the left and finds
appropriate Left Analytical Part (LAP), which can be a finite form of "werden", "haben" or "sein".
Modal verbs are not considered. The only one heuristics which is used is as follows:
a finite form of "werden", "haben" or "sein" cannot be LAP if there is a comma after it, and between 
LAP and RAP this comma is the only comma.  It is done for:
	"Ich habe das Auto, das sehr schmutzig war, gefahren."
*/
void BuildDividedAnalyticalForms(CGerSentence &C)
{
	vector< CPeriod >  RAPs;
	FindAllRAPs(C, RAPs);
	vector<CDividedUnionInfo > UnionInfos;

	for (int RapNo=RAPs.size()-1; RapNo>=0; RapNo--)
	{
		int CountOfComma = 0;

		for (int k = RAPs[RapNo].m_iFirstWord-1; k>=0; k--)
		{
			if (C.m_Words[k].m_bComma)
			{
				CountOfComma++;
				continue;
			}
			if	(		(CountOfComma == 1) 
					&&	C.m_Words[k+1].m_bComma 
				)
				continue;


			CDividedUnionInfo Info;
			Info.m_LeftPartHomonymNo = GetAuxVerbHomonym(C.m_Words[k]);
			Info.m_LeftPartWordNo = k;
			Info.m_RAP = RAPs[RapNo];

			//  no appropriate finite  form found, then continue searching...
			if (Info.m_LeftPartHomonymNo == -1) continue;

			
			if (CheckLeftAndRightPart(C,  Info) )
			{
				//  if there is a possible solution for the current auxiliary verb, 
				// but the current RAP is closer to the auxiliary verb than
				// the RAP of the existing solution, then replace  this solution
				// with the current one
				vector<CDividedUnionInfo >::iterator it = lower_bound(UnionInfos.begin(), UnionInfos.end(), Info);
				if	(		(it != UnionInfos.end())
						&&	(it->m_LeftPartWordNo == Info.m_LeftPartWordNo)
						&&	(it->Distance() > Info.Distance())
					)
					*it = Info;
				else
					if (		(it == UnionInfos.end())
							||	(it->m_LeftPartWordNo != Info.m_LeftPartWordNo)
						)
					UnionInfos.insert(it, Info);
			};
		
		};

	};
	
	sort (UnionInfos.begin(), UnionInfos.end(), IsLessByDistance );

	//  building divided forms
	for (size_t i=0; i < UnionInfos.size(); i++)
	{
		UniteLeftAndRightPart(C, UnionInfos[i]);
		CPeriod UsedPeriod = UnionInfos[i].m_RAP;

		//  since it->m_RAP  was deleted we should update indexes
		
		for (size_t k=i+1; k < UnionInfos.size(); k++)
		{
			CDividedUnionInfo& Info = UnionInfos[k];

			assert ( Info.m_LeftPartWordNo != UnionInfos[i].m_LeftPartWordNo);
			/*if (Info.m_LeftPartWordNo >  UsedPeriod.m_iLastWord)
				Info.m_LeftPartWordNo -= UsedPeriod.size() - 1;
			*/
			if	(		CPeriod(Info.m_RAP.m_iFirstWord).is_part_of(UsedPeriod)
					||	CPeriod(Info.m_RAP.m_iLastWord).is_part_of(UsedPeriod)
				)
			{
				UnionInfos.erase(UnionInfos.begin() + k);
				k--;
				continue;
			};

			/*if (Info.m_RAP.m_iFirstWord >  DeletedPeriod.m_iLastWord)
				Info.m_RAP.m_iFirstWord -= DeletedPeriod.size() - 1;

			if (Info.m_RAP.m_iLastWord >  DeletedPeriod.m_iLastWord)
				Info.m_RAP.m_iLastWord -= DeletedPeriod.size() - 1;
			*/
		};
	};

};



bool CheckNormalCompactAnalyticalForm(CGerSentence &C,  int WordNo, int& MainVerbWordNo)
{
	MainVerbWordNo = -1;
	int h = GetAuxVerbHomonym(C.m_Words[WordNo]);
	if (h == -1) return false;
	C.m_Words[WordNo].SetAllOtherHomsDel(h);

	CSynWord& W = C.m_Words[WordNo - 1];
	int InfinitiveHomonymNo = W.GetHomonymByPOSandGrammem(gVER, gInfinitiv);
	int PA2HomonymNo = W.GetHomonymByPOSandGrammem(gPA2, gPredikBenutz);
	if	 (		(InfinitiveHomonymNo == -1) 
			&&	(PA2HomonymNo == -1)
		)
	{
		C.m_Words[WordNo].SetHomonymsDel(false);
		return false;
	};

	W.SetHomonymsDel(true);
	if (InfinitiveHomonymNo	!= -1) 
		W.m_Homonyms[InfinitiveHomonymNo].m_bDelete = false;
	if (PA2HomonymNo	!= -1) 
		W.m_Homonyms[PA2HomonymNo].m_bDelete = false;

	//  PA2HomonymNo_2 is a homonym of a PA2 in a word which stands in 2 words from WordNo to the left
	int  PA2HomonymNo_2 = -1;
	if ( WordNo-2 >= 0 )
		PA2HomonymNo_2 = C.m_Words[WordNo-2].GetHomonymByPOSandGrammem(gPA2, gPredikBenutz);
		

	const CSynHomonym& H = C.m_Words[WordNo].m_Homonyms[h];

	if (		H.IsLemma("WERDEN")
			&&	(InfinitiveHomonymNo	!= -1) 
			&&	!W.IsWordUpper("WORDEN")
		)
	{
		
		if (		(		W.IsWordUpper("HABEN") 
						||	W.IsWordUpper("SEIN") 
					)
				&&  (PA2HomonymNo_2 != -1)
			)
		{
			// dass ich gefragt haben werde.
			// "dass er gefahren sein wird "
			MainVerbWordNo = WordNo-2;
		}
		else
		{
			// dass ich fragen werde.
			MainVerbWordNo = WordNo-1;
		};
		


	}
	else
	{
		if	(		H.IsLemma("SEIN")
				&&	W.IsWordUpper("WORDEN")	
				&&	(PA2HomonymNo_2 != -1)
			)
		{
			// dass ich gefragt worden bin
			// dass ich gefragt worden war
			MainVerbWordNo = WordNo-2;
			C.m_Words[WordNo-2].m_Homonyms[PA2HomonymNo_2].m_bPassive = true;
		}
		else
		if (PA2HomonymNo != -1)
		{
			// dass ich gefahren bin 
			// dass ich geweint habe 
			
			MainVerbWordNo = WordNo-1;

			// dass ich gefragt werde 
			if (H.IsLemma("WERDEN"))
				C.m_Words[WordNo-1].m_Homonyms[PA2HomonymNo].m_bPassive = true;

		}
		

	};

	if (MainVerbWordNo != -1)
	{
		C.m_Words[WordNo].DeleteMarkedHomonymsBeforeClauses();
		C.m_Words[MainVerbWordNo].DeleteMarkedHomonymsBeforeClauses();
		return true;
	}
	else
	{
		C.m_Words[WordNo].SetHomonymsDel(false);
		return false;
	};

};


/*
 processing simple cases of coordinating cases like 
  "geleistet und sich bewährt haben."
  "gehen oder kommen werden"
  We go the  left, pass a reflexive pronoun, pass a coordinating conjunction,
  build a new sentence from the first verb and the auxiliary verb sequence,
  if  it can be united then unite it. 

  This rule is more restructive to the position and the distances between coordinating verbs 
  than  BuildCoordinatedAuxVerb,  but BuildCoordinatedAuxVerb deals only with PA2.
*/
void SimilarCaseForNormalCompactAnalyticalForms(CGerSentence &C, int StartWordNo, int LastWordNo)
{
	
	int Similar = StartWordNo -1;

	if (Similar>0)
	{
		if (C.m_Words[Similar].GetHomonymByPOS(gPRONOMEN) != -1)
			Similar--;
		if (		(Similar-1 >= 0)
				&&	C.m_Words[Similar].m_bSimilarConj
			)
		{
			CGerSentence TestSent(C.GetOpt());
			assert (Similar-1 >= 0);
			CSynWord& SimilarWord = C.m_Words[Similar-1];
			TestSent.m_Words.push_back(SimilarWord);
			for (size_t i=StartWordNo+1; i <= LastWordNo; i++)
				TestSent.m_Words.push_back(C.m_Words[i]);
			TestSent.BuildAnalyticalForms();
			if (!TestSent.m_Words.back().m_MainVerbs.empty())
				C.m_Words[LastWordNo].m_MainVerbs.push_back( Similar-1 );
		};
	};
	
};
/*
This function uniting sequences lik e
			 gefahren bin 
			 geweint habe 
			 gefragt worden bin
	 The last word is auxiliary verb. The first verb is the main verb.
	 Theis function transfers all appropriate homonyms from the last verb to the first. Note
	 that it can more than one, for example:
		"gefallen ist"
	After it it builds morphology of the last verb and deletes all other words which the 
	analytical form contais.
*/
void UnitNormalCompactAnalyticalForms(CGerSentence &C, int StartWordNo, int LastWordNo)
{
	SimilarCaseForNormalCompactAnalyticalForms(C, StartWordNo, LastWordNo);

	// to be sure that all homonyms of "sein" were deleted by DeleteMarkedHomonymsBeforeClauses
	assert(C.m_Words[LastWordNo].m_Homonyms.size() == 1);

	// setting m_MainVerbNo
	string dump = C.m_Words[LastWordNo].m_strWord;
	for (int i = LastWordNo; i>StartWordNo;i--)
	{
		C.m_Words[i].m_MainVerbs.push_back(i-1);
		dump = C.m_Words[i-1].m_strWord+ " "+dump;

	};
	rml_TRACE("compact analytical form \"%s\" was created\n", dump.c_str());
	

};

/*
	This function checks if WordNo starts an "anomalous" perfect group. By "anomalous" we mean that
	the finite auxiliary verb "haben" is the first word of an analytical form, not the last.
	For example:
	Ich weiss, dass du die Gefahr hast kommen sehen 
*/
bool CheckAnomalousCompactAnalyticalForm(CGerSentence &C,  int WordNo, int& MainVerbWordNo)
{
	// the first verb should be "haben"
	int h = GetAuxVerbHomonym(C.m_Words[WordNo]);
	if (h == -1) return false;
	if (!C.m_Words[WordNo].m_Homonyms[h].IsLemma("HABEN")) return false;

	// the third verb should be with mark CSynWord::m_bPerfectAnomalie
	if (WordNo+2 >= C.m_Words.size()) return false;
	MainVerbWordNo = WordNo+2;
	CSynWord& W = C.m_Words[MainVerbWordNo];
	int i=0;
	for (; i<W.m_Homonyms.size(); i++)
		if (W.m_Homonyms[i].m_bPerfectAnomalie)
			break;
	if (i == W.m_Homonyms.size()) return false;

	// the second verb should be infinitive
	if (C.m_Words[WordNo+1].GetHomonymByPOSandGrammem(gVER, gInfinitiv) == -1) return false;

	// for sure  we delete all other homonyms though it is hard to invent an example with ambiguity
	C.m_Words[WordNo].SetAllOtherHomsDel(h);
	C.m_Words[WordNo].DeleteMarkedHomonymsBeforeClauses();

	C.m_Words[MainVerbWordNo].SetAllOtherHomsDel(i);
	C.m_Words[MainVerbWordNo].DeleteMarkedHomonymsBeforeClauses();
	return true;
};

/*
	this function  units analytical forms like:
		dass du die Gefahr hast kommen sehen 
		"Ich weiss, dass du das Problem hast loesen  koennen"
*/
void UnitAnomalousCompactAnalyticalForms(CGerSentence &C, int StartWordNo, int LastWordNo)
{
	assert(StartWordNo +2 == LastWordNo);
	assert(C.m_Words[StartWordNo].m_Homonyms.size() == 1);
	assert(C.m_Words[StartWordNo].m_Homonyms[0].IsLemma("HABEN"));
	assert(C.m_Words[LastWordNo].m_Homonyms.size() == 1);
	assert(C.m_Words[LastWordNo].m_Homonyms[0].m_bPerfectAnomalie);
	C.m_Words[StartWordNo].m_MainVerbs.push_back( LastWordNo );
	for (int i = LastWordNo; i>StartWordNo+1; i--)
		C.m_Words[i].m_MainVerbs.push_back(  i-1 );
};


/*
	this procedure builds analytical forms which are not divided by other words
	Such sequences can occurr in subclauses or with modal verbs, for example:

	  Er weiss dass ich gefragt worden bin.
	  Er weiss dass ich gefragt wurde.
	  Ich weiss, dass dein Mantel im Restaurant haengen geblieben ist.
	  
	  Ich weiss, dass du die Gefahr hast kommen sehen (Perfect Anomalie)
	
*/
void BuildCompactAnalyticalForms(CGerSentence &C)
{
	int WordNo;
	// first we should search for an anomalous perfect construction, since it is very  special
	for (WordNo=0; WordNo<C.m_Words.size(); WordNo++)
	{
		int MainVerbWordNo;
		if (CheckAnomalousCompactAnalyticalForm(C, WordNo, MainVerbWordNo))
			UnitAnomalousCompactAnalyticalForms(C,WordNo, MainVerbWordNo);
	};

	// then we should search for a normal compact analytical forms
	for (WordNo=C.m_Words.size()-1; WordNo>0; WordNo--)
	{
		int MainVerbWordNo;
		if (CheckNormalCompactAnalyticalForm(C, WordNo, MainVerbWordNo))
			UnitNormalCompactAnalyticalForms(C,MainVerbWordNo, WordNo);
	};

};

/*
this function finds a word "zu" which is followed by infinitive, then it moves "zu" to the infinitive.
and deletes all Person, Number  and Tense information and adds grammem gZuVerbForm.
So it makes "zu machen" morphologically equal to "einzugehen".
*/
void BuildZuForms(CGerSentence &C)
{
	// first we should search for an anomalous perfect construction, since it is very  special
	for (int WordNo=0; WordNo+1<C.m_Words.size(); WordNo++)
	{
		if (!C.m_Words[WordNo].IsWordUpper("ZU")) continue;
		CSynWord& W = C.m_Words[WordNo+1];
		if (W.m_Register != LowLow) continue;
		int InfinitiveHomonymNo = W.GetHomonymByPOSandGrammem(gVER, gInfinitiv);
		if (InfinitiveHomonymNo == -1) continue;

		//  all infinitives terminate with "N"
		assert(W.m_strUpperWord[W.m_strUpperWord.length()-1] == 'N');

		if (W.m_Homonyms.size() > 1)
		{
			/*
			 In ambigious cases  an infinitive should be at the end of the sentence, or it must be followed by
			 a comma, a similar conjunction or an auxiliary verb.
			 Otherwise we cannot safely convert it to  "compound" infinitive. For example,
				"zu meinen alten Büchern"
			 Here  "meinen" can be an infinitive, but the program pass it.
			*/
			if	(		(WordNo+2 < C.m_Words.size())
					&&	(GetAuxVerbHomonym(C.m_Words[WordNo+2]) == -1)
                    &&	!C.m_Words[WordNo+2].HasDes(OPun)
					&&	!C.m_Words[WordNo+2].m_bSimilarConj
				)
				continue;
		};

		W.SetAllOtherHomsDel(InfinitiveHomonymNo);
		W.DeleteMarkedHomonymsBeforeClauses();
		W.m_strWord =  "zu-"+W.m_strWord;

		QWORD g = (gAllVerbClasses & W.m_Homonyms[0].m_iGrammems) |  _QM(gZuVerbForm);
		if (!C.GetGerGramTab()->GetGramCodeByGrammemsAndPartofSpeechIfCan(gVER, g, W.m_Homonyms[0].m_GramCodes))
		{
			string debug = C.GetOpt()->GetGramTab()->GrammemsToStr(g);
			ErrorMessage("Cannot find Gramtab line  for "+ debug);
		};
		C.InitHomonymMorphInfo(W.m_Homonyms[0]);
		

		C.MarkWordAsDeleted(WordNo);
	};


};

//  this function finds all PA2 which were not included into an analytical form,
// it is followed by a conjunction, it goes to the right till the first analytical form
//  and borrows the grammar from the found analytical form.
void BuildCoordinatedAuxVerb(CGerSentence &C)
{
	for (size_t WordNo=0; WordNo+1<C.m_Words.size(); WordNo++)
	{
		CSynWord& W = C.m_Words[WordNo];
		int PA2HomonymNo = W.GetHomonymByPOSandGrammem(gPA2, gPredikBenutz);
		if (PA2HomonymNo == -1) continue;
		if (!C.m_Words[WordNo+1].m_bSimilarConj) continue;

		
		for (int AnalytForm=WordNo+2; AnalytForm<min(WordNo+10, C.m_Words.size()); AnalytForm++)
		{
			vector<int>& v = C.m_Words[AnalytForm].m_MainVerbs;
			if (!v.empty())
				if (find(v.begin(),v.end() , WordNo) == v.end() )
				{
					// we should check that W has the same properties as the main part of the 
					// found analytical form, but right now we have not enough  information for this check
					v.push_back(WordNo);
					W.SetAllOtherHomsDel(PA2HomonymNo);
					W.DeleteMarkedHomonymsBeforeClauses();
					break;
				};
		};
	};


};


void CGerSentence::BuildAnalyticalForms()
{
	// "I habe das zu erhalten." Here "erhalten" can be morphologically PA2, 
	// but it is an infinitive and we should not unit it with "habe",
	// that's  why we should first call BuildZuForms
	BuildZuForms(*this);


	BuildCompactAnalyticalForms(*this);
	BuildDividedAnalyticalForms(*this);
	BuildCoordinatedAuxVerb(*this);

	
}


