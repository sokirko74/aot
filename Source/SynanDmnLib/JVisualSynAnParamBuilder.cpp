// JVisualSynAnParamBuilder.cpp: implementation of the JVisualSynAnParamBuilder class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "JVisualSynAnParamBuilder.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



JVisualSynAnParamBuilder::JVisualSynAnParamBuilder(const CSyntaxHolder* pSyntaxHolder)
{
	m_pSyntaxHolder = pSyntaxHolder;
}

JVisualSynAnParamBuilder::~JVisualSynAnParamBuilder()
{
}


string JVisualSynAnParamBuilder::CodeSymbol(char ch)
{
	switch(ch)
	{
		case '#':
			return ";lattice";
		case '^':
			return ";cover";
		case '@':
			return ";at";
		case '$':
			return ";buks";
		case ';':
			return ";semicolon";
		case '"': // for German
			return ";quote_mark";
	}
	return string() + ch;
}

string JVisualSynAnParamBuilder::CodeWord(string strWord)
{
	string res;
	for(size_t i = 0 ; i < strWord.size() ; i++ )

		res += CodeSymbol(strWord[i]);
	return res;
}


bool JVisualSynAnParamBuilder::WriteWords(const CSentence& piSent, string& strRes, long StartWord, long LastWord)
{
	for(int i = StartWord ; i <= LastWord ; i++ )
	{
		strRes +=  "$";

		const CSynWord& W = piSent.m_Words[i];;

		
		strRes += CodeWord(W.m_strWord.c_str());
		long nHoms = W.m_Homonyms.size();
		strRes += "@homonyms";
		for(int j = 0 ; j < nHoms ; j++ )
		{			
			strRes += "@";
			const CSynHomonym& H = W.m_Homonyms[j];
			strRes += CodeWord(H.m_strLemma);
		}
	}
	return true;
}

void JVisualSynAnParamBuilder::GetTopClauses(const CSentence& piSent, vector<long>& topClauses)
{	
	long clCount = piSent.m_Clauses.size();
	const CClause*  piNextClause = 0;

	for(long i = piSent.m_Clauses.size() - 1 ; i >= 0 ; i-- )
	{
		const CClause* piClause = &piSent.m_Clauses[i];

		if( (piNextClause == NULL) || (piNextClause->m_iFirstWord == piClause->m_iLastWord + 1 ) )
		{
			piNextClause = piClause;
			topClauses.push_back(i);
		}			
	}
	reverse(topClauses.begin(), topClauses.end());		
}

// this procedure adds all variants of the clause iClause to allSynVariants
// and before it the procedure adds all variants of subclauses of clause iClause
void JVisualSynAnParamBuilder::AddVariants(vector<SSynVariant2Groups>& allSynVariants,long iClause,  const CSentence& piSent)
{
	const CClause& Clause = piSent.m_Clauses[iClause];
	long variantsCount = Clause.GetSynVariantsCount();

	for(CSVI i =  Clause.m_SynVariants.begin() ; i != Clause.m_SynVariants.end(); i++ )
	{
		vector<SSynVariant2Groups> synVariants;
		AddOneVariant(synVariants, *i, piSent, Clause);

		for( int j = 0 ; j < synVariants.size() ; j++ )
		{
			SGroup group;
			group.m_W1 = Clause.m_iFirstWord;
			group.m_W2 = Clause.m_iLastWord;
			group.m_bIsGroup = false;
			group.m_strDescr = m_pSyntaxHolder->GetClauseTypeDescr(Clause,  i->m_ClauseTypeNo);
			EngRusMakeLower(group.m_strDescr);
			synVariants[j].m_Groups.push_back(group);
		}

		allSynVariants.insert(allSynVariants.end(), synVariants.begin(), synVariants.end());
	}
}

void JVisualSynAnParamBuilder::AddOneVariant(vector<SSynVariant2Groups>& allSynVariants,const CMorphVariant& piVar,  const CSentence& piSent, const CClause& Clause)
{
	long unitsCount = piVar.m_SynUnits.size();
	SSynVariant2Groups var;
	allSynVariants.push_back(var);

	for(long i = 0 ; i < unitsCount ; i++ )
	{
		if( piVar.m_SynUnits[i].m_Type == EWord)
		{
			AddHomonym(allSynVariants, piVar.m_SynUnits[i]);
		}
		else
		{
			vector<SSynVariant2Groups> synVariants;
			int ClauseNo = piSent.FindClauseIndexByPeriod(piVar.m_SynUnits[i].m_SentPeriod);
			AddVariants(synVariants, ClauseNo, piSent);
			MultVariants(allSynVariants, synVariants);
		}
	}

	AddGroups(allSynVariants, piVar, Clause);
}

void JVisualSynAnParamBuilder::MultTwoVariants(SSynVariant2Groups& var1, SSynVariant2Groups& var2, SSynVariant2Groups& res)
{
	res = var1;
	res.m_SynUnits.insert(res.m_SynUnits.end(), var2.m_SynUnits.begin(), var2.m_SynUnits.end());
	res.m_Groups.insert(res.m_Groups.end(), var2.m_Groups.begin(), var2.m_Groups.end());
}


void JVisualSynAnParamBuilder::MultVariants(vector<SSynVariant2Groups>& synVariants1, vector<SSynVariant2Groups>& synVariants2)
{
	vector<SSynVariant2Groups> resVars;
	for(int i = 0 ; i < synVariants1.size() ; i++)
	{
		for(int j = 0 ; j < synVariants2.size() ; j++)
		{
			SSynVariant2Groups res;
			MultTwoVariants(synVariants1[i], synVariants2[j], res);
			resVars.push_back(res);
		}
	}

	if( synVariants1.size() == 0)
		synVariants1 = synVariants2;
	else
		synVariants1 = resVars;
}




void JVisualSynAnParamBuilder::WriteVariant(SSynVariant2Groups& var, string& strRes )
{
	strRes += "$";
	int i = 0;
	for(; i < var.m_SynUnits.size(); i++ )
	{
		const CSynUnit& U  = var.m_SynUnits[i];
		int HomNo = U.m_iHomonymNum;
		string str = Format("%i",HomNo);

		str += "=";
        str += var.m_SynUnits[i].GetPartOfSpeechStr() + string(" ");
		string  Grammems = U.GetGrammemsByAncodes();
		if (Grammems.empty())
			Grammems = m_pSyntaxHolder->m_pGramTab->GrammemsToStr(U.m_iGrammems);
        str += Grammems;

		if( i+1 < var.m_SynUnits.size())
		{
			strRes += str +"^";
		}
		else
			strRes += str;
	}

	for( i = 0 ; i < var.m_Groups.size(); i++ )
	{
		strRes += "@";
		SGroup group = var.m_Groups[i];
		char str1[4];
		char str2[4];
		sprintf(str1, "%i", group.m_W1);
		sprintf(str2, "%i", group.m_W2);
		strRes += string(str1) +  string("^") + string(str2) + string("^") + group.m_strDescr + "^";
		if( group.m_strDescr == "sp")
		{
			strRes += "sp";
		}
		else
		{
			if( group.m_bIsGroup )
				strRes += "gr";
			else
				strRes += "cl";
		}
	}
}
void JVisualSynAnParamBuilder::AddGroups(vector<SSynVariant2Groups>& synVariants, const CMorphVariant& piVar, const CClause& Clause)
{
	long i = 0;
	for(; i < piVar.m_vectorGroups.GetGroups().size() ; i++ )
	{
		const CGroup& G = 	 piVar.m_vectorGroups.GetGroups()[i];
		AddGroup(synVariants, G);
	}

	long iSubj = piVar.GetFirstSubject();
	long iPred = piVar.m_iPredk;
	if( (iSubj >= 0) && (iPred >= 0) )
	{
		iSubj = piVar.GetSentenceCoordinates(iSubj).m_iFirstWord;
		iPred = piVar.GetSentenceCoordinates(iPred).m_iFirstWord;
		for(i = 0 ; i < synVariants.size() ; i++ )
		{
			SGroup group;
			group.m_W1 = iSubj;
			group.m_W2 = iPred;			
			group.m_strDescr = "sp";			
			synVariants[i].m_Groups.push_back(group);
		}	
	}

}

void JVisualSynAnParamBuilder::AddGroup(vector<SSynVariant2Groups>& synVariants, const CGroup&  piGroup)
{
	for(int i = 0 ; i < synVariants.size() ; i++ )
	{
		SGroup group;
		group.m_W1 = piGroup.m_iFirstWord;
		group.m_W2 = piGroup.m_iLastWord;
		group.m_bIsGroup = true;
		group.m_strDescr = m_pSyntaxHolder->m_Synan.GetOpt()->GetGroupNameByIndex(piGroup.m_GroupType);
		EngRusMakeLower(group.m_strDescr);
		synVariants[i].m_Groups.push_back(group);
	}
}

void JVisualSynAnParamBuilder::AddHomonym(vector<SSynVariant2Groups>& synVariants, const CSynUnit& Unit)
{
	for(int i = 0 ; i < synVariants.size() ; i++ )
	{
		//assert(synVariants[i].m_Homonyms.size() == iW );
		synVariants[i].m_SynUnits.push_back(Unit);
	}
}


string JVisualSynAnParamBuilder::BuildStrParam(const CSentence& piSent)
{
	try {
		string strRes;
			
	
		vector<long> topClauses;
		GetTopClauses(piSent, topClauses);
		for(long i = 0; i < topClauses.size() ; i++ )
		{
			const CClause& C = 	piSent.m_Clauses[topClauses[i]];
			// writing words
			strRes += "#words";
			WriteWords(piSent, strRes, C.m_iFirstWord, C.m_iLastWord);

			// writing groups
			vector<SSynVariant2Groups> synVariants;
			strRes += "#groups";
			AddVariants(synVariants, topClauses[i],  piSent);
			for(long k = 0 ; k < synVariants.size() ; k++ )
			{
				vector<SGroup>& G =  synVariants[k].m_Groups;;
				for(long j = 0 ; j < G.size(); j++)
				{
					G[j].m_W1 -= C.m_iFirstWord;
					G[j].m_W2 -= C.m_iFirstWord;
				};
				sort(synVariants[k].m_Groups.begin(), synVariants[k].m_Groups.end());
				WriteVariant(synVariants[k], strRes);
			}
			//if (i+1 != topClauses.size())
			strRes += "#topclause";

		}
		return strRes;
	}
	catch (...)
	{
			return "an exception in JVisualSynAnParamBuilder occurred!";
	};
	
}

