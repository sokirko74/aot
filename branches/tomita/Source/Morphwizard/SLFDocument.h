#pragma once

#include "../MorphWizardLib/wizard.h"
#include "MWDocument.h"

class CSLFView;
class CMorphwizardView;


class CSLFDocument : public CDocument
{
	DECLARE_DYNCREATE(CSLFDocument)

public:
	CSLFDocument();
	CMorphwizardView* m_pParent;
	bool			m_bShouldBeClosed;
	
	int				m_GotoLine;
	CParadigmInfo	m_Paradigm;
	CString			m_CommonGrammems;
	CString			m_ParadigmComments;
	CString			m_Prefixes;
	CString			m_ParadigmText;

	CString			m_SaveParadigmText;
	CString			m_SaveCommonGrammems;
	CString			m_SavePrefixes;
	
	CParadigmInfo	m_SaveParadigm;
	

	MorphoWizard*	GetWizard();
	const MorphoWizard*	GetWizard() const;
	virtual ~CSLFDocument();
	virtual BOOL SaveModified(); 
	virtual BOOL IsModified();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL DoFileSave();
	virtual void OnCloseDocument( );
	CSLFView* GetSLFView();
	CString GetSavedLemma();
	CString GetLemma() const;
	string	GetBase()  const;
	BOOL	OpenDiffDialog ();
	BOOL	OpenParadigm( bool runAccentWizard=false );
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnFileClose();
private:
	void update_saved_paradigm();
public:
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
};
