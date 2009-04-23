#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "StdAfx.h"
#include "EngSynthes.h"
#include "EngVerbTense.h"



bool CEngSynthes::try_noun_node(int node_no)
{
	if(Res(node_no).do_not_put) return true; // for parts of termin
	int i;
	vector<long> rels;
	get_out_rels(node_no, rels);
	int parent_rel = get_in_rel(node_no);
	if (    (parent_rel != -1)
		 && (E.m_Nodes[Rel(parent_rel).m_SourceNodeNo].m_NodeType == MNA)
	   )
	   parent_rel = get_in_rel(Rel(parent_rel).m_SourceNodeNo);

   int ParentNodeNo = -1;
   if(parent_rel != -1)		ParentNodeNo = Rel(parent_rel).m_SourceNodeNo;
		


	if(      !node_is_noun(node_no)
	   &&	(E.m_Nodes[node_no].GetTense() != gerund_tn)
	  )
	{
		return false;
	}

	
	bool is_plural = E.has_plural_rel(node_no);
		
	
	int main_w = Node(node_no).m_MainWordNo;
	if( main_w != -1 )
	{
		if (E.m_Nodes[node_no].GetTense() == gerund_tn)
		{
			CEngVerbTense EngVerbTense(*this, node_no);
			Res(node_no).subject_node = -1;
			EngVerbTense.make_verb_form();
		}
		else
		{
			if (is_plural)
			{
				E.m_Nodes[node_no].DeleteGrammemsRich(eAllNumbers);
				E.m_Nodes[node_no].AddOneGrammemRich(ePlural);
			};
			helper.synthesize_by_node(E.m_Nodes[node_no]);
			Res(node_no).m_EngParadigmId = Node(node_no).m_Words[main_w].m_ParadigmId;
			Res(node_no).m_WordForms.push_back(Node(node_no).m_Words[main_w].m_Word);
		};
	}



	sort(rels.begin(), rels.end(), rel_pos_less(*this));

	vector<bool> ordered_rels(rels.size(), false);
	


	int curent_pos = 0;
	bool no_animative_gen_noun = false;

	// "все пять собак" -> "all five dogs"
	// "все мои  собаки" -> "all my dogs"
	// "мои все  собаки" -> "all my dogs"

	//  first put articles-stubs ("no", "all")
	for(i = 0; i < rels.size(); i++)
		if(     E.m_Nodes[Rel(rels[i]).m_TargetNodeNo].IsLemma("no")
			||  E.m_Nodes[Rel(rels[i]).m_TargetNodeNo].IsLemma("all")
		  )
		{
			ordered_rels[i] = true;
			Res(Rel(rels[i]).m_TargetNodeNo).order("<<", curent_pos++);
		}

	// then put numerals
	for(i = 0; i < rels.size(); i++){
		if(SyntaxRelIs(rels[i], "ЧИСЛ_СУЩ") || SyntaxRelIs(rels[i], "НАР_ЧИСЛ_СУЩ")){
			ordered_rels[i] = true;
			int sub = Rel(rels[i]).m_TargetNodeNo;
			Res(sub).order("<<", curent_pos++);
			if(FieldContainsValue(Node(sub), "POS", "NA"))
				Res(sub).m_Position = ">>";
			no_animative_gen_noun = true;
		}
	}

	
	// then put pronouns
	for(i = 0; i < rels.size(); i++)
	if (!ordered_rels[i]) // "all" - является PN_ADJ
	{
		if(E.m_Nodes[E.m_Relations[rels[i]].m_TargetNodeNo].GetPos() == ePN_ADJ)
		{
			ordered_rels[i] = true;
			int sub = Rel(rels[i]).m_TargetNodeNo;
			Res(sub).order("<<", curent_pos++);
			if(FieldContainsValue(Node(sub), "POS", "NA"))
				Res(sub).m_Position = ">>";
			no_animative_gen_noun = true;
		}
	}

	// others
	for(i = 0; i < rels.size(); i++){
		if(ordered_rels[i]) continue;
		// GEN_NOUN
		int rel = rels[i];
		int sub_node = Rel(rel).m_TargetNodeNo;
		if(SyntaxRelIs(rel, "ПРИЛ_СУЩ")){

			Res(sub_node).order("<<", curent_pos++);

			if(FieldContainsValue(Node(sub_node), "POS", "NA"))
				Res(sub_node).m_Position = ">>";

		}
	}


	Res(node_no).m_Grammems = is_plural ? _QM(ePlural) : _QM(eSingular);

	// sons are here
	for(i = 0; i < rels.size(); i++){
		translate_node(Rel(rels[i]).m_TargetNodeNo);
	}	


	

	string article = E.m_Nodes[node_no].m_ArticleStr;
	if(article == "-")
		Res(node_no).m_Article = ZeroArticle;
	else if(article == "a")
		Res(node_no).m_Article = IndefArticle;
	else if(article == "the")
		Res(node_no).m_Article = DefArticle;
		

 	handle_colloc_words(node_no, Res(node_no));


	/*
	  для герундия функция handle_rel_operators уже была вызвана 
	*/
    if (E.m_Nodes[node_no].GetTense() != gerund_tn)
	handle_rel_operators(node_no);
	return true;
}




bool CEngSynthes::try_adj_node(int node_no)
{
	if(!_node_is_adj (E.m_Nodes[node_no])) return false;
	vector<long> rels;
	get_out_rels(node_no, rels);
	sort(rels.begin(), rels.end(), rel_pos_less(*this));


	if (Node(node_no).m_MainWordNo != -1)
	{
		CEngSemWord& e_word = Node(node_no).m_Words[Node(node_no).m_MainWordNo];
		if(e_word.HasOneGrammem(eSupremum))
		{ 
			Res(node_no).m_Article = DefArticle;
		};
		int parent_rel = get_in_rel(node_no);
		translate_option_t main_option;
		if(parent_rel != -1){
			int parent = Rel(parent_rel).m_SourceNodeNo;
			long id = Res(parent).m_EngParadigmId;
			if(id != -1){
				main_option.dual_freq_dict = &helper.m_pData->m_AdjNounDualFreq;
				main_option.dual_id1 = -1; // ADJ
				main_option.dual_id2 = id; // NOUN id
			}
		}
		
		helper.synthesize_by_node(E.m_Nodes[node_no]);

		if (Node(node_no).HasRelOperator ("AntiMagn"))
			e_word.m_Word =   "the least "+ e_word.m_Word;


		Res(node_no).m_WordForms.push_back(e_word.m_Word);

	}


	// sons are here
	for(long i = 0; i < rels.size(); i++){
		translate_node(Rel(rels[i]).m_TargetNodeNo);
	}	


	handle_colloc_words(node_no, Res(node_no));
	handle_rel_operators(node_no);
	return true;
}

// "this", "that", "one"
// Эти местоимения имеют множественную форму ("these", "those", "ones")
bool CEngSynthes::lemma_is_demonstrative_pronoun (string Lemma) const
{
  try
  {
	vector<CFormInfo> ParadigmCollection; 
	E.m_pData->GetEngLemmatizer()->CreateParadigmCollection(true, Lemma, false, false, ParadigmCollection);

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		string AnCode = ParadigmCollection[i].GetAncode(0);

		if( E.m_pData->GetEngGramTab()->GetPartOfSpeech(AnCode.c_str()) == ePN_ADJ) 
		{
			QWORD g;
			bool bRes = E.m_pData->GetEngGramTab()->GetGrammems(AnCode.c_str(), g);
			assert (bRes);
			if( g & _QM(eDemonstrativePronoun)) 
			     return  true;
		}
	}
  }
  catch(...)
  {
  }
	return false;
};


bool CEngSynthes::try_pronoun_node(int node_no)
{
	if(E.m_Nodes[node_no].IsAbstract() || !node_is_pronoun(node_no)) return false;
	if(E.m_Nodes[node_no].m_Words.size() != 1) return false;
	CEngSemWord &EngWord = E.m_Nodes[node_no].m_Words[0];


	bool is_norm  = false;
	int rel = get_in_rel(node_no);
	if (rel != -1)
	  if (E.m_Nodes[E.m_Relations[rel].m_SourceNodeNo].m_NodeType == MNA)
		rel = get_in_rel(E.m_Relations[rel].m_SourceNodeNo);

	//  если местоимение - подлежащее или стоит в именительном падеже, то это номинатив,
	// иначе его нужно преобразовать в объектную форму.
	if (rel != -1 && is_subj_rel(rel))	is_norm = true;
	if (EngWord.HasOneGrammem(eNominative))	is_norm = true;
	
	
	if (lemma_is_demonstrative_pronoun (EngWord.m_Lemma))
	{
		long ParadigmId = helper.GetParadigmIdByLemma(morphEnglish, EngWord.m_Lemma,  ePN_ADJ);
		// создаем множественное число 
		if (ParadigmId != -1)
			if  (E.m_Nodes[node_no].GetGrammemsRich() & _QM(eSingular) )
		     EngWord.m_Word = helper.create_form_by_id(ParadigmId, _QM(eSingular));
			else
			 EngWord.m_Word = helper.create_form_by_id(ParadigmId, _QM(ePlural));
	}
	else
	if (!is_norm)
	{
		long ParadigmId = helper.GetParadigmIdByLemma(morphEnglish, EngWord.m_Lemma, ePN);
		// не у всех местоимений есть объектная форма
		if (ParadigmId != -1)
		 EngWord.m_Word = helper.create_form_by_id(ParadigmId, _QM(eObjectCase));
	};

	if ( EngWord.HasOneGrammem(ePossessive) )
		set_possessive(EngWord);

	if(EngWord.m_Word == "i") EngWord.m_Word= "I";


	Res(node_no).m_Article = ZeroArticle;
	Res(node_no).m_WordForms.push_back(EngWord.m_Word);

	// sons are here
	vector<long > rels;
	get_out_rels(node_no, rels);
	for(int i = 0; i < rels.size(); i++){
		translate_node(E.m_Relations[rels[i]].m_TargetNodeNo);
	}	

	handle_rel_operators(node_no);
	return true;
}

