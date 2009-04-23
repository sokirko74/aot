
// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)


// ============          Consent.H       ==========================
// ==================================================================
//  ���� ���� ���������� - �����  �������������������� �������. 
//  ����� ��������� ��� (ConSent); ���������� ����� ���� ����� 
//  ��������� �������������������  �������. ������������������� 
//  �������  ������ � ������������������� ��������� (CSList.H).
//==================================================================


#ifndef consent_h
#define consent_h

#include "StdGraph.h"
#include "graline.h"

class CGraphmatFile;


enum RubiconEnum {	rUndef, 	rELine, 	rPar, 	rPoint,	rBeg, 	rEnd,	rQQ1,
	rQQ2, 	rMaxLimit
};

inline const char* GetStrByRubiconType (RubiconEnum t )
{
	switch (t) {
		case rUndef : return "????";
		case rELine : return "������ ������";
		case rPar : return "������";
		case rPoint	: return "������ ������";
		case rBeg : return "������ ������";
		case rEnd : return "����� ������";
		case rQQ1 : return "������ �������";
		case rQQ2 : return "����� �������";
		default  : return "����� �� (���������� �� �����)";
	};

};


// ����� StartNo � EndNo  (������ ����� � ����. �������) ���������  
// �������� UnGroundNo, ���� �� 
// � ����� ��������  "��������" (�����������).  ����������� �� 
// �� ����� ������ �� ����� (����� ����. �������), � ��� ��������  
// ����� ���������� ��� �������� �������������������� �������. 
// ��������, ��� ������ ������ ����. ������� �� ��������� � ������,
// ��������������, UnGroundNo  ��������� �� ������, �������  
// ���� ������� ���������� ��� �� ����������.
const long UnGroundNo = 0;

struct CConSent {

	// �� ��� ����� ������������� ����� ������������	
	enum  BulletLastDelimEnum {
		Bracket,
		FullStop,
		Other
	};

	// ����������� ����� ���������
	enum  CUsualLawDocumentDivision {
		None = 0,	
		Chapter = 1,
		Article = 2,
		Section = 3,
		SubSection = 4,
		Part  = 5,
	};

	// ������ �� ��������������� �������
	const CGraphmatFile* m_GraFile;

	const CGraLine& GetUnit(size_t i) const;


	size_t m_StartNo;     /* number of the first line of the CS  in Graphematical Table*/
	size_t m_EndNo;     /* number of the last line of the CS in Graphematical Table */

	size_t m_HardGraphStartNo; // ����� ������ �������, � ������� ���������� ������ ������������ �������     
	size_t m_HardGraphEndNo;   // ����� ������ �������, � ������� ���������� ��������� ������������ �������       

	// ����� ������ ��������������� �������, �� ������� ����� ��������� ����������
	size_t m_GraLineNoToMark; 

	// ����� ������� ������� ��
	int    m_HostNo;
	// ����� ��, ������� ��������� � ������� ��
	int     m_ParatNo;

	/* number of lines in the input text */
	// that should be recalculated after changing of borders
	size_t	m_NumberOfLines;     

	// ��,  ���, ������ ... 
	Descriptors m_Type;


	/* Type of the upper Rubicon,  see Rubicon Type below */
	RubiconEnum		m_UpperRubiconType;
	/* Type of the lower Rubicon,  ------ || ----------- */
	RubiconEnum		m_LowerRubiconType;  


	/* True, if the first letter of the CS is an uppercase letter */
	bool   m_bFirstUpper;   

	/* Number of  "," */
	short  m_CountFullStops;   

	// ������, ���� �� ��������� ������-�� �� � ��������� � ��
	// ������ ���� ����� � ����-��
	bool   m_bConnected;

	// contains  the number of the field of similarity
	// For example all sentences that are finished  with comma...
	// By default all CS are in Similar Field with Number 0.
	int    m_SimilarFieldNo;    

	// the depth of Bullet for cases like "1.2.3 Some text. 1.2.4"
	short  m_BulletDepth;


	// ������������ �� ������ �� �� ������������������� �������� ��� ��������, ���� ��� ������ �� ��������
	bool   m_bForceConnected;  

	// The last delimiter that follow the bullet id. For example in  "1)" it is ")",
	BulletLastDelimEnum  m_BulletLastDelim;

	CUsualLawDocumentDivision m_UsualLawDocumentDivision;

	void SetEmpty ()
	{
		m_GraFile = 0;
		m_bFirstUpper = false;
		m_HostNo = m_ParatNo = -1;
		m_bConnected  = false;
		m_UpperRubiconType = m_LowerRubiconType = rUndef;
		m_SimilarFieldNo = 0;
		m_Type = CS_Undef;
		m_bForceConnected = false;
		m_BulletLastDelim = Other;
	};

	CConSent ()
	{
		SetEmpty();

	};
	CConSent (const CGraphmatFile* _GraFile)
	{
		SetEmpty();
		m_GraFile = _GraFile;
	};

	BYTE LastChar () const { return GetUnit(m_HardGraphEndNo).GetToken()[GetUnit(m_HardGraphEndNo).GetTokenLength()-1]; };
	BYTE FirstChar ()	const { return GetUnit(m_HardGraphStartNo).GetToken()[0]; }; 
	const CGraLine&     FirstUnit ()	 const { return GetUnit(m_HardGraphStartNo); };
	bool  IsFirstWord(const char* s)	 const;
	const CGraLine&     LastUnit ()	const { return GetUnit(m_HardGraphEndNo); };

	bool  IsGround() const {return m_StartNo != UnGroundNo;};

	bool  IsBullet () const;
	bool  IsParag  () const;
	bool  IsSoft() const;



	bool	SetUsualLawDocumentDivision ();
	bool	InitBasicInformation ();
	string  GetString() const;

  
};


typedef vector<CConSent> CSList;

#endif
