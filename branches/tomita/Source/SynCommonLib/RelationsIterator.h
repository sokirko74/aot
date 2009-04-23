// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef __RELATIONSITERATOR_H_
#define __RELATIONSITERATOR_H_

#include "Sentence.h"

struct CSynOutputRelation
{
	CRelation	m_Relation;
	int m_iSourceGroup;
	int m_iTargetGroup;
	const CSyntaxOpt* m_pSyntaxOptions;

	const CSyntaxOpt* GetOpt() const;
	bool Init(const CRelation& pRel,int iSourceGroup, int iTargetGroup, const CSyntaxOpt* pSyntaxOptions);
	void CopyFrom(const CSynOutputRelation& X);

};


class CRelationsIterator
{
protected:
	const CSentence*		m_pSent;
	map<int,CSVI>			m_mapClauseNo2SynVar;
	vector<CSynOutputRelation>	m_vectorIRelations;
	vector<CGroup>			m_vectorGroups;

	const CSyntaxOpt* GetOpt() const;
	int		FindFirmGroupClauseRelations(int iClauseNum, CSVI pSynVar, int iRelMember);
	int		FindRelPeriodInGroup(const CPeriod& rel, const CGroup& group) const;
	int     TryToFindDisruptConjGroup(CSVI pSynVar, const CRelation& rel, int iWord );
	bool	BuildRelationsInClause(int iClauseNum, CSVI pSynVar);
	bool	BuildRelationsOfGroups(const CClause& clause, CSVI pSynVar);
	void	BuildSubjRel(const CMorphVariant& SynVar, int iClauseNo);
	int		FindFirmGroup(const CGroup& group) const;
	void	ResetAll();

public:
	void								SetSentence(const CSentence* pSent);
	const vector<CGroup>&				GetFirmGroups() const;
	const vector<CSynOutputRelation>&	GetRelations() const;

	bool		BuildRelations();
	void		AddClauseNoAndVariantNo(long iClauseNo, long iVarNo);

};

#endif //__RELATIONSITERATOR_H_
