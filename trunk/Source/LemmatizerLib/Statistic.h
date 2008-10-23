// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#ifndef morph_statistics_h
#define morph_statistics_h

#include "../common/utilit.h"

typedef troika<int, int, int>	_homonode_t;		 
typedef pair<int, int>		_statnode_t; 

class CStatistic  
{
	vector<_statnode_t>			m_WordWeights;
	vector<_homonode_t>			m_HomoWeights;
public:
	void	Load(const string& path);
	int		get_HomoWeight(int paradigmid, int form) const;
	int		get_WordWeight(int paradigmid) const;
};

#endif 
