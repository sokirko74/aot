#include "stdafx.h"
#include "SemanticRusStructure.h"

struct CProductivePrefix 
{
	const char* m_Prefix;
	const char* m_GraDescr;
};

const int PrefixesCount = 2;
const CProductivePrefix Prefixes[PrefixesCount] = { {"����", "����"},{"����", "����"} };


poses_mask_t CSemanticStructure::GetPosesFromRusArticle(CRossHolder& Ross,WORD UnitNo) const
{
	StringVector GramFets;
    Ross.GetSimpleFieldItemsFromArticle (UnitNo, "GF", 0, 0, GramFets);
	if (GramFets.size() == 0) return 0;
	long ret_poses = 0;
	for (long i=0; i < GramFets.size(); i++)
	{
     string S = GramFets[i].substr(0, GramFets[i].find_first_of(":"));
	 
	 if (S == "���")	 
		 ret_poses |=  (1 << NOUN);
	 else
     if (S == "��") 	 
		 ret_poses |= (1<<PRONOUN);
	 else
    
     if ((S == "�������") || (S == "��-�")  || (S == "����_�����") )
		 ret_poses |= (1 << PRONOUN_P);
	 else
     if (S == "���")
		 ret_poses |=  (1 << ADV);
	 else
      if (S == "����")
		  ret_poses |= (1 << PARTICLE);
	 else
     if (S == "�����")
		  ret_poses |= (1 << PREDK)  | (1 << PRONOUN_PREDK)	 ;
	  else
      if (S == "��")
		  ret_poses |= (	(1 << VERB) |
							(1 << ADVERB_PARTICIPLE) |
							(1 << PARTICIPLE) |
							(1 << PARTICIPLE_SHORT) |
							(1 << INFINITIVE) );
	  else
      if (S == "����")
		 ret_poses |=	( (1<<ADJ_FULL) 
						| (1 << ADJ_SHORT)
						| (1 << PARTICIPLE)
						| (1 << PARTICIPLE_SHORT)
						);
	 else
	 if (S == "����")		  
		ret_poses |= ((1<<NUMERAL_P) | (1<<NUMERAL) );		
	 else
	 {
		 ErrorMessage (string("Unknown POS ")+string(GramFets[i])
			 + string(" in article ") 
			 + string(Ross.GetRoss()->GetEntryStr(UnitNo)));

		 #ifdef  DEBUG
		  throw CExpc ("Unknown POS ");
		 #endif

		 return 0;
	 };
	}
	return ret_poses;
}

bool CRusSemStructure::VerbCanBeWithoutReflexiveSuffix(string Lemma) const
{
   if (!HasReflexiveSuffix(Lemma)) return false;
   Lemma.erase(Lemma.length() - 2);
   const CLemmatizer* P = m_pData->GetRusLemmatizer(); 
   

   vector<CFormInfo> ParadigmCollection;
   P->CreateParadigmCollection(true,	Lemma, false, false, ParadigmCollection);

   for (int i =0; i < ParadigmCollection.size(); i++)
   {
	   string GramCodes = ParadigmCollection[i].GetAncode(0);
	   long POS = m_pData->GetRusGramTab()->GetPartOfSpeech(GramCodes.c_str());
	   if (POS == INFINITIVE) return true;
   };
   return false;

};
string CRusSemStructure::GetNormOfParticiple (const CRusSemWord& W) const 
{
	QWORD Grammems = _QM(rMasculinum) | _QM(rSingular) | _QM(rNominativ);
	if (W.HasOneGrammem(rActiveVoice))
		Grammems |= _QM(rActiveVoice);
	if (W.HasOneGrammem(rPassiveVoice))
		Grammems |= _QM(rPassiveVoice);
	// ������ �� ������� ����� "����������" ����� "�������������"
    if (W.HasOneGrammem(rPastTense) || W.HasPOS(PARTICIPLE_SHORT))
		Grammems |= _QM(rPastTense);
	else
	if (W.HasOneGrammem(rPresentTense))
		Grammems |= _QM(rPresentTense);
	if (W.m_ParadigmId == -1) return "error_!";
	CFormInfo Paradigm;
	if (!m_pData->GetRusLemmatizer()->CreateParadigmFromID(W.m_ParadigmId, Paradigm))
		return "error_!";

    
	long k=0;
	for (; k < Paradigm.GetCount(); k++)
	{
		string  AnCode = Paradigm.GetAncode(k);
		QWORD currGrammems;
		m_pData->GetRusGramTab()->GetGrammems(AnCode.c_str(), currGrammems);
		if ( (Grammems & currGrammems) == Grammems)
				  break;
	};
	if (k == Paradigm.GetCount()) return "error_!";

	return Paradigm.GetWordForm(k);
};




WORD CRusSemStructure::GetInterpWithoutPrefix (CRusSemWord& W) const
{
	for (long i=0; i < PrefixesCount; i++)
	  if  (W.m_Lemma.substr(0, strlen(Prefixes[i].m_Prefix)) == Prefixes[i].m_Prefix) 
	  {
		 string UnitStr=  W.m_Lemma.substr(strlen(Prefixes[i].m_Prefix));
		 EngRusMakeLower(UnitStr);
		 WORD UnitNo  = GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(), 1);
		 if (UnitNo == ErrUnitNo)  continue;

		 DWORD ParadigmId = m_pData->GetFirstParadigmId(morphRussian, UnitStr,  W.m_Poses);
		 if (ParadigmId == UnknownParadigmId) continue;
 		 W.m_Lemma =  UnitStr;
		 W.m_GraphDescrs += " "+string(Prefixes[i].m_GraDescr);
		 W.m_ParadigmId = ParadigmId;
		 W.m_Word.erase(0, strlen(Prefixes[i].m_Prefix));
		 return UnitNo; 
	  };
  return ErrUnitNo;
};

/*
������� ������ ��������� ��������������:
  ����������� -> �������
  ������������ -> ��������
  ��������� -> ��������
  �������������� -> ������������
*/
bool CRusSemStructure::BuildWordWithoutFemineSuffix (string& Word) const
{
	string NewWord;
	if (  (Word.length() > 8)
	    &&(Word.substr(Word.length()-8) == "��������")
	   )
		NewWord = Word.substr(0,Word.length()-4);
	else
	if (  (Word.length() > 4)
	    &&(Word.substr(Word.length()-4) == "����")
	   )
		NewWord = Word.substr(0,Word.length()-2) + "�";
	else
	if (  (Word.length() > 3)
	    &&(Word.substr(Word.length()-3) == "���")
	   )
		NewWord = Word.substr(0,Word.length()-2) + "�";
	else
	if (  (Word.length() > 4)
	    &&(Word.substr(Word.length()-4) == "����")
	   )
		NewWord = Word.substr(0,Word.length()-2);
	else
		return false;
	
    if (m_pData->GetFirstParadigmId(morphRussian, NewWord, 1<<NOUN) == -1)
	return false;

	Word = NewWord;
	return true;
};
 
void CRusSemStructure::InitInterps (CRusSemWord&	W, bool	PassiveForm, long ClauseNo,	CRusSemNode& N,	bool UsePlugArticle) const
{
	try	
	{
		N.DelAllInterps();

		if (W.m_IsPunct) return;

		//==================================================================
		// ========	 ������������� �� �����	(�������� �������)
		//=================================================================

		// ����	��������� ������ � ����� ��	�����
		// ��������, �������������
		string UnitStr;
		WORD UnitNo	= ErrUnitNo;
		if (   //( W.m_Poses & (1	<< PARTICIPLE) ) 
			 ( W.m_Poses & (1	<< PARTICIPLE_SHORT) ) 
			)
		{
			UnitStr	= GetNormOfParticiple(W);
			EngRusMakeLower(UnitStr);
			UnitNo	= GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(), 1);	
			/*
			(1)	���	���� ����, �� ���������������� � ������������ ����������
			(2)	���	�����������	��������� ������ ��������� �������
			� ������  ������� �����	������	�����������, ��������� ����� 
			"�����������" ���������	� ���� ��������	���������������. ����� ��������	�������	
			�����������.
			*/
			if (UnitNo != ErrUnitNo)
					PassiveForm	= false;

		}


		if (UnitNo == ErrUnitNo)
		{
			UnitStr=  W.m_Lemma;
			EngRusMakeLower(UnitStr);
		};

		bool bHasSuffixFemine =	true;

TryWithoutFeminSuffix:

		if (UnitNo == ErrUnitNo)
		{
			UnitNo	= GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(), 1);
		};

		// ����	����� �� �������, �� ����������� ����� ��� ������� ����
		if (UnitNo == ErrUnitNo)
			for	(long i=0; i < W.m_WordEquals.size(); i++) 
		 {
			 UnitStr =	W.m_WordEquals[i];
			 EngRusMakeLower(UnitStr);
			 UnitNo	 = GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(),	1);
			 if	(UnitNo	!= ErrUnitNo) break;
		 };

		/*
		�������	��������  �� ����� ��������� ���� (����-)
		*/
		if (UnitNo == ErrUnitNo)
			UnitNo =  GetInterpWithoutPrefix(W);


		// ����� ���� REF		
		bool ReferenceFound	= false;
		if (UnitNo != ErrUnitNo)
		{
			if (!GetRoss(Ross)->IsEmptyArticle(UnitNo))
				for	(size_t	i =	GetRoss(Ross)->GetUnitStartPos(UnitNo);	i<=	GetRoss(Ross)->GetUnitEndPos(UnitNo); i++)
				{
					if ( GetRoss(Ross)->GetCortegeFieldNo(i) ==	 GetRossHolder(Ross)->RefFieldNo )
					{
						TCortege C = GetCortege(GetRoss(Ross), i);
						string S =	 GetRossHolder(Ross)->GetDomItemStrInner(C.m_DomItemNos[0]);
						long MeanNum =	-1;
						if (C.m_DomItemNos[1] !=  -1) 
						{
							string Q = GetRossHolder(Ross)->GetDomItemStrInner(C.m_DomItemNos[1]);
							if (Q.length() == 1)
							{
								MeanNum	= Q[0] - '0';
							};
						};
						if (MeanNum	== -1)
							ErrorMessage ("Reference without MeanNum used");
						else
						{
							ReferenceFound = true;
							long UnitNo1 = GetRossHolder(Ross)->LocateUnit(S.c_str(), MeanNum);
							if (UnitNo1	!= ErrUnitNo)
								if (GramFetAgreeWithPoses (*GetRossHolder(Ross), UnitNo1, W))
									N.AddInterp(CDictUnitInterp(GetRossHolder(Ross),Ross,	UnitNo1, false,	PassiveForm) );
						}
					}
				};
		};

		if (UnitNo != ErrUnitNo)
			if (!ReferenceFound)
				for	(long i	= UnitNo; i<GetRoss(Ross)->GetUnitsSize() && UnitStr == GetRoss(Ross)->GetEntryStr(i);	i++)
					if (GramFetAgreeWithPoses (*GetRossHolder(Ross), i,	W))
						N.AddInterp( CDictUnitInterp(GetRossHolder(Ross),Ross,	i, false, PassiveForm));

		if (	N.GetInterps().empty()  
			&&	HasReflexiveSuffix(W.m_Lemma)
			&&	VerbCanBeWithoutReflexiveSuffix(W.m_Lemma)
			// ��� ������� �� �� ��	�������� �������������
			&&	W.HasOneGrammem(rNonTransitive)
			)
		{ 
			// ��������� ������� -��, �����	����� ���� ��� ��������
			string suffix =	W.m_Lemma.substr(W.m_Lemma.length()	- 2);
			W.m_Lemma =	W.m_Lemma.substr(0,	W.m_Lemma.length() - 2);
			// ���������� �������������� ������	����������������
			W.SetTypeGrammems((W.GetTypeGrammems() & ~_QM(rNonTransitive)) | _QM(rTransitive));
			StringVector SuffixAspVerb;
			SuffixAspVerb.resize(W.m_WordEquals.size());
			for	(long j=0; j < W.m_WordEquals.size();  j++)
			{
				// �� ������ ������	�� -�� ����� ��������� ����	� -��.
				// ��������	"��������"-"�����"
				if (HasReflexiveSuffix(W.m_WordEquals[j]))
				{
					SuffixAspVerb[j] = W.m_WordEquals[j].substr(W.m_WordEquals[j].length() - 2);
					W.m_WordEquals[j] =	W.m_WordEquals[j].substr(0,	W.m_WordEquals[j].length() - 2);
				}
				else
				{
					SuffixAspVerb[j] = W.m_WordEquals[j];
					W.m_WordEquals[j] =	"";

				};
			};


			InitInterps(W, true,	ClauseNo, N);
			// ... �������
			W.SetTypeGrammems((W.GetTypeGrammems() & ~_QM(rTransitive)) | _QM(rNonTransitive));

			// ��������� ������� ��	�������, ����� ����� ��������� �������������� 
			// "������������ � ��������"

			W.m_Lemma += suffix;
			for	(long j=0; j	< W.m_WordEquals.size();  j++)
				W.m_WordEquals[j] += SuffixAspVerb[j];

			return;
		};

		//==================================================================
		// ========	 ������������� �� ������ ��������
		//=================================================================

		GetTimeInterps(UnitStr,	W.m_Word, W.m_PostPuncts, N);

		GetThesInterps(UnitStr,	W, m_pData->GetThes(OmniThes), OmniRoss, N);
		GetThesInterps(UnitStr,	W, m_pData->GetThes(FinThes), FinRoss,	N);
		GetThesInterps(UnitStr,	W, m_pData->GetThes(CompThes), CompRoss, N);
		GetThesInterps(UnitStr,	W, m_pData->GetThes(LocThes), LocRoss, N);

		/*�������� ��� ������� ����� �������� ����������*/
		DelProhibitedInterps(UnitStr, N);	

		if ( N.GetInterps().empty() )  
			if (   bHasSuffixFemine
				&& W.HasPOS(NOUN)
				&& W.HasOneGrammem(rFeminum)
				&& W.HasOneGrammem(rAnimative)
				)
			{
				bHasSuffixFemine = false;
				if (BuildWordWithoutFemineSuffix(UnitStr))
				{
					/*
					���	"�����������" ������ � ������ ������������ ����� "�������"
					*/
					W.m_WordEquals.push_back(UnitStr);

					goto TryWithoutFeminSuffix;
				};
			};



		if (UsePlugArticle)
			if (N.GetInterps().empty())  
			{ 
				long ArtNo = FindAbstractPlugArticle(Ross, W.GetAllGrammems(), W.m_Poses,	m_Clauses[ClauseNo].m_ClauseType);
				if (ArtNo != -1)
					N.AddInterp( CDictUnitInterp(GetRossHolder(Ross), Ross, (*m_pData->GetAbstractArticles(Ross))[ArtNo].m_UnitNo,	false, PassiveForm));


			}
			DelProhibitedInterps(UnitStr, N);	

	}
	catch (...)
	{
		ErrorMessage ("������ ���������	�������������");
		throw;
	};
};


void CRusSemStructure::InitWordFeatures(long WordNo, CRusSemWord& SemWord) 
	{
		const CSynWord& W = m_piSent->m_Words[WordNo];
		SemWord.m_Word = W.m_strWord;
		EngRusMakeUpper(SemWord.m_Word);
        SemWord.m_GraphDescrs = " "+W.BuildGraphemDescr()+ " ";

		SemWord.m_IsPunct = !SemWord.m_Word.empty() && ispunct((BYTE)SemWord.m_Word[0]);
		SemWord.m_ILE = SemWord.m_GraphDescrs.find(" LLE " ) != string::npos;
		if (SemWord.m_ILE) SemWord.SetFormGrammems (rAllCases);
		SemWord.m_CharCase = W.m_Register;



		if ( (WordNo == 0) && (SemWord.m_CharCase == UpLow))
			if (		!SemWord.HasOneGrammem(rToponym)
					&&	!SemWord.HasOneGrammem(rName)
					&&	!SemWord.HasOneGrammem(rPatronymic)
			   )
			SemWord.m_CharCase = LowLow; 
		
		SemWord.m_PostPuncts = "";
		for (long i=WordNo+1; i < m_piSent->m_Words.size(); i++)
		{
			string w = m_piSent->m_Words[i].m_strWord;
			if (!w.empty() && ispunct((BYTE)w[0]))
  			   SemWord.m_PostPuncts += w;
			else
				break;
		};

				/*
		 ������� ����� ������������ ���: � ��� ������ ���� ��������� ������� ��
		 � ������ ������ ������ ���� ���������
		*/
		if (SemWord.m_GraphDescrs.find("DC ") !=  -1)

		  if (is_english_alpha( (BYTE)SemWord.m_Word[0]) )
		  {
			SemWord.m_bRomanNumber =  true;
			/*
			 ������� ����� ����� �������� �������� ��� ���������� �������,
			  ��������� - �� ���������,
			  ������� - �����  ���������.
			  ������� ����� �� �������� ���, ������� ����� �� � �� �� �����.
			*/
			if ( is_english_upper((BYTE)SemWord.m_Word[0]) )
				SemWord.m_CharCase = UpUp;
		  };
		


    		
};




CRusSemNode CRusSemStructure::CreatePrimitiveNode(size_t WordNo) 
{
	
	long ClauseNo = m_piSent->GetPrimitiveClauseNoByClauseNo(m_piSent->m_Words[WordNo].m_iClauseNo);
	CRusSemNode N;
	const CSynWord& W = m_piSent->m_Words[WordNo];
	
	CSynHomonym H(const_cast<CSentence*>(m_piSent));
	GetHomonym(ClauseNo, WordNo, H);
		

	string  S = H.m_strLemma;
	
	EngRusMakeLower(S);
	unsigned short UnitNo = ErrUnitNo;
	CRusSemWord SemWord(WordNo, H.m_strLemma);
	SemWord.m_SynWordNo = WordNo;
	SemWord.m_ParadigmId = H.m_lPradigmID; 
	string TypeAncode;
	SemWord.SetFormGrammems (H.m_iGrammems);
	{
		//  reading grammems from "type ancode"

		if ( (H.m_CommonGramCode.length() > 1) && (H.m_CommonGramCode != "??") )
		{
			TypeAncode = H.m_CommonGramCode;	
			SemWord.SetTypeGrammems ( H.m_CommonGramCode.length() > 2 ? H.m_TypeGrammems
				: m_pData->GetRusGramTab()->GetAllGrammems(TypeAncode.c_str()));
		};

	}
	SemWord.m_Poses = H.m_iPoses;
	string GramCodes = H.GetGramCodes();
	SemWord.m_pData =  m_pData;
	SemWord.m_GramCodes = GramCodes;

	
	InitWordFeatures(WordNo, SemWord);

	if	(		SemWord.HasPOS(PARTICIPLE_SHORT) 
			&&	SemWord.HasOneGrammem(rPerfective) 
			&&  SemWord.HasOneGrammem(rPastTense) 
			&&  (SemWord.m_Word.find("-") == string::npos)
		)
	{
		//  ����� �������, ��� ����� ������� ��������� "��,���,���" ��������� ��������
		// ��������:
		// "���������"
		// "���������"
		SemWord.SetFormGrammems ((SemWord.GetFormGrammems() & ~_QM(rPastTense)) | _QM(rPresentTense));
	};

	if ( !HasAuxiliaryPOS(SemWord.m_Lemma) )
		if (SemWord.m_ParadigmId != -1)
		{
			CFormInfo Paradigm;
			bool bRes = m_pData->GetRusLemmatizer()->CreateParadigmFromID(SemWord.m_ParadigmId, Paradigm);
			assert (bRes);
			SemWord.m_WordWeight = Paradigm.GetWordWeight();
		};

	if (SemWord.HasOneGrammem(rPerfective))
		SemWord.m_WordEquals = m_pData->GetAspVerb(SemWord.m_ParadigmId, true );

	if (SemWord.HasOneGrammem(rNonPerfective))
		SemWord.m_WordEquals = m_pData->GetAspVerb(SemWord.m_ParadigmId, false );


	// ��������� ���������� ������� �� ������������ �������
	/*{
		vector<DWORD> ids, afixes;
		if( m_pData->m_piSupAdjDict.SourceToTarget(SemWord.m_ParadigmId, ids, afixes) > 0)
		{
			if (ids.empty())
				throw CExpc ("Empty vector in SupAdjDict interface");
			DWORD ParadigmId = ids[0];
			if (ParadigmId != -1)
			{
				SemWord.m_ParadigmId = ParadigmId;
				CFormInfo Paradigm;
				bool bRes =  m_pData->GetRusLemmatizer()->CreateParadigmFromID(ParadigmId, Paradigm);
				assert (bRes);
				SemWord.m_Lemma = Paradigm.GetWordForm(0);
				N.m_RelOperators.push_back ("Magn");
			};
		};
	}*/
	  
	// ��������� ���������, ������� ������ �� ���������� (��� ���� �������� � �������
	// �� Mapost�)
	
	if (SemWord.m_GraphDescrs.find("#����") !=  -1)
		N.m_RelOperators.push_back ("����");

	if (SemWord.m_GraphDescrs.find("���-") !=  -1)
		N.m_RelOperators.push_back ("���-");

	if (SemWord.m_GraphDescrs.find("����-") !=  -1)
		N.m_RelOperators.push_back ("����-");

	if (SemWord.m_GraphDescrs.find("#���_�����") !=  -1)
	{
		N.m_RelOperators.push_back ("���_�����");
		long ParadigmId = m_pData->GetAdverbWith_O_ByAdjective(SemWord.m_ParadigmId, SemWord.m_Word);
		if (ParadigmId != -1)
		{
			CFormInfo Paradigm;
			bool bRes = m_pData->GetRusLemmatizer()->CreateParadigmFromID(ParadigmId, Paradigm);
			assert (bRes);
			SemWord.m_ParadigmId = ParadigmId;
			SemWord.m_Lemma = Paradigm.GetWordForm(0);
			SemWord.m_Poses = (1 << ADV);
			SemWord.SetFormGrammems(0);
			GramCodes = "";
		};
	};

	/*
		������������� ���� m_AdditParadigmId ���
		���������� ���� "���-���������"
	*/

	if  (   (SemWord.m_ParadigmId == -1)
		 && (SemWord.m_Lemma.find("-") != -1)
		 && (SemWord.m_Poses == (1 << NOUN))
		)
	{
		int i = SemWord.m_Lemma.find("-");
		SemWord.m_ParadigmId = m_pData->GetFirstParadigmId(morphRussian, SemWord.m_Lemma.substr(0, i), 1<<NOUN);
		SemWord.m_AdditParadigmId = m_pData->GetFirstParadigmId(morphRussian, SemWord.m_Lemma.substr(i+1),  1<<NOUN);
	};


	S = SemWord.m_Word;
	EngRusMakeUpper(S);
	//  ������� ������������� ��� ������� ���� "��-���������", "��-�����������"
	if	(			(S.substr(0,3) == "��-")
				&&	
				(		(SemWord.m_Poses == (1<<ADV))
					||	(		(SemWord.m_ParadigmId == -1)
							&&	(S.length() > 3)
							&&	(		(S.substr(S.length() - 2)  == "��")
									||	(S.substr(S.length() - 2)  == "��")
								)
						)
				)

		)
	{
		if (S.substr(S.length() - 2)  == "��")
		{
			// "��-���������"
			SemWord.m_Lemma = S.substr(3) + "�";	
			SemWord.m_ParadigmId = m_pData->GetFirstParadigmId(morphRussian, SemWord.m_Lemma,  1<<ADJ_FULL);
		}
		else
		{
			// "��-��������"
			SemWord.m_ParadigmId = m_pData->GetFirstParadigmIdAndLemma(morphRussian, S.substr(3),  1<<ADJ_FULL, SemWord.m_Lemma);
		};
		SemWord.m_bAdverbFromAdjective = true;

	};
	

	// ============================================================
	// =========   ������ ������������� � ������������� ��������
   	if (!H.m_SimplePrepNos.empty())
	{
	  for (long l=0; l < H.m_SimplePrepNos.size(); l++) 
		  N.AddInterp(  CDictUnitInterp(OborRoss, H.m_SimplePrepNos[l]) );
	}
	else
	{
		/*
		������ ���� �������� ��������, ���� ���, ��������, "���"
		*/
		if (H.m_OborotNo != -1)
			N.AddInterp( CDictUnitInterp(OborRoss, H.m_OborotNo) );
		else
			InitInterps(SemWord, SemWord.HasOneGrammem(rPassiveVoice), ClauseNo, N);
	};

	
	
	N.m_bQuoteMarks  = (SemWord.m_GraphDescrs.find(" #QUOTED") !=  -1);

	if (SemWord.m_GraphDescrs.find("����") !=  -1)
		N.m_RelOperators.push_back ("����");

	
	// ���� � �������� ���������������  ��� ������� �������,
	// �������� ��� �������� ���������� �������
	if (SemWord.HasPOS(ADJ_SHORT))
		if (!SemWord.HasOneGrammem(rFutureTense) && !SemWord.HasOneGrammem(rPastTense))
		  SemWord.SetFormGrammems(SemWord.GetFormGrammems() | _QM(rPresentTense));

	N.m_Words.push_back(SemWord);


	N.SetGrammems(SemWord.GetAllGrammems());
	N.m_GramCodes  = GramCodes;
	N.m_SynGroupTypeStr = "";
	N.SetMainWordNo(0);
	N.m_ClauseNo = ClauseNo;
	N.m_TerminId = W.m_iTerminID;
	N.m_ThesaurusId = W.m_ThesType;
	N.m_bFirstInTermin = W.m_bFirstWordInTermin;
	N.m_TypeAncode = TypeAncode;

	N.m_bQuestionWord = N.IsLemmaList ("���", "���", "���", "�����", "����", "������", "�����", "�����", "���", "������", "������", "�������", "���������", "");

	return N;

};


long CRusSemStructure::FindNodeBySyntaxGroupNo (long SyntaxGroupNo) const 
{
  for (long i=0; i< m_Nodes.size(); i++)
		 if ( m_Nodes[i].m_SyntaxGroupNo == SyntaxGroupNo )
		   return i;
		
 return -1;
};


void CRusSemStructure:: DelProhibitedInterps (string UnitStr, CRusSemNode& N) const
{
  EngRusMakeUpper(UnitStr);
  for (long  i=0;i < N.GetInterps().size(); i++)
  {
	   CRossInterpAndLemma M;
	   M.m_DictType  = N.GetInterps()[i].m_DictType;
	   M.m_UnitNo  = N.GetInterps()[i].m_UnitNo;
	   M.m_Lemma = UnitStr;
	   if (_find (m_UserProhibitedLexVars, M))
	   {
		   N.DeleteInterp(i);
		   i--;
	   };
  };
};

bool CRusSemStructure::HasAuxiliaryPOS(string Lemma) const 
{
	

	vector<CFormInfo>  ParadigmCollection; 
	m_pData->GetRusLemmatizer()->CreateParadigmCollection(true,Lemma, true, false, ParadigmCollection);


	for (int i =0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		string ancode = Paradigm.GetSrcAncode();
		BYTE POS = m_pData->GetRusGramTab()->GetPartOfSpeech(ancode.c_str());
		if (		(POS == PRONOUN) 
				||	(POS == PRONOUN_P) 
				||	(POS == PREP) 
				||	(POS == CONJ) 
				||	(POS == PARTICLE) 
				||	(POS == INTERJ) 
			)
			return true;
	}
	return false;
}

void CRusSemStructure::GetMaxWordWeightByLemma(string Lemma, CRusSemWord& SemWord) const
{
	

	vector<CFormInfo>  ParadigmCollection; 
	m_pData->GetRusLemmatizer()->CreateParadigmCollection(true, Lemma, true, false, ParadigmCollection);

	for (int i =0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if (SemWord.m_WordWeight < Paradigm.GetWordWeight())
			SemWord.m_WordWeight = Paradigm.GetWordWeight();
	}

}

CRusSemNode CRusSemStructure::CreateNode(const CRelationsIterator* RelIt, long GroupNo) 
{
	const CGroup& G = RelIt->GetFirmGroups()[GroupNo];
	long ClauseNo = m_piSent->GetPrimitiveClauseNoByClauseNo(m_piSent->m_Words[G.m_iFirstWord].m_iClauseNo);

 	CRusSemNode N;

	for (size_t j = G.m_iFirstWord; j <= G.m_iLastWord; j++)
	{
	   const CSynWord& W = m_piSent->m_Words[j];
	   string  Lemma;
	   
	   CSynHomonym H(const_cast<CSentence*> (m_piSent));
	   GetHomonym(ClauseNo, j, H);
	   Lemma = H.m_strLemma;
	   CRusSemWord SemWord (j,   Lemma);
  	   SemWord.m_SynWordNo = j;
	   SemWord.m_ParadigmId = H.m_lPradigmID; 

	   SemWord.SetFormGrammems (H.m_iGrammems);
	   SemWord.m_GramCodes = H.GetGramCodes();
	   SemWord.m_Poses = H.m_iPoses;
	   SemWord.m_pData =  m_pData;
	   	{
			//  reading grammems from "type ancode"
			if ( (H.m_CommonGramCode.length() > 1) && (H.m_CommonGramCode != "??") )
			{
				string TypeAncode = H.m_CommonGramCode;	
				SemWord.SetTypeGrammems (  H.m_CommonGramCode.length() > 2 ? H.m_TypeGrammems :
					m_pData->GetRusGramTab()->GetAllGrammems(TypeAncode.c_str()));
			};
		}

	   
		InitWordFeatures(j, SemWord);
		if (SemWord.HasOneGrammem(rPerfective))
			SemWord.m_WordEquals = m_pData->GetAspVerb(SemWord.m_ParadigmId, true );
		if (SemWord.HasOneGrammem(rNonPerfective))
				SemWord.m_WordEquals = m_pData->GetAspVerb(SemWord.m_ParadigmId, false );

		

		if ( !HasAuxiliaryPOS(SemWord.m_Lemma) )
			if (SemWord.m_ParadigmId != -1)
			{
				CFormInfo Paradigm;
				bool bRes = m_pData->GetRusLemmatizer()->CreateParadigmFromID(SemWord.m_ParadigmId, Paradigm);
				assert (bRes);
				SemWord.m_WordWeight = Paradigm.GetWordWeight();
			}
			else
				GetMaxWordWeightByLemma(Lemma, SemWord);




		N.m_Words.push_back(SemWord);
		if ( j == G.m_MainWordNo )
		{
			N.SetMainWordNo(N.m_Words.size()-1);
			N.m_GramCodes = H.GetGramCodes();
			InitInterps(SemWord, false, ClauseNo, N);
		};

	};


	CSynHomonym H(const_cast<CSentence*> (m_piSent));
	GetHomonym(ClauseNo, G.m_iFirstWord, H);

	if (H.m_OborotNo != -1)
		N.AddInterp( CDictUnitInterp(OborRoss, H.m_OborotNo) );

    N.m_ClauseNo = ClauseNo;
	N.m_SynGroupTypeStr = m_piSent->GetOpt()->GetGroupNameByIndex(G.m_GroupType) ;
	N.m_SyntaxGroupNo =  GroupNo;
	/*SYNANLib::IWordPtr W = m_piSent->Word[N.m_Words[0].m_WordNo];
	if (G->ThesID != NoneThes)
	{
			  N.m_TerminId = G->TerminID;
			  N.m_ThesaurusId = G->ThesID;
	};*/
		
			
	N.SetGrammems(G.GetGrammems());

    return N;	
};




void CRusSemStructure::InterpretPrepNouns(long ClauseNo)
{
    for (long RelNo=0; RelNo < m_SynRelations.size(); )
        if (m_Nodes[m_SynRelations[RelNo].m_SourceNodeNo].m_ClauseNo != ClauseNo)
            RelNo++;
        else	
            if (m_SynRelations[RelNo].m_SynRelName == "��")
            {
                long PrepNode = m_SynRelations[RelNo].m_SourceNodeNo;

                long NounNode =  m_SynRelations[RelNo].m_TargetNodeNo;

                // � �������������� ��������  ���� ��� ���������,  � ���� ����� ������, ��� �� ������ �����
                if   (m_Nodes[NounNode].m_Colloc.m_Type !=  RossType) 
                {
                    m_Nodes[NounNode].m_SynReal.m_Preps.clear();
                    for (long i=0; i < m_Nodes[PrepNode].GetInterps().size(); i++)
                        m_Nodes[NounNode].m_SynReal.m_Preps.push_back (m_Nodes[PrepNode].GetInterps()[i]);
                };

                MoveIncomingSynRelations(PrepNode, NounNode);

                m_SynRelations.erase(m_SynRelations.begin() +RelNo);

                MoveSynRelations(PrepNode, NounNode);

                m_Nodes[NounNode].m_PrepWordNo = m_Nodes[PrepNode].GetMinWordNo();
                // � �������� ����� ���� ���������, �������� "������ ����� ������������ �������",
                // ��� �������� "������" ������������� � �������� "�����"
                // ��������� ��� ��������� � �������� �����
                m_Nodes[NounNode].m_RelOperators.insert (
                    m_Nodes[NounNode].m_RelOperators.begin(), 
                    m_Nodes[PrepNode].m_RelOperators.begin(),
                    m_Nodes[PrepNode].m_RelOperators.end());
				
				//if(m_Nodes[PrepNode].m_Words.size() == 1 && m_Nodes[PrepNode].m_Words[0].m_Word.size() == 1)
				if(!m_Nodes[PrepNode].HasOneGrammem(rNominativ))
					m_Nodes[NounNode].DeleteGrammems(_QM(rNominativ)); //������� ��.�.
                
				if(m_Nodes[PrepNode].GetGrammems() & rAllCases)
					m_Nodes[NounNode].SetGrammems(m_Nodes[NounNode].GetGrammems() & (m_Nodes[PrepNode].GetGrammems() | ~rAllCases));

				if (CanBeDeleted(PrepNode))
                    DelNode(PrepNode);
            }
            else
                RelNo++;


};

/*
������������ �����������: 
	��������� �� ������  
� �����������
	��������� �������.
*/
void CRusSemStructure::InterpretSelectiveRelations(long ClauseNo)
{
  for (long RelNo=0; RelNo < m_SynRelations.size(); RelNo++)
   if (m_Nodes[m_SynRelations[RelNo].m_SourceNodeNo].m_ClauseNo == ClauseNo)
    if (m_SynRelations[RelNo].m_SynRelName == "�����_��")
	{
		long NumeralNode = m_SynRelations[RelNo].m_SourceNodeNo;
		long NounNode =  m_SynRelations[RelNo].m_TargetNodeNo;

		// ��������� �������������� �������������� � ���. ��������� �� ������������� �   ����������������
		m_Nodes[NounNode].DeleteGrammems(rAllCases);
		m_Nodes[NounNode].AddGrammems(m_Nodes[NumeralNode].GetGrammems() & rAllCases);
		m_Nodes[NounNode].m_SynReal  = m_Nodes[NumeralNode].m_SynReal;
		m_Nodes[NumeralNode].m_SynReal.m_Preps.clear();

		MoveIncomingSynRelations(NumeralNode, NounNode);

		// �����������  ���������
		swap(m_SynRelations[RelNo].m_SourceNodeNo, m_SynRelations[RelNo].m_TargetNodeNo);

	}
};

void CRusSemStructure::InterpretRepeatConj(long ClauseNo) 
{
	/*
	����� W1,W2 - �������,������� ����������� ���������
	��������. ��������� �������� �������� �� �����,
	������� � ���� �������, � ����, ������� �� ����� � ���� �������.
	��������, ��������. ���������, ����� ����������� 
	���������  MUA, �����, ����� ������� ���������������� MUA, ���� � �������������� ��������� � ��������� �� �����.  
	*/
	for (long i=0; i < m_SynRelations.size(); i++)
		if (m_Nodes[m_SynRelations[i].m_SourceNodeNo].m_ClauseNo == ClauseNo)
			if (m_SynRelations[i].m_SynRelName == "������")
			{
				long Word1 = m_Nodes[m_SynRelations[i].m_SourceNodeNo].m_Words[0].m_SynWordNo;
				long Word2 = m_Nodes[m_SynRelations[i].m_TargetNodeNo].m_Words[0].m_SynWordNo;
				// ��������� ������ ���� ����� ������� 
				for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
				{
					CRusSemNode& N = m_Nodes[NodeNo];
					long W0 = N.m_Words[0].m_SynWordNo;
					if ( (Word1 < W0) && (W0 < Word2))
					{
						N.m_MinActantWordNo = Word1;
						N.m_MaxActantWordNo = Word2;
					};
				};
			};

	/*
	�������� ������ ������ ������������� ������	
	*/

	SetNodeToDeleteFalse();
	for (size_t NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo<m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
		if (   (m_Nodes[NodeNo].m_MNAType != RepeatMNA)
			&& !m_Nodes[NodeNo].m_bToDelete
			&& CanBeDeleted (NodeNo)
			)
		{
			vector<long> Rels;
			GetOutcomingSynRelations(NodeNo, Rels);
			vector<long> Nodes;
			for (long i=0; i < Rels.size();)
				if (m_SynRelations[Rels[i]].m_SynRelName == "������")
				{
					Nodes.push_back (m_SynRelations[Rels[i]].m_TargetNodeNo);
					i++;
				}
				else
					Rels.erase(Rels.begin() + i) ;

			if (Nodes.size() == 0)  continue;

			sort ( Nodes.begin(),Nodes.end(), IsLessByMinWordNo(this));

			// ���������� ��� ���������  ������, ������ �� �������� ����
			DeleteSynRelations(Rels);
			long ToNode =  Nodes[Nodes.size() -  1];
			MoveSynRelations(NodeNo, ToNode);
			m_Nodes[ToNode].m_NodeType = MNA;
			m_Nodes[ToNode].m_MNAType = RepeatMNA;
			// ��� ��������� ����� ��������� � ������� ��������
			m_Nodes[ToNode].DelAllInterps();


			for (long i=0; i <  m_Nodes[NodeNo].GetInterps().size();i++)
				if (m_Nodes[NodeNo].GetInterps()[i].m_DictType == OborRoss)
					m_Nodes[ToNode].AddInterp( m_Nodes[NodeNo].GetInterps()[i] );



			
			m_Nodes[NodeNo].m_bToDelete = true;
			for (long i=0; i < Nodes.size() -1; i++)
				if (CanBeDeleted(Nodes[i]))
					m_Nodes[Nodes[i]].m_bToDelete = true;



		};

	DelNodesToDelete();
};


// ��������� ������� ������� "��" � ������, ������� �� ������ � ������� ��� 
// �������������� (����������� ����). ���� �����, �� ���������� �� �  ������������ ���� m_HasParticleBY �
// m_Clauses[ClauseNo] TRUE.
// ������� "�" ���� ����� ������������ � �������� "���� �", ������� ��� ���� �� ������,
// ����� �� ������ c ��������� ���� "����� �", "������ �"
void CRusSemStructure::FindParticleBY(long ClauseNo)
{
 for (long i=0; i < m_Nodes.size(); i++)
 if (m_Nodes[i].IsPrimitive() && CanBeDeleted(i))
  if (IsInClause(i, ClauseNo))
   if (   (m_Nodes[i].m_Words[0].m_Lemma == "��")
	   || (     (m_Nodes[i].m_Words[0].m_Lemma == "�")
	        &&  (m_Nodes[i].m_Words[0].m_PostPuncts.size() == 0)
            &&  (m_Nodes[i].m_Words[0].m_CharCase == LowLow)
			&&  (i>0)
			&&  (m_Nodes[i-1].m_Words[0].m_PostPuncts.size() == 0)
	      )
	  )
   {
        DelNode(i);
		m_Clauses[ClauseNo].m_HasParticleBY = true;
		i=-1;
   };

};














void CRusSemStructure::BuildPustychaWithETO (long ClauseNo)
{
 if (m_Clauses[ClauseNo].m_ClauseSyntaxTop  != -1) return;


 size_t i=0;
 for (; i < m_Nodes.size(); i++)
 if (m_Nodes[i].m_ClauseNo == ClauseNo)
 if (m_Nodes[i].m_MainWordNo != -1)
 {
	 string Q = m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].m_Word;
	 if ((Q == "���") || (Q == "���")) break;
 };

 size_t ETONodeNo = i;

 if (ETONodeNo == m_Nodes.size()) return;

 for (i = FindRightClosestNode(ETONodeNo); i!= -1; i = FindRightClosestNode(i))
   if ( m_Nodes[i].HasOneGrammem( rNominativ) )
	   break;

 if (i == -1) return;

 
 CRusSemNode  RusSemNode = CreateCopulNode(ClauseNo);
 long CopulNodeNo = InsertNode(RusSemNode);
 AddRelation(CRusSemRelation(m_Nodes[CopulNodeNo].m_Vals[0], CopulNodeNo, ETONodeNo,  ""));
 //AddRelation(CSemRelation(m_Nodes[CopulNodeNo].m_Vals[1], CopulNodeNo, i,  Indifferent, -1));

/* ����� ����� ���������� ������ 
 m_Nodes[CopulNodeNo].m_Patterns.resize(2);
m_Nodes[CopulNodeNo].m_Patterns[0].m_Valency.m_RelationStr= "F-ACT"; m_Nodes[CopulNodeNo].m_Patterns[0].m_pRossDoc = GetRossHolder(Ross);
 m_Nodes[CopulNodeNo].m_Patterns[0].m_GramCorteges.push_back(GetSubjCortege(GetRossHolder(Ross)));
 m_Nodes[CopulNodeNo].m_Patterns[0].m_GramCorteges[0].Items[0] = -1;
 m_Nodes[CopulNodeNo].m_Patterns[1].m_SourceNo = CopulNodeNo;
 m_Nodes[CopulNodeNo].m_Patterns[1].m_Valency = m_Nodes[CopulNodeNo].m_Vals[1];
 m_Nodes[CopulNodeNo].m_Patterns[1].m_pRossDoc = GetRossHolder(Ross);
 m_Nodes[CopulNodeNo].m_Patterns[1].m_GramCorteges.push_back(GetSubjCortege(GetRossHolder(Ross)));
*/
 m_Clauses[ClauseNo].m_ClauseSyntaxTop = CopulNodeNo;
  
};



// ���� ������ � ������� UnitNo �������� �0(S) � ���� SF, �� ������� ������ ����� ������ S, 




void CRusSemStructure::DealInvitatoryMoodBeforeTree (long ClauseNo)
{
	long RootNodeNo =  GetClauseSyntaxTop(ClauseNo); 
	if (RootNodeNo == -1) return;
	if (!m_Nodes[RootNodeNo].IsPrimitive()) return;

	// ���� �����-����
	int DAVAI_NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo;
	for (; DAVAI_NodeNo<m_Clauses[ClauseNo].m_EndNodeNo; DAVAI_NodeNo++)
		if (   m_Nodes[DAVAI_NodeNo].IsPrimitive() 
			&& (    (m_Nodes[DAVAI_NodeNo].m_Words[0].m_Word == "�������")
			||  (m_Nodes[DAVAI_NodeNo].m_Words[0].m_Word == "�������-��")	
			||  (m_Nodes[DAVAI_NodeNo].m_Words[0].m_Word == "�����")	
			||  (m_Nodes[DAVAI_NodeNo].m_Words[0].m_Word == "�����-��")	
			)
			)
			break;
	// ���� ������  �����-����, �� ���������, ������ �����, ��� ������� ������� ������ ������, ��� �����������.
	// ��������� ������� ���� ���� �� ����� � �������� ������ �������������� �������, ���� ��� �� �����
	// �������, �� ��� �� ��������������� ����������.
	if (DAVAI_NodeNo < m_Clauses[ClauseNo].m_EndNodeNo && CanBeDeleted(DAVAI_NodeNo) ) 
	{
		m_Nodes[DAVAI_NodeNo].m_bToDelete = true;
		long SaveRoot = m_Clauses[ClauseNo].m_RootWordNo; 
		m_Clauses[ClauseNo].m_RootWordNo = -1;
		RootNodeNo =  GetClauseSyntaxTop(ClauseNo); 
		m_Nodes[DAVAI_NodeNo].m_bToDelete = false;
		m_Clauses[ClauseNo].m_RootWordNo = SaveRoot;
		if (RootNodeNo == -1)  		return;
		MoveSynRelations(DAVAI_NodeNo, RootNodeNo);
	};


	// ����� ����� ������������ ��� ����� ������ �� ��������� ����� ������ 

	// ������ �����q: ������ ��������������� �����������
	for (;;)
	{
		if  (  !m_Nodes[RootNodeNo].HasOneGrammem(rFutureTense))  break;

		if (DAVAI_NodeNo == m_Clauses[ClauseNo].m_EndNodeNo) break;
		if (	CanBeDeleted(DAVAI_NodeNo) )
		{
			if (!m_Nodes[RootNodeNo].HasRelOperator("_����_���������_����������"))     
				m_Nodes[RootNodeNo].m_RelOperators.push_back("_����_���������_����������");
			MoveSynRelations(DAVAI_NodeNo, RootNodeNo);	
			DelNode(DAVAI_NodeNo);
		}

		return;
	};

	// ������ ������  ��������������� �����������
	for (;;)
	{
		if (  !m_Nodes[RootNodeNo].HasOneGrammem(rImperative) )  break;


		if (DAVAI_NodeNo == m_Clauses[ClauseNo].m_EndNodeNo) break;
		if (	CanBeDeleted(DAVAI_NodeNo) )
		{
			if (!m_Nodes[RootNodeNo].HasRelOperator("_���������_����������"))     
				m_Nodes[RootNodeNo].m_RelOperators.push_back("_���������_����������");
			MoveSynRelations(DAVAI_NodeNo, RootNodeNo);
			DelNode(DAVAI_NodeNo);
		}
		return;
	};

	// ������ ������  ��������������� �����������
	for (;;)
	{
		if  (     !m_Nodes[RootNodeNo].HasGrammems ( _QM(rImperative) | _QM(rFirstPerson) )   
			&&	!m_Nodes[RootNodeNo].HasGrammems ( _QM(rPlural) | _QM(rPastTense) )
			)
			break;
		string word = m_Nodes[RootNodeNo].m_Words[0].m_Word;
		if (word.length() < 3) break; 
		if (    (word.substr (word.length() - 3, 3) != "-��") 
			&& (word.substr (word.length() - 2, 2) != "��") 
			) 
			break;
		if (!m_Nodes[RootNodeNo].HasRelOperator("_���������_����������"))     
			m_Nodes[RootNodeNo].m_RelOperators.push_back("_���������_����������");
		return;
	};


	// ��������� ������  ��������������� ����������� �������������� ����� ���������� 
	// ������, ��������� ������������� ��� ������� �� ���������� ������������ � ��� ���������� 
	// ��� ���������� �����������

};

void CRusSemStructure::DealInvitatoryMoodAfterTree ()
{
  for (long ClauseNo=0; ClauseNo<m_Clauses.size(); ClauseNo++)
  {
	  long RootNodeNo =  GetClauseSyntaxTop(ClauseNo); 
	  if (RootNodeNo == -1) continue;;
	  // ��������, ��� � ������ �����. ������ ��������� ������ �����������, ����� ������������ 
	  // 
	  if ( m_Nodes[RootNodeNo].HasRelOperator ("_����_���������_����������") )
	  {
		if (GetRusSubj(RootNodeNo) == -1)
		{
		  m_Nodes[RootNodeNo].DelRelOperator ("_����_���������_����������");
		  m_Nodes[RootNodeNo].m_RelOperators.push_back("_���������_����������");
		};
	  }
	  else
	  {
		  // ��������� ������  ��������������� �����������
		  if (ClauseNo == 0)  
   		  if  (     m_Nodes[RootNodeNo].HasGrammems ( _QM(rFirstPerson) | _QM(rSingular)  | _QM(rImperative) )  
				 || m_Nodes[RootNodeNo].HasGrammems ( _QM(rPastTense) | _QM(rPlural) )
			  )
		  if (HasRichPOS (RootNodeNo, VERB))  
		  if (GetRusSubj(RootNodeNo) == -1) 
			if (HasSemFet(m_Nodes[RootNodeNo], "MOV"))		  if (GetSubordConjFromTheBegining(ClauseNo) == 0) 
			  m_Nodes[RootNodeNo].m_RelOperators.push_back("_���������_����������");

		  // ����� ������ ��������������� �����������
	  	  if  (     m_Nodes[RootNodeNo].HasGrammems (_QM(rFirstPerson) | _QM(rSingular)  | _QM(rImperative))
				 || m_Nodes[RootNodeNo].HasGrammems (_QM(rPastTense) | _QM(rPlural) )
				 
			  )
		  if (m_Clauses[ClauseNo].m_ClauseRuleNo != -1)
		  if (m_ClauseRules[m_Clauses[ClauseNo].m_ClauseRuleNo].m_Name == "���������")
		  {
			 vector<long> Rels;
			 GetIncomingClauseRelations(ClauseNo, Rels);
			 if (Rels.size() == 1)
			   if (m_Nodes[m_Relations[Rels[0]].m_SourceNodeNo].HasRelOperator("_���������_����������"))
				   m_Nodes[RootNodeNo].m_RelOperators.push_back("_���������_����������");
		  };
		  
	  };
  };
  
};

// ���������� ��� "�������"  � ������� ��������� ����,
// ������� ����� ���������� � ���������� ����������� ��������� ����� (��������, ���������)

void CRusSemStructure::DeleteWordNoGaps() 
{
	vector<long> WordNos;
	long NodeNo=0;
	for (;NodeNo< m_Nodes.size(); NodeNo++)
	  for (long i=0;i< m_Nodes[NodeNo].m_Words.size(); i++)
		  WordNos.push_back(m_Nodes[NodeNo].m_Words[i].m_WordNo);

    sort(WordNos.begin(),WordNos.end());

	for (NodeNo=0;NodeNo< m_Nodes.size(); NodeNo++)
	  for (long i=0;i< m_Nodes[NodeNo].m_Words.size(); i++)
	  {
		  vector<long>::const_iterator It = find(WordNos.begin(),WordNos.end(),m_Nodes[NodeNo].m_Words[i].m_WordNo);
		  m_Nodes[NodeNo].m_Words[i].m_WordNo = It - WordNos.begin();
	  };

   for (long ClauseNo=0; ClauseNo < m_Clauses.size(); ClauseNo++)
   {
	   CRusSemClause& C = m_Clauses[ClauseNo];
	   if (C.m_SubjectWordNo != -1)
	     C.m_SubjectWordNo = find(WordNos.begin(),WordNos.end(),C.m_SubjectWordNo) - WordNos.begin();
	   if (C.m_PredicateWordNo != -1)
	     C.m_PredicateWordNo = find(WordNos.begin(),WordNos.end(),C.m_PredicateWordNo) - WordNos.begin();
	   if (C.m_RootWordNo != -1)
	     C.m_RootWordNo = find(WordNos.begin(),WordNos.end(),C.m_RootWordNo) - WordNos.begin();


   };

};

CRusSemNode Create_EST_Node(CRusSemStructure& R, long ClauseNo)
{
   const CLemmatizer* P = R.m_pData->GetRusLemmatizer(); 
   
   vector<CFormInfo> ParadigmCollection;
   string Est ="����";
   P->CreateParadigmCollection(false,Est,false, false, ParadigmCollection);

   
   assert (!ParadigmCollection.empty());
   long i=0;
   for (; i < ParadigmCollection.size(); i++)
	   if (ParadigmCollection[i].GetWordForm(0) == "����")
		   break;
   assert ( i < ParadigmCollection.size() );

   string GramCodes = ParadigmCollection[i].GetSrcAncode();
   CRusSemWord SemWord(-1, "����");
   QWORD dummy = R.m_pData->GetRusGramTab()->GetAllGrammems(GramCodes.c_str());
   SemWord.SetFormGrammems( dummy );
   SemWord.m_Poses = 1 << VERB;
   SemWord.m_Word = "����";
   SemWord.m_CharCase = LowLow;
   SemWord.m_ParadigmId = ParadigmCollection[i].GetParadigmId();
   SemWord.m_IsPunct =  false;
   SemWord.m_pData =  R.m_pData;
   SemWord.m_ILE =  false;
   CRusSemNode N;
   N.m_Words.push_back(SemWord);
   N.SetGrammems ( SemWord.GetAllGrammems() );
   N.m_GramCodes = GramCodes;
   N.m_SynGroupTypeStr = "";
   N.SetMainWordNo(0);
   R.InitInterps(SemWord, false, ClauseNo, N, false);
   N.m_ClauseNo = ClauseNo;
   return N;
};


// ��������� ���� � ������ ���� "����" � ������ "����"
void CRusSemStructure::ProcessDashClauses()
{
	for (long ClauseNo=0; ClauseNo < m_Clauses.size(); ClauseNo++)
	 // "��� ������ ����������"
	  if (m_Clauses[ClauseNo].m_ClauseType == DASH_T)
	  {
		  bool NodeWasCreated = false; 
		  for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo].m_EndNodeNo;  NodeNo++)
			 if (m_Nodes[NodeNo].IsOneDash())
			  {
				  CRusSemNode N = Create_EST_Node(*this, ClauseNo);
				  N.m_Words[0].m_WordNo = m_Nodes[NodeNo].m_Words[0].m_WordNo;
				  m_Nodes[NodeNo] = N;
				  //  ������ ������ "����" � �������, ���� ���������� �����  ���� � ��������
				  if (NodeNo > 0)
				   if (    (m_Nodes[NodeNo-1].m_ClauseNo == ClauseNo)
					    &&  m_Nodes[NodeNo-1].m_bQuoteMarks
					  )
					  m_Nodes[NodeNo].m_bQuoteMarks = true;

				  NodeWasCreated = true;
			  };

		  // �� ����� "��� - ��� ���" ���������� "���"
		  if (	NodeWasCreated )
  		   for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo].m_EndNodeNo;  NodeNo++)
			   if (m_Nodes[NodeNo].IsLemma("���") && CanBeDeleted(NodeNo) )
			   {
				   DelNode(NodeNo);
				   break;
			   };

	  }
	  else
     // "��� ��?"
	 // "��� ��� ������?"
	  if (m_Clauses[ClauseNo].m_ClauseType == UnknownSyntaxElement)
	  {
		  long NodeNo = GetConjWordFromClause(ClauseNo);
		  if (NodeNo == -1) continue;
		  // "����� ����!" �����  �� ���� ��������������� ������
		  if (m_Nodes[NodeNo].IsLemma("�����")) continue;
		  // �������, ������� ���������� � "���", ����� ������� ������ �����. �������
	      if (    m_Nodes[NodeNo].IsPrimitive()   )
		  {
			  string Word = m_Nodes[NodeNo].m_Words[0].m_Word;
			  EngRusMakeUpper(Word);
              if (Word == "���") continue;
		  };
		  FreeWordNo(m_Nodes[NodeNo].GetMaxWordNo() + 1);
		  CRusSemNode N = Create_EST_Node(*this, ClauseNo);
		  N.m_Words[0].m_WordNo = m_Nodes[NodeNo].GetMaxWordNo() + 1;
		  InsertNode(N);
	  };

};

/*
 ���� � ������ ���� ���� ��� � ����  ����, ����� ������� 
 ���� ���
 �� ������ ����:
"
 ����, ����� ����������� � ���������� ������������: ��� ����,
 ��� ����, ..., ��� � ���� � ������ ����, ������ ����, ... �
  ����� ����. � ������, ��� � ������ ������ ����� �������
 "���".
 "
*/
void CRusSemStructure::ProcessSAM_SEBJA()
{
	for (long i =0; i < m_Nodes.size(); i++)
	  if (m_Nodes[i].IsLemma("����") )
		  if (     (i+1 < m_Nodes.size())
			    && m_Nodes[i+1].IsLemma("���") 
				&& CanBeDeleted(i+1)
			 )
			 DelNode (i+1);
		  else
  		  if (     (i >0 )
			    && m_Nodes[i-1].IsLemma("���") 
				&& CanBeDeleted(i-1)
			 )
		  {
			  DelNode (i-1);
			  i--;
		  };
};


long CRusSemStructure::GetClauseSyntaxTop(long ClauseNo) const 
{
    vector <long> Nodes;

    // ���� � ������ ���� ������ ���� ModalCopul
    FindIf (ClauseNo, &CRusSemStructure::IsModalCopul, Nodes);
    if (Nodes.size() == 1)
		  return Nodes[0];

	// ���� ��� ���������
	if (m_Clauses[ClauseNo].m_RootWordNo == -1) 
	{

      // ���� � ������ ���� ������ ���� ������ � ������ �����
	  FindIf (ClauseNo, &CRusSemStructure::IsFiniteVerb, Nodes);
	  if (Nodes.size() == 1)
		  return Nodes[0];

      // ���� � ������ ���� ������ ���� ���������, �� ����� ������� ��� ����������
	  FindIf (ClauseNo, &CRusSemStructure::IsInfinitive, Nodes);
	  if (Nodes.size() == 1)
		  return Nodes[0];

       // ���� � ������ ���� ������ ���� ���������, �� ����� ������� ��� ����������
	  FindIf (ClauseNo, &CRusSemStructure::IsImperativeVerb, Nodes);
	  if (Nodes.size() == 1)
		  return Nodes[0];

       // ���� � ������ ���� ������ ���� ���������, �� ����� ������� ��� ����������
	  FindIf (ClauseNo, &CRusSemStructure::IsPredicative, Nodes);
	  if (Nodes.size() == 1)
		  return Nodes[0];

	  FindIf (ClauseNo, &CRusSemStructure::IsCompAdj, Nodes);
	  if (Nodes.size() == 1)
		  return Nodes[0];



	}
	else
	{
		if (m_Clauses[ClauseNo].m_RootWordNo == -1) return -1;

	    return FindNodeByWordNo(m_Clauses[ClauseNo].m_RootWordNo, ClauseNo);
	}
	


	return -1;
};


void CRusSemStructure::CreateVerbAnalyticalForm( long AuxVerbNodeNo)
{
	CRusSemNode* AuxVerb = &m_Nodes[AuxVerbNodeNo];
	if (AuxVerb->m_Words.size() != 1) return;
	CRusSemWord* Word = &AuxVerb->m_Words[0];
	if (Word->m_SynWordNo == -1) return;
	const CSynWord& AuxVerbHom  = m_piSent->m_Words[Word->m_SynWordNo];
	if (AuxVerbHom.m_MainVerbs.empty()) return;

	// ���� ������������ ������ ���� ������� ������, ���� ����� ���������  ��� ��������� ��� 
	// �������������� ������
	assert (AuxVerbHom.m_MainVerbs.size() == 1);

	size_t MainVerbNodeNo = 0;
	{
		
		
		for (;MainVerbNodeNo < m_Nodes.size(); MainVerbNodeNo++)
			if (		(m_Nodes[MainVerbNodeNo].m_Words.size() == 1) 
				&&	(m_Nodes[MainVerbNodeNo].m_Words[0].m_SynWordNo == AuxVerbHom.m_MainVerbs[0])
				)
			break;

		if (MainVerbNodeNo == m_Nodes.size())
		{
			assert (false);
			// the main verb is not found, why?
			return;
		};
	};


	string GramCodes;
	QWORD Grammems = 0;
	long Poses = 0;
	

	const CRusGramTab* piRusGramTab = m_pData->GetRusGramTab();

	for (int j = 0; j < m_Nodes[MainVerbNodeNo].m_GramCodes.length(); j += 2)
		for (int i = 0; i < AuxVerb->m_GramCodes.length(); i += 2)
		{
			string strPos;

			//  �������� ���������������� �������
			QWORD VerbGrammems;
			piRusGramTab->GetGrammems(AuxVerb->m_GramCodes.c_str()+i, VerbGrammems);;
			//  �������� ��������� �����
			QWORD MainGrammems;
			piRusGramTab->GetGrammems(m_Nodes[MainVerbNodeNo].m_GramCodes.c_str()+j, MainGrammems);

			BYTE POS = piRusGramTab->GetPartOfSpeech(m_Nodes[MainVerbNodeNo].m_GramCodes.c_str()+j);

			//  ���������� ����� �������� � ����� ����
			QWORD CurrentGrammems = 0;
			size_t CurrentPoses = 0;

			

			if (POS == INFINITIVE)
			{
				CurrentGrammems = MainGrammems;
				if (VerbGrammems & _QM(rImperative))
					{
						//  ����. �����  �������������� ���������� ��. ����� �� ����������
						if (VerbGrammems & _QM(rSingular)) continue;

						CurrentGrammems |= _QM(rImperative); // "����� �� ����!"
					}
					else
						CurrentGrammems |= (VerbGrammems & rAllTimes); // "� ���� ������" ��� "����� ������"

				if ((MainGrammems & _QM(rImpersonal)) == 0)
				{
					CurrentGrammems |= (VerbGrammems & rAllPersons);
					CurrentGrammems |= (VerbGrammems & rAllNumbers);
					CurrentGrammems |= (VerbGrammems & rAllGenders);
				}
				CurrentPoses = Word->m_Poses;
				strPos = "�";
			}
			else
                if ( POS == PARTICIPLE_SHORT )
			{
				// "*����� �� �����!" 
				if (VerbGrammems & _QM(rImperative))
					continue;

				CurrentGrammems = MainGrammems;
				// ������� ����� ���������
				CurrentGrammems &= ~rAllTimes;

				CurrentGrammems |= (VerbGrammems & rAllPersons);
				CurrentGrammems |= (VerbGrammems & rAllTimes);
				
				if ( (VerbGrammems & _QM(rImperative)) != 0 )
					CurrentGrammems |= _QM(rImperative);

				strPos = "���������";
				CurrentPoses |= (1<<PARTICIPLE_SHORT);
			}
			else
			if (POS == ADJ_SHORT) 
			{
				if (VerbGrammems & (1 << rImperative))
						continue; // ����� c������, ��� "����� �� �������!" - ��� �����!
								 // �� ����� ����, ������ �� ������� �������  �� ���� �����.

				CurrentGrammems =  VerbGrammems | (MainGrammems & ( rAllNumbers | rAllGenders | rAllAnimative));

				if ( (CurrentGrammems & _QM(rActiveVoice)) != 0 )
					CurrentGrammems &= ~_QM(rActiveVoice);

				CurrentPoses |= (1<<ADJ_SHORT);
				strPos = "�";
			}
			else
			//  "��� ���� ������"
			if (POS == PREDK)
			{
				CurrentGrammems = VerbGrammems;

				if ( (CurrentGrammems & (1 << rActiveVoice)) != 0 )
					CurrentGrammems &= ~(1 << rActiveVoice);

				strPos = "�����";
				CurrentPoses |= (1<<POS);
			}
			else
			// ��� �� � ��� ���� ������������
			if (POS == PRONOUN_PREDK) 
			{
				// ������������ ���������� ���������, �������� ������
				CurrentGrammems = VerbGrammems | (rAllCases & MainGrammems);

				if ( (CurrentGrammems & (1 << rActiveVoice)) != 0 )
					CurrentGrammems &= ~(1 << rActiveVoice);

				strPos = "��-�����";
				CurrentPoses |= (1<<POS);
			}
			else
			// �� ��� ������ ����
			if ( (MainGrammems & _QM(rComparative)) != 0 )
			{
				if ( (VerbGrammems & _QM(rActiveVoice)) != 0 )
					VerbGrammems &= ~_QM(rActiveVoice);

				CurrentGrammems = (VerbGrammems | _QM(rComparative));
				strPos = "�";
				CurrentPoses  |= (1<<POS);
			}
			else
			if (POS == ADJ_FULL)  // ���������� ��������, ��������� ����. �����  � ������ ��������. �� ��������.
			{
				continue;
			};

			// ���� ���������������  ������ ��� �����������, �����  ��� ������ - ���������
			//  "����  �����" ��� "������ ���� ���������"
			if ( AuxVerb->HasPOS(INFINITIVE) )
			{
				CurrentPoses |= (1 << INFINITIVE);
				strPos = "���������";
			}
			else
			//  "������ ������"
			if ( AuxVerb->HasPOS(ADVERB_PARTICIPLE))
			{
				CurrentPoses |= (1 << ADVERB_PARTICIPLE);
				strPos = "������������";
			}
			
			if (strPos == "") continue;


			BYTE iPos;
			QWORD iGrm;
			strPos += " ";
			strPos += piRusGramTab->GrammemsToStr(CurrentGrammems);
			piRusGramTab->ProcessPOSAndGrammems(strPos.c_str(), iPos, iGrm); 

			string buffer;
			if (!piRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan(iPos, iGrm, buffer))
			{
				//ErrorMessage(Format("Cannot get gramcode  by %s",strPos.c_str(), Word->m_Lemma.c_str()));
				//assert (false);
			}
			else
				GramCodes += buffer;


			Grammems |= CurrentGrammems;
			Poses |= CurrentPoses;
		
		};

	assert ((Grammems != 0) && (Poses != 0));

	if ( (Grammems != 0) && (Poses != 0) && CanBeDeleted(MainVerbNodeNo) )
	{
		// "���� ������" "����� ������" "����� ������" - ����������� ���
		if (AuxVerb->IsLemma("�����") )
		{
			Grammems |= _QM(rPerfective);
		}

		Word->m_Lemma = m_Nodes[MainVerbNodeNo].m_Words[0].m_Lemma;
		Word->m_Word += "-"+m_Nodes[MainVerbNodeNo].m_Words[0].m_Word;
		Word->SetFormGrammems  (Grammems);
		Word->m_Poses = Poses;
		Word->m_ParadigmId = m_Nodes[MainVerbNodeNo].m_Words[0].m_ParadigmId;
		
		AuxVerb->SetGrammems(Grammems);
		AuxVerb->m_GramCodes = GramCodes;
		AuxVerb->m_RichPoses = Poses;
		AuxVerb->CopyInterps(m_Nodes[MainVerbNodeNo]);
		AuxVerb->m_TypeAncode = m_Nodes[MainVerbNodeNo].m_TypeAncode;
		AuxVerb->m_Colloc = m_Nodes[MainVerbNodeNo].m_Colloc;
		MoveSynRelations(MainVerbNodeNo, AuxVerbNodeNo);
		DelNode(MainVerbNodeNo);

	};




}



void CRusSemStructure::BuildSemNodesBySyntax() 
{
  	rml_TRACE ("starting BuildSemNodesBySyntax\n");	
	m_SynRelations.clear();
	m_SynClauseRels.clear();
	m_Relations.clear();
	m_LexFuncts.clear();
	m_ThesSemRelations.clear();
	m_DopRelations.clear();

	CRelationsIterator RelIt; 
	RelIt.SetSentence(m_piSent);
	for (long i=0; i <m_Clauses.size(); i++)
		RelIt.AddClauseNoAndVariantNo(m_piSent->m_vectorPrClauseNo[i], m_Clauses[i].m_ClauseVariantNo);


	if (!RelIt.BuildRelations())
		throw CExpc ("Cannot  build relations");


	// ������� ������ 
	for(size_t j = 0 ; j < RelIt.GetFirmGroups().size() ; j++ )
	{
		CRusSemNode N = CreateNode(&RelIt, j);
		if (N.IsWordContainer()) 
			InsertNode(N);
	}


	// ������� ����������� ���� 
	for (size_t j = 0; j < m_piSent->m_Words.size(); j++)
		if (FindNodeByWordNo(j) == -1)
			if (!m_piSent->m_Words[j].m_bDeleted)
			{
				CRusSemNode N = CreatePrimitiveNode(j);
				if (N.IsWordContainer()) 
					InsertNode(N);
			};



	string Sentence = "Find Situations :"; 
	// ������� ����������� ���� 
	for (size_t j = 0; j < m_piSent->m_Words.size(); j++)
	{
		Sentence += m_piSent->m_Words[j].m_strWord;
		Sentence += " ";


	};



	// ������� ���������
	for(long RelNo = 0 ; RelNo < RelIt.GetRelations().size() ; RelNo++ )
	{
		const CSynOutputRelation& piRel = RelIt.GetRelations()[RelNo];
		string RelName = m_piSent->GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type);
		long SourceNodeNo = (piRel.m_iSourceGroup == -1) ? FindNodeByWordNo(piRel.m_Relation.m_iFirstWord) :  FindNodeBySyntaxGroupNo(piRel.m_iSourceGroup);
		if (SourceNodeNo == -1) 
			continue; // ������: ���� �� �� ���� , � ��� �� ����������
		long TargetNodeNo = (piRel.m_iTargetGroup == -1)? FindNodeByWordNo(piRel.m_Relation.m_iLastWord) :  FindNodeBySyntaxGroupNo(piRel.m_iTargetGroup);
		if (TargetNodeNo == -1) 
			continue; // ������: ���� �� �� ���� , � ��� �� ����������
		
		// ���� ��������� �����������, ����� ��������� � ����������� 
		if (m_Nodes[SourceNodeNo].m_ClauseNo != m_Nodes[TargetNodeNo].m_ClauseNo)
			m_SynClauseRels.push_back(CSynRelation(m_Nodes[SourceNodeNo].m_ClauseNo, m_Nodes[TargetNodeNo].m_ClauseNo, RelName));
		else
			// ����� - � ������� 
		{
			m_SynRelations.push_back(CSynRelation(SourceNodeNo, TargetNodeNo, RelName));

			/*����� ������� ������. �������� �� ����� " � ���� ��� ������ ����", ��� � "����" ������
			��������� ����������� �����, ������� ���������� ����������� */

			if  (m_SynRelations.back().m_SynRelName == "���_����_���")
				if (   m_Nodes[m_SynRelations.back().m_TargetNodeNo].IsWordForm("������")
					|| m_Nodes[m_SynRelations.back().m_TargetNodeNo].IsWordForm("������")  
					)
					m_Nodes[m_SynRelations.back().m_SourceNodeNo].AddOneGrammem (rGenitiv);
			//������ ����_���
			if( m_SynRelations.back().m_SynRelName.find("����_���")!=string::npos
				|| piRel.m_Relation.type == 25) //NOUN_NUMERAL_APPROX
			{
				const CRusGramTab *R = (CRusGramTab*)piRel.GetOpt()->GetGramTab();
				if( piRel.m_Relation.m_GramCodes != "" && m_SynRelations.back().m_SynRelName!="���_����_���")
					m_Nodes[TargetNodeNo].m_GramCodes = piRel.m_Relation.m_GramCodes; //����
				QWORD RelGr = piRel.m_Relation.m_iGrammems;
				if( !(RelGr& rAllGenders) ) 
					RelGr |= m_Nodes[SourceNodeNo].GetGrammems() & rAllGenders;
				m_Nodes[SourceNodeNo].m_GramCodes = R->GleicheAncode1(0, "����������������������������������������������������������������������������",
					R->GetGramCodes(NOUN, RelGr, CaseNumberGender0));
				m_Nodes[SourceNodeNo].SetGrammems(RelGr);
				m_Nodes[TargetNodeNo].SetGrammems(RelGr); //m_Nodes[TargetNodeNo].GetGrammems() & ~(rAllCases|rAllGenders|rAllPersons|rAllAnimative) | 
			}
		};
		// ��������������� "�������������" ����� ����� �� �������� �����, ���������,
		//  ��������, ������ ����_���, ���������� �������� ���������������� � ��������������,
		// � � ��������������� �� ������ ��������������. ������� � � ���� ������ �� ����� ���� ��������������.
		if (piRel.m_Relation.m_iGrammems != 0)
			m_Nodes[SourceNodeNo].AddGrammems ( piRel.m_Relation.m_iGrammems | (m_Nodes[SourceNodeNo].GetGrammems() & (_QM(rAnimative)|_QM(rNonAnimative))));

	};

	for (size_t i=0; i < m_Nodes.size(); i++)
		CreateVerbAnalyticalForm(i);

	for (long ClauseNo=0; ClauseNo< m_Clauses.size(); ClauseNo++)
		if (m_Clauses[ClauseNo].m_BeginNodeNo+1 < m_Clauses[ClauseNo].m_EndNodeNo)
			if (   ( m_Nodes[m_Clauses[ClauseNo].m_BeginNodeNo].m_Words[0].m_GraphDescrs.find("OPN") != -1)
				/*
				���� ���������, ��� ��������� ���
				������������� (�  ����� ����� ������ �����) ���� ����������� ������ .
				*/
				&& (    ( m_Nodes[m_Clauses[ClauseNo].m_EndNodeNo-1].m_Words[0].m_GraphDescrs.find("CLS") != -1)
				||  ( m_Nodes[m_Clauses[ClauseNo].m_EndNodeNo-2].m_Words[0].m_GraphDescrs.find("CLS") != -1)
				)
				)
				m_Clauses[ClauseNo].m_bBracketed = true;

	for (size_t i=0; i < m_Nodes.size(); i++)
		if (!m_Nodes[i].IsOneDash() )  // ���� ������������ ProcessDashClauses
			if (m_Nodes[i].IsPrimitive() && CanBeDeleted(i))
				if (m_Nodes[i].m_Words[0].m_IsPunct)
					if (GetSynRelationsCount(i) == 0)
					{
						if (m_Nodes[i].m_Words[0].m_GraphDescrs.find ("PLU") != -1) continue;
						/*
						������ 1 "� ���� ���� �����, ������ � ������". ��������� ����� �� ��������
						���������� ����������. ����� �� ������� ������� �������� ������������.
						*/
						if (	 (m_Nodes[i].m_Words[0].m_Word == ",")
							&& ( i+1 < m_Nodes.size() )
							&& (m_Nodes[i].m_ClauseNo == m_Nodes[i+1].m_ClauseNo)
							&& (m_Nodes[i].GetMinWordNo()+1 == m_Nodes[i+1].GetMinWordNo())
							&& (GetIncomingSynRelationsCount(i + 1) == 0)
							&& (i > 0)
							&& (m_Nodes[i-1].m_ClauseNo == m_Nodes[i].m_ClauseNo)
							&& (m_Nodes[i-1].GetMinWordNo()+1 == m_Nodes[i].GetMinWordNo())
							)
						{
							m_SynRelations.push_back(CSynRelation(i, i+1, "�����"));
							continue;
						};

						DelNode(i);
						i--;
					};

	DeleteEmptyClauses ();

	FindMotherLandForPustycha();



	for (size_t i =0; i < m_Clauses.size() ; i++)
	{
		FindParticleBY(i);
		m_Clauses[i].m_ClauseSyntaxTop = GetClauseSyntaxTop(i);
		m_Clauses[i].m_bHasCHEM = ClauseHasChem(i);
		ZaplataCompar1(i);

	};

	ConvertRusNumeralsToArabic();


	ProcessDashClauses();

	ProcessSAM_SEBJA();


	// ��������� ���������� (������������� �� ���������)


	m_SentenceMood = Indicative;

	if (!m_piSent->m_Words.empty())
	{
		string t = m_piSent->m_Words.back().m_strWord;
		if (t == "?")
			m_SentenceMood = Interrogative;
		else
			if (t == "!")
				m_SentenceMood = Exclamative;
	};

	/*
	� ����� ���� �� �������� �����, � ����� ���� ��� ��, ��� � ��.
	��������, "�� ������ ����", "*��� ��������� ������� ����� �����"
	*/
	for (size_t i=0; i < m_Nodes.size(); i++)
		if (   (m_Nodes[i].IsPrimitive())
			&& (m_Nodes[i].m_Words[0].m_Lemma == "����")
			)

			m_Nodes[i].AddGrammems (rAllNumbers);

	for (size_t i=0; i < m_Nodes.size(); i++)
		m_Nodes[i].m_bCompAdj =    IsCompAdj(i)  || HasSynRelation (i, "����_�����");

	/*
	������������ ������� ���_����� �� ����� 
	"��� ����� ����� ������� ������" �� ����� "�����" �� "�������",
	*/
	for (size_t i=0; i < m_SynRelations.size(); i++)
		if (m_SynRelations[i].m_SynRelName == "����_�����")
			if ( m_Nodes[m_SynRelations[i].m_TargetNodeNo].HasRelOperator("���_�����") && CanBeDeleted(m_SynRelations[i].m_TargetNodeNo) )
			{
				m_Nodes[m_SynRelations[i].m_TargetNodeNo].DelRelOperator("���_�����");
				m_Nodes[m_SynRelations[i].m_SourceNodeNo].m_RelOperators.push_back("���_�����");
				DelNode(m_SynRelations[i].m_TargetNodeNo);
			};


	BuildAnalyticalSupelative();

	for (size_t ClauseNo =0; ClauseNo< m_Clauses.size() ; ClauseNo++)
	{
		m_Clauses[ClauseNo].m_bQuoted = true;
		for (int NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo<m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
			m_Clauses[ClauseNo].m_bQuoted  = m_Clauses[ClauseNo].m_bQuoted && m_Nodes[NodeNo].m_bQuoteMarks; 
	};
	//*

};

