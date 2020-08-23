// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "StdMorph.h"
#include "Predict.h"

CPredictBase::CPredictBase(MorphLanguageEnum langua) :m_SuffixAutomat(langua, MorphAnnotChar)
{
	
};


void CPredictBase::Load (const std::string &path)
{
	m_SuffixAutomat.Load(path);
};

void CPredictBase::FindRecursive(int NodeNo, std::string& curr_path, std::vector<CPredictTuple>& Infos) const
{
	const CMorphAutomNode& N = m_SuffixAutomat.GetNode(NodeNo);
	if (N.IsFinal())
	{
		int i = curr_path.find(m_SuffixAutomat.m_AnnotChar);
		assert (i != std::string::npos);
		int j = curr_path.find(m_SuffixAutomat.m_AnnotChar, i+1);
		assert (j != std::string::npos);
		int k = curr_path.find(m_SuffixAutomat.m_AnnotChar, j+1);
		assert (k != std::string::npos);
		CPredictTuple A;
		A.m_PartOfSpeechNo = m_SuffixAutomat.DecodeFromAlphabet(curr_path.substr(i+1,j-i-1));
		A.m_LemmaInfoNo = m_SuffixAutomat.DecodeFromAlphabet(curr_path.substr(j+1, k-j-1));
		A.m_ItemNo = m_SuffixAutomat.DecodeFromAlphabet(curr_path.substr(k+1));
		Infos.push_back(A);
	};

	size_t Count =  m_SuffixAutomat.GetChildrenCount(NodeNo);
	size_t CurrPathSize = curr_path.size();
	curr_path.resize(CurrPathSize + 1);
	for (size_t i=0; i<Count; i++)
	{
		const CMorphAutomRelation& p = m_SuffixAutomat.GetChildren(NodeNo)[i];
		curr_path[CurrPathSize] = p.GetRelationalChar();
		FindRecursive(p.GetChildNo(), curr_path, Infos);
	};
	curr_path.resize(CurrPathSize);
};

bool CPredictBase::Find(const std::string &ReversedWordForm, std::vector<CPredictTuple>& res) const
{
	//  we don't want to predict words which contains "AnnotChar" 
	//if (ReversedWordForm.find(AnnotChar) != std::string::npos)
	//	return false;

	size_t TextLength = ReversedWordForm.length();
	int r = 0;
	size_t i=0;
	for (; i<TextLength; i++)
	{
		int nd =  m_SuffixAutomat.NextNode(r,(BYTE)ReversedWordForm[i]);
		
		if (nd ==  -1) 
			break;		
		r = nd;
	};
	// no prediction by suffix which is less than 3
	if (i < MinimalPredictionSuffix) 
		return false;
	
	assert ( r != -1);
	std::string curr_path;
	FindRecursive(r, curr_path,res);
	return true;
};


