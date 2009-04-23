#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "StdAfx.h"
#include "EngSynthes.h"
#include "EngVerbTense.h"
#include "EngNumerals.h"
#include "EngNumerals.h"


// ==================================================================

CGrammemsAnalyzer::CGrammemsAnalyzer () 
{
	m_Grammems = 0;
};

int	CGrammemsAnalyzer::GetNumber () const 
{  
	if (m_Grammems & _QM(ePlural)) 
		return 2;
	else
		if (m_Grammems & _QM(eSingular)) 
			return 1;
		else
			return 0;
}
void CGrammemsAnalyzer::SetNumber (int num) 
{  m_Grammems &= ~eAllNumbers;
	if (num==1)
		m_Grammems |= _QM(eSingular);
	else
		m_Grammems |= _QM(ePlural);
}

int	CGrammemsAnalyzer::GetPerson () const 
{ 
	if (m_Grammems & _QM(eFirstPerson))
		return 1;
	else 
	if (m_Grammems & _QM(eSecondPerson)) 
		return 2;
	else
	if (m_Grammems & _QM(eThirdPerson)) 
		return 3;
	else
		return 0;
}
int	CGrammemsAnalyzer::GetGender () const 
{  
	return (m_Grammems & _QM(eMasculinum)) > 0 ? 1: (m_Grammems & _QM(eFeminum)) ? 2: 3;
}




// ==================================================================
CEngSynthes::CEngSynthes(CEngSemStructure& EngStr)
:NodeHelper(EngStr)
{
	helper.text_kind = translate_helper::TEXT_HUD;
	helper.m_pData =  EngStr.RusStr.m_pData;
	helper.init_private_lists();
	m_bConnected = false;
}


// ����������� ������ - ��� ������, � ���� �� ����� ������� ������ �����������  �������
bool CEngSynthes::IsSlaveClause(long ClauseNo) const
{
	for (long i=0; i <E.m_Relations.size();i++)
	  if(IsBetweenClause(i))
		 if (E.m_Nodes[E.m_Relations[i].m_TargetNodeNo].m_ClauseNo == ClauseNo)
			  return true;
   return false;
};




void CEngSynthes::find_position_for_slave_clause_components()
{
    // ���� �� ���� ������� ������� 
	for(long ClauseNo=0; ClauseNo< E.m_Clauses.size(); ClauseNo++)
	{
        // ����� ������� ������
		vector<long> roots;	
		E.GetClauseRoots(ClauseNo, roots);
		long MainClauseRoot = E.GetMainClauseRoot(ClauseNo);

		// ������ ������?
		if (MainClauseRoot == -1) continue;

		// ������� ����� ����� ���� ������� ���������� ����������.
	    vector<long> Nodes;
		E.dfs_out(MainClauseRoot, Nodes);
		sort(Nodes.begin(),Nodes.end(),IsLessByMinWordNo(&E));
	    assert( Nodes.size() > 0);
		long MainLeftNodeNo =  Nodes[0];


		for (long i =0; i < roots.size();i++)
		 if( roots[i] !=  MainClauseRoot)
		 {
		   Nodes.clear();
		   E.dfs_out(roots[i], Nodes);
		   sort(Nodes.begin(),Nodes.end(),IsLessByMinWordNo(&E));
		   assert(Nodes.size() > 0);
		   long WordMo = E.m_Nodes[Nodes[0]].GetMinWordNo();
		   // �������, ��� ��������� ���������� ����������, ��� ��, ����������� ������
		   if (E.IsTheVeryLeftNodeOfClause(Nodes[0]))
		   {
				node_sub_clause_put_after.insert(make_pair(MainLeftNodeNo, make_pair(roots[i], false)));
		   }
		   else
		   {
			   long NodeNo = E.FindLeftClosestNode(Nodes[0]);
			   node_sub_clause_put_after.insert(make_pair(NodeNo, make_pair(roots[i], true)));
		   };

		 };
	};


};

void SetIndefiniteArticle(string& str, const translate_helper& helper)
{
	/*
		������� ������ ���� ������� ����� ������� �������, ����� �������
		such a boy
		all the day
		half a day
		half the day
		��:
		the half of day
	*/
	StringTokenizer tok1(str.c_str(), " ");
	StringTokenizer next_tok1(str.c_str(), " ");
	str.erase();
	string prev_word;
	next_tok1();
	while(tok1())
	{
		
		string word = tok1.val();
		if (    (  (prev_word == "<a>") && (word == "such"))
			 || (  (prev_word == "the") && (word == "all"))
		   )
			  swap (word, prev_word);

		if (next_tok1())
		{
			string next_word = next_tok1.val();
			if (next_word != "of")
			 if (    (  (prev_word == "<a>") && (word == "half"))
				  || (  (prev_word == "the") && (word == "half"))
				)
				swap (word, prev_word);
		};


		str += prev_word;
		str += " ";
	    prev_word = word;
	};
	str += prev_word;

	/*
	  ��������� ���������� ����� ��������������� ������� (�/an)
	*/
	StringTokenizer tok2(str.c_str(), " ");
	str.erase();
	prev_word = "";
	while(tok2())
	{
		string word = tok2.val();
		if (prev_word == "<a>")
		{
			prev_word = helper.an_article_before(word);
		};
		str += prev_word;
		str += " ";
	    prev_word = word;
	};
	str += prev_word;

};

string CEngSynthes::BuildSentence()
{

	m_bConnected = E.IsConnected();
	/*
	  ���� ���� �� ������, ��������� ����������� �����
	*/
	if (!m_bConnected)
	{
		for (long i=0; i < E.m_Relations.size(); i++)
			if (E.IsRelBetweenClauses(E.m_Relations[i]))
			{
				E.m_Relations.erase(E.m_Relations.begin() + i);
				i--;
			};

	};

    // ��������� SynthesResult ��� ������� ����, � ������� ����� ����������  ���������������
	// ����������
	result_vec.clear();
	result_vec.resize(E.m_Nodes.size());
	clause_conj.clear();
	E.PrintNodes();
	E.PrintRelations();

	find_all_clause_connectors();
	detect_clause_openers();
	find_position_for_slave_clause_components();
	// �������������� ���� m_bParenth, ������� ����������, ��� "���� ���� ������� ������"
	for (long i=0; i < E.m_Nodes.size(); i++)
	  Res(i).m_bParenth  = FieldContainsValue(E.m_Nodes[i], "GF", "PARENTH");


	// ���������� ������� - ��, � ������� ��  ������ �� ����� ������� (���� �����������),
	// �� ���� ������� ������� ����� ( � ����������� ������ �  �����������  ���� ��� ����� ���� 
	// ���������� �������
	vector<long> final_roots;

    // ���� �� ���� ������� 
	for(long ClauseNo=0; ClauseNo< E.m_Clauses.size(); ClauseNo++)
	{
        // ����� ������� ������
		long MainClauseRoot = E.GetMainClauseRoot(ClauseNo);
		if (!IsSlaveClause(ClauseNo)) final_roots.push_back(MainClauseRoot);
		vector<long> roots;	
		E.GetClauseRoots(ClauseNo, roots);
		for (long i =0; i < roots.size();i++)
		  translate_node(roots[i]);
	}

	   
    // �������� �� ���� ���������� �������� � ����� �� ��� ����������� ������
	string str;
	for(long i = 0; i < final_roots.size(); i++)
	{
		str += collect_results(final_roots[i]);
		// ������ ������� � ���������� ����������� (������� �� ���������� �� ������)
		// ��������, "���� ����, ���� ������"

		if ( i+1 < final_roots.size())
		  if (E.m_Nodes[final_roots[i]].m_ClauseNo != E.m_Nodes[final_roots[i+1]].m_ClauseNo)
			 str += ",";


	}
	if (E.m_bHasConjBut)
		str = "But " + str;

	SetSpacesAndRegisterInSentence(str, morphEnglish);
	SetIndefiniteArticle(str, helper);
	SetSpacesAndRegisterInSentence(str, morphEnglish);

	E.SetLastSentencePunctuationMark(str);

	return str.c_str();
}



void CEngSynthes::translate_node(int node)
{
	do{
		if(Res(node).times_visited++ > 0)	return;
		if(Res(node).times_visited > 5)		assert(false);
		if(Res(node).times_visited > 10)	throw logic_error("Cycle error");

		handle_prep(node);

		if(try_oneself_node(node))		break;
		if(try_conj_disrupt(node))		break;
		if(try_mna_node(node))		break;
		if(try_numeral_node(node))		break;

		if(try_simple_group(node))		break;

        if(try_noun_node(node))		break;
		if(try_pronoun_node(node))		break;
		if(try_verb_node(node))		break;
		if(try_adj_node(node))		break;

		if(try_default_node(node))		break;
		
	}while(false);

	handle_other_relations(node);

}

bool CEngSynthes::try_numeral_node(int node_no)
{
	if (!Node(node_no).IsPrimitive()) return  false;

	const CEngSemWord& W = Node(node_no).m_Words[0];

	//  "���������"
	if (   (W.m_Poses == 1<<eADV) 
		&& (W.m_Lemma == "") 
		&& ( atoi (W.m_Word.c_str()) >= 2) //  � "�������"  �� ������
	   )
	{
		Res(node_no).m_WordForms.push_back(spellout_number(W.m_Word, true));
		Res(node_no).m_WordForms.push_back("together");
		Res(node_no).m_Position = ">>";
		return true;
	}

	// ����� "���" ������ ����������� �� �������
	if (Node(node_no).GetType() != NoneRoss) return  false;

	if ( (W.m_Poses != 1<<eNUMERAL) && (W.m_Poses != 1<<eORDNUM))  return false;
	Res(node_no).m_WordForms.push_back(spellout_number(W.m_Word, W.HasPOS(eNUMERAL) ));

	/*
	 ���������� ������� � ���������� ������������, ������� �������� ���� ���������������
	 �������� "������ ������, ������ ����."
	*/
	if ( W.HasPOS(eORDNUM) )
	{
		vector<long> Rels;
		E.GetIncomingInClauseRelations(node_no, Rels);
		long InRelNo = -1;
		if (Rels.size() == 1)
			InRelNo = Rels[0];

		if	(  (InRelNo == -1)
			|| (   (E.m_Relations[InRelNo].m_Valency.m_RelationStr != "PROPERT")
				// ���� �������			     

				&& (E.m_Relations[InRelNo].m_Valency.m_RelationStr != "") 
				// ���� ���� ������
			    ) 
			)
			Res(node_no).m_Article = DefArticle;
	};

    // ��������� ������� ������������� (��� �����_��)
	vector<long> rels;
	get_out_rels(node_no, rels);
	for(long i = 0; i < rels.size(); i++)
		translate_node(Rel(rels[i]).m_TargetNodeNo);

    return true;
};


bool CEngSynthes::try_verb_node(int node_no)
{

	// ���� ���������� 
	vector<long> rels;
	get_out_rels(node_no, rels);
	int i = 0;
	for(; i < rels.size(); i++)
	  if(is_subj_rel(rels[i])) break;
	int subj = i == rels.size() ? -1 : Rel(rels[i]).m_TargetNodeNo;
	
	// ���� ��� ���������� � ��� �� ���������� �����, �� �������
	if(    (subj == -1) 
		&& !node_has_poses(node_no, (1 << eVERB) | (1 << eVBE) | (1 << eMOD))
	  ) return false;

	// �������� ���������� ������ CEngVerbTense � ��������� ��� �������� ������� make_verb_form
	CEngVerbTense EngVerbTense(*this, node_no);
	Res(node_no).subject_node = subj;
	EngVerbTense.make_verb_form();


    // ��������� ������� ������� 
	for(int i = 0; i < rels.size(); i++)
	{
		int sub = Rel(rels[i]).m_TargetNodeNo;
		if(Res(sub).do_not_put) continue;
		translate_node(sub);
		
		if (Rel(rels[i]).m_Position == "") 
		  if(is_subj_rel(rels[i]))
			Res(sub).order("<", 0); // ���������� � ������.
	}

	return true;
}

bool CEngSynthes::try_default_node(int node_no)
{
	vector<long> out_rels;
	get_out_rels(node_no, out_rels);
	CEngSemNode node = Node(node_no);
	if(node_is_pronoun(node_no)) Res(node_no).m_Article = ZeroArticle;

	for(int i = 0; i < node.m_Words.size(); i++){
			CEngSemWord& W = Node(node_no).m_Words[i] ;
			helper.synthesize(W);
			result_vec[node_no].m_WordForms.push_back(W.m_Word);

		}
// sons are here
	for(int i = 0; i < out_rels.size(); i++){
		translate_node(Rel(out_rels[i]).m_TargetNodeNo);

   	   
	}	
	handle_rel_operators(node_no);
	return true;

}



bool CEngSynthes::try_mna_node(int node_no)
{
	const CEngSemNode &node = Node(node_no);
	if(!node.m_NodeType == MNA) return false;
	string tr;
	if(node.GetType() == EngObor){
		// �������� ��� �������� ������� ����������� ����, � �������� � ����� ������� ��� ������
		assert ( node.IsPrimitive() );
		tr = node.m_Words[0].m_Word;
	};

	Res(node_no).m_WordForms.push_back(tr);
	vector<long> rels;
	get_out_rels(node_no, rels);
	sort(rels.begin(), rels.end(), rel_pos_less(*this));

	for(int i = 0; i < rels.size(); i++)
	{
		CEngSemRelation& rel = Rel(rels[i]);
		int sub_node = Rel(rels[i]).m_TargetNodeNo;
		translate_node(sub_node);
		SynthesResult &rrr = Res(sub_node);
		if (rel.m_Valency.m_RelationStr.empty())
		{
			// Empty relations are members of sumilar phrase, named relations are their common valencies
			if(rel.m_Position == ">"){
				rrr.m_Position = ">>";
			}else if(rel.m_Position == "<"){
				;
			}else{
				if(i == (rels.size() - 1))
					rrr.m_Position = ">>";
			}
		}
		else
		{
			if (Node(sub_node).GetMinWordNo() < Node(node_no).GetMinWordNo()) 
			{
				// move all left-hand relations to the first similar member
				for(int j = i+1; j < rels.size(); j++)
					if (Rel(rels[j]).m_Valency.m_RelationStr.empty())
					{
						rel.m_SourceNodeNo = Rel(rels[j]).m_TargetNodeNo;
						break;
					}
			}

			
		}
		
		
	}
	return true;
}


bool CEngSynthes::try_simple_group(int node_no)
{
	string str;
	CEngSemNode &node = Node(node_no);

	if(node.m_SynGroupTypeStr == WEB_ADDR)
	{
		for(int i = 0; i < node.GetWordsSize(); i++){
			str += node.GetWord(i).m_Word;
		}
	}
	else if(node.m_SynGroupTypeStr == NAMES)
	{
		int i = 0;
		for(; i < node.GetWordsSize(); i++)
		{
			
			helper.transliterate(node.m_Words[i]);

			// ����, ����� "���������� ����" �� �������������������!
			if	(		(node.RusNode != -1) 
					&&	(i < E.RusStr.GetNode(node.RusNode).GetWordsSize())
				)
			{

			  const CSemWord& W = E.RusStr.GetNode(node.RusNode).GetWord(i);
			  long ParadigmId = W.m_ParadigmId;
			  vector<long> EngIds;
			  helper.translate_id(ParadigmId, EngIds, W.m_Poses);
			  if (!EngIds.empty() && !W.HasOneGrammem(rPatronymic))
			  {
			    node.m_Words[i].m_Lemma = helper.create_norm_by_id (EngIds[0]);
				node.m_Words[i].SetFormGrammems(node.GetGrammems());
				helper.synthesize(node.m_Words[i]);
			  };
			}

			if(i > 0)
				str += " ";
			str += node.m_Words[i].m_Word.c_str();
		}

		vector<long> rels;
		get_out_rels(node_no, rels);
		for(i = 0; i < rels.size(); i++)
		{
			const CEngSemRelation& rel = Rel(rels[i]);
			translate_node(Rel(rels[i]).m_TargetNodeNo);
		}

	}
	else
		return false;
		

	Res(node_no).m_WordForms.push_back(str);
	return true;
}


bool CEngSynthes::HasInfinitiveInTree(int node_no) const
{
  if (is_infinitive(E.m_Nodes[node_no].GetTense()) ) return true;
  vector<long> Rels;
  get_out_rels(node_no, Rels);
  for (long i =0; i < Rels.size(); i++)
	if (HasInfinitiveInTree(E.m_Relations[Rels[i]].m_TargetNodeNo) )
		return true;
 return false;
};


bool ordered_rel_pos_less::operator ()(int r1, int r2)const
{
			const SynthesResult &res1 = S.result_vec[S.E.m_Relations[r1].m_TargetNodeNo];
			const SynthesResult &res2 = S.result_vec[S.E.m_Relations[r2].m_TargetNodeNo];
			long value1 =  ValuePosition(res1.m_Position);
			long value2 =  ValuePosition(res2.m_Position);
			if (value1  != value2)
				return value1 < value2;
			/*
			   ��������������  � �������������� so, such 
			   ������ ������ ����� ��������������� ��� ����� �������������
			*/

			if (   S.node_is_adj(S.E.m_Relations[r1].m_TargetNodeNo)
				&& S.node_is_adj(S.E.m_Relations[r2].m_TargetNodeNo)
			   )
			{
bool b1 =  !S.E.HasOutRelationByName(S.E.m_Relations[r1].m_TargetNodeNo, "PROPERT");bool b2 =  !S.E.HasOutRelationByName(S.E.m_Relations[r2].m_TargetNodeNo, "PROPERT");				if (b1 !=  b2)
					return b1 < b2;
			};

			// ������ ��������� ��������� ������  ������ ������ ������� ������������, ��������,
			// "� ������ ��������� ���������� ����� ������� ������������ �������."
			if (value1 > 0)
			  if (S.rel_is_valency(r1) !=  S.rel_is_valency(r2))
				return S.rel_is_valency(r1) > S.rel_is_valency(r2);
		   /*
		     ����  ���� ������ �������� ����������� (��� � ���� � �������� ���� ���������),
			 � � ������ ���, ����� ������ ������ ������ ����� �������
		   */
			if (   S.rel_is_valency(r1) 
				&& S.rel_is_valency(r2))
			{
				bool b1 =  S.HasInfinitiveInTree(S.E.m_Relations[r1].m_TargetNodeNo);
				bool b2 =  S.HasInfinitiveInTree(S.E.m_Relations[r2].m_TargetNodeNo);
				if (b1 !=  b2)
					return b1 < b2;
			};

		   /*
		     �����������, ����������  ��������� of, ������ ������ ����� ����� �������
		   */
			if (		S.node_is_noun(S.E.m_Relations[r1].m_SourceNodeNo)   )
			{
					if (S.result_vec[S.E.m_Relations[r1].m_TargetNodeNo].m_prep == "of")
							return true;

					if (S.result_vec[S.E.m_Relations[r2].m_TargetNodeNo].m_prep == "of")
							return false;
			};

				
		   return res1.pos_order < res2.pos_order;

}

struct ordered_rel_pos_less_by_word_no
{
	const CEngSynthes &S;
	ordered_rel_pos_less_by_word_no(const CEngSynthes &inS): S(inS){}
	bool operator ()(int r1, int r2) const
	{
		long WordNo1 = S.E.m_Nodes[S.E.m_Relations[r1].m_TargetNodeNo].GetMinWordNo();
		long WordNo2 = S.E.m_Nodes[S.E.m_Relations[r2].m_TargetNodeNo].GetMinWordNo();
		return WordNo1 < WordNo2;

	};
};

string CEngSynthes::collect_results(int node)
{
	if(Res(node).do_not_put) return "";
	
	string res;
	SynthesResult &result = result_vec[node];
	string MainNodeStr;
	int i;

	// 
	if(get_conj_word(node) != -1){
		int ConjWordNodeNo = get_conj_word(node);
		Res(ConjWordNodeNo).do_not_put = false;
		Res(ConjWordNodeNo).times_visited = 0;
		Res(ConjWordNodeNo).m_WordForms.clear();
		translate_node(ConjWordNodeNo);	
		res += collect_results(ConjWordNodeNo);	
		Res(ConjWordNodeNo).do_not_put = true;
	}else if(Res(node).do_not_put){
		return "";
	}

	bool ShouldPutCommaAfterClause = false;;
	bool IsClauseTop  = E.IsLeftClauseTop(node);
    if  (IsClauseTop )
      if (clause_conj.find(E.m_Nodes[node].m_ClauseNo) != clause_conj.end())
	  {
		  // ���� ������ "but" ��� "or" ����� ��������� �������
		  if (  (clause_conj[E.m_Nodes[node].m_ClauseNo].m_WordStr == "but")
			  ||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_WordStr == "or")
			 )
			  res += ",";

		  res += " "+clause_conj[E.m_Nodes[node].m_ClauseNo].m_WordStr+" ";

		  if (E.m_Nodes[node].m_ClauseNo == 0)
			if (  (clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "instead_of") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "while") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "as") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "as_soon_as") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "after") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "before") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "in_order_to") 
				||(clause_conj[E.m_Nodes[node].m_ClauseNo].m_UnitStr == "when") 
				// ��� unlike, ��������� ��� �������
				)
				ShouldPutCommaAfterClause = true;
	  };
	
	// ������������ opener, ���� �������, ��� �� ����� ���� ������ ����
	// opener ������ ������ ����� ����������� ������ � ������� ����, 
	if (E.GetMainClauseRoot(E.m_Nodes[node].m_ClauseNo) == node)
	if(clause_openers.count(E.m_Nodes[node].m_ClauseNo))
	{
			int OpenerNodeNo = clause_openers[E.m_Nodes[node].m_ClauseNo];
			// ���������� do_not_put � times_visited, ����� translate_node �� �������
			Res(OpenerNodeNo).do_not_put = false;
			Res(OpenerNodeNo).times_visited = 0;
			Res(OpenerNodeNo).m_WordForms.clear();
			translate_node(OpenerNodeNo);
			res  +=  " " +collect_results(OpenerNodeNo);	

			// 2 ����� ���� ������� ������ -> On 2 of March, he laughed
			if (E.m_Nodes[node].m_ClauseNo == 0)			res += ",";

			Res(OpenerNodeNo).do_not_put = true;
			clause_openers.erase(E.m_Nodes[node].m_ClauseNo);
	}


/*
   ������ �������� ����
*/

	if (E.m_Nodes[node].m_bQuoteMarks)
	MainNodeStr += "<Quote> ";

	MainNodeStr += result.prefix + " ";



	for(int w = 0; w < result.m_WordForms.size(); w++){
		MainNodeStr += result.m_WordForms[w];
		MainNodeStr += " ";
	}

	if (E.m_Nodes[node].m_bQuoteMarks)
		MainNodeStr += "</Quote>";



	MainNodeStr += result.postfix + " ";

	vector<long> out_rels;
	get_out_rels(node, out_rels);
	  
	
	for(i = 0; i < out_rels.size(); i++)
	{
		string Position = Rel(out_rels[i]).m_Position;

		if (Rel(out_rels[i]).m_bReverseRel)
			Position = GetDualPosition(Rel(out_rels[i]).m_Position);

		long sub_node = Rel(out_rels[i]).m_TargetNodeNo;

 		if (Res(sub_node).pos_order == -1)
			Res(sub_node).pos_order = E.m_Nodes[sub_node].GetMinWordNo();



		if  (    (  ValuePosition(Position) != 0  )
			)
			Res(sub_node).m_Position = Position;

		// ����� ����, �����-������ �������� � translte_node ��� ��������� 
		// ������� (�������� ������� �����������), ������� �������� 
		// �������, ���� ��� �� ���� ��������� 
		if (Position == "")
			Position = Res(sub_node).m_Position;


		/*
		  ���� ���� �� ������, ����� ����� ����������� ������  �������, ������� ������ �� ���������
		*/
		if (!m_bConnected)
		{
			 if (E.m_Nodes[sub_node].GetMinWordNo() < E.m_Nodes[node].GetMinWordNo())
				 Res(sub_node).m_Position = "<";
			 else
			 	 Res(sub_node).m_Position = ">";
			continue;
		};

		// �������� ������ ����� ������ �������
		if (Position == "")
		   if (Rel(out_rels[i]).m_SyntacticRelation == "indir_obj")
			 Position = ">!";
		   else
			 if (   (Rel(out_rels[i]).m_SyntacticRelation == "obj")
				 || (Rel(out_rels[i]).m_SyntacticRelation == "dir_obj")
				 || (Rel(out_rels[i]).m_SyntacticRelation == "obj_pl")
				)
			 {
				/*
				���� ������ ������� �����������, �� ���, �������, �� ���� ������ ������ 
				��������� ������� ("indir_obj"), ��������� ����� ����� 
				���������� ������� ������� �������� ������ � ���������� 
				��������:
					This technology promised (A1:chance to work on the same line) (A2:to several users)
					����� A1 - ������������ ������;
					      A2 - ������� ��������� ������.
				   �������, ��� ������ ������ ��������, ��� ������� � �������, � ������:
				   This technology promised  (A2:to several users) (A1:chance to work on the same line)
				*/
			   if (HasInfinitiveInTree(Rel(out_rels[i]).m_TargetNodeNo))
				   Position = ">!";
			   else
				   Position = ">>";
               /*
			    �������� ����� ���� ���, ��������: "I consider him a teacher"
				��� ������� ����� �������������  �� ������ ����������� � ������.
			   */
			   if ( rel_is_valency(out_rels[i]) )
					Res(sub_node).pos_order = E.m_Relations[out_rels[i]].m_Valency.m_LeafId*10 + E.m_Relations[out_rels[i]].m_Valency.m_BracketLeafId;
			 };


	    // ��� ���
		if  (  ValuePosition(Position) != 0  )
			Res(sub_node).m_Position = Position;
		else
		/*
		  �������, ������� ��������� �������� ������ ����� � �����������, (??)
		  ��������   "Going quickly is dangerous" 
				���  "Going faster is dangerous" 
		*/
		if (    is_gerund(E.m_Nodes[node].GetTense()) 
			&& (   node_is_adv(E.m_Nodes[sub_node])
			   ||  (   node_is_adj(sub_node) 
					&& (E.m_Nodes[sub_node].GetGrammemsRich() & _QM(eComparativ) )
				   )
			   )
		   )
		{
			Res(sub_node).m_Position = ">";

		}
		else
		// ��������������, ������� ��������� ���������������� 
		// ������ ������ ����� ����� ��������
		if (    _node_is_adj(E.m_Nodes[sub_node]) 
			&&  node_is_noun(node)
		   )
		{
			Res(sub_node).m_Position = "<<";
		}
		else
		// �������, ������� ��������� ������� ������� ��� ���������������
		// ������ ������ ����� ����� ��������
		if (    node_is_adv(E.m_Nodes[sub_node]) 
			&& (   node_is_adv(E.m_Nodes[node])
			    || _node_is_adj(E.m_Nodes[node])
			   )
		   )
		{
			Res(sub_node).m_Position = "<<";
		}
		else
		if (    
			    rel_is_valency(out_rels[i])
			 && (    E.m_Relations[out_rels[i]].HasSomePrep()
				)
		   )
		{
			Res(sub_node).m_Position = ">!";
		}
		else
			/*
			  ����� �������: ������������������ ������������ ������ ������ ������
			  ����� ������ �������, ���� ������ - ������
			*/
		if ( 
			     node_is_verb(node)
			 &&  rel_is_valency(out_rels[i])
			 && !E.IsSubj(E.m_Relations[out_rels[i]])
			 
		   )
		{
			Res(sub_node).m_Position = ">";
		}
		else
		// ���� Position �� �����, ��������� �������, �������� �������� ����
		 if (Position == "")
		{
			 if (E.m_Nodes[sub_node].GetMinWordNo() < E.m_Nodes[node].GetMinWordNo())
				 Res(sub_node).m_Position = "<";
			 else
			 	 Res(sub_node).m_Position = ">";
		};



	}

	if (m_bConnected)
	   sort(out_rels.begin(), out_rels.end(), ordered_rel_pos_less(*this));
	else
	   sort(out_rels.begin(), out_rels.end(), ordered_rel_pos_less_by_word_no(*this));


	


	//  ����� ��, ������� �������� "<" "!<" "<<"
	for(i = 0; i < out_rels.size() && ValuePosition(Res(Rel(out_rels[i]).m_TargetNodeNo).m_Position) < 0; i++)
	{
		int sub_node = Rel(out_rels[i]).m_TargetNodeNo;
		if(Res(sub_node).do_not_put) continue;
		res += collect_results(sub_node) + " ";

		// �� ���� ��� ��  ��������� -> for once, he laughed
		if  (    ( Rel(out_rels[i]).m_Position == "begin")
			  && ( Rel(out_rels[i]).m_PosType ==   FromArticlePosType)
			)
		res += ",";

		// ����  ���� MUA ������ ���� ��������� ��������		
		if (     (E.m_Nodes[node].m_NodeType == MNA)  )
		if (	Rel(out_rels[i]).m_Valency.m_RelationStr.empty() ) 
		if (   (
				   (i+1 < out_rels.size())
				&& (ValuePosition(Res(Rel(out_rels[i+1]).m_TargetNodeNo).m_Position) < 0)
			   )
		    || (E.m_Nodes[node].IsComma())  
		   )
			res += ",";
		
	}	

	res += MainNodeStr + " ";

	//  ����� ��, ������� �������� ">" ">!" ">>"
	for(; i < out_rels.size(); i++)
	{
		int sub_node = Rel(out_rels[i]).m_TargetNodeNo;
		if(Res(sub_node).do_not_put) continue;
		assert ( ValuePosition(Res(Rel(out_rels[i]).m_TargetNodeNo).m_Position) >=0);
		res += collect_results(sub_node) + " ";
	}	

	/*
	  ����� ����������� �������� �  �������, ���� ���� ����
	  ���������� � ��� ���������������� ������ ���������
	*/
	bool bQuoted = E.m_Clauses[E.m_Nodes[node].m_ClauseNo].m_bQuoted;
	if (E.m_Nodes[node].m_bQuoteMarks)
	{
		long in_rel = get_in_rel(node);
		if (in_rel != -1)
			if (E.m_Nodes[Rel(in_rel).m_SourceNodeNo].m_bQuoteMarks)
				bQuoted = true;
	};
	string QuoteStart = (bQuoted) ? "<Quote>" :"";
	string QuoteEnd = (bQuoted) ? "</Quote>" :"";

	if (result.m_Article == IndefArticle)	res = QuoteStart+" <a>  "+QuoteEnd+ res;
		else if (result.m_Article == DefArticle) res = QuoteStart+"  the " +QuoteEnd+ res;
	

	if(result.has_prep())
		res = QuoteStart+result.m_prep+QuoteEnd+ " " + res;

	if(!result.rel_operator.empty())
		res = QuoteStart + result.rel_operator + QuoteEnd+ " " + res;

	if (!result.before_prep.empty())
	res = QuoteStart + result.before_prep + QuoteEnd + "  " + res;


    if (Res(node).m_bParenth)
		res = " , " + res + " , ";

    if (Node(node).HasRelOperator("���_�����"))
	{
		res  = " as "+ res + " as possible ";
	};

	if (IsClauseTop)
		if (E.m_Clauses[Node(node).m_ClauseNo].m_bBracketed)
		{
			res  = " ( "+ res + " ) ";

		};

	
	if (ShouldPutCommaAfterClause)
		res += ",";


	// check for clause rel
	if(node_sub_clause_put_after.count(node))
	{
		typedef multimap<int, pair<int, bool> >::iterator iter;
		pair<iter, iter> range = node_sub_clause_put_after.equal_range(node);
		for(iter it = range.first; it != range.second; ++it){
			bool put_after = it->second.second;
			int sub = it->second.first;
			string sub_str = collect_results(sub);
		

			if(put_after)
				res += sub_str;
			else 
					res = sub_str + res;
		}
	}

	
	return res;
}



// ��������� �������� �� ������ ���� � ������ � ������ tmp  ��� �����, ������� ����� �� �������� �����,
// � �����, ������� ����� ����� �������� �����, ������ � ������ res.
// �����, ���� tmp �� ����, ����� ������ tmp ����������� � ������ ������� res
// ���������, ��� ���� ������� ����� ����� �� �����������. ��������������, ���
// ��� ��� ��� ���� ��������� �� ������ ���� ���������.

void CEngSynthes::handle_colloc_words(int node_no, SynthesResult &res)
{
	StringVector tmp;
	for(int i = 0; i < Node(node_no).m_Words.size(); i++)
	{
		if(i < Node(node_no).m_MainWordNo){
			tmp.push_back(Node(node_no).m_Words[i].m_Word);
		}
		else if(i > Node(node_no).m_MainWordNo)
			res.m_WordForms.push_back(Node(node_no).m_Words[i].m_Word);
	}
	if(tmp.size()) res.m_WordForms.insert(res.m_WordForms.begin(), tmp.begin(), tmp.end());
}


int CEngSynthes::compare_node_positions_by_min_word_no(int n1, int n2) const
{
	return E.m_Nodes[n1].GetMinWordNo() - E.m_Nodes[n2].GetMinWordNo();
}






bool CEngSynthes::dir_obj_has_prep(int node_no)
{
	return Res(node_no).has_prep();
}


bool CEngSynthes::is_conj_word(int parent, int son)
{
	return get_conj_word(parent) == son;
}
	
bool CEngSynthes::is_clause_opener(int ClauseNo, int OpenerNodeNo)
{
	map<int,int>::iterator  It =  clause_openers.find (ClauseNo);

	return    (It !=  clause_openers.end())
		   && (*It).second == OpenerNodeNo;
}





string CEngSynthes::translate_son(int node_no)
{
	translate_node(node_no);
	return collect_results(node_no);
}

bool IsWh_NounWord(string Lemma)
{
	EngRusMakeUpper(Lemma);
	return    (Lemma == "WHO") 
			||(Lemma == "WHICH")  
			||(Lemma == "WHAT")  
			||(Lemma == "WHERE")  
			||(Lemma == "WHEN")  
			||(Lemma == "WHICH")  
			||(Lemma == "WHOSE");
};

void CEngSynthes::find_all_clause_connectors()
{

	for(int i = 0; i < E.m_Relations.size(); i++)
	{
		if(!IsBetweenClause(i)) continue;

		int master  = Rel(i).m_SourceNodeNo;
		int slave = Rel(i).m_TargetNodeNo;

		/*
		   � ��� ���� ����������� �������, ������� ������ �� � ������� ������.
		   ������, �� ����� ���� � ������� ������.
		   
			 "� ����, ��� ������."
			 "� ����, � ����� ���� �� �����"
		   ������ ������ ������� �����, � �������� �����, ������� �� ������� ������.
		   �� ����� ��� � �������� ����� ���� ������������� ����. ������ �����
		   ������  ������ � ������ ������. �� �� �� ����� ������ �������� ��� ����
		   � ��������� � ������.
		   ��������, ������ �������
			"I know which you live in house" ("� ����, � ����� ���� �� �����")
			����� � ������ ���� ���������� ��� ������ "� ����� ����", �����
			���������� "I know in which house you live".
			����� �������, �������, ��� ���� ������� ����� �������� ��������������,
			������� ��������� ����������������, 
			����� ������ ���� �������� ��� ������� ������, ������� �������� ��� �������������� 
		*/
		int ConjWordNodeNo = slave;
		long in_rel = get_in_rel(slave);
		int slave_root = E.GetMainClauseRoot(E.m_Nodes[slave].m_ClauseNo);			
		/*
		  �������� ������, ����� ����� �� ��������� � ������� ����� ����� � 
		  ��������� ���������� ����������, �����  �������  ����� ���� ���� ������,
		  � ������ ����� ������ � ������.
		*/
		if (in_rel != -1)
		{
			if(E.m_Nodes[slave].GetPos() == ePN_ADJ)
			  while (  (E.m_Nodes[Rel(in_rel).m_SourceNodeNo].GetPos() == eNOUN)
				     ||(E.m_Nodes[Rel(in_rel).m_SourceNodeNo].GetPos() == ePN_ADJ)  //@each
					)
			  {
				  ConjWordNodeNo = Rel(in_rel).m_SourceNodeNo;
				  in_rel = get_in_rel(ConjWordNodeNo);
				  if (in_rel == -1) break;
			  };

			Res(ConjWordNodeNo).do_not_put = true;
			clause_conj_words.push_back(CClauseRootToConjWord(slave_root, ConjWordNodeNo));
		}

		
		
		// ��������� ����, ����� �������� ����� ��������� ����������� ������
		/*
		 ������ � �����, ��� ������ ����. ����������� � who, which ����� ������� ����� ����� �����. �����,
		 � ������� ������� ����� �������, 
			 "if (!IsWh_NounWord(E.m_Nodes[slave].m_Words[E.m_Nodes[slave].m_MainWordNo].m_Lemma) )"
		 ������ �������� �������� �������, ���� ������������ ����� - ���������������, ��������:
		  "��������, ��� �� ���� ���������� ������, ������������ ���� ���� ����"
		  ������� ����� ����. ������� ������ ���� ���������� ������� (NP). ��������,
		     "he assures me that I am mistaken"

		  � � �������
		  "�� ���� ������� �����������, ����� ������������ � ������."
		  ����������� �����  ���� �� �������, ������� ������ ������� ����� ���� ������� 
		*/
	    int node_to_put_after = master;		
		vector<long> roots;	
		E.GetClauseRoots(E.m_Nodes[master].m_ClauseNo, roots);
		if (roots.size() > 0)
		 if (   (E.m_Nodes[master].m_MainWordNo != -1) 
			 && (E.m_Nodes[slave].m_MainWordNo != -1) 
			)
		 {
		  node_to_put_after = roots[0];		

		  if  (   IsWh_NounWord(E.m_Nodes[slave].m_Words[E.m_Nodes[slave].m_MainWordNo].m_Lemma) 
				// �� ����� "�� ���� ����� ��������, ��� ��� ����������" ����� �����_����
				// �� �������� �����������
				|| (Rel(i).m_Valency.m_RelationStr == "PROPERT") // ���: ������������� ������������� ��� ����� �������			  
			  )
		  {
			  if(   (E.m_Nodes[master].GetPos() == eNOUN )
				 || (E.m_Nodes[master].GetPos() == ePN ) 
				)
				node_to_put_after = master;
			  else
				/*
					����  ����� ������ � ����. ��������������, �����
					����� ��������� �� ����������������, ������� ��������� ���� ��������������
					� ��������� ������ ����� ����� ����������������
					"��� ������ ��� �������, ������� ��� � ������� ������, � ����������� �� ������.	"

				*/
				if(E.m_Nodes[master].GetPos() == ePN_ADJ ) 
				{
					long rel = get_in_rel(master);
					if (   (rel != -1)
					    && (E.m_Nodes[Rel(rel).m_SourceNodeNo].GetPos() == eNOUN )
					   )
					   node_to_put_after = Rel(rel).m_SourceNodeNo;
				};
		  }
		     
		 };

		// �� ������� �������� ��� ��, �������, ��� � � ������� ������������
		bool put_after = E.GetClauseFirstWordNo(Node(master).m_ClauseNo) < E.GetClauseFirstWordNo(Node(slave).m_ClauseNo);
		

	
		node_sub_clause_put_after
			.insert(make_pair(node_to_put_after, make_pair(slave_root, put_after)));



		/*
		   ��������� �����
		*/
		const CSynRealization &syn = Rel(i).m_SynReal;
		const CRossInterp &conj = syn.m_Conj;
		string conj_for_slave; // if
		string conj_for_slave_unit_str; // if
		string conj_for_master; // then
		
		if(conj.m_DictType == EngObor){
			conj_for_slave = E.m_pData->GetEngOborStr(conj.m_UnitNo);
			conj_for_slave_unit_str = E.m_pData->GetRoss(EngObor)->GetEntryStr(conj.m_UnitNo);
			// ���������, ���� ���� �������� ���������, �����
			// ����� �������� ������ ����� (����� "...") � �������� ��
			// � ������� ������
			if (conj_for_slave.find ("...") != -1)
			{
				int j = conj_for_slave.find ("...");
				conj_for_master = conj_for_slave.substr(j+3);
				Trim(conj_for_master);
				conj_for_slave = conj_for_slave.substr(0, j);
				Trim(conj_for_slave);
			};
		};

		if(!syn.m_Other.empty()){
			conj_for_slave = syn.m_Other;
		}

		if(conj_for_slave.size()){
			clause_conj[E.m_Nodes[slave].m_ClauseNo] = CClauseConj(conj_for_slave,conj_for_slave_unit_str);
		}

		if(conj_for_master.size()){
			clause_conj[E.m_Nodes[master].m_ClauseNo] = CClauseConj(conj_for_master,"");
		}

		// ������� ������� � ���������� ����������� 
		if (    (E.m_Relations[i].m_Valency.m_RelationStr == "AND")
			 &&  syn.m_Conj.m_UnitNo == ErrUnitNo
		  )
		{
			clause_conj[E.m_Nodes[slave].m_ClauseNo] = CClauseConj(",","");
		};

	}
	
}




bool CEngSynthes::try_oneself_node(int node_no)
{
	const CEngSemNode &node = E.m_Nodes[node_no];
	bool one_s = node.IsPrimitive() && node.m_Words[0].m_Word == "#one's"; 
	bool oneself = node.IsPrimitive() && node.m_Words[0].m_Word == "#oneself";
	if(!one_s && !oneself) return false;

	long AntecedentNodeNo = E.GetEquNode(node_no);
 
	QWORD Grammems = 0;
	
    if (AntecedentNodeNo != -1)
	{
		const CEngSemNode* AntecedentNode = &E.m_Nodes[AntecedentNodeNo];

		if  (		AntecedentNode->HasGrammemRich(eAnimative)
				||  E.HasSemFet(*AntecedentNode, "ANIM")		    
			)
		      Grammems |= _QM(eAnimative);
	    else
		      Grammems &= ~_QM(eAnimative);

		// ��� �����������
	    if ( (Grammems & ( _QM(eFirstPerson) |  _QM(eSecondPerson) | _QM(eThirdPerson) ) )  == 0)
		{
		   if (AntecedentNode->GetGrammemsRich() & _QM(eFirstPerson))
			  Grammems |= _QM(eFirstPerson);
		    else
		    if (AntecedentNode->GetGrammemsRich() & _QM(eSecondPerson))
			   Grammems |= _QM(eSecondPerson); 
             else  
			   Grammems |= _QM(eThirdPerson);
		}
		else
		  // ��� ���������������
			Grammems |= _QM(eThirdPerson);

	    if ((Grammems & eAllGenders) == 0)
		{
			QWORD debug = (eAllGenders & AntecedentNode->GetGrammemsRich());
			Grammems |= (eAllGenders & AntecedentNode->GetGrammemsRich());
		}

  	     if ((Grammems & eAllNumbers) == 0)
	       Grammems |= (eAllNumbers & AntecedentNode->GetGrammemsRich());

	}
	else
	{


		/*
			���� ������ � #oneself ���������� � ���������� ��.�, �� �����
				���������� ��� ... yourself, � ���� � ���������� ��.�., ��
			...yourselves: ���������� � ������ - acquaint yourself with the
			book.
		*/
		vector<long> Rels;
		E.GetIncomingRelations(node_no, Rels);
		if (Rels.size() == 1)
		{
			CEngSemNode& N =  E.m_Nodes[E.m_Relations[Rels[0]].m_SourceNodeNo];
			if (   N.IsPrimitive()
				&& N.m_Words[0].m_bImperative
			   )
			  {
				  Grammems |=  N.GetGrammemsRich() &  eAllNumbers;
				  Grammems |=  _QM(eSecondPerson);
			  };
		};
	};

	string debug_str = E.m_pData->GetEngGramTab()->GrammemsToStr(Grammems);

	Res(node_no).m_WordForms.push_back(GetPronounEnglishFormByGrammems(Grammems,oneself));

	/* ���� oneself ������� �� ����������������, 
	����� ����� �������, ��� oneself ����� ������ �����
	��������, Lord John himself.
	����� himself ������� �� Lord.
	*/
	if (oneself)
	{
		vector<long> Rels;
		E.GetIncomingRelations(node_no, Rels);
		if (Rels.size() == 1)
		{
			const CEngSemNode& N =  E.m_Nodes[E.m_Relations[Rels[0]].m_SourceNodeNo];
			if (N.GetPos() == eNOUN)
				if (E.m_Relations[Rels[0]].m_bReverseRel)
				  E.m_Relations[Rels[0]].m_Position = "<";
				else
				  E.m_Relations[Rels[0]].m_Position = ">";
		};
	};
    
	return true;
}


int CEngSynthes::get_conj_word(int root_node_no)
{
	for (int i = 0; i < clause_conj_words.size(); i++)
		if (root_node_no == clause_conj_words[i].m_RootNodeNo)
			return clause_conj_words[i].m_ConjWordNodeNo;

	return -1;	
};

bool CEngSynthes::is_some_conj_word(int conj_word)
{
	for (int i = 0; i < clause_conj_words.size(); i++)
		if (conj_word == clause_conj_words[i].m_ConjWordNodeNo)
			return true;

	return false;	
};

void CEngSynthes::detect_clause_openers()
{
    // ������� ��� ����-����, ������� ������ �� ������ ����� � ������� �����������
	for(int i = 0; i < E.m_Nodes.size(); i++)
      if (    E.m_Nodes[i].IsWordContainer()
		  && is_time_or_loc_node_and_first_in_clause(i)
		  && !E.m_Nodes[i].IsLemma("when")
			// ����� "�����" ����� ��������� � ���� opener � � ���� ��������  �����
			// ������� �� opener  �� ��� ���������
		  && !E.IsLeftClauseTop(i)
		  	// ���� ������ ������� ������ ��  ������ opener, 
			// ����� ����� opener ����� �� ������� openerom
		 )
	  {
		  /*
		   ���� ���� ����������� �������, ����� �� ����� ������� ��� opener, ��������:
		   "��� ��� �� �� ����" -> "He is still singing"
		  */
		  vector<long> Rels;
		  E.GetIncomingRelations(i, Rels);
		  if (Rels.size() == 1)
			  if (E.m_Relations[Rels[0]].m_Position != "")
				  continue;
		  clause_openers[E.m_Nodes[i].m_ClauseNo] = i;
		  Res(i).do_not_put = true;
	  };

}




void CEngSynthes::handle_rel_operators(int node, bool with_no)
{
	int in_rel = get_in_rel(node);
	int res_node = node;
	if(	(node_is_pronoun(node) || FieldContainsValue(Node(node), "GF", "PN_ADJ"))
		&& in_rel != -1 		
		&& Res(Rel(in_rel).m_SourceNodeNo).has_prep()
		)
		res_node = Rel(in_rel).m_SourceNodeNo;

	if(with_no)
	if(HasRelOperator(node, "��"))		Res(res_node).rel_operator += "not ";

	if(HasRelOperator(node, "����"))	Res(res_node).rel_operator += "even ";
	if(HasRelOperator(node, "������"))
		Res(res_node).rel_operator += "only ";
}

void CEngSynthes::handle_other_relations(int node)
{
	vector<long> out_rels, rels;
	get_out_rels(node, out_rels);

	int i;
	for(i = 0; i < out_rels.size(); i++)
	{
		int rel = out_rels[i];
		SynthesResult &res = Res(Rel(rel).m_TargetNodeNo);
		
		if(res.do_not_put) continue;
if(   ValencyIs(rel, "IDENT")||ValencyIs(rel, "NAME")		  )
		res.order(">>", 0);// ����� �����
	}

}


bool CEngSynthes::is_time_or_loc_node_and_first_in_clause(int node)
{
	bool is_time = Node(node).IsTimeRossNode() || Node(node).GetType() == TimeRoss  
|| (node_is_adv(Node(node)) && E.HasSemFet(Node(node), "TIME") ) ;
	return is_time && E.IsTheVeryLeftNodeOfClause(node);
}

bool CEngSynthes::try_conj_disrupt(int node_no)
{
	const CEngSemNode &node = Node(node_no);
	if(!FieldContainsValue(node, "GF", "CONJ:DISRUPT")) return false; 
	assert(node.GetType() == EngObor);
	string tr = E.m_pData->GetEngOborStr(node.GetUnitNo());
	size_t elpsis = tr.find("...");
	assert(elpsis != string::npos);
	vector<long>  rels;
	get_out_rels(node_no, rels);
	assert(rels.size() == 2);
	sort(rels.begin(), rels.end(), rel_pos_less(*this));
	int rel1 = rels[0];
	int rel2 = rels[1];
	Res(node_no).before_prep = tr.substr(0, elpsis);
	// ����� ������ ������ �������� ����� ������ �������� �������
	Res(Rel(rel2).m_TargetNodeNo).before_prep = ","+tr.substr(elpsis + strlen("..."));

	get_out_rels(node_no, rels);
	for(int i = 0; i < rels.size(); i++){
		translate_node(E.m_Relations[rels[i]].m_TargetNodeNo);
	}	
	return true;
}



