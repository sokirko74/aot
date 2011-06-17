#include "GerSynan.h"
#include "GerFormatCaller.h"
#include "GerWord.h"

	const size_t MaxNPSize = 100;


inline bool MixedGleiche (QWORD noun, QWORD adj) 
{	
	if (	((adj & _QM(gAdjektiveMitUnbestimmte)) == 0) 
		&&	((adj & _QM(gPossessiv)) == 0) 	
		)
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};

inline bool WeakGleiche (QWORD noun, QWORD adj) 
{	
	if ((adj & _QM(gAdjektiveMitBestimmte)) == 0) 
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};

inline bool SoloGleiche (QWORD noun, QWORD adj) 
{	
	if ((adj & _QM(gAdjektiveOhneArtikel)) == 0) 
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};


void CreateAdjNounGroup(int GroupType, CGerFormatCaller& Caller, size_t  noun, CGroup& G, const size_t* LeftModifiers, size_t LeftModifiersCount, int AdjectiveAsNoun)
{
	//  creating new group
	G.m_iLastWord = Caller.get_maximal_group(noun).m_iLastWord;
	G.m_MainGroup = Caller.get_maximal_group(noun);
	G.m_GroupType = GroupType;

	// creating relation from the noun to adjectives
	for (size_t j=0; j < LeftModifiersCount; j++)
	{
		size_t k = LeftModifiers[j];
		Caller.create_syn_rel(G, noun , k , GroupType);
	};

	// Now we should change grammems  in all adjectives and in the main  noun
	// to the common intersection. Note that  we the main noun could already subdue another nouns 
	// (for example rGenitiv in postposition) which should be left untouched
	CPeriod P (G.m_iFirstWord, noun);
	if (G.GetGrammems() & _QM(gPlural))
		Caller.change_words_in_group_grammems(P, G.GetGrammems(), (gAllNumbers | gAllCases));  
	else
		Caller.change_words_in_group_grammems(P, G.GetGrammems(), (gAllNumbers | gAllCases |gAllGenders ));  
	
	if (noun == AdjectiveAsNoun)
		Caller.sent[noun].SetFlag(fl_adjective_as_noun, true);

}


// Examples:
// die schwache Resolution
// ein junger Boxer
// des jungen Boxers
// der bunte Stoff
// die alte Burg
// die kleinen Kinder
// dem bunten Stoff
// der kleinen Kinder
// jeden Tag
// die ersten 2 Jahre 



bool CGerFormatCaller::format_for_det_adj_nomen (CGroup& G)
{	
	// G.m_iFirstWord is the first word of a possible group
	int i = get_main_word(G.m_iFirstWord);
	

	const CGroup& H = get_maximal_group(i);
	QWORD PrepositionGrammems;
	bool bPrepositionArticle  =		(H.m_iFirstWord > 0) 
								&&	IsPrepositionArticle(sent[H.m_iFirstWord-1].m_word_upper, PrepositionGrammems);

	// the group should start with an article, or before it should be a compound preposition
	bool bMixed = Wi.HasFlag(fl_article_for_mixed_declination);
	bool bWeak = Wi.HasFlag(fl_article_for_weak_declination) || bPrepositionArticle;

	if (!bMixed &&  !bWeak)  return false;
	
	size_t LeftModifiers[MaxNPSize];
	size_t LeftModifiersCount = 0;

	int k  = i;
	if (!bPrepositionArticle)
	{
		LeftModifiers[LeftModifiersCount++] = i;
		k  = get_next_main_word(i);
	};

	int AdjectiveAsNoun = -1;
	// going through all adjectives
	for(; k < sent.size(); k = get_next_main_word(k))
	{
		if ( !is_left_noun_modifier (sent[k])  ) 
			break;

		LeftModifiers[LeftModifiersCount++] = k;
		if (LeftModifiersCount == MaxNPSize) break;

		if (sent[k].HasFlag(fl_first_char_is_upper))
			AdjectiveAsNoun = k;
	};

	// if no noun found then exit
	int noun = k;
	if	(		(noun == sent.size()) 
			||	!is_morph_noun(sent[noun]) 
		) 
	{
		if (AdjectiveAsNoun != -1)
			noun = AdjectiveAsNoun;
		else
			return false;
	};

	// the group should be contain at least two subgroups
	if (LeftModifiersCount == 0)  return false;

	G.SetGrammems( sent[noun].GetGrammems());

	// the adjectives, the determiner and the noun should be agreed in Number and Case and Gender
	size_t j=0;
	for (j=0; j < LeftModifiersCount; j++)
	{
		size_t k = LeftModifiers[j];

		QWORD CurrGleiche;

		if ( IstZahlWort(Wk) )
		{
			G.SetGrammems(G.GetGrammems() ^  _QM(gSingular));
		}
		else
		{
			if (bMixed)
				CurrGleiche = GetGramTab()->Gleiche(MixedGleiche, sent[noun].m_gramcodes, Wk.m_gramcodes);
			else
				CurrGleiche = GetGramTab()->Gleiche(WeakGleiche, sent[noun].m_gramcodes, Wk.m_gramcodes);

			if (!CurrGleiche)
				return false;

			G.SetGrammems(G.GetGrammems() & CurrGleiche);
		}
		string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());
	};
		

	if (bPrepositionArticle)
		G.SetGrammems(G.GetGrammems() & PrepositionGrammems);

	string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());

	if (!check_noun_or_adj_group_grammems(G.GetGrammems()) ) return false;
	
	CreateAdjNounGroup (gDET_ADJ_NOMEN, *this, noun,G, LeftModifiers, LeftModifiersCount, AdjectiveAsNoun);
	return true;
};

// it builds NP without determiner:
// gutes Wetter
// kleiner guter Junge
// junger Frau // rDativ
// Mindestens 43 Menschen 
// ungefahr 2 Franken 

bool CGerFormatCaller::format_for_adj_nomen (CGroup& G)
{
	int i = get_main_word(G.m_iFirstWord);
	if ( sent[i].m_UnitType != EWord  ) 
			return false;

	bool bFirstAdjIsNumeral = IstZahlWort(sent[i]);
	if (!bFirstAdjIsNumeral)
		if ( !is_left_noun_modifier(sent[i])  ) 
				return false;

	
	size_t LeftModifiers[MaxNPSize];
	size_t LeftModifiersCount = 0;
	LeftModifiers[LeftModifiersCount++] = i;
	int FirstAdjWordNo = -1;
	if (!bFirstAdjIsNumeral)
		FirstAdjWordNo = i;


	// going through the afjectives, finding a noun
	int k  = get_next_main_word(i);
	int AdjectiveAsNoun = -1;
	for (; k < sent.size(); k  = get_next_main_word(k))
	{
		if ( !is_left_noun_modifier(sent[k])  ) 
			break;

		if (FirstAdjWordNo != -1)
		{
			if (!HaveEqualAdjEndungen(sent[k], sent[FirstAdjWordNo]))
				break;
		}
		else
			FirstAdjWordNo = k;

		LeftModifiers[LeftModifiersCount++] = k;
		if (LeftModifiersCount == MaxNPSize) break;

		if (sent[k].HasFlag(fl_first_char_is_upper))
			AdjectiveAsNoun = k;

	};

	int noun = k;

	if	(		(noun >= sent.size()) 
			||	!is_morph_noun(sent[noun]) 
		) 
	{
		if (AdjectiveAsNoun != -1)
		{
			noun = AdjectiveAsNoun;
			for (size_t i=0; i<LeftModifiersCount;i++)
				if (AdjectiveAsNoun == LeftModifiers[i])
				{
					LeftModifiersCount = i;
					break;
				};
		}
		else
			return false;
	};



	// since all adjective's flexia are equal we can get group grammems from the first adjective 
	// and from the noun
	if (FirstAdjWordNo != -1)
		G.SetGrammems( GetGramTab()->Gleiche(SoloGleiche, sent[noun].m_gramcodes, sent[FirstAdjWordNo].m_gramcodes) );
	else
	{
		// "zwei Gefechte"
		G.SetGrammems(  sent[noun].GetGrammems() );
	};

	string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());

	if (!check_noun_or_adj_group_grammems(G.GetGrammems()) ) return false;

	
	
	CreateAdjNounGroup (gADJ_NOMEN, *this, noun,G, LeftModifiers, LeftModifiersCount, AdjectiveAsNoun);

	return true;
};
