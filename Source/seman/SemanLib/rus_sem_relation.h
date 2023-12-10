#pragma once 

struct COutputRelation {
    std::string sem_rel;
    std::string syn_rel;
    std::string src;
    std::string trg;
};

class CRusSemRelation : public CSemRelation {
public:
    // истина, что отношение не может проходить только над одной запятой (оно должно проходить еще над чем-то)
    bool m_CannotHaveOnlyCommaBetween;
    // истина, если отношение согласовано по SF (для текущего древесного варианта)
    bool m_bSemFetAgree;
    // номер поля эквивалнтности, которое включает отношения, выходящие из одного узла и противоречащие друг другу
    long m_EqualFieldId;
    //длина отношения в "жестких" синтаксических деревьях
    long m_LeapsCount;
    //технический слот: помечает отношения, которые точно останутся в древесном варианте
    long m_bUniqueRelation;
    // номер межклаузного правила, по которому была проведена эта межклаузная связь
    long m_ClauseRuleNo;

    // истина, если название отношение согласовано с частью речи
    // узла, в которое оно идет
    // например, PROPERT обычно идет в прилашательное// а SUB в существительное
    bool m_bSemRelPOS;

    CRusSemRelation(const CValency& v, long src_node, long trg_node,
        std::string SyntacticRelation);
};
