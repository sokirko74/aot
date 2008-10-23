#pragma once

#include "SyntaxColorizer.h"

class CColorRichEditView : public CRichEditView
{
	
	DECLARE_DYNAMIC(CColorRichEditView)
public:
	CColorRichEditView( MorphoWizard& morphWizard ) 
		: m_morphWizard(morphWizard), m_redrawInProcess(false) {}


	void	RedrawLine( int index=-1, bool bModified=true );
	void	RedrawLines( bool bModified=true );

	CSyntaxColorizer m_Colorizer;
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	MorphoWizard& m_morphWizard;

	bool	m_redrawInProcess;
	void	SetAccentFromThisLineToTheEndofParadigm();
	void	SetAccentByIndex(int ind);
	UINT	GetCodePage() const; 
public:
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString GetText() const;
	void  SetText(const CString& S);
};


//----------------------------------------------------------------------------
class CRichEditRedrawer
{
public:
	CRichEditRedrawer( CColorRichEditView& view );
	~CRichEditRedrawer();

	void	RedrawLine( int index=-1, bool bModified=true );

private:
	CRichEditCtrl& GetRichEditCtrl()	{ return m_view.GetRichEditCtrl(); }

	CColorRichEditView& m_view;
	long	m_selBegin;
	long	m_selEnd;
};

