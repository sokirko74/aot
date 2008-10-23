/****************************************************************************
FileEditCtrl.h : header file for the CFileEditCtrl control class
written by PJ Arends
pja@telus.net

For updates check http://www3.telus.net/pja/CFileEditCtrl.htm

-----------------------------------------------------------------------------
This code is provided as is, with no warranty as to it's suitability or usefulness
in any application in which it may be used. This code has not been tested for
UNICODE builds, nor has it been tested on a network ( with UNC paths ).

This code may be used in any way you desire. This file may be redistributed by any
means as long as it is not sold for profit, and providing that this notice and the
authors name are included.

If any bugs are found and fixed, a note to the author explaining the problem and
fix would be nice.
-----------------------------------------------------------------------------
****************************************************************************/

#if !defined(AFX_FILEEDITCTRL_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_)
#define AFX_FILEEDITCTRL_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _SHLOBJ_H_
#include <shlobj.h>  // for SHBrowseForFolder() and BROWSEINFO structure
#endif // _SHLOBJ_H_

#ifndef __AFXCMN_H__
#include <afxcmn.h>  // for CToolTipCtrl
#endif // __AFXCMN_H__

// PJAImage.h has to be included if you want to use a bitmap or icon on the
// button. If you do not want to use an image on the button, simply comment
// out the '#include "PJAImage.h"' line below, and the button will have the
// ellipses drawn on it. If you include "PJAImage.h" you have to add the 
// "PJAImage.cpp" file to your project.
//#include "PJAImage.h"

/////////////////////////////////////////////////////////////////////////////
// CFECFileDialog dialog

// The sole purpose for this class is to change the
// text of the "OPEN" button to "OK".

class CFECFileDialog : public CFileDialog
{
    DECLARE_DYNAMIC(CFECFileDialog)

public:
    CFECFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnInitDone();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFileEditCtrl control

// Flags used by dwFlags parameter of the Create(), SetFlags() and DDX_FileEditCtrl() functions
// also returned by GetFlags() and used by ModifyFlags()
//
// Usage :
// FEC_FILE               - the control is used to find a file, can not be used with FEC_FOLDER
// FEC_MULTIPLE           - used with FEC_FILE, the control is used to find multiple files (same as OFN_ALLOWMULTISELECT)
// FEC_FOLDER             - the control is used to find folders, can not be used with FEC_FILE
// FEC_TRAILINGSLASH      - used with FEC_FOLDER, adds a slash to the end of the folder string
// FEC_BUTTONLEFT         - places the button on the left side of the control
// FEC_BUTTONTIP          - enables the tooltip for the browse button
// FEC_CLIENTTIP          - enables the tooltip for the client area
// FEC_WILDCARDS          - used with FEC_FILE, the control resolves wildcards ('*' and '?') in the file name
// FEC_NODEREFERENCELINKS - used with FEC_FILE, the control will return the pathname of the shortcut (*.lnk & *.pif) file.
//                          by default, the control returns the pathname of the file referenced by the shortcut
//                          (same as OFN_NODEREFERENCELINKS)
// FEC_AUTOCOMPLETE       - adds an autocompletion feature to the control ( a work in progress )
// FEC_FLAT               - the control is displayed as a flat, hot to mouse control
// FEC_GRAYSCALE          - used with FEC_FLAT, the button's dots/image is drawn in gray when the control is flat

#define FEC_FILE               0x00000001
#define FEC_MULTIPLE           0x00000002
#define FEC_MULTIPLEFILES      0x00000003   /* FEC_FILE | FEC_MULTIPLE */
#define FEC_FOLDER             0x00000004
#define FEC_TRAILINGSLASH      0x00000008
#define FEC_BUTTONLEFT         0x00000010
#define FEC_BUTTONTIP          0x00000020
#define FEC_CLIENTTIP          0x00000040
#define FEC_WILDCARDS          0x00000080
#define FEC_NODEREFERENCELINKS 0x00000100
#define FEC_AUTOCOMPLETE       0x00000200   /* Not yet implemented */
#define FEC_FLAT               0x00000400
#define FEC_GRAYSCALE          0x00000800

class CFileEditCtrl : public CEdit
{
    DECLARE_DYNAMIC (CFileEditCtrl)

public:
// Constructor & Destructor
    CFileEditCtrl(BOOL bAutoDelete = FALSE);    // constructor
    virtual ~CFileEditCtrl();                   // destructor

// Overrides
    virtual BOOL Create(DWORD dwFlags,          // FEC_* flags
                        DWORD dwExStyle,        // WS_EX_* styles
                        LPCTSTR lpszWindowName, // initial text
                        DWORD dwStyle,          // WS_* and ES_* styles
                        const RECT& rect,       // control's size and position
                        CWnd* pParentWnd,       // control's parent window
                        UINT nID);              // control's ID
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void PostNcDestroy();

public:
// User functions
    BROWSEINFO* GetBrowseInfo() const;      // returns a pointer to the internal BROWSEINFO structure
    int GetButtonWidth();                   // returns the width, in pixels, of the browse button
    DWORD GetFlags();                       // returns the control's functionality
    CString GetNextPathName(POSITION &pos); // get the file at pos and then update pos
    OPENFILENAME* GetOpenFileName() const;  // returns a pointer to the internal OPENFILENAME structure
    POSITION GetStartPosition();            // get the starting position for GetNextPathName()
    BOOL ModifyFlags(DWORD remove, DWORD add); // modifies the control's functionality
    int SetButtonWidth(int width = -1);     // sets the width of the browse button (-1 is use default width)
    void SetClientTipText(CString text);    // sets the text for the client area tooltip
    BOOL SetFlags(DWORD dwFlags);           // sets the control's functionality

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
    BOOL SetButtonImage(HANDLE hImage, DWORD PJAIFlags, COLORREF Transparent = CLR_DEFAULT); // set the image for the browse button
#endif

protected:
// Internal methods
    void AddFile(CString FileName);         // adds files to the m_files array
    void ButtonClicked();                   // handles a mouse click on the button
    BOOL DereferenceLink(CString &FileName);// resolves shortcut (*.lnk and *.pif) files
    void DrawButton (int nButtonState = 0); // draws the button
    void DrawDots (CDC *pDC, COLORREF CR, int nOffset = 0); // draws the dots on the button
    void ExpandWildCards(const CString &FileName); // handles wildcards in file names
    BOOL FECBrowseForFolder();              // starts and handles the returns from the SHBrowseForFolder() shell function
    BOOL FECOpenFile();                     // starts and handles the returns from the CFileDialog
    void FillBuffers();                     // fills the buffers used by GetStartPosition() and GetNextPathName() functions
    BOOL GetValidFolder(LPTSTR Path);       // converts Path to an existing folder path
	void Redraw (BOOL Flat);                // redraws the borders if the FEC_FLAT flag is set
    BOOL ScreenPointInButtonRect(CPoint point); // checks if the given point is in the browse button

// Message handlers
    BOOL OnTTNNeedText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult); // Set the tooltip text
    afx_msg BOOL OnChange();
    afx_msg LRESULT OnSetReadOnly(WPARAM, LPARAM);
    afx_msg LRESULT OnSetText(WPARAM, LPARAM);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
    afx_msg void OnNcPaint();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()

// member variables
private:
	DWORD            ImageDrawFlags;        // button image flags
    BOOL             m_bAutoDelete;         // delete this in PostNCDestroy() handler?
    DWORD            m_bButtonLeft;         // browse button on left side of control?
    BOOL             m_bMouseCaptured;      // button has captured the mouse?
    BOOL             m_bTextChanged;        // window text has changed since last time FillBuffers() was called
    DWORD            m_dwFlags;             // control flags - do not access this member directly, use GetFlags() instead
    CStringArray     m_Files;               // file names
    int              m_nButtonState;        // current button state (up, down, or disabled)
    int              m_nButtonWidth;        // the width of the button (-1 is default)
    BROWSEINFO*      m_pBROWSEINFO;         // only active when FEC_FOLDER flag is set
    CFECFileDialog*  m_pCFileDialog;        // only active when FEC_FILE flag is set
    CRect            m_rcButtonRect;        // window coordinates of the button
    CString          m_szCaption;           // caption of CFileDialog
    CString          m_szClientTip;         // client area tooltip text
    CString          m_szFolder;            // absolute path to first file in m_Files.
    CToolTipCtrl     m_ToolTip;             // tooltip control

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
    CPJAImage*       m_pButtonImage;        // pointer to the button image class
#endif
};

/////////////////////////////////////////////////////////////////////////////
// FEC_NOTIFY structure

// A pointer to this structure is sent to the parent window of the CFileEditCtrl as the
// NMHDR* pNMHDR parameter in a WM_NOTIFY message. It is sent with the FEC_NM_PREBROWSE
// notification after the user has clicked on the browse button, but before the CFileDialog
// or SHBrowseForFolder dialogs are displayed. Setting the LRESULT parameter of the OnNotify
// handler to a nonzero value will stop the dialogs from executing. It is sent with the
// FEC_NM_POSTBROWSE notification after the user has pressed the 'OK' button on these dialogs.
typedef struct tagFEC_NOTIFY {
    NMHDR hdr;
    CFileEditCtrl* pFEC;                    // pointer to control that sends this notification
    tagFEC_NOTIFY (CFileEditCtrl *FEC, UINT code); // Constructor
} FEC_NOTIFY;

#define FEC_NM_PREBROWSE  1                 // notification code sent before dialogs pop up
#define FEC_NM_POSTBROWSE 2                 // notification code sent after dialogs successfully return
#define FEC_NM_DROP       3                 // notification code sent when user drops files onto control

/////////////////////////////////////////////////////////////////////////////
// DDV_/DDX_FileEditCtrl functions
void DDV_FileEditCtrl (CDataExchange *pDX, int nIDC);   // verify that the file/folder entered exists
void DDX_FileEditCtrl (CDataExchange *pDX, int nIDC, CFileEditCtrl &rCFEC, DWORD dwFlags); // setup the control
void DDX_FileEditCtrl (CDataExchange *pDX, int nIDC, CString& rStr, DWORD dwFlags);        // setup the control

/////////////////////////////////////////////////////////////////////////////
// FECFolderProc callback function used by SHBrowseForFolder function
int CALLBACK FECFolderProc(HWND hWnd, UINT nMsg, LPARAM, LPARAM lpData);

#endif // !defined(AFX_FILEEDITCTRL_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_)

/////////////////////////////////////////////////////////////////////////////
//
//  End of FileEditCtrl.h
//
/////////////////////////////////////////////////////////////////////////////
