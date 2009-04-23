#include "StdAfx.h"
#include "EngSynthes.h"



void	NodeHelper::get_out_rels(int node, vector<long> &res) const
{
	if(node == -1) return;
	vector<long> rels;
	E.GetOutcomingRelations(node,rels);
	for(int i = 0; i < rels.size(); i++){
		if(IsBetweenClause(rels[i])) continue;
		res.push_back(rels[i]);
	}
}

int	NodeHelper::get_in_clause_rel(int node)
{
	if(node == -1) return -1;
	int res = -1;
	vector<long> rels;
	E.GetIncomingRelations(node, rels, false);
	for(int i = 0; i < rels.size(); i++){
		if(!IsBetweenClause(rels[i])) continue;
		//assert(res == -1);
		res = rels[i];
	}
	return res;

}


int	NodeHelper::get_in_rel(int node)
{
	if(node == -1) return -1;
	int res = -1;
	vector<long> rels;
	E.GetIncomingRelations(node, rels, false);
	for(int i = 0; i < rels.size(); i++){
		if(IsBetweenClause(rels[i])) continue;
		assert(res == -1);
		res = rels[i];
	}
	return res;
}

void	NodeHelper::get_out_clause_rel(int node, vector<long> &res)
{
	if(node == -1) return;
	vector<long> rels;
	E.GetOutcomingRelations(node,rels);
	for(int i = 0; i < rels.size(); i++){
		if(!IsBetweenClause(rels[i])) continue;
		res.push_back(rels[i]);
	}
}




bool NodeHelper::node_has_poses(int node_no, UINT eng_poses) const
{
	if(E.m_Nodes[node_no].IsAbstract()) return false;
	const CEngSemNode &node = E.m_Nodes[node_no];
	assert(!node.m_Words.empty());

	if(node.m_MainWordNo == -1) return false;
	const CEngSemWord &word = node.m_Words[node.m_MainWordNo];
	return (word.m_Poses & eng_poses) != 0;
}

bool NodeHelper::node_is_verb(int node_no) const
{
	return node_has_poses(node_no, 
		_QM(eVERB) | _QM(eVBE) | _QM(eMOD));
}

bool NodeHelper::node_is_noun(int node_no) const
{
	if (E.m_Nodes[node_no].IsThesNode()) return true;

	return node_has_poses(node_no, 
		_QM(eNOUN));
}

bool NodeHelper::node_is_numeral(int node_no) const
{
	return node_has_poses(node_no, 1 << eNUMERAL);
}

bool NodeHelper::node_is_pronoun(int node_no) const
{
	bool pos_ok =  node_has_poses(node_no, 
		1 << ePN);
	if(
		  FieldContainsValue(E.m_Nodes[node_no], "GF", "PN_ADJ:Indef")
		||FieldContainsValue(E.m_Nodes[node_no], "GF", "PN_ADJ:Quest")
		||FieldContainsValue(E.m_Nodes[node_no], "GF", "PN_ADJ")
		||FieldContainsValue(E.m_Nodes[node_no], "GF", "PN:Neg")
		||FieldContainsValue(E.m_Nodes[node_no], "GF", "PN")
		||FieldContainsValue(E.m_Nodes[node_no], "GF", "PN:Quest")
		)return true;

	return pos_ok;
}

bool NodeHelper::node_is_adj(int node_no) const
{
	return node_has_poses(node_no, 
		1 << eADJ);

}

bool NodeHelper::FieldContainsValue(const CEngSemNode& node, const string &field, const string &value, int leaf, int leaf2) const
{
	DictTypeEnum dict_kind = (node).GetType();
	if (dict_kind == NoneRoss) return false;
	WORD unit_no = (node).GetUnitNo();
	if(node.IsThesNode())
	{
		int ThesId = node.m_Colloc.GetThesInterp().m_ThesaurusId;
		const CThesaurus* Thes = E.m_pData->GetThes(ThesId);
		const CRossHolder* Dict = E.m_pData->GetRossHolder(GetRossIdByThesId(ThesId));
		if (Dict == 0) return false;

		long eng_termin;
		vector<int> EnglishTranslations;
		Thes->QueryEnglishTranslations(node.m_Colloc.GetThesInterp().m_TerminId, EnglishTranslations);
		if(EnglishTranslations.empty()) return false;
		eng_termin = EnglishTranslations[0];
		const CInnerTermin& piTermPtr = 	Thes->m_Termins[eng_termin];
		string S;
		long eng_termin_id = piTermPtr.m_TerminId;
		S = Format("%i",eng_termin_id);
		unit_no = Dict->GetRoss()->LocateUnit(S.c_str(), 1);
		return FieldContainsValue(Dict, unit_no, field, value, leaf, leaf2);
	}
	else 
		if(node.m_Colloc.m_Type == RossType)
		{
		dict_kind = node.m_Colloc.GetRossInterp().m_DictType;
		unit_no = node.m_Colloc.GetRossInterp().m_UnitNo;
	}
	return FieldContainsValue(E.GetRossHolder(dict_kind), unit_no, field, value, leaf, leaf2);
}


bool NodeHelper::FieldContainsValue(const CRossHolder* RossHolder, WORD unit_no, 
	const string &field, const string &value, int leaf, int leaf2) const
{
	if(unit_no == ErrUnitNo) return false;
	if (RossHolder == 0) return false;
	vector<TCortege> vec;
	StringTokenizer tok(value.c_str(), " \t");
	StringVector value_vec;
	while(tok()) value_vec.push_back(tok.val());

	try{
		RossHolder->GetFieldValues(field.c_str(), unit_no, vec, leaf);
	}catch(...){
		return false;
	}
	for(int i = 0; i < vec.size(); i++){
//		if(vec[i].GetBracketLeafId() != leaf2) continue;
		StringVector field_vec;
		for(int j = 0; vec[i].m_DomItemNos[j] != -1; j++){
			field_vec.push_back((const char*)RossHolder->GetRoss()->GetDomItemStr(vec[i].m_DomItemNos[j]));
		}
		if(search(field_vec.begin(), field_vec.end(), 
			value_vec.begin(), value_vec.end())
			!= field_vec.end())
			return true;

	}
	return false;


}



void NodeHelper::GetFieldValues(DictTypeEnum dict_kind, WORD unit_no, const string &field, 
				 StringVector &res, int max_items) const
{
	vector<TCortege> vec;
	E.GetRossHolder(dict_kind)->GetFieldValues(field.c_str(), unit_no, vec);
	for(int i = 0; i < vec.size(); i++){
		string value;
		for(int j = 0; vec[i].m_DomItemNos[j] != -1; j++){
			if(j || j > max_items) break;
			value += (const char*)E.GetRoss(dict_kind)->GetDomItemStr(vec[i].m_DomItemNos[j]);
		}
		res.push_back(value);
	}


}




bool NodeHelper::rel_is_valency(int rel)  const
{
	return E.m_Relations[rel].m_bInterpreted;
}
