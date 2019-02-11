#ifndef DictUnitInterp_H
#define DictUnitInterp_H

#include "TranslatorHolder.h"

// словарная интерпретация 
struct  CRossInterp 
{
	// тип словаря 
	DictTypeEnum   m_DictType;
	// номер словарной статьи или id термина
	WORD		   m_UnitNo;
	CRossInterp();
	CRossInterp(DictTypeEnum DictType, WORD UnitNo);
	bool operator == ( const CRossInterp& X ) const;
	CRossInterp& operator = ( const CRossInterp& X );
};


struct  CDictUnitInterp : public CRossInterp {
    // ссылка от прилагательного к сущетсвительному по лексической фнккции A0, если
	// последняя записана в поле SF прилагательного
	CRossInterp		m_AOReference;

	// отрицательная форма 
	bool           m_bNegativeForm;

    // ссылка на TerminId для однословных терминов
	long		   m_TerminId;

    // либо морфология заявила, что это страдательная глагольная форма, либо это
    // возвратная форма глагола, которая не была найдена в словаре целиком, а была
    // найдена только без частицы "ся".  В последенем случае мы считаем, что это пассивная
    // форма вплоть до идеализации, где, возможно, она превратится в декаузативную или коллективную  форму.
	bool           m_bPassiveForm;
	

	CDictUnitInterp();
	CDictUnitInterp (DictTypeEnum   DictType,	WORD UnitNo);
	CDictUnitInterp (const  CRossHolder* RossDoc, DictTypeEnum   DictType,	WORD UnitNo, bool bNegativeForm, bool bPassiveForm);
	CDictUnitInterp ( const CRossInterp& X );
	bool operator == ( const CDictUnitInterp& X ) const;
	WORD  GetSemCorrectUnitNo () const;
};


const  CDictUnitInterp  EmptyInterp =  CDictUnitInterp (NoneRoss, ErrUnitNo);
typedef vector<CDictUnitInterp>  CDictUnitInterpVector;


#endif
