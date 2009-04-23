// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko



#include "StdSynan.h"
#include "RusSentence.h"
#include "assert.h"




bool CRusSentence::IsAdjDeclination (const CSynHomonym& H) const
{
	if (!H.IsMorphNoun()) return false;
	if (H.m_strLemma.length() < 3)  return false;
	if (H.m_lPradigmID == -1) return false;
	string suffix = H.m_strLemma.substr(H.m_strLemma.length()-2);
	bool bMasc = (suffix == "ÈÉ") ||  (suffix == "ÛÉ");
	bool bFem = (suffix == "Àß") ||  (suffix == "ßß");
	if (!bMasc && !bFem) return false;

	CFormInfo Info;
	GetOpt()->GetLemmatizer()->CreateParadigmFromID(H.m_lPradigmID, Info);
	for (long k=0; k < Info.GetCount(); k++)
	{
		string Ancode = Info.GetAncode(k);
		QWORD  g = GetRusGramTab()->GetAllGrammems(Ancode.c_str());
		if (  g & _QM(rSingular) )
			if ( g & _QM(rGenitiv) )
			{
				string Form = Info.GetWordForm(k);
				int l = Form.length();
				if (l < 3) return false;
				if (bMasc)
					return		(Form.substr(l-3) == "ÅÃÎ")
							||	(Form.substr(l-3) == "ÎÃÎ");
				else
					return		(Form.substr(l-2) == "ÎÉ")
							||	(Form.substr(l-2) == "ÅÉ");
			};
	};
	return false;		
};

void CRusSentence::InitHomonymMorphInfo (CSynHomonym& H)
{

    H.InitAncodePattern( );
	
	//ñðàâíåíèå ñî ñëîâíèêàìè
	H.m_bMonth = GetOpt()->GetGramTab()->is_month(H.m_strLemma.c_str());
	H.m_bAdvAdj = H.CompareWithPredefinedWords(*(GetOpt()->AdvAdj));
	H.m_bCanSynDependOnAdj = H.CompareWithPredefinedWords(*(GetOpt()->SynDependOnAdj));
	H.m_bCanSynDependOnAdv = H.CompareWithPredefinedWords(*(GetOpt()->SynDependOnAdv));
	H.m_bCanSubdueInfinitive = H.CompareWithPredefinedWords(*(GetOpt()->VerbsThatCanSubdueInfinitive));
	H.m_bCanSubdueInstr = H.CompareWithPredefinedWords(*(GetOpt()->m_pVerbsWithInstrObj));
	H.m_bNounHasAdjectiveDeclination = IsAdjDeclination(H);

	
	
};

const UINT SmallNumbersCount = 5;
const char SmallNumbers [SmallNumbersCount][10] = {"ÄÂÀ", "ÒÐÈ", "×ÅÒÛÐÅ","ÎÁÀ", "ÏÎËÒÎÐÀ"};


void InitSmallNumberSlot(CSynHomonym& H, const CLemWord* pWord)
{
	int iLen = pWord->m_strWord.length();
	assert(iLen > 0);
    H.m_bSmallNumber =    pWord->HasDes(ODigits)
	                  && (    pWord->m_strWord[iLen - 1] == '2' 
					       || pWord->m_strWord[iLen - 1] == '3'
						   || pWord->m_strWord[iLen - 1] == '4');
	H.m_bRussianOdin =	 pWord->HasDes(ODigits)
					&&	pWord->m_strWord[iLen - 1] == '1'
					//  ìîæåò çàêàí÷èâàòüñÿ íà 01, 21,31,41,51,61,71,81,91, íî íå íà 11
					&&	(		(iLen == 1) 
							||	(pWord->m_strWord[iLen - 2] != '1')
						);

	if( pWord->HasDes(ODigits) && (iLen == 1) && ( pWord->m_strWord[0] == '1') )
	{
		H.m_GramCodes = "éàéáéâéãéäéåéæéçéèéééêéëéìéíéîéïéðéñ";
        H.InitAncodePattern( );
		
	}

	if( (iLen > 1) && pWord->HasDes(ODigits) )
		if(pWord->m_strWord[iLen - 2] == '1')
			H.m_bSmallNumber = false;

	if( !pWord->HasDes(ODigits ))
	{
		for (long  i=0; i<SmallNumbersCount; i++)
		  if (	H.IsLemma(SmallNumbers[i]) // m_strLemma ìîæåò áûòü ðàâíà "îäèí-äâà",
			  || (   (H.m_strLemma.find('-') != string::npos) // íàïðèìåð, "îäèí-äâà äíÿ", òîãäà íàäî ñðàâíèâàòü ñ ïîñëåäíèì ÷èñëèòåëüíûì
				  && (strlen(SmallNumbers[i]) < H.m_strLemma.length())
				  && (SmallNumbers[i] == H.m_strLemma.substr(H.m_strLemma.find('-') + 1))
				 )
			  )
		  {
				H.m_bSmallNumber = true;
		  };
	};

	if( (H.m_strLemma=="ÎÄÈÍ") &&  H.m_GramCodes[0] == 'ý'/*(m_iPoses & (1 << NUMERAL))*/)
		H.m_bRussianOdin = true;
}

void CRusSentence::InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord)
{
	InitSmallNumberSlot(H, pWord);
};



bool CRusSentence::HasNounInNom(const CSynWord& _W  )  const
{
	for (int i = 0; i < _W.m_Homonyms.size(); i++)
		if ( _W.m_Homonyms[i].IsSynNoun() ) 
		{
			if ( _W.m_Homonyms[i].HasGrammem(rNominativ)
			     && !_W.m_Homonyms[i].HasGrammem(rGenitiv) 
			     && !_W.m_Homonyms[i].HasGrammem(rDativ)
			     && !_W.m_Homonyms[i].HasGrammem(rInstrumentalis)
			     && !_W.m_Homonyms[i].HasGrammem(rLocativ) )
				return true;
		}
		
	return false; 
}











bool CRusSentence::AllHomonymsArePredicates(const CSynWord& W) const
{
	int i = 0;
	for (; i < W.m_Homonyms.size(); i++)
	{
		const CSynHomonym& Hom = W.m_Homonyms[i];

	    if ( (Hom.HasPos(VERB)) || (Hom.HasPos(ADVERB_PARTICIPLE)) ||
			 (Hom.HasPos(ADJ_SHORT)) || (Hom.HasPos(PARTICIPLE_SHORT)) ||
			 (Hom.HasPos(PREDK)) )
			 continue;

		//ãëàãîëû, êîòîðûå íå ÿâëÿþòñÿ ñàìîñòîÿòåëüíûì ïðåäèêàòîì èëè ñóòü àí.ô.
		if (	!Hom.IsLemma("ÄÀÂÀÒÜ") 
			&&	!Hom.IsLemma("ÑÒÀÍÎÂÈÒÜÑß")
			&&	!Hom.IsLemma("ÎÊÀÇÀÒÜÑß")
			&&	!Hom.IsLemma("ÑÒÀÒÜ")
			)
			  break;
	 }


	return i == W.m_Homonyms.size();
}






bool CRusSentence::WordSchemeEqForThesaurus(const CSynHomonym& Homonym, const CSynPlmLine& word_scheme) const
{
	bool bRes;
	bRes = (	(Homonym.m_iGrammems & word_scheme.GetGrammems()) == word_scheme.GetGrammems()) 
        && (Homonym.m_iPoses & word_scheme.GetPoses());	

	return bRes;	
}

