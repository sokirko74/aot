// CMorphCorpusCreator.cpp: implementation of the CMorphCorpusCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MorphCorpusCreator.h"
#include "../common/util_classes.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMorphCorpusCreator::CMorphCorpusCreator()
{
	m_b_show_clause = false;
	m_bArtificialCases = false;
}


string GetSentenceStr (SYNANLib::ISentencePtr piSent)
{
	string Result;
	for (size_t  i=0; i < piSent->WordsNum; i++)
	{
		Result += piSent->Word[i]->WordStr;
		Result +=  " ";
	};
	return Result;

};

string CMorphCorpusCreator::get_xml_result(const CCOMSyntaxHolder& Holder)
{
	m_CurrentLanguage = Holder.m_CurrentLanguage;
	m_piLemmatizer = Holder.m_piLemmatizer;
	m_pGramTab = Holder.m_pGramTab;

	int i;
	try
	{
		string processed_text = "<?xml version=\"1.0\" encoding=\"windows-1251\" ?>\n<text>\n";

		for(i = 0 ; i < Holder.m_piSentCollection->SentencesCount ; i++ )
		{
			SYNANLib::ISentencePtr piSent = Holder.m_piSentCollection->GetSentence(i);
			fprintf (stderr, "generating xml %i/%i \r",  i, Holder.m_piSentCollection->SentencesCount);
			process_sent(piSent, processed_text);
		}
		fprintf (stderr, "generating xml %i/%i \n",  i, Holder.m_piSentCollection->SentencesCount);

		processed_text += "\n</text>\n";
		return processed_text;
	}
	catch(...)
	{
		string sent_str = GetSentenceStr(Holder.m_piSentCollection->GetSentence(i));
		ErrorMessage( Format("Error while processing sentnce \"%s\"\n", sent_str.c_str()));
		throw ;
	}
}

void CMorphCorpusCreator::get_top_clauses(SYNANLib::ISentencePtr piSent, vector<long>& topClauses)
{	
	topClauses.clear();
	long clCount = piSent->ClausesCount;
	SYNANLib::IClausePtr piNextClause;

	for(long i = piSent->ClausesCount - 1 ; i >= 0 ; i-- )
	{
		SYNANLib::IClausePtr piClause = piSent->Clause[i];

		if( (piNextClause == NULL) || (piNextClause->FirstWord == piClause->LastWord + 1 ) )
		{
			piNextClause = piClause;
			topClauses.push_back(i);
		}			
	}
	reverse(topClauses.begin(), topClauses.end());		
}



void CMorphCorpusCreator::process_sent(SYNANLib::ISentencePtr piSent, string& result)
{
	vector<long> topClauses;
	get_top_clauses(piSent, topClauses);

	string SentenceStr  ="<s>\n";
	
	for(long i = 0 ; i < topClauses.size() ; i++ )
	{
		SYNANLib::IClausePtr piClause = piSent->Clause[topClauses[i]];
		process_clause(piSent, piClause, SentenceStr);
	}

	SentenceStr += "</s>\n";

	result += SentenceStr;
}

void CMorphCorpusCreator::add_close_clause_tag(string& result)
{
	if( !m_b_show_clause )
		return;

	result += "</cl>";
}

void CMorphCorpusCreator::add_open_clause_tag(string& result, string clause_type)
{
	if( !m_b_show_clause )
		return;

	result += "<cl type=\"";
	result += clause_type;
	result += "\"> ";
}




void CMorphCorpusCreator::process_clause(SYNANLib::ISentencePtr piSent, SYNANLib::IClausePtr piClause, string& result)
{
	if( piClause->VariantsCount == 0 )
	{
		fprintf (stderr, "a clause with no morph. variant is found in:\n%s\n", GetSentenceStr(piSent).c_str());
		return;
	};
	SYNANLib::IClauseVariantPtr piVar = piClause->GetClauseVariant(0);	

	{
		int ClauseRootNo = piVar->ClauseRootNo;
		string ClauseRootStr;
		if (ClauseRootNo != -1)
			ClauseRootStr = (const char*)piClause->ClauseRoots[ClauseRootNo]->Description;
		add_open_clause_tag(result, ClauseRootStr);
	};

	for(int i = 0 ; i < piVar->UnitsCount ;  i++)
	{
		SYNANLib::ISyntaxUnitPtr piUnit = piVar->Unit[i];		
		EUnitType unit_type = (EUnitType)piUnit->Type;
		long word_num;
		if( unit_type == EWord )
			word_num = piUnit->WordNum;
		else
		{
			long i_cl = piUnit->ClauseNum; 
			word_num = piSent->Clause[piUnit->ClauseNum]->FirstWord;
		}

		if( unit_type == EWord )
		{		
			i = process_word(piSent, i, piClause,  result, true);
		}
		else
		{
			SYNANLib::IClausePtr piSubClause = piSent->Clause[piUnit->ClauseNum];
			process_clause(piSent, piSubClause,  result);
		}		
	}

	add_close_clause_tag(result);
}


string& tabify(string& s)
{
	s = "\t"+s;
	for (size_t i=0; i+1 < s.length(); i++)
		if (s[i] == '\n')
		{
			s.insert(s.begin()+i+1,1,'\t');
		};
	return s;

};

long  CMorphCorpusCreator::process_oborot(SYNANLib::ISentencePtr piSent, SYNANLib::IClausePtr piClause, long iUnit, SYNANLib::IClauseVariantPtr piVar, string& result)
{

	SYNANLib::IWordPtr piWord = piSent->GetWord(piVar->Unit[iUnit]->WordNum);

	SYNANLib::IHomonymPtr piHom = piWord->GetHomonym(piVar->Unit[iUnit]->ActiveHomonymNum);

	// we exclude single words from the possible oborots
	if (piHom->IsOb1 && piHom->IsOb2) 
	{
		return -1;
	};
	
	bool bAddOborTag = !(piHom->IsOb1 && piHom->IsOb2 );

	long iUnit1 = iUnit;

	string oborot_from_text;
	string oborot_lemma;
	string oborot_as_single_words;

	while(true)
	{
		if( iUnit >=  piVar->UnitsCount )
			return -1;

		SYNANLib::ISyntaxUnitPtr piUnit = piVar->Unit[iUnit];

		if( piUnit->Type != EWord )
			return -1;

		SYNANLib::IWordPtr piWord = piSent->GetWord(piUnit->WordNum);	

		oborot_from_text += piWord->WordStr;
		oborot_lemma += piWord->WordStr;
		{
			string WordInterp;
			process_word(piSent,iUnit,piClause, WordInterp, false);
			oborot_as_single_words += WordInterp;
		};

		long iHom = piUnit->ActiveHomonymNum;
		SYNANLib::IHomonymPtr piHom = piWord->Homonym[iHom];
		if( piHom->IsOb2 )
			break;

		oborot_from_text += " ";
		oborot_lemma += "_";

		if( !piHom->IsInOb  )
			return -1;

		
		iUnit++;
	}

	result += "\t<ob> " + oborot_from_text;

	SYNANLib::ISyntaxUnitPtr piUnit = piVar->Unit[iUnit1];

	if( piUnit->Type != EWord )
		return -1;

	
	piWord = piSent->GetWord(piUnit->WordNum);

	piHom = piWord->GetHomonym(piUnit->ActiveHomonymNum);

	
	long oborNo = piHom->GetOborotId();
	if(  oborNo == -1 )
		return -1;

	result += "\n\t\t<ana lemma=\"";
	RmlMakeLower(oborot_lemma, m_CurrentLanguage);
	result += oborot_lemma;
	result += "\" pos=\"";
	result += (const char*)piHom->GetOborotGF();
	result += "\" gram=\"\" stress=\"255\"/>\n";
	
	result += "\t\t<singles>\n";
	result += tabify(tabify(oborot_as_single_words));
	//result += oborot_as_single_words;
	result += "\t\t</singles>\n";

	result += "\t</ob>\n";

	return iUnit;
}


/*
SA> Слушай, а в двух словах, зачем нужна процедура
SA> try_to_process_dash_word?

DP>Дело в том, что если дефисное слово хреново обработано на Maposte
DP>(такое бывает в некоторых случаях), то я пытаюсь отдельно обработать
DP>каждую часть, приписав каждой части свою лемму и граммемы.
DP>То есть для слова слово1-слово2-слово3 , строится

SA> Лучше я мапост буду исправлять....

bool CMorphCorpusCreator::try_to_process_hyphen_word(SYNANLib::IWordPtr piWord, string& sRes) 
{

	string wrd = piWord->WordStr;
	if( wrd.find("-") == string::npos )
		return false;

	
	
	for(int i = 0 ; i < piWord->HomonymsCount ; i++ )
	{
		string Lemma = piWord->Homonym[i]->Lemma;
		if( Lemma.find("-") != string::npos)
			return false;
	}


	StringTokenizer tokenizer(piWord->WordStr, "-");
	LEMMATIZERLib::IParadigmCollectionPtr	piParadigmCollection;
	LEMMATIZERLib::IParadigmPtr		piParadigm;
	
	bool bRes = false;
	while( tokenizer.has_next() )
	{
		const char* t = tokenizer(); 
		if( !t )
			return bRes;
		string str(t) ;
		piParadigmCollection = m_piLemmatizer->CreateParadigmCollectionFromForm(str.c_str(), piWord->Register == UpLow || piWord->Register == UpUp );

		sRes +=str;
		sRes +="{";			
		for(int i = 0 ; i < piParadigmCollection->Count ; i++ )
		{
			piParadigm = piParadigmCollection->Item[i];
			string lemma = piParadigm->GetNorm();
			string str_ancodes = piParadigm->SrcAncode;
			long iParadigm = piParadigm->ParadigmID;
			long POS = m_pGramTab->GetPartOfSpeech(str_ancodes.c_str());
			long POSes = 1 << POS; 
			string POSstr = m_pGramTab->GetPartOfSpeechStr(POS);
			

			if( str_ancodes.length() == 0)
			{
				process_gram_homonym(lemma, POSes, iParadigm, POSstr, str_ancodes,  sRes, piWord);
			}
			else
			{				

				for( int j = 0 ; j < str_ancodes.size() ; j+=2)
				{
					if( j + 1 >= str_ancodes.size() )
						break;
					string ancode = str_ancodes.substr(j, 2);
					if( i > 0 )
						sRes += "|";
					process_gram_homonym(lemma, POSes, iParadigm, POSstr,  ancode,  sRes, piWord);
					if( (j + 2) < str_ancodes.size() )
						sRes += "|";				
				}
			}			

		}
		sRes +="}";
		if( tokenizer.has_next() )
			sRes += "-";
		bRes = true;
	}
	sRes += " ";
	return true;
}

*/

long CMorphCorpusCreator::process_word(SYNANLib::ISentencePtr piSent, long iUnit, SYNANLib::IClausePtr piClause, string& result, bool bCheckOborot)
{
	SYNANLib::IClauseVariantPtr piVar = piClause->GetClauseVariant(0);	

	SYNANLib::ISyntaxUnitPtr piUnit = piVar->Unit[iUnit];

	if( piUnit->Type != EWord )
		return iUnit;
	
	SYNANLib::IWordPtr piWord = piSent->GetWord(piUnit->WordNum);	

	if( piWord->bArtificialCreated )
		return iUnit;

	long iHom = piUnit->ActiveHomonymNum;
	SYNANLib::IHomonymPtr piHom = piWord->Homonym[iHom];

	result += " ";	


	if (bCheckOborot)
		if( piHom->IsInOb  )
		{
			long iUnitRet;
			string str_res;
			iUnitRet = process_oborot(piSent, piClause, iUnit, piVar, str_res);
			if( iUnitRet != -1 )
			{
				result += str_res;
				return iUnitRet;
			}		
		}
	
	string sRes;
	/*if( try_to_process_hyphen_word(piWord, sRes) )
	{
		result += sRes;
		return iUnit;
	}*/

	
	if( piWord->Register == AnyRegister )
	{	
		result += string("\t<pun> ")+ (const char*)piWord->WordStr + string(" </pun>\n");	
		return iUnit;
	}

	result += Format("\t<w> %s\n", (const char*)piWord->WordStr);

	// process homonyms
	set<long> processed_homonyms;
	set<string> good_morph_interps;
	for (size_t i=0; i < piClause->GetVariantsCount(); i++)
	{
		SYNANLib::IClauseVariantPtr piVar = piClause->GetClauseVariant(i);	
		SYNANLib::ISyntaxUnitPtr piUnit = piVar->Unit[iUnit];
		long iHom = piUnit->ActiveHomonymNum;
		if (!processed_homonyms.insert(iHom).second) continue;

		process_homonym(piWord->Homonym[iHom], piUnit, piWord, good_morph_interps);	
	};

	for (set<string>::const_iterator it =  good_morph_interps.begin(); it !=  good_morph_interps.end(); it++)
	{
		result +=  "\t\t<ana " + *it + " />\n";
	};
	string WordStr = (const char*)piWord->WordStr;
	if (is_alpha(WordStr[0], m_CurrentLanguage))
	{
		set<string> all_morph_interps;
		get_all_morph_interps(piWord, all_morph_interps);
		vector<string> Result(all_morph_interps.size());
		vector<string>::iterator end = set_difference( all_morph_interps.begin(), all_morph_interps.end(),
			good_morph_interps.begin(), good_morph_interps.end(), Result.begin());	
		for (vector<string>::iterator it = Result.begin(); it != end; it++)
		{
			result +=  "\t\t<ana_del " + *it + " />\n";
		};
	};
	
	


	result += "\t</w>\n";

	return iUnit;
}


void CMorphCorpusCreator::get_all_morph_interps(SYNANLib::IWordPtr piWord , set<string>& all_morph_interps)
{
try {
		all_morph_interps.clear();
		string WordStr = piWord->WordStr;

		LEMMATIZERLib::IParadigmCollectionPtr piParadigmCollection = m_piLemmatizer->CreateParadigmCollectionFromForm(
			WordStr.c_str(), 
			is_upper_alpha((BYTE)WordStr[0], m_CurrentLanguage),
            true);
		
		for(int i = 0 ; i < piParadigmCollection->Count ; i++ )
		{
			LEMMATIZERLib::IParadigmPtr piParadigm = piParadigmCollection->Item[i];
			string lemma = piParadigm->GetNorm();
			string str_ancodes = piParadigm->SrcAncode;
			long iParadigm = piParadigm->ParadigmID;
			for( int i = 0 ; i < str_ancodes.size() ; i+=2)
			{
				string interp = process_gram_homonym(lemma, iParadigm, str_ancodes.substr(i, 2), piWord);
				all_morph_interps.insert(interp);
			};
		};

			
}
catch(...)
{
	ErrorMessage( Format("error while processing word \"%s\"\n", (const char*)piWord->WordStr));
	return ;
}


};

void CMorphCorpusCreator::AddAccent(string& lemma, int ParadigmID)
{
	LEMMATIZERLib::IParadigmPtr piParad = m_piLemmatizer->CreateParadigmFromID(ParadigmID);
	int iPos = piParad->GetAccent(0);
	if( (iPos < 0) || (iPos >= lemma.length()) )
		return;
	string lemma_save = lemma;
	lemma = lemma_save.substr(0, iPos);
	lemma += '`';
	lemma += lemma_save.substr(iPos);		
}

string CMorphCorpusCreator::GetLemma(string lemma, BYTE Pos, long ParadigmID, SYNANLib::IWordPtr piWord, QWORD lexema_grammems, string ancode)
{	
	RmlMakeLower(lemma, m_CurrentLanguage);

	bool bProperNoun =		(m_CurrentLanguage == morphRussian)
						&&	(Pos == AGRAMTABLib::NOUN)
						&& (		lexema_grammems 
								&	(		_QM(AGRAMTABLib::rSurName) 
										|	_QM(AGRAMTABLib::rName)
										|	_QM(AGRAMTABLib::rOrganisation)
										|	_QM(AGRAMTABLib::rPatronymic)
										|	_QM(AGRAMTABLib::rToponym)
										|	_QM(AGRAMTABLib::rInitialism)
									)
							);
	
	
	if( bProperNoun)
	{
		// making  the first word uppercase
		string c(lemma, 0, 1);
		string save_lemma = lemma;
		RmlMakeUpper(c, m_CurrentLanguage);
		lemma = c;
		lemma += save_lemma.substr(1, save_lemma.size() - 1);
	}

	if( ParadigmID != -1 )
	{
		AddAccent(lemma, ParadigmID);
	}

	if( ParadigmID == -1 )
		lemma += "?";

	string graph_descr = (const char*)piWord->GraphDescrs;
	
	if( graph_descr.find("#ПОЛУ") != string::npos )
	{
		string wrd = (const char*)piWord->WordStr;
		RmlMakeLower(wrd, m_CurrentLanguage);
		if( wrd.find("полу") == 0)
			lemma = string("полу") + lemma;
		else 
			if( wrd.find("пол-") == 0)
				lemma = string("пол-") + lemma;
			else
				lemma = string("пол") + lemma;		
		lemma += "?";
	}

	return lemma;
}

bool CMorphCorpusCreator::hasSecondCase(long paradigm_id, string strForm, BYTE eCase)
{
	if( paradigm_id == -1)
		return false;

	RmlMakeLower(strForm, m_CurrentLanguage);
	LEMMATIZERLib::IParadigmPtr piParadigm = m_piLemmatizer->CreateParadigmFromID(paradigm_id);		
	bool bFoundFirst = false;
	int iFirst = -1, iSecond = -1;
	bool bFoundDativ = false;
	for( int i = 0 ; i < piParadigm->GetCount() ; i++ )
	{
		string strFormFromParadigm = (const char*)piParadigm->GetForm(i) ;
		RmlMakeLower(strFormFromParadigm, m_CurrentLanguage);
		

		string strAncode = (const char*)piParadigm->GetAncode(i);
		for(int j = 0 ; j < strAncode.size() ; j+=2 )
		{
			string strOneAncode = strAncode.substr(j, 2);
			unsigned int grammems = m_pGramTab->GetGrammems(strOneAncode.c_str());
			if( grammems & _QM(AGRAMTABLib::rPlural) )
				continue;

			if( strForm == strFormFromParadigm)
			{
				if( grammems & _QM(AGRAMTABLib::rDativ) )
					bFoundDativ = true;
			}

			if( grammems & _QM(eCase) ) 
				if( bFoundFirst )
				{
					iSecond = i;
				}
				else
				{
					bFoundFirst = true;
					iFirst = i;
				}
		}
	}

	if( (iSecond != -1) && ( iFirst != -1 ) && bFoundDativ )
		return true;

	return false;
}	

int CMorphCorpusCreator::get_index_in_paradigm(SYNANLib::IWordPtr piWord, LEMMATIZERLib::IParadigmPtr piPard, const string& str_ancode)
{
	assert (str_ancode.length() == 2 );
	string word;
	word = piWord->WordStr;
	RmlMakeLower(word, m_CurrentLanguage);

	for( int i = 0 ; i < piPard->Count ; i++ )
	{
		string str = (const char*)piPard->GetForm(i);
		RmlMakeLower(str, m_CurrentLanguage);
		if( word != str )
			continue;

		string ancode = piPard->GetAncode(i);
		if(  ancode == str_ancode )
			return i;
	}
	return -1;
}

string CMorphCorpusCreator::process_gram_homonym(string lemma, long paradigm_id, const string& ancode, SYNANLib::IWordPtr piWord)
{
	BYTE pos = m_pGramTab->GetPartOfSpeech(ancode.c_str());
	string strPos = m_pGramTab->GetPartOfSpeechStr(pos);
	string str_word = (const char*)piWord->WordStr;
	QWORD grammems = 0;

	if( !ancode.empty() )
		grammems = m_pGramTab->GetGrammems(ancode.c_str());

	LEMMATIZERLib::IParadigmPtr piParadigm;
	if (paradigm_id != -1)
		piParadigm = m_piLemmatizer->CreateParadigmFromID(paradigm_id);		

	string type_grammems;
	{ // add common grammems
		string TypeAncode;
		if (paradigm_id != -1)
			TypeAncode = piParadigm->TypeAncode;
		if (!TypeAncode.empty())
		{
			type_grammems  = m_pGramTab->GrammemsToStr(m_pGramTab->GetGrammems(TypeAncode.c_str()));
		};
	};

	int FormNo = -1;
	if (paradigm_id != -1)
		FormNo = get_index_in_paradigm(piWord,  piParadigm, ancode);

	string str_gramems = m_pGramTab->GrammemsToStr(grammems);

	if (m_bArtificialCases)
	{
		if (pos == AGRAMTABLib::NOUN) 
		{
			if( grammems & _QM(AGRAMTABLib::rGenitiv) )
				if( hasSecondCase(paradigm_id, str_word, AGRAMTABLib::rGenitiv) )
				{
					int iPos = str_gramems.find("рд");
					if( iPos != -1 ) 
					{
						str_gramems.replace(iPos, 2, "рд2");
					}
				}
				

			if( grammems & _QM(AGRAMTABLib::rLocativ) )
				if( hasSecondCase(paradigm_id, str_word, AGRAMTABLib::rLocativ) )
				{	
					int iPos = str_gramems.find("пр");
					if( iPos != -1 ) 
						str_gramems.replace(iPos, 2, "пр2");
				}
		}
	}
	string result;
	result += "lemma=\"";
	result += GetLemma(lemma, pos, paradigm_id, piWord, grammems, ancode);
	result += "\" pos=\"";
	result += strPos;		
	result += "\" gram=\"";
	result += type_grammems+'|'+str_gramems;
	result += Format("\" stress=\"%i\"", FormNo==-1 ?  255 : piParadigm->GetAccent(FormNo));
	return  result;
}



void CMorphCorpusCreator::process_homonym(SYNANLib::IHomonymPtr piHom, SYNANLib::ISyntaxUnitPtr piUnit, SYNANLib::IWordPtr piWord, set<string>& result_inters)
{
try
{
	string GramCodes = (const char*)piHom->GetGramcodes();
	if( GramCodes.empty() || GramCodes[0] == '?' )
		return;

	string str_word = piWord->WordStr;

	if( piHom->IsInOb && !(piHom->IsOb1 && piHom->IsOb2 ) )
		return;

	string str_ancodes;

	if( piUnit != NULL )			
		str_ancodes = piUnit->Gramcodes;	

	if (str_ancodes.empty())
		str_ancodes = piHom->Gramcodes;

	string result;

	for( int i = 0 ; i < str_ancodes.size() ; i+=2)
	{
		string ancode = str_ancodes.substr(i, 2);

		BYTE pos = m_pGramTab->GetPartOfSpeech(ancode.c_str());
		string str_lemma =  piHom->Lemma;
		if( str_lemma.empty()  )
			str_lemma = str_word ;
		string interp = process_gram_homonym(str_lemma, piHom->ParadigmID, ancode, piWord);
		result_inters.insert(interp);
	}
	
}
catch(...)
{
	ErrorMessage( Format("error while processing word \"%s\"\n", (const char*)piWord->WordStr));
	return ;
}
}

