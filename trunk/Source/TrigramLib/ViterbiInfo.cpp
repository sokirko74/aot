#include "TrigramModel.h"


CViterbiInfo::CViterbiInfo () 
{
    m_FirstMaximum = make_pair (-MAXPROB, UnknownTag);
    m_SecondMaximum = make_pair (-MAXPROB, UnknownTag);
};

void CViterbiInfo::SetProb(WORD i,  WORD j, prob_t  value) 
{
	m_Probs[make_pair(i,j)] = value;
};
prob_t CViterbiInfo::GetProb(WORD i,  WORD j) const
{
	map<pair<WORD,WORD>, prob_t>::const_iterator it = m_Probs.find(make_pair(i,j));
	if (it == m_Probs.end()) 
		return -MAXPROB;
	else
		return it->second;
};

void CViterbiInfo::SetTagRef(WORD i,  WORD j, WORD TagRef) 
{
	m_TagRefs[make_pair(i,j)] = TagRef;
};

WORD CViterbiInfo::GetTagRef(WORD i,  WORD j) const
{
    map<pair<WORD,WORD>, WORD>::const_iterator it = m_TagRefs.find(make_pair(i,j));
	if (it == m_TagRefs.end()) 
		return UnknownTag;
	else
		return it->second;
};

const map<pair<WORD,WORD>, prob_t>&  CViterbiInfo::GetAllProbs() const
{
	return m_Probs;	
}

/*
void  CViterbiInfo::UpdateMaximum(prob_t prob, WORD tag) 
{
    m_Maximums.insert(make_pair(prob,tag));
}
bool  CViterbiInfo::IsAmbig()  const
{
    return m_Maximums.size() > 1;
}

pair<prob_t, WORD>  CViterbiInfo::GetFirstMaximum()  const
{
    return *m_Maximums.rbegin();
}

pair<prob_t, WORD>  CViterbiInfo::GetSecondMaximum()  const
{
    set< pair<prob_t, WORD> >::const_reverse_iterator it = m_Maximums.rbegin();
    for (it++;  it!= m_Maximums.rend(); it++)
       if (it->second != m_Maximums.rbegin()->second)
              break;
    if (it == m_Maximums.rend()) 
        return make_pair (-MAXPROB, UnknownTag);
    else
        return *it;
}*/

void  CViterbiInfo::UpdateMaximum(prob_t prob, WORD tag) 
{
    if (prob >m_FirstMaximum.first)
    {
        if (m_FirstMaximum.second != tag)
            m_SecondMaximum = m_FirstMaximum;
        m_FirstMaximum = make_pair(prob,tag);
    }
    else
        if (prob>m_SecondMaximum.first)
        {
            if (m_FirstMaximum.second != tag)
                m_SecondMaximum = make_pair(prob,tag);
        }
    assert (m_FirstMaximum.first >= m_SecondMaximum.first);
    assert (m_FirstMaximum.second != m_SecondMaximum.second);
}
bool  CViterbiInfo::IsAmbig()  const
{
    return m_SecondMaximum.second != UnknownTag;
}

pair<prob_t, WORD>  CViterbiInfo::GetFirstMaximum()  const
{
    return m_FirstMaximum;
}

pair<prob_t, WORD>  CViterbiInfo::GetSecondMaximum()  const
{
    return m_SecondMaximum;
}
