// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko



#include "StdSynan.h"
#include "RusSentence.h"
#include "assert.h"




bool CRusSentence::IsAdjDeclination (const CSynHomonym& H) const
{
	if (!H.IsMorphNoun()) return false;
	if (H.GetLemma().length() < 3)  return false;
	if (H.m_lPradigmID == -1) return false;
	std::string suffix = H.GetLemma().substr(H.GetLemma().length()-2);
	bool bMasc = endswith(H.GetLemma(), "ИЙ") || endswith(H.GetLemma(), "ЫЙ");
	bool bFem = endswith(H.GetLemma(), "АЯ") || endswith(H.GetLemma(), "ЯЯ");
	if (!bMasc && !bFem) return false;

	CFormInfo Info;
	GetOpt()->GetLemmatizer()->CreateParadigmFromID(H.m_lPradigmID, Info);
	for (long k=0; k < Info.GetCount(); k++)
	{
		std::string Ancode = Info.GetAncode(k);
		uint64_t  g = GetRusGramTab()->GetAllGrammems(Ancode.c_str());
		if (  g & _QM(rSingular) )
			if ( g & _QM(rGenitiv) )
			{
				std::string Form = convert_to_utf8(Info.GetWordForm(k), morphRussian);
				if (bMasc)
					return		endswith(Form, "ЕГО")
							||  endswith(Form, "ОГО");
				else
					return		endswith(Form, "ОЙ")
							|| endswith(Form, "ЕЙ");
			};
	};
	return false;		
};

const static std::unordered_set<std::string> MONTHS = {
	"ЯНВАРЬ", "ФЕВРАЛЬ", "МАРТ",
	 "АПРЕЛЬ","МАЙ","ИЮНЬ",
	 "ИЮЛЬ","АВГУСТ","СЕНТЯБРЬ"
	 ,"ОКТЯБРЬ","НОЯБРЬ","ДЕКАБРЬ" };


void CRusSentence::InitHomonymMorphInfo (CSynHomonym& H)
{

    H.InitAncodePattern( );
	
	//сравнение со словниками
	H.m_bMonth = _find(MONTHS, H.GetLemma());
	H.m_bAdvAdj = H.CompareWithPredefinedWords(GetOpt()->m_AdvAdj);
	H.m_bCanSynDependOnAdj = H.CompareWithPredefinedWords(GetOpt()->m_SynDependOnAdj);
	H.m_bCanSynDependOnAdv = H.CompareWithPredefinedWords(GetOpt()->m_SynDependOnAdv);
	H.m_bCanSubdueInfinitive = H.CompareWithPredefinedWords(GetOpt()->m_VerbsThatCanSubdueInfinitive);
	H.m_bCanSubdueInstr = H.CompareWithPredefinedWords(GetOpt()->m_pVerbsWithInstrObj);
	H.m_bNounHasAdjectiveDeclination = IsAdjDeclination(H);
};

const uint32_t SmallNumbersCount = 5;
std::string SmallNumbers[SmallNumbersCount] = {
	"ДВА", "ТРИ", "ЧЕТЫРЕ", "ОБА", "ПОЛТОРА"
};


void CRusSentence::InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord)
{
    int iLen = pWord->m_strWord.length();
    assert(iLen > 0);
    H.m_bSmallNumber =    pWord->HasDes(ODigits)
                          && (    pWord->m_strWord[iLen - 1] == '2'
                                  || pWord->m_strWord[iLen - 1] == '3'
                                  || pWord->m_strWord[iLen - 1] == '4')
                          && FindFloatingPoint(pWord->m_strWord) == -1;
    H.m_bRussianOdin =	 pWord->HasDes(ODigits)
                           &&	pWord->m_strWord[iLen - 1] == '1'
                           //  может заканчиваться на 01, 21,31,41,51,61,71,81,91, но не на 11
                           &&	(		(iLen == 1)
                                          ||	(pWord->m_strWord[iLen - 2] != '1')
                           )
                           && FindFloatingPoint(pWord->m_strWord) == -1;


    if( (iLen > 1) && pWord->HasDes(ODigits) )
        if(pWord->m_strWord[iLen - 2] == '1')
            H.m_bSmallNumber = false;

    if( !pWord->HasDes(ODigits ))
    {
        for (long  i=0; i<SmallNumbersCount; i++)
            if (	H.IsLemma(SmallNumbers[i]) // GetLemma() может быть равна "один-два",
                    || (   (H.GetLemma().find('-') != std::string::npos) // например, "один-два дня", тогда надо сравнивать с последним числительным
                           && (SmallNumbers[i].length() < H.GetLemma().length())
                           && (SmallNumbers[i] == H.GetLemma().substr(H.GetLemma().find('-') + 1))
                    )
                    )
            {
                H.m_bSmallNumber = true;
            };
    };

    if( (H.GetLemma() == "ОДИН") &&   GetRusGramTab()->GetPartOfSpeech(H.GetGramCodes().c_str()) == NUMERAL)
        H.m_bRussianOdin = true;
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

		//глаголы, которые не являются самостоятельным предикатом или суть ан.ф.
		if (	!Hom.IsLemma("ДАВАТЬ")
			&&	!Hom.IsLemma("СТАНОВИТЬСЯ")
			&&	!Hom.IsLemma("ОКАЗАТЬСЯ")
			&&	!Hom.IsLemma("СТАТЬ")
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

