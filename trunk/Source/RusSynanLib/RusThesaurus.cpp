// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusThesaurus.h"
#include "RusPlmLine.h"


CRusThesaurusForSyntax::CRusThesaurusForSyntax(const CSyntaxOpt* Opt): CThesaurusForSyntax(Opt)
{
};

static bool  TryToAssignMainGroupToNOUN_GR(CGroups& model, int iGr, const CInnerModel& piModel)
{
	try
	{

		CGroup& group = *model.GetGroupPtr(iGr);

		if( group.m_GroupType!= NOUN_GR)
			return false;

		//  ���� �����, � ������� �� ������ �� ������ ��������� �� ���� ������ ������ ������
		int i = 0;
		vector<bool> HasParent(group.size(), false);
		for( ; i < piModel.m_SynRelations.size(); i++ )
		{
			const CThesSynRelation& piRel =  piModel.m_SynRelations[i];;
			if	(		CPeriod(piRel.m_Word1-1).is_part_of(group) 
					&&	CPeriod(piRel.m_Word2-1).is_part_of(group)
				)
				HasParent[piRel.m_Word2 - 1 - group.m_iFirstWord] = true;
		}
		for (i=0; i < group.size(); i++)
			if (!HasParent[i])
				break;

		//  ������� ����� �� �������, �������, ����� �������� ���� ���������
		if( i ==  group.size() )
			return false;

		//  ������� ����� �������, ����� ����� ������������ ���������, ������� �������� ��� �����
		group.m_MainGroup = model.get_maximal_subgroup(iGr, i + group.m_iFirstWord);
		return true;
	}
	catch(...)
	{
		return false;
	}

}

static void  AssignGroupMainGroup(CGroups& model, const size_t GroupNo, const CInnerModel& piModel)
{
	assert (GroupNo < model.GetGroups().size());

	CPeriod m_MainGroup(-1,-1);
	QWORD iGrammems;

	//����� ����� ������� ����� � �� ����� �������� �� ���������
	if	(			(model.GetGroups()[GroupNo].m_GroupType == NOUN_GR)
			&&		TryToAssignMainGroupToNOUN_GR(model, GroupNo, piModel) 
		)
		return;

	//����������, ��� � ��������� ������ ������� ������ ��� �����_������
	//����� ���� ��� ������� ������, �� ������� ������ - max ��������� ����� ��� ������ ���������������
	// ����� - max ��������� ������ ��� ������ ������
	//(����� �����_������ ���� ������� + ���������)
	// ���� ������� ����������� ������, ���� �� �������, ����� ���� ������ �����.
	bool bGroupFound = false;
	int j = GroupNo - 1 ;
	for(; j >= 0 ; j--)		
	{
		if( (model.GetGroups()[GroupNo].m_GroupType!= ADV_VERB) && 
			 model.GetGroups()[j].m_iFirstWord == model.GetGroups()[GroupNo].m_iFirstWord)
		{
			bGroupFound = true;
			break;
		}

		if(model.GetGroups()[GroupNo].m_GroupType== ADV_VERB)
		{
			bGroupFound = true;
			break;
		}

	}

	if( bGroupFound )
	{
		m_MainGroup.SetPeriod(model.GetGroups()[j]);
		iGrammems = model.GetGroups()[j].GetGrammems();
	}
	else
	{
		//  ���� ������� ����� � ������
		for(int i = model.GetGroups()[GroupNo].m_iFirstWord ; i <= model.GetGroups()[GroupNo].m_iLastWord ; i++ )
		{
			assert( (i >= 0) && (i < model.sent.size()));
			if(    (model.sent[i].is_morph_noun() && (model.GetGroups()[GroupNo].m_GroupType!= ADV_VERB) )
				|| (model.GetOpt()->GetGramTab()->is_verb_form(model.sent[i].GetPoses()) && (model.GetGroups()[GroupNo].m_GroupType == ADV_VERB)) )
			{
				m_MainGroup.SetPeriod(CPeriod(i,i));
				assert (i < model.sent.size());
				iGrammems = model.sent[i].GetGrammems();
				break;
			}
		} 
	}

    if  (m_MainGroup.m_iFirstWord == -1) {
        throw CExpc("RusThesaurus.AssignGroupMainGroup: cannot find main group for Mode id %i", piModel.m_ModelId);
    }

	assert( m_MainGroup.m_iFirstWord != -1 );

	//  ���� ���-���� �� ���� ������� ������� ������, ����� ��������� ������� ������ ������ ������ �����
	if( m_MainGroup.m_iFirstWord == -1)
		m_MainGroup.SetPeriod(CPeriod(model.GetGroups()[GroupNo].m_iFirstWord, model.GetGroups()[GroupNo].m_iFirstWord));

	
	

	CPeriod P = m_MainGroup;
	CGroup& G = *model.GetGroupPtr(GroupNo);
	G.m_MainGroup.SetPeriod(P);
	G.SetGrammems(iGrammems);
}


void CRusThesaurusForSyntax::AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel)
{
	for(size_t i = 0 ; i < model.GetGroups().size(); i++ )
	{
		AssignGroupMainGroup(model,i, piModel); 
	}
	
}
