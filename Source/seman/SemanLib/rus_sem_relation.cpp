#include "rus_sem_relation.h"

CRusSemRelation::CRusSemRelation(const CValency& v, long src, long trg,
    std::string syn_rel) : CSemRelation(v, src, trg,syn_rel) 
{
    m_CannotHaveOnlyCommaBetween = false;
    m_bSemFetAgree = false;
    m_bReverseRel = false;
    m_LeapsCount = 0;
    m_bUniqueRelation = false;
    m_ClauseRuleNo = -1;
    m_bSemRelPOS = true;
};
