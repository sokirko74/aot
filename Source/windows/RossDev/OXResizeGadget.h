#ifndef __RESIZEGADGET_H__
#define __RESIZEGADGET_H__


typedef void* GADGETRESIZEHANDLE;

typedef struct _Rect_And_Hwnd
	{
	CRect 	m_rect;
	HWND	m_hWnd;
	}	RECT_AND_HWND;

GADGETRESIZEHANDLE CreateGadgetResizeHandle(CWnd* pWnd);
void DestroyGadgetResizeHandle(GADGETRESIZEHANDLE Handle);
void ResizeGadgetsOnWindow(GADGETRESIZEHANDLE Handle, int cx, int cy);

#endif
