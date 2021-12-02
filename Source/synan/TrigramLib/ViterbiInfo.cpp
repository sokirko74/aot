#include "TrigramModel.h"


CViterbiInfo::CViterbiInfo () 
{
    m_FirstMaximum = std::make_pair (-MAXPROB, UnknownTag);
    m_SecondMaximum = std::make_pair (-MAXPROB, UnknownTag);
};

void CViterbiInfo::SetProb(uint16_t i,  uint16_t j, prob_t  value) 
{
	m_Probs[std::make_pair(i,j)] = value;
};
prob_t CViterbiInfo::GetProb(uint16_t i,  uint16_t j) const
{
	std::map<std::pair<uint16_t,uint16_t>, prob_t>::const_iterator it = m_Probs.find(std::make_pair(i,j));
	if (it == m_Probs.end()) 
		return -MAXPROB;
	else
		return it->second;
};

void CViterbiInfo::SetTagRef(uint16_t i,  uint16_t j, uint16_t TagRef) 
{
	m_TagRefs[std::make_pair(i,j)] = TagRef;
};

uint16_t CViterbiInfo::GetTagRef(uint16_t i,  uint16_t j) const
{
    std::map<std::pair<uint16_t,uint16_t>, uint16_t>::const_iterator it = m_TagRefs.find(std::make_pair(i,j));
	if (it == m_TagRefs.end()) 
		return UnknownTag;
	else
		return it->second;
};

const std::map<std::pair<uint16_t,uint16_t>, prob_t>&  CViterbiInfo::GetAllProbs() const
{
	return m_Probs;	
}

/*
void  CViterbiInfo::UpdateMaximum(prob_t prob, uint16_t tag) 
{
    m_Maximums.insert(std::make_pair(prob,tag));
}
bool  CViterbiInfo::IsAmbig()  const
{
    return m_Maximums.size() > 1;
}

std::pair<prob_t, uint16_t>  CViterbiInfo::GetFirstMaximum()  const
{
    return *m_Maximums.rbegin();
}

std::pair<prob_t, uint16_t>  CViterbiInfo::GetSecondMaximum()  const
{
    std::set< std::pair<prob_t, uint16_t> >::const_reverse_iterator it = m_Maximums.rbegin();
    for (it++;  it!= m_Maximums.rend(); it++)
       if (it->second != m_Maximums.rbegin()->second)
              break;
    if (it == m_Maximums.rend()) 
        return std::make_pair (-MAXPROB, UnknownTag);
    else
        return *it;
}*/

void  CViterbiInfo::UpdateMaximum(prob_t prob, uint16_t tag) 
{
    if (prob >m_FirstMaximum.first)
    {
        if (m_FirstMaximum.second != tag)
            m_SecondMaximum = m_FirstMaximum;
        m_FirstMaximum = std::make_pair(prob,tag);
    }
    else
        if (prob>m_SecondMaximum.first)
        {
            if (m_FirstMaximum.second != tag)
                m_SecondMaximum = std::make_pair(prob,tag);
        }
    assert (m_FirstMaximum.first >= m_SecondMaximum.first);
    assert (m_FirstMaximum.second != m_SecondMaximum.second);
}
bool  CViterbiInfo::IsAmbig()  const
{
    return m_SecondMaximum.second != UnknownTag;
}

std::pair<prob_t, uint16_t>  CViterbiInfo::GetFirstMaximum()  const
{
    return m_FirstMaximum;
}

std::pair<prob_t, uint16_t>  CViterbiInfo::GetSecondMaximum()  const
{
    return m_SecondMaximum;
}
