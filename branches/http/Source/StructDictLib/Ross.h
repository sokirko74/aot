// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#ifndef ROSS_H
 #define ROSS_H
#pragma warning  (disable : 4530)

#include "../common/utilit.h"

#include <time.h>


#include "ItemsContainer.h"


const int NullItemId = 1;

const size_t MaxItemStr = 100;

// ссылки на словaрную статью, лежащую в массиве Ross :: Cortege
const int InitialStartPos = 5000000;
const int InitialEndPos = -1;
// Индекс  - WORD
class CStructEntry {
public:
	int		m_EntryId;
	char	m_EntryStr[EntryStrSize];
	BYTE	m_MeanNum;
	int		m_StartCortegeNo;
	int		m_LastCortegeNo;
	bool	m_bSelected;
	// Графематические дескрипторы
	BYTE	__dummy;
	char	m_AuthorStr[AuthorNameSize];


	CStructEntry (const char* _Lemma = "\x0", int _MeanNum = 0) 
	{
		m_StartCortegeNo  = InitialStartPos;
		m_LastCortegeNo  = InitialEndPos;
		size_t l = strlen (_Lemma);
		if (l < EntryStrSize - 1)
			strcpy (m_EntryStr, _Lemma);
		else
		{
			strncpy (m_EntryStr, _Lemma, EntryStrSize - 1);
			m_EntryStr[EntryStrSize - 1] = 0;
		};

		m_MeanNum = _MeanNum; 
		m_bSelected = true;
	}

	bool HasEmptyArticle () const  {return m_StartCortegeNo == InitialStartPos;};

	bool operator==(const CStructEntry& X) const
	{	
		return    !strcmp (m_EntryStr, X.m_EntryStr)
				&& (m_MeanNum == X.m_MeanNum); 
	}

	bool operator<(const CStructEntry& X) const
	{ 
		int i = strcmp (m_EntryStr, X.m_EntryStr);
		return   (i < 0) 
			||(    (i == 0)
			&& (m_MeanNum < X.m_MeanNum)); 
	}

};	  

//===========================================
const size_t UnitCommentSize = 100;
class TUnitComment {
public:
	  int  m_EntryId;
	  char Editor[AuthorNameSize];
	  char Comments[UnitCommentSize];
      tm   modif_tm;

	  TUnitComment();
	  TUnitComment(int _UnitId);

      bool operator==(const TUnitComment& X) const
			{return    m_EntryId == X.m_EntryId; }

      bool operator<(const TUnitComment& X) const
	  { return    m_EntryId < X.m_EntryId;}

     
};	  


class TCortegeContainer 
{
  typedef TBasicCortege<3> CortegeType3;
  typedef TBasicCortege<10> CortegeType10;

  vector<CortegeType3>      m_Corteges3;
  vector<CortegeType10>     m_Corteges10;
public :
  BYTE m_MaxNumDom;

  TCortegeContainer(BYTE MaxNumDom);
  TCortege10*		GetCortege (size_t i);
  const TCortege10*	GetCortege (size_t i)  const;

  void		_AddCortege(const TCortege10& C);
  size_t	_GetCortegesSize ()  const;
  void		ClearCorteges();
  void		EraseCorteges (size_t start, size_t last);
  void		WriteCorteges (const char* CortegeFile) const;
  void		ReadCorteges (const char* CortegeFile);
 
};




class TRoss  : public TCortegeContainer, public TItemContainer  
{
	char UnitsFile[_MAX_PATH];
	char CortegeFile[_MAX_PATH];
	char UnitCommentsFile[_MAX_PATH];
	char ConfigFile[_MAX_PATH];
	

public:
	BYTE					m_MaxMeanNum;
	string					m_DictName;
	
	vector<CStructEntry>	m_Units;
	vector<TUnitComment>	m_UnitComments;
	bool					m_bShouldSaveComments;


	TRoss (BYTE MaxNumDom);
	~TRoss ();
	bool	FullLoad(const char* _RossPath);
	bool	LoadOnlyConstants(const char* _RossPath);
	bool	Save ();


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++       Словарный вход  +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++

	void	BuildUnits();
	WORD	GetUnitsSize() const { return (WORD)m_Units.size();	};
	bool	ClearUnits();
	void	ClearUnit(WORD UnitNo);
	void	DelUnit(vector<CStructEntry>::iterator It);
	WORD	LocateUnit (const char * EntryStr, int MeanNum) const;
	WORD    GetSelectedUnitNo (WORD i) const;
	WORD	GetSelectedUnitsSize() const;
	WORD    InsertUnit (CStructEntry& T);
	WORD	InsertUnit(const char* EntryStr, BYTE MeanNum);
	WORD    UnitsLowerBound(const char* EntryStr)  const 
	{ 
		return lower_bound(m_Units.begin() , m_Units.end(), CStructEntry(EntryStr, 1)) - m_Units.begin();
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++       Комментарий ко входу +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool			ReadUnitComments();
	WORD			InsertUnitComment (WORD m_EntryId);
	TUnitComment*			GetCommentsByUnitId(WORD m_EntryId); 
	const TUnitComment*		GetCommentsByUnitId(WORD EntryId)   const;


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++       Словарная статья в виде кортежей ++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool			BuildCorteges();
	virtual bool	UpdateSignatsOfTheFieldInCorteges (BYTE FieldNo, vector<CSignat>& Signats);
	void			DelCorteges (size_t start, size_t last);
						 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++  Чтение/запись строки словарной статьи +++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++


	bool	ReadFromStrWithOneSignatura (const char* s, TCortege10& C, BYTE SignatNo);
	bool	ReadFromStr (const char* s, TCortege10& C);
	void	WriteToStr (const int* Items, const char* Frmt, char* OutBuffer) const;
	void	CortegeToStr (const TCortege10& C, char* OutBuffer) const;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++      Перечни констант  ++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool	InsertDomItem (const char* ItemStr, BYTE DomNo, int& ItemNo);
	void	DelDomItem	(int ItemNo);

	const char* GetTitleFieldName() const;
	const char* GetSenseFieldName() const;
	const char* GetCommFieldName() const;
	const char* GetAuthorFieldName() const;
	const char* GetTimeCreatFieldName() const;
    const char* GetRedactFieldName() const;


	void SetUnitCommentStr(WORD UnitNo, const char* Str);
	void SetUnitAuthor(WORD UnitNo, const char* Author);
	void SetUnitModifTimeStr(WORD UnitNo, const char* TimeStr);
	void SetUnitEditor(WORD UnitNo, const char* Editor);
	string GetUnitModifTimeStr(WORD UnitNo) const;
	string GetUnitTextHeader(WORD UnitNo) const;

	bool	ReadConfig();

};


typedef enum {	iceOverwrite = 0, iceSkip = 1, iceAppend = 2} ImportConflictEnum;

struct CSourceLine 
{
	string m_Line;
	int m_SourceLineNo;
	CSourceLine (string Line,  int  SourceLineNo)
	{
		m_Line = Line;
	    m_SourceLineNo = SourceLineNo;
	};
};



class CDictionary : public TRoss
{
public:
	
	/*
		By default  TCortegeContainer::m_MaxNumDom is equal to 3.
		Before invoking TRoss::Load it can be adjusted.
	*/
	vector<CStructEntry>& GetUnits();

	CDictionary();
	
	BYTE		GetCortegeFieldNo(size_t i) const;
	BYTE		GetCortegeLeafId(size_t i) const;
	BYTE		GetCortegeBracketLeafId(size_t i) const;
	const char* GetDomItemStrInner(int ItemNo) const;
	bool		IsEmptyArticle(WORD UnitNo) const;
	int			GetUnitStartPos(WORD UnitNo) const;
	int			GetUnitEndPos(WORD UnitNo) const;
	BYTE		GetFieldNoByFieldStr(const char* Str) const;
	BYTE		GetDomItemDomNo (int ItemNo) const;
	int			GetCortegeItem(long CortegeNo, BYTE PositionInCortege) const;
	const char*	GetDomItemStr(int ItemNo) const;
	string		GetEntryStr (WORD EntryNo) const;
	BYTE		GetUnitMeanNum(WORD EntryNo) const;
	bool		IncludeArticle(WORD UnitNo, string Article) const;

	bool		Load(const char* Path);
	bool		ProcessOneArticle ( vector<CSourceLine>& L, int start, int last, bool bSimulating, ImportConflictEnum ConflictSolver, string& Messages);
	bool		ImportFromText(string FileName, bool bSimulating, ImportConflictEnum ConflictSolver, int StartEntry,string& Messages);
	void		SetUnitCurrentTime(WORD UnitNo);
	bool		AddField(string FieldStr);
	string		GetUnitEditor(WORD UnitNo) const;
	void		SetUnitStr(WORD UnitNo, const char*  UnitStr);
	

};	




#endif
