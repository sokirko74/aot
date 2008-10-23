// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef PREDICT_H
#define PREDICT_H


#pragma warning(disable:4786)
#include "../common/utilit.h"
#include "MorphAutomat.h"


//////////////////////////////////////////////////////////////////////////////
struct  CPredictTuple 
{
	WORD							m_ItemNo;
	DWORD							m_LemmaInfoNo;
	BYTE							m_PartOfSpeechNo;
};


//////////////////////////////////////////////////////////////////////
class CPredictBase 
{	
	CMorphAutomat				m_SuffixAutomat;
	void FindRecursive(int r, string& curr_path, vector<CPredictTuple>& Infos) const;
public:
	DwordVector			m_ModelFreq;

	
	CPredictBase(MorphLanguageEnum);

	void Load (const string &path);
	bool Find(const string &lettId, vector<CPredictTuple>& res) const;
};





#endif 

