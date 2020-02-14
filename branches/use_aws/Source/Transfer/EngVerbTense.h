#pragma warning(disable:4786)
#pragma warning(disable:4503)
#ifndef INCL_VERB_TENCE_H
#define INCL_VERB_TENCE_H
#include "EngSynthes.h"



class CEngVerbTense : public CGrammemsAnalyzer{
	//  ������, ���� ����������� �������� ��������������
	bool		 m_bQuestion;

	//  ����� ����, ������� �������� ������ 
	int			 m_VerbNodeNo;

	//  ����, ������� �������� ������
	CEngSemNode* m_pVerbNode;

	//  ������� ����� ����, ������� �������� ������
    CEngSemWord* m_pVerbMainWord;

	// ����� ����, ������� �������� ����������
	int			 m_SubjNodeNo;

	// ����� ����, ������� �������� ������ ���������� 
	int			  m_DirObjNodeNo;


	string rel_operators;


	string	post_verb_advs,
		dir_obj_postfix;

	SynthesResult result;

	EngVerbTenseEnum m_Tense;




public:
	CEngSynthes &S;
	CEngSemStructure &E;
	translate_helper &helper;


	CEngVerbTense (CEngSynthes &EngSynthes_Arg, int VerbNodeNo)
		:S(EngSynthes_Arg),
		E(EngSynthes_Arg.E),
		helper(EngSynthes_Arg.helper)
	{
	  m_VerbNodeNo = VerbNodeNo;
	  m_bQuestion = false;
	  m_pVerbNode = 0;
	  m_pVerbMainWord = 0;
	  m_Tense = zero_tn;
	}

	
	void make_verb_form();

private:
	void make_string();
	void CreateAuxVerbsThatStandBeforeMain(EngVerbTenseEnum Tense, string& AuxVerbs, UINT& MainVerbGrammems) const;

	bool		is_be_verb() const;
	bool		has_not();
	long		GetPersonGrammemsIfCan(CEngSemNode& EngNode);
};





#endif // INCL_VERB_TENCE_H
