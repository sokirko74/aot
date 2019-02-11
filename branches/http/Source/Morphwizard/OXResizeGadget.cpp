#include "stdafx.h"
#include "OXResizeGadget.h"

#ifdef DETECT_MEMORY_LEAK
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/////////////////////////////////////////////////////////////////////////////
// Gadget resizing functions - used to resize all the child windows of a dialog
// bar above. Routines extracted as they may be more used in general

// For a given window, creates an array of the child windows, that can be used
// for resizing later on.
GADGETRESIZEHANDLE CreateGadgetResizeHandle(CWnd* pWnd)
	{
	CPtrArray* pArray = new CPtrArray;
	
	// 1st element of the array stores the client rectangle of the given window
	RECT_AND_HWND* pRectWnd = new RECT_AND_HWND;
	pWnd->GetClientRect(&(pRectWnd->m_rect));
	pRectWnd->m_hWnd = NULL;
	pArray->Add(pRectWnd);
	TCHAR szClassName[64];
	
	for (HWND hWndChild = ::GetTopWindow(pWnd->m_hWnd); hWndChild != NULL;
	hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
		RECT_AND_HWND* pRectWnd = new RECT_AND_HWND;
		pRectWnd->m_hWnd = hWndChild;
		GetClassName(pRectWnd->m_hWnd, szClassName, 64); 
		::GetWindowRect(hWndChild, &(pRectWnd->m_rect));
		pWnd->ScreenToClient(&(pRectWnd->m_rect));

		// if it's a drop down combo box, then we set bottom -ve to indicate
		// we should keep the height fixed. Otherwise we might find that the drop
		// down size gets stupidly small.
		if (_tcscmp(szClassName, _T("ComboBox")) == 0 &&
			(GetWindowLong(hWndChild, GWL_STYLE) & (CBS_DROPDOWN | CBS_DROPDOWNLIST)) )
			{
			pRectWnd->m_rect.bottom = - (pRectWnd->m_rect.bottom - pRectWnd->m_rect.top);
			}

		pArray->Add(pRectWnd);
		}
	
	return (GADGETRESIZEHANDLE) pArray;
	}


// deletes the underlying array and it's elements
void DestroyGadgetResizeHandle(GADGETRESIZEHANDLE Handle)
	{
	CPtrArray* pArray = (CPtrArray*)Handle;		
	if (pArray != NULL)
		{
		for (int i = pArray->GetUpperBound(); i >= 0; i--)
			delete ((RECT_AND_HWND*)pArray->GetAt(i));
		delete pArray;
		}
	}


// Resizes all the gadgets in an array to match the new size.
// Generally called from an OnSize routine (or the like)
void ResizeGadgetsOnWindow(GADGETRESIZEHANDLE Handle, int cx, int cy)
	{
	CPtrArray* pArray = (CPtrArray*)Handle;		
	
	if (pArray == NULL || cx == 0 || cy == 0)
		return;
	
	ASSERT(pArray->GetSize() > 0);	// must be at least one element in the array
	
	// if auto-sizing, go thru and reposition all the controls.
	CSize sizeWndOrig = ((RECT_AND_HWND*)pArray->GetAt(0))->m_rect.Size();
	for (int i = 1; i < pArray->GetSize(); i++)
		{
		RECT_AND_HWND* pRectWnd = (RECT_AND_HWND*)pArray->GetAt(i);
		CRect newRect = pRectWnd->m_rect;
		CSize sizeCtlOrig = newRect.Size();
		newRect.top    = newRect.top    * cy / sizeWndOrig.cy;
		newRect.left   = newRect.left   * cx / sizeWndOrig.cx;
		newRect.right  = newRect.right  * cx / sizeWndOrig.cx;
		// if bottom is < 0, then kepp it fixed height - used for combo boxes
		// where we don't really want to resize most of them...
		if (newRect.bottom < 0)
			newRect.bottom = newRect.top - newRect.bottom;
		else 
			newRect.bottom = newRect.bottom * cy / sizeWndOrig.cy;

		::SetWindowPos(pRectWnd->m_hWnd, NULL,
			newRect.left, newRect.top, newRect.Width(), newRect.Height(),
			SWP_NOZORDER);
		}
	}
