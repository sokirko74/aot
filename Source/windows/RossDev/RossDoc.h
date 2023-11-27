// RossDoc.h : interface of the CRossDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef RossDoc_h
#define RossDoc_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "common/cortege.h"
class CWordlist;
#include "seman/SemanLib/struct_dict_holder.h"

struct CRossDevTextField;


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
	    lstrcpy(Font.lfFaceName, L"FixedSys");
		Color = 0;
		DropDownCount = 10;
	};

	void InitFromString (CString Line)
	{
		int i = 0; // substring index to extract
		CString sToken = _T("");
		while (AfxExtractSubString(sToken, Line, i, ' '))
		{
			if (i == 0) {
				DomStr = sToken;
			}
			else if (i == 1) {
				Color = _ttoi(sToken);
			}
			else if (i == 2) {
				DropDownCount = _ttoi(sToken);
			}
			i++;
		}
	}
	bool operator == (CString S) const 
	{
		return S ==  DomStr;
	};
	
};
 






class CRossDoc : public CDocument
                 
{
	CStructDictHolder			m_ExternalRossHolder;
	bool				m_bSerialized;
	DictTypeEnum		m_RossId;
	bool				m_IsDefault;
	bool				m_FirstLoadReadonly;
	bool				m_DoNotLock;
	bool				m_bDoNotSaveLastUserName;	
	std::vector<CString>		m_OptionsComments;

protected: // create from serialization only
	CRossDoc();
	DECLARE_DYNCREATE(CRossDoc)
	

// Attributes
public:
	
	
	std::vector<CDomainParam>	m_DomainParams;
	std::vector<BYTE>			m_DomParamsPtr;
	CString					m_Author;
	bool					m_ReadOnly;
	bool					m_bArticleInitTextMode;

  

// Operations
public:
   
	CDictionary*				GetRoss();
	const CDictionary*			GetRoss() const;

	CStructDictHolder*				GetRossHolder();
	const CStructDictHolder*			GetRossHolder() const;

	CView*  GetWordList();
	void SetModifiedFlag( BOOL bModified = TRUE );

	const char* GetGramFetFieldName() const;
	const char* GetReferenceFieldName() const;
	const char* GetEnglishFieldName() const;
	bool		IsThesRoss() const;
	int			GetThesId() const;
	DictTypeEnum GetRossType() const;
	std::string		GetLockFileName()  const;
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
    std::unordered_map<std::string, BYTE> m_BasicDomItems;
	std::set<std::string>		m_Fields;

	
	virtual ~CRossDoc();
    void BuildBasicDomItems();
	void SerializeInner(CArchive& ar);
	void MakeReadOnly() ;

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
