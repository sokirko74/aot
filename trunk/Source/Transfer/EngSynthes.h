#pragma warning(disable:4786)
#pragma warning(disable:4503)
#ifndef INCL_ENGSYNTHES_H
#define INCL_ENGSYNTHES_H
#include "EngSemStructure.h"

struct CGrammemsAnalyzer {
	QWORD        m_Grammems;

	CGrammemsAnalyzer ();
	int			GetNumber () const;
	void		SetNumber (int num); 
	int			GetPerson () const;
  	int			GetGender () const;
};
    

enum ArticleEnum { ZeroArticle, DefArticle, IndefArticle};
struct SynthesResult  : public CGrammemsAnalyzer{
	// �����, ������� �����  �����������  �  ��� ������������������, � ������� ��� ����� �������
	StringVector m_WordForms;
    // �����, �������  ����� ��������� ����� ���� � ����� ���� ������� ����
	string prefix, postfix;
    // �����, �������  ����� ��������� ����� ���� ������� ����
	string before_prep;
    // �������, ������� ����� ��������� ����� ��� ������� ����
	ArticleEnum   m_Article;

    // ����� � �����������
	int		pos_order;
	string	m_Position;
	bool    m_bParenth;

	void order(string Position, int pos){
		m_Position = Position;
		pos_order = pos;
	}


	// ������, ����  ���� ���� �� ���� ������ � ������� ������
	bool			do_not_put; 
	// ParadigmId ���� �����, ������� ���������� � m_WordForms (assert (m_WordForms.sizE() == 1) )
	long			m_EngParadigmId;

	
	// �������, ������� ����� ��������� ����� ������
	string			m_prep;
	bool			has_prep() const  {return m_prep.length() > 0; }; 

    // ����������� ��������
	string			rel_operator;

    // ������ �� ����  �����������
	int				subject_node;

	bool is_passive_verb;

	int times_visited;


	SynthesResult()
		:pos_order(-1),
		m_Position(""),
		do_not_put(false),
		m_EngParadigmId(-1),
		subject_node(-1),
		times_visited(0),
		is_passive_verb(false),
		m_bParenth ( false )
	{}



};

enum CAdvPosType
{
		adv_degree,
		adv_freq,
		adv_manner,
		adv_place,
		adv_sent,
		adv_time,
		adv_quest,
		adv_neg,
		adv_other
};



struct NodeHelper{
	     CEngSemStructure &E;

	NodeHelper(CEngSemStructure& EngStr)
		:E(EngStr){}

	// ������ ��������� ��������� ��� �����������,  �������� ���� additional_rels
	void	get_out_rels(int node, vector<long> &res) const;
    // ������ �������t ��������� ��� ����������� 
	int		get_in_rel(int node);
    // ������ ��� ��������� �������� ���������
	void	get_out_clause_rel(int node, vector<long> &res);
	// ������ �������� �������� ���������
	int     get_in_clause_rel(int node);
	
	CEngSemNode&         Node(int node)				{return E.m_Nodes[node];}
	CEngSemRelation&     Rel(int rel)				{return E.m_Relations[rel];}
	const CEngSemNode&         Node(int node) const				{return E.m_Nodes[node];}
	const CEngSemRelation&     Rel(int rel)	const		{return E.m_Relations[rel];}
    // ���������, ��� � ���� ���� ��������� eng_poses,  � � ���
	// �������� ��������� ���� rus_poses
	bool node_has_poses(int node, UINT eng_poses) const;
	bool node_is_verb(int node_no) const;
	bool node_is_noun(int node_no) const;
	bool node_is_numeral(int node_no) const;	
	bool node_is_pronoun(int node_no) const;
	bool node_is_adj(int node_no) const;

	bool is_subj_rel(int rel) const	{return E.IsSubj(E.m_Relations[rel]);}
 
   // ���������, ��� � ����  field ���� ��������  value
	bool FieldContainsValue(const CRossHolder* RossHolder, WORD UnitNo, 
		const string &field, const string &value, int leaf = 0, int leaf2 = 0) const;
	// ���������, ��� � ��������� ������������� ���� � ����  field ���� ��������  value
	bool FieldContainsValue(const CEngSemNode& node, const string &field, const string &value, int leaf = 0, int leaf2 = 0) const;
    // ��� ���������� ������� �������� GetFieldValues
	void GetFieldValues(DictTypeEnum StructDict, WORD UnitNo, const string &field, 
		StringVector &res, int max_items = 10000) const;


	// ������, ���� ��������� �� ���� ����������
	bool rel_is_valency(int rel) const;
	bool ValencyIs(int rel, const char *str)	{return Rel(rel).m_Valency.m_RelationStr == str;}
	bool SyntaxRelIs(int rel, const char *str)	{return Rel(rel).m_SyntacticRelation == str;}

	bool HasRelOperator(int node, const char *op)	
	{
		return Node(node).HasRelOperator(op);

	}

	bool IsBetweenClause(long RelNo)  const
	{
		return E.m_Nodes[E.m_Relations[RelNo].m_SourceNodeNo].m_ClauseNo != E.m_Nodes[E.m_Relations[RelNo].m_TargetNodeNo].m_ClauseNo;
	};


};


struct CClauseRootToConjWord {
	long m_RootNodeNo;
	long m_ConjWordNodeNo;

	CClauseRootToConjWord() {
		m_RootNodeNo = -1;
		m_ConjWordNodeNo = -1;
	};
	CClauseRootToConjWord(long RootNodeNo, long ConjWordNodeNo) 
	{
		m_RootNodeNo = RootNodeNo;
		m_ConjWordNodeNo = ConjWordNodeNo;
	};
};


struct CClauseConj
{
	string m_WordStr;
	string m_UnitStr;
	CClauseConj () {};
	CClauseConj (string WordStr, 	string UnitStr)
	{
		m_WordStr = WordStr;
		m_UnitStr = UnitStr;
	};

};
class CEngSynthes: public NodeHelper{
public:
	CEngSynthes(CEngSemStructure& EngStr);
	string BuildSentence();
private:


	// ������ �� �������� ���� ������ � �������� ����� ������
	// ��������, I know where to live (clause_conj_words(live)==where )
	// ������� ����� ���������� �� ������ ���, ��� ��������� �� �������� ��������� �����  � ���������
	vector<CClauseRootToConjWord> clause_conj_words; 
	// �� ������ ������ �������� ����� ����, ������� �������� opener�� � ���� ������
	map<int, int> clause_openers;
	// �� ������ ������ �������� ����� ����, ������� �������� ������ � ���� ������
	map<int, CClauseConj> clause_conj;
	// �� ���� � �������� ������ �����, ������� ����������� ��� ������ ������, ��
	// ������ ������ ����� ���� �
	multimap<int, pair<int, bool> > node_sub_clause_put_after;
	

	void find_all_clause_connectors();

	void detect_clause_openers();


	friend class CEngVerbTense;
	translate_helper helper;

	vector<SynthesResult> result_vec;
	SynthesResult& Res(int node)	{return result_vec[node];}

	void translate_node(int node);


	bool try_noun_node(int node);
	bool try_adj_node(int node);
	bool try_default_node(int node);


	bool try_oneself_node(int node);
	bool try_simple_group(int node);
	bool try_verb_node(int node_no);
	bool try_numeral_node(int node_no);
	bool try_mna_node(int node_no);
	bool try_pronoun_node(int node_no);
	

	bool try_conj_disrupt(int node_no);

	void handle_colloc_words(int node_no, SynthesResult &res);
	



	void handle_prep(int node_no);
	int rus_node(int node){return Node(node).RusNode;}


	string collect_results(int node);

	// ����������� ������ - ��� ������, � ���� �� ����� ������� ������ �����������  �������
	bool		IsSlaveClause(long ClauseNo) const;
	// ������� ������� ��� �������� ��������� ������
	void		find_position_for_slave_clause_components();
	

	

	bool		lemma_is_demonstrative_pronoun (string Lemma) const;



public:
	CAdvPosType get_adv_pos_kind(int node_no);
	bool IsInfinitivePatternWithoutTo (long NodeNo) const;
	bool m_bConnected;

private:




	int get_conj_word(int parent);
	bool is_some_conj_word(int conj_word);
	bool is_conj_word(int parent, int son);
	bool is_clause_opener(int ClauseNo, int OpenerNodeNo);
	bool dir_obj_is_long(int node_no);
	bool dir_obj_has_prep(int node_no);
	
	string translate_son(int node_no);	
	

	void handle_rel_operators(int node, bool with_no = true);

	void handle_other_relations(int node);
	
	bool is_time_or_loc_node_and_first_in_clause(int node);
public:
	int compare_node_positions_by_min_word_no(int node1, int node2) const; 
	bool HasInfinitiveInTree(int node_no) const;

	friend struct rel_pos_less;
	friend struct ordered_rel_pos_less;

};





	struct rel_pos_less
	{
		const CEngSynthes &S;

		rel_pos_less(const CEngSynthes &inS): S(inS)
		{

		}

		bool operator ()(int r1, int r2)const{
			return S.compare_node_positions_by_min_word_no(S.E.m_Relations[r1].m_TargetNodeNo, S.E.m_Relations[r2].m_TargetNodeNo) < 0;
		}
	};

	struct ordered_rel_pos_less{
		const CEngSynthes &S;
		ordered_rel_pos_less(const CEngSynthes &inS): S(inS){}
		bool operator ()(int r1, int r2)const;
	};

extern long ValuePosition (string Position);
extern string  GetDualPosition (string Position);

#endif // INCL_ENGSYNTHES_H

