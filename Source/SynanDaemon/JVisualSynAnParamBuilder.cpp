#include "JVisualSynAnParamBuilder.h"

#include <SynanLib/SyntaxHolder.h>

#include <common/json.h>

struct SGroup
{
public:
	int m_W1;
	int m_W2;
	bool m_IsGroup;
	bool m_IsSubj;
	string m_strDescr;
	bool operator < (const SGroup& gr) const {
		if ((m_W1 <= gr.m_W1) && (m_W2 >= gr.m_W2))
			return true;

		if (m_W2 < gr.m_W1)
			return true;

		return false;
	}

	nlohmann::json ToJson() const {
		auto result = nlohmann::json::object();
		result["start"] = m_W1;
		result["last"] = m_W2;
		result["isGroup"] = m_IsGroup;
		result["isSubj"] = m_IsSubj;
		result["descr"] = m_strDescr;
		return result;
	}

};


struct SSynVariant2Groups
{
public:
	vector< CSynUnit> m_SynUnits;
	vector<SGroup> m_Groups;

};

class JVisualSynAnParamBuilder
{
	nlohmann::json WriteWords(const CSentence& piSent, long StartWord, long LastWord);
	void GetTopClauses(const CSentence& piSent, vector<long>& topClauses);
	void AddVariants(vector<SSynVariant2Groups>& synVariants, long iClause, const CSentence& piSent);
	void AddOneVariant(vector<SSynVariant2Groups>& synVariants, const CMorphVariant& piVar, const CSentence& piSent, const CClause& Clause);
	void AddGroups(vector<SSynVariant2Groups>& synVariants, const CMorphVariant& piVar, const CClause& Clause);
	void MultVariants(vector<SSynVariant2Groups>& SynVariants1, vector<SSynVariant2Groups>& synVariants2);
	void MultTwoVariants(SSynVariant2Groups& var1, SSynVariant2Groups& var2, SSynVariant2Groups& res);
	void AddHomonym(vector<SSynVariant2Groups>& synVariants, const CSynUnit& Unit);
	void AddGroup(vector<SSynVariant2Groups>& synVariants, const CGroup& piGroup);
	nlohmann::json JVisualSynAnParamBuilder::WriteVariant(const SSynVariant2Groups& var);

public:
	const CSyntaxHolder* m_pSyntaxHolder;
	JVisualSynAnParamBuilder(const CSyntaxHolder* pSyntaxHolder);
	virtual ~JVisualSynAnParamBuilder();
	nlohmann::json BuildJson(const CSentence& piSent);
};

JVisualSynAnParamBuilder::JVisualSynAnParamBuilder(const CSyntaxHolder* pSyntaxHolder)
{
	m_pSyntaxHolder = pSyntaxHolder;
}

JVisualSynAnParamBuilder::~JVisualSynAnParamBuilder()
{
}


nlohmann::json JVisualSynAnParamBuilder::WriteWords(const CSentence& piSent, long StartWord, long LastWord)
{
	auto result = nlohmann::json::array();
	for(int i = StartWord ; i <= LastWord ; i++ ) {
		const CSynWord& W = piSent.m_Words[i];
		auto word = nlohmann::json::object();
		word["str"] = W.m_strWord;
		word["homonyms"] = nlohmann::json::array();
		for(auto& h : W.m_Homonyms)	{			
			word["homonyms"].push_back(h.m_strLemma);
		}
		result.push_back(word);
	}
	return result;
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
			group.m_IsGroup = false;
			group.m_IsSubj = false;
			group.m_strDescr = m_pSyntaxHolder->GetClauseTypeDescr(Clause,  i->m_ClauseTypeNo);
			EngRusMakeLower(group.m_strDescr);
			synVariants[j].m_Groups.push_back(group);
		}

		allSynVariants.insert(allSynVariants.end(), synVariants.begin(), synVariants.end());
	}
}

void JVisualSynAnParamBuilder::AddHomonym(vector<SSynVariant2Groups>& synVariants, const CSynUnit& Unit)
{
	for (auto& v: synVariants) {
		v.m_SynUnits.push_back(Unit);
	}
}

void JVisualSynAnParamBuilder::AddGroup(vector<SSynVariant2Groups>& synVariants, const CGroup&  piGroup)
{
	for (auto& v : synVariants)	{
		SGroup group;
		group.m_W1 = piGroup.m_iFirstWord;
		group.m_W2 = piGroup.m_iLastWord;
		group.m_IsGroup = true;
		group.m_IsSubj = false;
		group.m_strDescr = m_pSyntaxHolder->m_Synan.GetOpt()->GetGroupNameByIndex(piGroup.m_GroupType);
		EngRusMakeLower(group.m_strDescr);
		v.m_Groups.push_back(group);
	}
}


void JVisualSynAnParamBuilder::AddGroups(vector<SSynVariant2Groups>& synVariants, const CMorphVariant& piVar, const CClause& Clause) {
	for (long i = 0; i < piVar.m_vectorGroups.GetGroups().size(); i++) {
		AddGroup(synVariants, piVar.m_vectorGroups.GetGroups()[i]);
	}

	long iSubj = piVar.GetFirstSubject();
	long iPred = piVar.m_iPredk;
	if ((iSubj >= 0) && (iPred >= 0))
	{
		iSubj = piVar.GetSentenceCoordinates(iSubj).m_iFirstWord;
		iPred = piVar.GetSentenceCoordinates(iPred).m_iFirstWord;
		for (auto& v : synVariants) {
			SGroup group;
			group.m_W1 = iSubj;
			group.m_W2 = iPred;
			group.m_strDescr = "sp";
			group.m_IsSubj = true;
			group.m_IsGroup = false;
			v.m_Groups.push_back(group);
		}
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

nlohmann::json JVisualSynAnParamBuilder::WriteVariant(const SSynVariant2Groups& var) {
	auto result = nlohmann::json::object();
	result["units"] = nlohmann::json::array();
	for(auto& u : var.m_SynUnits) {
		auto unit = nlohmann::json::object();
		unit["homNo"] = u.m_iHomonymNum;
		string  grammems = u.GetGrammemsByAncodes();
		if (grammems.empty())
			grammems = m_pSyntaxHolder->m_pGramTab->GrammemsToStr(u.m_iGrammems);
		unit["grm"] += u.GetPartOfSpeechStr() + " " + grammems;
		result["units"].push_back(unit);
	}

	result["groups"] = nlohmann::json::array();
	for (auto& group : var.m_Groups) {
		result["groups"].push_back(group.ToJson());
	}
	return result;
}

nlohmann::json JVisualSynAnParamBuilder::BuildJson(const CSentence& piSent) {
	try {
		vector<long> topClauses;
		GetTopClauses(piSent, topClauses);
		auto result = nlohmann::json::array();
		for(long i = 0; i < topClauses.size() ; i++ ) {
			const CClause& C = 	piSent.m_Clauses[topClauses[i]];
			auto clause = nlohmann::json::object();
			clause["words"] = WriteWords(piSent, C.m_iFirstWord, C.m_iLastWord);

			vector<SSynVariant2Groups> synVariants;
			AddVariants(synVariants, topClauses[i],  piSent);
			clause["variants"] = nlohmann::json::array();
			for(auto& v : synVariants) {
				vector<SGroup>& G =  v.m_Groups;
				for(auto& g : v.m_Groups)	{
					g.m_W1 -= C.m_iFirstWord;
					g.m_W2 -= C.m_iFirstWord;
				};
				sort(v.m_Groups.begin(), v.m_Groups.end());
				clause["variants"].push_back(WriteVariant(v));
			}
			result.push_back(clause);
		}
		return result;
	}
	catch (CExpc c) {
		throw;
	}
	catch (...)	{
		throw CExpc("an exception in JVisualSynAnParamBuilder occurred!");
	};
	
}

string BuildJson(CSyntaxHolder* pSyntaxHolder, const string& query) {
	if (!pSyntaxHolder->GetSentencesFromSynAn(query, false)) {
		throw CExpc("Synan has crushed\n");
	};

	JVisualSynAnParamBuilder builder(pSyntaxHolder);
	auto result = nlohmann::json::array();
	for (auto& s : pSyntaxHolder->m_Synan.m_vectorSents) {
		result.push_back(builder.BuildJson(*s));
	}
	ConvertToUtfRecursive(result, pSyntaxHolder->m_CurrentLanguage);
	return result.dump();
	
}

