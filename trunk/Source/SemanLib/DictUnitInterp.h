#ifndef DictUnitInterp_H
#define DictUnitInterp_H

#include "TranslatorHolder.h"

// ��������� ������������� 
struct  CRossInterp 
{
	// ��� ������� 
	DictTypeEnum   m_DictType;
	// ����� ��������� ������ ��� id �������
	WORD		   m_UnitNo;
	CRossInterp();
	CRossInterp(DictTypeEnum DictType, WORD UnitNo);
	bool operator == ( const CRossInterp& X ) const;
	CRossInterp& operator = ( const CRossInterp& X );
};


struct  CDictUnitInterp : public CRossInterp {
    // ������ �� ��������������� � ���������������� �� ����������� ������� A0, ����
	// ��������� �������� � ���� SF ���������������
	CRossInterp		m_AOReference;

	// ������������� ����� 
	bool           m_bNegativeForm;

    // ������ �� TerminId ��� ����������� ��������
	long		   m_TerminId;

    // ���� ���������� �������, ��� ��� ������������� ���������� �����, ���� ���
    // ���������� ����� �������, ������� �� ���� ������� � ������� �������, � ����
    // ������� ������ ��� ������� "��".  � ���������� ������ �� �������, ��� ��� ���������
    // ����� ������ �� �����������, ���, ��������, ��� ����������� � ������������� ��� ������������  �����.
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
