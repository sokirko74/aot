
// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)


// ============          Consent.H       ==========================
// ==================================================================
//  Этот файл заголовков - часть  макросинтаксического анализа. 
//  Здесь определен тип (ConSent); переменные этого типа будут 
//  содержать макросинтаксические  единицы. Макросинтаксические 
//  единицы  входят в макросинтаксическую структуру (CSList.H).
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
		case rELine : return "Пустая строка";
		case rPar : return "Отступ";
		case rPoint	: return "Начало пункта";
		case rBeg : return "Начало текста";
		case rEnd : return "Конец текста";
		case rQQ1 : return "Начало кукушки";
		case rQQ2 : return "Конец кукушки";
		default  : return "Конец УП (превышение по длине)";
	};

};


// Слота StartNo и EndNo  (номера строк в граф. таблице) принимают  
// значение UnGroundNo, если УП 
// в целом является  "неземным" (нетекстовые).  Нетекстовые УП 
// не имеют ссылки на текст (читай граф. таблица), и они вводятся  
// самим алгоритмом для общности макросинтаксического анализа. 
// Полагаем, что первая строка граф. таблицы не относится к тексту,
// соответственно, UnGroundNo  ссылается на строку, которая  
// была введена алгоритмом как бы виртуально.
const long UnGroundNo = 0;

struct CConSent {

	// На что может заканчиваться пункт перечисления	
	enum  BulletLastDelimEnum {
		Bracket,
		FullStop,
		Other
	};

	// стандартные части документа
	enum  CUsualLawDocumentDivision {
		None = 0,	
		Chapter = 1,
		Article = 2,
		Section = 3,
		SubSection = 4,
		Part  = 5,
	};

	// ссылка на графематическую таблицу
	const CGraphmatFile* m_GraFile;

	const CGraLine& GetUnit(size_t i) const;


	size_t m_StartNo;     /* number of the first line of the CS  in Graphematical Table*/
	size_t m_EndNo;     /* number of the last line of the CS in Graphematical Table */

	size_t m_HardGraphStartNo; // Номер строки таблицы, в которой содержится первая непробельная графема     
	size_t m_HardGraphEndNo;   // Номер строки таблицы, в которой содержится последняя непробельная графема       

	// Номер строки графематической таблицы, на которую будет поставлен дескриптор
	size_t m_GraLineNoToMark; 

	// Номер хозяина данного УП
	int    m_HostNo;
	// Номер УП, которое примыкает к данному УП
	int     m_ParatNo;

	/* number of lines in the input text */
	// that should be recalculated after changing of borders
	size_t	m_NumberOfLines;     

	// УП,  ЗГЛ, УПВвод ... 
	Descriptors m_Type;


	/* Type of the upper Rubicon,  see Rubicon Type below */
	RubiconEnum		m_UpperRubiconType;
	/* Type of the lower Rubicon,  ------ || ----------- */
	RubiconEnum		m_LowerRubiconType;  


	/* True, if the first letter of the CS is an uppercase letter */
	bool   m_bFirstUpper;   

	/* Number of  "," */
	short  m_CountFullStops;   

	// истина, если УП подчинена какому-то УП и примыкает к УП
	// должна быть связь к чему-то
	bool   m_bConnected;

	// contains  the number of the field of similarity
	// For example all sentences that are finished  with comma...
	// By default all CS are in Similar Field with Number 0.
	int    m_SimilarFieldNo;    

	// the depth of Bullet for cases like "1.2.3 Some text. 1.2.4"
	short  m_BulletDepth;


	// подсоединено ли данное УП по макросинтаксическим правилам или насильно, если уже ничего не осталось
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
