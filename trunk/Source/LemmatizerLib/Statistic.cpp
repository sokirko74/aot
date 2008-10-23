// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "StdMorph.h"
#include "Statistic.h"
#include "../common/bserialize.h"


#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW

	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif


void	CStatistic::Load(const string& path)
{
	ReadVector(path + HOMOWEIGHT_BIN_PATH, m_HomoWeights);
	ReadVector(path + WORDWEIGHT_BIN_PATH, m_WordWeights);
}




static bool less4homonode(const _homonode_t& x, const _homonode_t& y) 
{	
		return (	x.first == y.first
				?		x.second < y.second	
					:	x.first < y.first);	
};


int		CStatistic::get_HomoWeight(int paradigmid, int form) const
{
	_homonode_t findnode;
	findnode.first = paradigmid;
	findnode.second = form;
	vector<_homonode_t>::const_iterator	presult = lower_bound(m_HomoWeights.begin(), m_HomoWeights.end(), findnode, less4homonode);
	if (	(presult != m_HomoWeights.end())
		&&	(presult->first == findnode.first)
		&&	(presult->second == findnode.second)
		)
		return presult->third;
	return 0;
}


static bool less4statnode(const _statnode_t& x, const _statnode_t& y) 
{
	return (x.first < y.first);	
}


int		CStatistic::get_WordWeight(int paradigmid) const
{
	
	_statnode_t findnode;
	findnode.first = paradigmid;
	vector<_statnode_t>::const_iterator	presult =  lower_bound(m_WordWeights.begin(), m_WordWeights.end(), findnode, less4statnode);

	if (	(presult != m_WordWeights.end())
		&&	(presult->first == findnode.first)
		)
		return presult->second;
	return (0);
};
