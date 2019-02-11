/*

 ���������� ��������� l:\documents\�������.doc

*/
#include "StdAfx.h"


void update_article(const CSemPattern& P, CEngSemNode& N)
{
	try{
		for (long i=0; i < P.m_ArticleCorteges.size(); i++)
		{
			const TCortege& C = P.m_ArticleCorteges[i];
			string ArticleStr = (const char*)P.m_pRossDoc->GetRoss()->GetDomItemStr(C.m_DomItemNos[0]);
		    if (ArticleStr != "")
			{
              if (ArticleStr == "the|NOUN")
			  {
				  if (N.HasPOS(eNOUN))
					N.m_ArticleStr = "the";
			  }
			  else
               if (ArticleStr == "a|NOUN")
			   {
				   if (N.HasPOS(eNOUN))
						N.m_ArticleStr = "a";
			   }
               else
				  N.m_ArticleStr =  ArticleStr;

			   N.m_ArticleCauseHistory.push_back(ArticleFromDict);
			};
		};

	}catch(...){
		assert(false);
	}
}

void CEngSemStructure::GetOutcomingInClauseRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<GetRelationsSize(); i++)
  {
     if  (   (GetRelation(i)->m_SourceNodeNo == NodeNo)
		  && !IsRelBetweenClauses(*GetRelation(i))
		 )
	    Relations.push_back(i);

	 // if there is a MUA-node, to which NodeNo belongs, then we should collect all named valencies from 
	 // this MUA-node.
	 if		(			(GetRelation(i)->m_TargetNodeNo == NodeNo)
				&&		(GetRelation(i)->m_Valency.m_RelationStr.empty())
				&&		(m_Nodes[GetRelation(i)->m_SourceNodeNo].m_NodeType == MNA)
			)
	 {
		 for (size_t k = 0; k<GetRelationsSize(); k++)
			if  (			(GetRelation(k)->m_SourceNodeNo == GetRelation(i)->m_SourceNodeNo)
					&&	!IsRelBetweenClauses(*GetRelation(k))
					&&	!GetRelation(k)->m_Valency.m_RelationStr.empty()
				)
			{
				Relations.push_back(k);
			 }
	}
  }
};



bool CEngSemStructure::HasOutRelationByName (long NodeNo, string RelationStr) const
{
	vector<long> Rels;
	GetOutcomingInClauseRelations(NodeNo, Rels);
	for (long j =0; j < Rels.size();j++)
	  if (m_Relations[Rels[j]].m_Valency.m_RelationStr == RelationStr)
		  return true;

    return false;
};


bool CEngSemStructure::CheckAllOutcomingRelationsIfAnyExists (long NodeNo, string RelationStr) const
{
	vector<long> Rels;
	GetOutcomingInClauseRelations(NodeNo, Rels);
	for (long j =0; j < Rels.size();j++)
	  if (m_Relations[Rels[j]].m_Valency.m_RelationStr != RelationStr)
		  return false;

    return true;
};



bool CEngSemStructure::HasOutRelationByGrammems(long NodeNo, QWORD Grammems) const
{
	vector<long> rels;
	GetOutcomingRelations(NodeNo,rels);
	for(int i = 0; i < rels.size(); i++)
	{
		const CEngSemRelation &rel = m_Relations[rels[i]];
		if (   m_Nodes[rel.m_TargetNodeNo].GetGrammemsRich() & Grammems )
			return true;
	}
	return false;
}

int CEngSemStructure::GetOutRelationByWord(long NodeNo, string Word) const
{
	vector<long> rels;
	GetOutcomingRelations(NodeNo,rels);
	for(int i = 0; i < rels.size(); i++)
	{
		const CEngSemRelation &rel = m_Relations[rels[i]];
		if ( m_Nodes[rel.m_TargetNodeNo].IsPrimitive() )
		if (   m_Nodes[rel.m_TargetNodeNo].m_Words[0].m_Word == Word)
			return rels[i];
	}
	return -1;
}


bool CEngSemStructure::HasOutRelationByPoses(long NodeNo, poses_mask_t  Poses) const
{
	vector<long> rels;
	GetOutcomingRelations(NodeNo,rels);
	for(int i = 0; i < rels.size(); i++)
	{
		const CEngSemRelation &rel = m_Relations[rels[i]];
		if  ( ( (1<<m_Nodes[rel.m_TargetNodeNo].GetPos()) & Poses ) > 0 )
			return true;
		// ���� ���� MUA ������� "����������"		
		if (m_Nodes[rel.m_TargetNodeNo].m_NodeType == MNA)
		{
				vector<long> rels1;
				GetOutcomingRelations(rel.m_TargetNodeNo,rels1);
				for(int j = 0; j < rels1.size(); j++)
				{
					const CEngSemRelation &rel = m_Relations[rels1[i]];
					if  ( ( (1<<m_Nodes[rel.m_TargetNodeNo].GetPos()) & Poses ) > 0 )
						return true;
				};
		};
	}
	return false;
}

void  CEngSemStructure::InitArticleField ()
{
	vector<bool> ReverseRelArticles;
	ReverseRelArticles.resize(m_Nodes.size());
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	{
		if (m_Nodes[NodeNo].m_bProper) continue;
		// 1. �������� ���������
		vector<long> Rels;
		GetIncomingInClauseRelations(NodeNo, Rels);
		long InRelNo = -1;
		if (Rels.size() == 1)
		{
			/*��� ����� "�� ��� �������� � ����������"*/
			if (m_Nodes[m_Relations[Rels[0]].m_SourceNodeNo].m_NodeType == MNA)
			{
				GetIncomingInClauseRelations(m_Relations[Rels[0]].m_SourceNodeNo, Rels);
			}
			if (Rels.size() == 1)
			InRelNo = Rels[0];
		};

		if (InRelNo != -1)
			update_article(m_Relations[InRelNo].m_Pattern, m_Nodes[NodeNo]);

		// 2. ��������� ����������� ���������
		ReverseRelArticles[NodeNo] = m_Nodes[NodeNo].m_ArticleStr == "";

		/*ReverseRelArticle - ������, ����  �����-������ ���������� ����� �������
		 ������ �������. ��� ����� ������� ��������. ����� ���� ���� �������...
		 ��������, � ������� ����������� ������ �����������, ��� ����
		 ���� ����. ����, ����� ����� ������ �������.
		*/

		GetOutcomingInClauseRelations(NodeNo, Rels);
		for (long j =0; j < Rels.size();j++)
		 if (m_Relations[Rels[j]].m_bReverseRel)
		 {
			 update_article(m_Relations[Rels[j]].m_Pattern, m_Nodes[NodeNo]);
		 };
		ReverseRelArticles[NodeNo] = m_Nodes[NodeNo].m_ArticleStr != "";
		 
		// �������� ���� DETERM �� ���������� ������

		vector<long> Articles;
		FindAbstractAdditionArticle (Aoss, m_Nodes[NodeNo], Articles, false, -1);
		
		for (int i =0; i<Articles.size(); i++)
		{
			const vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(Aoss);
			CSemPattern P;
			P.InitSemPattern(GetRossHolder(Aoss), AbstractArticles[Articles[i]].m_UnitNo,0,0);
			P.LoadGramFromDict();
			update_article(P, m_Nodes[NodeNo]);
		};
		


		// 3. ����������� �������� ���� DETERM		
		if (m_Nodes[NodeNo].GetUnitNo() !=  ErrUnitNo)
		{
			CSemPattern P;
			P.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),0,0);
			P.LoadGramFromDict();
			update_article(P, m_Nodes[NodeNo]);
		};

		// 4. ��������� ����������� ��������� (�����������)
		for (int j =0; j < Rels.size();j++)
		 if (m_Relations[Rels[j]].m_bReverseRel)
			 if (m_Nodes[m_Relations[Rels[j]].m_TargetNodeNo].HasPOS(ePN_ADJ) )
				update_article(m_Relations[Rels[j]].m_Pattern, m_Nodes[NodeNo]);
	}

	// ���� ������� ��k ������� �� ��������������, ����� ��� ����� ���������� �� 
	// ���������������, �� �������� ��� �������������� �������
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
		if (   m_Nodes[NodeNo].HasPOS(ePN_ADJ) 
			|| m_Nodes[NodeNo].HasPOS(eADJ) 		
		  )
		if (m_Nodes[NodeNo].m_ArticleStr != "")
	{
		vector<long> InRels;
		GetIncomingInClauseRelations(NodeNo, InRels);
		long InRelNo = -1;
		if (InRels.size() == 1)
			InRelNo = InRels[0];

		if (InRelNo != -1)
		  if (m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].HasPOS(eNOUN))
		  {
			  m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].m_ArticleStr =  m_Nodes[NodeNo].m_ArticleStr;
			  m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].m_ArticleCauseHistory.push_back( m_Nodes[NodeNo].m_ArticleCauseHistory.back() );
		  };
	};


	PrintNodes();
 	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	{
		vector<long> InRels;
		GetIncomingInClauseRelations(NodeNo, InRels);
		long InRelNo = -1;
		if (InRels.size() == 1)
			InRelNo = InRels[0];

		//=================== zero article =================
        if (m_Nodes[NodeNo].m_ArticleStr == "-") continue;

		// IDENT, NAME ��� ���� ����� � ��������
		if (		HasOutRelationByName(NodeNo, "IDENT")|| HasOutRelationByName(NodeNo, "NAME")			
				|| (    m_Nodes[NodeNo].m_bQuoteMarks
					&&	(		(NodeNo+1 == m_Nodes.size())
						 ||		!m_Nodes[NodeNo+1].m_bQuoteMarks
						)
					&&	(		(NodeNo == 0)
							 ||	!m_Nodes[NodeNo-1].m_bQuoteMarks
						)
					)
		   )
		{
		  m_Nodes[NodeNo].m_ArticleStr = "-";
		  m_Nodes[NodeNo].m_ArticleCauseHistory.push_back(  ZeroArticleForProperNames );
		  
		  continue;
		};

		 if (InRelNo != -1)
			if	(		(m_Relations[InRelNo].m_Valency.m_RelationStr == "IDENT")
					||	(m_Relations[InRelNo].m_Valency.m_RelationStr == "NAME")			   
				)
			{
				m_Nodes[NodeNo].m_ArticleStr = "-";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( ZeroArticleForProperNames );
				continue;
			};

		// prop ��� geo 
		if  (		m_Nodes[NodeNo].HasGrammemRich(eGeographics) 
				||	m_Nodes[NodeNo].HasGrammemRich(eProper) 
			)
			{
				m_Nodes[NodeNo].m_ArticleStr = "-";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( ZeroArticleForProperNames );
				continue;
			};

		// prop ��� geo 
		if  (m_Nodes[NodeNo].m_bProper)
			{
				m_Nodes[NodeNo].m_ArticleStr = "-";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( ZeroArticleForProperNames );
				continue;
			};

		if (   HasOutRelationByGrammems(NodeNo, _QM(ePossessive))
			|| ( GetOutRelationByWord(NodeNo, "#one's") != -1)
		   )
			{
				m_Nodes[NodeNo].m_ArticleStr = "-";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( ZeroArticleBecauseOfPossessive );
				continue;
			};

		

		//=================== "the" article =================
		if (m_Nodes[NodeNo].m_ArticleStr == "the") continue;
		/*
		 ����� �������, ��� ����-������� �� ���� 
		 ����������� �������, ����  � ������������ ����� ��� 
		 � ��, ��������, � ������ "�t first" ��������������� the
		 ��-�� �����������
		*/
		if (m_Nodes[NodeNo].IsMainTimeRossNode()) continue;

		/*
		 �� ����� ������� �������, ���� ��������������� ����� � 
		 �������������� ������. ������ ������, ��� �������, 
         ��������, "a dog's life" ��� "the king's daughter",
		 �� �����, ��-�����, �� �������
		 �������
		*/
		if (m_Nodes[NodeNo].HasGrammemRich(ePossessive)) continue;

		// eORDNUM
		vector<long> OutRels;
		GetOutcomingInClauseRelations(NodeNo, OutRels);
		long j =0;
		for (; j < OutRels.size();j++)
		  if ( m_Nodes[m_Relations[OutRels[j]].m_TargetNodeNo].HasPOS(eORDNUM) )
			  break;

		if (j < OutRels.size())
		{
		  m_Nodes[NodeNo].m_ArticleStr = "the";
		  m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( ArticleFromOrdNum );
		  continue;
		};

		/*
			���� �� ���� �������� �� ������� � ����������� ������, 
			��������, � ����. �����������
		*/
		if (!ReverseRelArticles[NodeNo])
		{
			// ���� - ����
			vector<long> Roots;
			GetClauseRoots(m_Nodes[NodeNo].m_ClauseNo, Roots);

			if (   (Roots.size() == 1)
				&& (m_Nodes[NodeNo].IsWordContainer())
				&& (m_Nodes[Roots[0]].IsWordContainer())
				&& (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[Roots[0]].GetMinWordNo())
				&& (m_Nodes[NodeNo].GetPos() == eNOUN)
				&& (m_Nodes[NodeNo].GetType()  != TimeRoss)
				 // ��������� �������� �������, ����� �� �������� ������� ����� ����-�������
				 // "� 2000 ���� �������� ���������"
			   )
			  {
				  m_Nodes[NodeNo].m_ArticleStr = "the";
				  m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleBeforeClausePredicate );
				  continue;
			  };

			// SF = CONTNR, VAL = TOGETHER
			if  (    HasSemFetPro (m_Nodes[NodeNo],"CONTNR")&& HasOutRelationByName(NodeNo, "TOGETHER")				 )
			 {
			  m_Nodes[NodeNo].m_ArticleStr = "the";
			  m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleBecauseOfNominalSupplement );
			  continue;
			 };
		

// TRG-PNT ��� SRC-PNT		 
			if (InRelNo != -1)
			if (		(m_Relations[InRelNo].m_Valency.m_RelationStr == "TRG-PNT")
					||	(m_Relations[InRelNo].m_Valency.m_RelationStr == "SRC-PNT")			   
				)
			{
				m_Nodes[NodeNo].m_ArticleStr = "the";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleForAbstractLocal );
				continue;
			};

         /*
		  ����� ������� ���� ��� � �������� 
	      ����, ����� ����� ���������� ������, ������� ������� ������ �� ������ �����,
		  ������������ � ����������� ��������
          �������� "� ����", "� ����"
          �������, ����� ����� "� ���������" "� �������" 
		  ����� ������������ ��� "in the trouble", "in the fear",
		������� ����� ����� ������ LABL.OBJ.		  */
		 if (InRelNo != -1)
			if (		(m_Relations[InRelNo].m_Valency.m_RelationStr == "LOK")
					&&	CheckAllOutcomingRelationsIfAnyExists(NodeNo, "SPECIF")
					&&	(m_Nodes[NodeNo].RusNode != -1)
					&&	(RusStr.GetNode(m_Nodes[NodeNo].RusNode).m_SemCategory == scObject)
			   )
			{
				m_Nodes[NodeNo].m_ArticleStr = "the";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleForAbstractLocal );
				continue;
			};

		};

		 // �����_��
		for (j =0; j < OutRels.size();j++)
		  if ( m_Relations[OutRels[j]].m_SyntacticRelation == "�����_��" )
		  {
			  m_Nodes[NodeNo].m_ArticleStr = "the";
			  m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleBecauseOfNominalSupplement );
			  break;
		  };
		if (j < OutRels.size()) continue;


	
		
		// NP � ��������� of
		PrintNodes();
		WORD UnitNo = GetRoss(EngObor)->LocateUnit("of",1);
		vector<long> OutAllRels;
		GetOutcomingRelations(NodeNo, OutAllRels, false);
  		for (j =0; j < OutAllRels.size();j++)
		{
		  if(m_Relations[OutAllRels[j]].m_SynReal.HasThePrep(UnitNo))
			  break;
		}
		if (j < OutAllRels.size())
		{
			m_Nodes[NodeNo].m_ArticleStr = "the";
			m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleBecauseOfNominalSupplement );
			continue;
		};

		

		//=================== "a" article =================
		/* 
		   ���� ���� �� ������� ������� ������� "a", a  �������� ����u���� uncount, 
		   mass ��� pl, ����� ������� ������� �������
		*/
		if (m_Nodes[NodeNo].GetGrammemsRich() & 
			   (  _QM(eMass)| _QM(eUncountable) | _QM(ePlural) ) ) 
		{
			m_Nodes[NodeNo].m_ArticleStr = "";
			m_Nodes[NodeNo].m_ArticleCauseHistory.push_back(  NoIndefArticleForMassNouns );
			continue;
		};

		if (m_Nodes[NodeNo].m_ArticleStr == "a") continue;

		long i = InRelNo;
		// ������ ������ ����������������
		
		if (m_Nodes[NodeNo].GetPos() == eNOUN)
		while (i != -1)
			{
				if (IsSubj(m_Relations[i])) break;

			if (HasSemFetPro(m_Nodes[m_Relations[i].m_SourceNodeNo],"MODL"))				{
					m_Nodes[NodeNo].m_ArticleStr = "a";
					break;
				};
				vector<long> Rels;
				GetIncomingInClauseRelations(m_Relations[i].m_SourceNodeNo, Rels);
				if (Rels.size() == 1) 
					i = Rels[0];
				else
					i = -1;
			}

	 /*
	  ����� as ���� ������� �������������� �������
	  Internet datagram as an independent entity
	 */
	 if (InRelNo != -1)
			if (     
				     (m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].m_NodeType  == MNA)
				  && m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].IsLemma("as")
				  && m_Nodes[NodeNo].GetMinWordNo() > m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].GetMinWordNo() 
			   )
			{
				m_Nodes[NodeNo].m_ArticleStr = "a";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( IndefArticleAfterAs );
				continue;
			};



	   if (i!= -1)
			continue;
		// ��������� ������� � ������� "if" ���� �� �����������������


	   /* ���  ���������,� ������ �� ��������� �����������, ����� ����� ������������ �������*/
			if  (HasSemFetPro(m_Nodes[NodeNo].m_NodeSemFets, "PROF"))			
			{
				m_Nodes[NodeNo].m_ArticleStr = "the";
				m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleForSingleRanks );
				continue;
			};

			   
		/*
		   ����, ����� ����� NP, ������� ��������� ��������������� 
		   �����������, �������� ������� the. ��� ������� � ����� ������ �����������, ������,
		   ����� ���� ���-������ ������.
	   */
		GetIncomingRelations(NodeNo, InRels, false);
  		for (j =0; j < InRels.size();j++)
		{
		  if (m_Nodes[m_Relations[InRels[j]].m_SourceNodeNo].m_ClauseNo != m_Nodes[m_Relations[InRels[j]].m_TargetNodeNo].m_ClauseNo)
			 if(m_Relations[InRels[j]].m_Type == Kotoryj)
				 break;
		}
		if (j < InRels.size())
		{
			m_Nodes[NodeNo].m_ArticleStr = "the";
			m_Nodes[NodeNo].m_ArticleCauseHistory.push_back( DefArticleBecauseDefiniteClause );
			continue;
		};


	};


};
