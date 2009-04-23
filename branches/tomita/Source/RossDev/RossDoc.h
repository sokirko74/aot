// RossDoc.h : interface of the CRossDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef RossDoc_h
#define RossDoc_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "..\common\cortege.h"
class CWordlist;
#include "../SemanLib/RossHolder.h"

struct CRossDevTextField;

struct TBaseDomItem {
	char ItemStr[100];
	BYTE DomNo;

	bool operator==(const TBaseDomItem& X) const
	{return     strcmp (ItemStr, X.ItemStr) == 0; }

    bool operator<(const TBaseDomItem& X) const
	  { return  strcmp (ItemStr, X.ItemStr) < 0; }
};


struct CDomainParam {
	CString DomStr;
	COLORREF Color;
	int DropDownCount;
	LOGFONT Font;
	CDomainParam ()
	{
		Font.lfHeight = -12;
	    Font.lfWidth =  0;
	    Font.lfEscapement =  0;
	    Font.lfOrientation =  0;
	    Font.lfWeight =  FW_BOLD;	
	    Font.lfItalic =  0;	
	    Font.lfUnderline =  0;	
	    Font.lfStrikeOut =  0;	
	    Font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	    Font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	    Font.lfQuality = DEFAULT_QUALITY;
	    Font.lfPitchAndFamily  = DEFAULT_PITCH;
	    Font.lfCharSet = RUSSIAN_CHARSET;
	    strcpy(Font.lfFaceName, "FixedSys");
		Color = 0;
		DropDownCount = 10;
	};

	void InitFromString (CString Line)
	{
		char s[100];
		char Name[100];
		strcpy (s,Line);
		sscanf  (s, "%s %i %i", Name, &Color, &DropDownCount);
		DomStr = Name;
	}
	bool operator == (CString S) const 
	{
		return S ==  DomStr;
	};
	
};
 






class CRossDoc : public CDocument
                 
{
	bool				m_bRussianFields;
	CRossHolder			m_ExternalRossHolder;
	bool				m_bSerialized;
	DictTypeEnum		m_RossId;
	bool				m_IsDefault;
	vector<CString>		m_Authors;
	bool				m_FirstLoadReadonly;
	bool				m_DoNotLock;
	bool				m_bDoNotSaveLastUserName;	
	vector<CString>		m_OptionsComments;


	void ReadConfig(CArchive& ar, CString& Login);
	void WriteConfig(CArchive& ar);

protected: // create from serialization only
	CRossDoc();
	DECLARE_DYNCREATE(CRossDoc)
	

// Attributes
public:
	
	
	vector<CDomainParam>	m_DomainParams;
	vector<BYTE>			m_DomParamsPtr;
	CString					m_Author;
	bool					m_ReadOnly;
	bool					m_bArticleInitTextMode;
	LanguageEnum			m_LanguageId;

  

// Operations
public:
   
	CDictionary*				GetRoss();
	const CDictionary*			GetRoss() const;

	CRossHolder*				GetRossHolder();
	const CRossHolder*			GetRossHolder() const;

	CView*  GetWordList();
	void SetModifiedFlag( BOOL bModified = TRUE );

	const char* GetGramFetFieldName() const;
	const char* GetReferenceFieldName() const;
	const char* GetEnglishFieldName() const;
	bool		IsThesRoss() const;
	int			GetThesId() const;
	string		GetLockFileName()  const;
	void		RemoveLock() const;
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRossDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
    vector<TBaseDomItem> m_BasicDomItems;
	StringVector		m_Fields;

	
	virtual ~CRossDoc();
    void    BuildBasicDomItems();
	CString SerializeInner(CArchive& ar);
	void	MakeReadOnly() ;
	void	DelTextDomains();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message  functions
protected:
	//{{AFX_MSG(CRossDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
