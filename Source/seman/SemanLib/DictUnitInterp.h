#ifndef DictUnitInterp_H
#define DictUnitInterp_H

#include "dict_type.h"

// словарная интерпретация 
struct  CRossInterp 
{
	// тип словаря 
	DictTypeEnum   m_DictType;
	// номер словарной статьи или id термина
	uint16_t		   m_UnitNo;
	CRossInterp();
	CRossInterp(DictTypeEnum DictType, uint16_t UnitNo);
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
	CDictUnitInterp (DictTypeEnum   DictType,	uint16_t UnitNo);
	CDictUnitInterp (const  CStructDictHolder* RossDoc, DictTypeEnum   DictType,	uint16_t UnitNo, bool bNegativeForm, bool bPassiveForm);
	CDictUnitInterp ( const CRossInterp& X );
	bool operator == ( const CDictUnitInterp& X ) const;
	uint16_t  GetSemCorrectUnitNo () const;
};


const  CDictUnitInterp  EmptyInterp =  CDictUnitInterp (NoneRoss, ErrUnitNo);
typedef std::vector<CDictUnitInterp>  CDictUnitInterpVector;


#endif
