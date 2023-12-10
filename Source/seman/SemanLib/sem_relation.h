#pragma once

#include "Valency.h"
#include "syn_realization.h"

struct CSimpleBinaryRelation {
	// исходный узел
	long    m_SourceNodeNo;
	// конечный узел
	long    m_TargetNodeNo;

	CSimpleBinaryRelation(long SourceNodeNo = -1, long    TargetNodeNo = -1)
	{
		m_SourceNodeNo = SourceNodeNo;
		m_TargetNodeNo = TargetNodeNo;
	};
};

enum PositionTypeEnum {
	UnknownPosType,
	FromArticlePosType,
	FromAlgorithmPosType
};

struct CSemRelation : public CSimpleBinaryRelation {
    // валентность, которая приписана этому слову
    CValency m_Valency;
    // синтаксическое отношение, которое лежит в основе этого семантического отношения
    // если данное СемО пришло из жесктих синтаксических отношений, то здесь лежит СинО,
    // взятое непосредственно из синтаксиса, иначе здесь лежит отношение, которое записано в поле GFi
    std::string m_SyntacticRelation;
    //технический слот: помечает те отношения, которые нужно будет удалить
    bool m_bToDelete;
    //здесь записывается информация о том, как реализуется лексически и грамматически это отношение
    CSynRealization m_SynReal;
    //технический слот: используется для выделения одного подмножества отношений среди некоторого множества
    long m_bRelUse;
    // помета того, что GFi был помечен реверсинвым отношением ("X!")
    bool m_bReverseRel;

    // перечень LEX, которым должно удовлетворять данное отношение
    StringVector m_LexFets;

    //SF отношения
    std::vector<uint64_t> m_SemFets;

    bool m_bDopRelation;

    // позиция, которая указывается для синтеза
    std::string m_Position;
    PositionTypeEnum m_PosType;

    // отношение, которое на этапе идеализации должно подвеситься к узлу SIT , а пока подвешивается	// к вершине клаузы
    bool m_bSituatRelation;

    void Init();

    CSemRelation();

    CSemRelation(const CValency& Valency,
        long SourceNodeNo,
        long TargetNodeNo,
        std::string SyntacticRelation);

    // истина, если отношению приписаны предлоги
    bool HasSomePrep() const;
};


// класс синтаксического отношения
struct CSynRelation : public CSimpleBinaryRelation
{
	std::string			   m_SynRelName;
	bool			   m_bPragmaticallyStrong;
	CSynRelation(long SourceNodeNo, long    TargetNodeNo, std::string SynRelName) :
		CSimpleBinaryRelation(SourceNodeNo, TargetNodeNo)
	{
		m_SynRelName = SynRelName;
		m_bPragmaticallyStrong = false;
	};

	CSynRelation() : CSimpleBinaryRelation()
	{
		m_bPragmaticallyStrong = false;
	};
	bool operator == (const CSynRelation X) const
	{
		return   (X.m_SourceNodeNo == m_SourceNodeNo)
			&& (X.m_TargetNodeNo == m_TargetNodeNo)
			&& (X.m_SynRelName == m_SynRelName);
	};

};


// класс синтаксического отношения
struct CSemThesRelation : public CSimpleBinaryRelation
{
	std::string			   m_SemRelName;
	CSemThesRelation(long SourceNodeNo, long    TargetNodeNo, std::string SemRelName) :
		CSimpleBinaryRelation(SourceNodeNo, TargetNodeNo)
	{
		m_SemRelName = SemRelName;
	};

	CSemThesRelation() : CSimpleBinaryRelation()
	{
	};
	bool operator == (const CSemThesRelation X) const
	{
		return   (X.m_SourceNodeNo == m_SourceNodeNo)
			&& (X.m_TargetNodeNo == m_TargetNodeNo)
			&& (X.m_SemRelName == m_SemRelName);
	};

};
