// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


// �������:
// �� �������

bool CRusFormatCaller::format_for_neg_verb (CGroup& G)
{
  if (!W1.is_lemma("��")) return false;
  if (G.m_iFirstWord+1 >= sent.size()) return false;  
  // "��" ����� ������� � ������ ("���� ��")
  if (W1.HasFlag(fl_in_oborot)) 
  {
	  if (!W1.HasFlag(fl_oborot1) || !W1.HasFlag(fl_oborot2))
	  return false;  
  }
  if( get_maximal_group_size(G.m_iFirstWord + 1) > 1)
	  return false;
  if (!is_verb_form(W2))  return false;
  G.m_GroupType = NEG_VERB;
  G.m_MainGroup = G.m_iFirstWord + 1;
  G.m_iLastWord = G.m_iFirstWord + 1;
  G.m_Cause = "������� '��', ������ ������� ����� ���������� �����"; 
  create_syn_rel(G, get_main_word(G.m_iLastWord),G.m_iFirstWord,NEG_VERB);
  return true;
}

// �������:
// ������ ���� 
// �� ������ �������� ���� 

bool CRusFormatCaller::format_for_dir_obj (CGroup& G)
{ 
    const CGroup& VerbGroup =  get_maximal_group (G.m_iFirstWord);
    int i = VerbGroup.m_MainWordNo;
	if (!Wi.has_grammem(rTransitive)) return false;
	if (Wi.has_grammem(rPassiveVoice)) return false;


	if ( VerbGroup.m_GroupType == DIR_OBJ ) return false;
	if ( VerbGroup.m_GroupType == VERB_INF ) return false;

	int k = get_next_main_word(i);

	if (k == sent.size()) return false;

	const CGroup& ObjGroup = get_maximal_group(k);

	if (		!Wk.is_syn_noun() 
            && ! Wk.HasPOS(PRONOUN)  
			&&	( ObjGroup.m_GroupType != SELECTIVE_GR )
		) 
	return false;

	
	
	if (!(ObjGroup.GetGrammems() & _QM(rAccusativ)) )
			return false;

	G.m_iLastWord = ObjGroup.m_iLastWord;
	G.m_MainGroup = VerbGroup;
	G.m_GroupType = DIR_OBJ;
	G.SetGrammems( Wi.GetGrammems() );
 	QWORD g = ObjGroup.GetGrammems() & (_QM(rAccusativ) | ~rAllCases); //������� ������ ������, "������ �����"
	((CGroup&)ObjGroup).SetGrammems(g);
	if (Wk.GetGrammems() & _QM(rAccusativ) )
		Wk.SetGrammems(Wk.GetGrammems() & (_QM(rAccusativ) | _QM(rDativ) | ~rAllCases)); //rDativ ��� "������������� �������� ������ ������"
	return true;
}

// "��� ��� �������� ������"
// �� �� �� "������ �����"
bool CRusFormatCaller::format_for_instr_obj (CGroup& G)
{ 
    const CGroup& VerbGroup =  get_maximal_group (G.m_iFirstWord);
    int i = VerbGroup.m_MainWordNo;
	if (Wi.HasFlag(fl_ambiguous)) return false;
	if (!Wi.HasFlag(fl_can_subdue_instr))
	{
		if (!Wi.has_grammem(rTransitive)) return false;
		if (!Wi.has_grammem(rPassiveVoice)) return false;
	};

	int k = get_next_main_word(i);

	if (k == sent.size()) return false;

	const CGroup& ObjGroup = get_maximal_group(k);
	
	if (		!Wk.is_syn_noun() 
            && !Wk.HasPOS(PRONOUN)  
			&&	( ObjGroup.m_GroupType != SELECTIVE_GR )
		) 
	return false;
	
	if ((ObjGroup.GetGrammems() & rAllCases) != _QM(rInstrumentalis))
			return false;

	if (		Wk.is_word_upper("�����")
			||	Wk.is_word_upper("����")
			||	Wk.is_word_upper("�����")
			||	Wk.is_word_upper("�������")
		)
	return  false;

	G.m_iLastWord = ObjGroup.m_iLastWord;
	G.m_MainGroup = VerbGroup;
	G.m_GroupType = INSTR_OBJ;
	G.SetGrammems( Wi.GetGrammems() );
	create_syn_rel(G,i,k,INSTR_OBJ);
	return true;
}


/*
	���������� ������ ������� ����������, ����� ������ ���������� ����� ����� �������� � ��������
	������������:
	"� �� ����"
	"���� ������� � ���� ��������".
*/
bool CRusFormatCaller::format_for_dir_obj_rev (CGroup& G)
{ 
    const CGroup& LeftGroup = get_maximal_group(G.m_iFirstWord);
    int i = LeftGroup.m_MainWordNo;
    if (!sent[i].is_syn_noun() || !sent[i].HasPOS(PRONOUN)  )
		return false;
	
	if (!(LeftGroup.GetGrammems() & _QM(rAccusativ))) return false;
	
	int k = get_next_main_word (i);
	if (k == sent.size()) return false;
	if (!Wk.has_grammem(rTransitive)) return false;
	if (Wk.has_grammem(rPassiveVoice)) return false;

	const CGroup& RightGroup =  get_maximal_group (k);
	if ( RightGroup.m_GroupType == DIR_OBJ ) return false;

	if (	!Wi.is_word_upper("����") 
		&&	!Wi.is_word_upper("����") 
		&&	!Wi.is_word_upper("���") 
		&&	!Wi.is_word_upper("��") 
		&&	!Wi.is_word_upper("���") 
		&&	!Wi.is_word_upper("���") 
		&&	!Wi.is_word_upper("��") 
		&&	!Wi.is_word_upper("����") 
		&&	!Wi.is_word_upper("������") 
		&&	!Wi.is_word_upper("������") 
		&&	!Wi.is_word_upper("���-��") 
		&&	!Wi.is_word_upper("����-��") 
		)
	return false;

	G.m_iLastWord = RightGroup.m_iLastWord;
	G.m_MainGroup = RightGroup;
	G.m_GroupType = DIR_OBJ_REV;
	G.SetGrammems( Wk.GetGrammems() );
	create_syn_rel (G, k, i, DIR_OBJ_REV);
	return true;
}



// �������:
// ������ ������� // ����������
// ���� ������  // ������������

bool CRusFormatCaller::format_for_verb_inf (CGroup& G)
{
  const CGroup& H = get_maximal_group(G.m_iFirstWord);
  int i = H.m_MainWordNo;
  if	(		( G.m_iFirstWord + 1 >= sent.size())
			||	(		!Wi.HasFlag(fl_can_subdue_infinitive)
					&&	!is_morph_predk(Wi) 
				)
		)
		return false;

  if( H.m_GroupType == DIR_OBJ)
	  return false;

  size_t k  = H.m_iLastWord + 1;
  if( k >= sent.size() ) return false;
  const CGroup& H1 = get_maximal_group(k);
  int j = H1.m_MainWordNo;
  if ( !is_infinitive(Wj) ) return false;
  if (Wj.m_UnitType  != EWord) return false;
  
  G.m_iLastWord = H1.m_iLastWord;
  G.m_Cause = "�� + ���������";
  G.m_MainGroup = H;
  G.m_GroupType = VERB_INF;
  G.SetGrammems( H.GetGrammems() );
  create_syn_rel(G, i,j, VERB_INF);  
  return true;
}
