// MorphwizardDoc.h : interface of the CMorphwizardDoc class
//


#pragma once

#include "MWDocument.h"
#include "afxtempl.h"


class CSLFDocument;

//----------------------------------------------------------------------------
class CMorphwizardDoc : public CDocument
{
protected: // create from serialization only
	CMorphwizardDoc();
	DECLARE_DYNCREATE(CMorphwizardDoc)

// Attributes
public:
	MorphoWizard m_Wizard;

// Operations
public:

// Overrides
public:
	virtual	MorphoWizard*	GetWizard()	{ return &m_Wizard; }
	
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsModified();
	virtual BOOL DoFileSave();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
// Implementation
public:
	virtual ~CMorphwizardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool	HasModifiedSlf() const;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	CMap<CSLFDocument*,CSLFDocument*,CSLFDocument*,CSLFDocument*>	m_slfDocs;
	virtual void OnCloseDocument();
	virtual BOOL SaveModified(); // return TRUE if ok to continue
};

