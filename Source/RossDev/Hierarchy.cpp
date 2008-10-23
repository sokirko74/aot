// graphtest3View.cpp : implementation of the CHierarchyView class
//

#include "stdafx.h"

#include "HierarchyDoc.h"
#include "Hierarchy.h"
#include "..\rossdev\resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Tcl_Interp* theInterp;

int TclWindowCounter = 0;

/////////////////////////////////////////////////////////////////////////////
// CHierarchyView

IMPLEMENT_DYNCREATE(CHierarchyView, CView)

BEGIN_MESSAGE_MAP(CHierarchyView, CView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CHierarchyView)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()


bool CHierarchyView::FindRelation (size_t  node1, size_t node2)
{
	for (size_t i = 0; i < GetDocument()->Relations.size(); i++)
		if  (     (node1 == GetDocument()->Relations[i].node1)
			   && (node2 == GetDocument()->Relations[i].node2) )
        return true;
    return false;

};


bool CHierarchyView::FindOmniDirectionRelation (size_t  node1, size_t node2)
{
	return     FindRelation (node1, node2)
		   ||  FindRelation (node2, node1);
		  

};


/////////////////////////////////////////////////////////////////////////////
// CHierarchyView construction/destruction


CHierarchyView::CHierarchyView()
{
	// TODO: add construction code here
	m_NewNodeCount = 0;
    m_tkwin=0;
	m_tkhwnd=0;


}

CHierarchyView::~CHierarchyView()
{
}

void CHierarchyView::OnDraw(CDC* pDC)
{
	CHierarchyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


/////////////////////////////////////////////////////////////////////////////
// CHierarchyView diagnostics

#ifdef _DEBUG
void CHierarchyView::AssertValid() const
{
	CView::AssertValid();
}

void CHierarchyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHierarchyDoc* CHierarchyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHierarchyDoc)));
	return (CHierarchyDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHierarchyView message handlers





void CHierarchyView::CreateRelation(int node1, int node2, bool bWork) 
{
	GetDocument()->Relations.push_back(CHierarchyRelation(node1, node2,"",bWork));
  char s[2000];
  sprintf (s, "$GT($main,graph) create edge $nods(%i) $nods(%u)", node1, node2);
  int retcode = Tcl_Eval(theInterp,s);
};



void CHierarchyView::CreateNode(const char* Name, CPoint p)
{
	CNodePlace P;
	P.x = p.x;
	P.y = p.y;
	GetDocument()->Nodes.push_back(CHierarchyNode(Name, P));
	char s[2000];
	sprintf (s, "set nods(%i) [$GT($main,graph) create node]", GetDocument()->Nodes.size() - 1);
	int retcode = Tcl_Eval(theInterp,s);
    sprintf (s, "$GT($main,graph) set $nods(%i)  -x %u -y %u -label \"%s\"", GetDocument()->Nodes.size() - 1, p.x, p.y, Name);
	retcode = Tcl_Eval(theInterp,s);
	
};


void CHierarchyView::ShowGraph() 
{
	char s[200];
	sprintf (s, "$GT($main,graph) draw");
    Tcl_Eval(theInterp,s);
};

// добавить узел
void CHierarchyView::OnButton1() 
{
	// TODO: Add your control notification handler code here
	
}

void CHierarchyView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}

void SwitchGraph(const char* tkname);


BOOL CHierarchyView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
   char s[2000];
	
   BOOL ok = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

   if (ok) {

		int retcode;
		CString winhnd, cmd;

		m_tkname.Format(".tk%d",++TclWindowCounter);   // name this window .tk1 .tk2 etc

		winhnd.Format("0x%08X",m_hWnd);
		cmd.Format("toplevel %s -use %s",m_tkname,winhnd);
        strcpy (s, cmd);
		retcode = Tcl_Eval(theInterp,s);
		if (retcode!=TCL_OK) {
			Tcl_SetResult(theInterp,"Cannot create TK child window",TCL_VOLATILE);
			return ok;
		}

		Tk_Window tkmain = Tk_MainWindow(theInterp);
		if (tkmain) {
			
			//get the tk window token
			strcpy (s, m_tkname);
			m_tkwin = Tk_NameToWindow(theInterp, s, tkmain);
			Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT);   //force window creation
			
			// get the HWND
			cmd.Format("winfo id %s",m_tkname);
			strcpy (s, cmd);
			retcode = Tcl_Eval(theInterp,s);
			if (retcode==TCL_OK) {
				int i;
				retcode = Tcl_GetInt(theInterp,theInterp->result,&i);
				if (retcode==TCL_OK) {
				  m_tkhwnd = (HWND)i;
				}
			}
			SwitchGraph(m_tkname);

			CString cmd;
            cmd.Format("source $env(GRAPHLET_DIR)/lib/graphscript/ross/graph.tcl");
            strcpy (s, cmd);
            retcode = Tcl_Eval(theInterp,s);
	        if (retcode!=TCL_OK) {
				AfxMessageBox (theInterp->result);
				return FALSE;
	};

		}
	}
	return ok;
	
}

void CHierarchyView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (m_tkwin) {
		CRect clrect;
		GetClientRect(clrect);
		Tk_MoveResizeWindow(m_tkwin,0,0,clrect.Width(),clrect.Height());
		char s[2000];
		sprintf (s, "place  configure $main.drawing -x 1 -y 1 -height %u -width %u", clrect.Height(), clrect.Width());;
		int retcode = Tcl_Eval(theInterp,s);

		sprintf (s, "place configure $GT($main,canvas) -x 1 -y 1 -height %u -width %u", clrect.Height(), clrect.Width());;
		retcode = Tcl_Eval(theInterp,s);
	}
	
}

BOOL CHierarchyView::OnEraseBkgnd(CDC* pDC) 
{
	return 0; // let tk do it
}

void CHierarchyView::OnDestroy() 
{
    CString cmd;
	cmd.Format("destroy %s",m_tkname);
	char s[1000];
	strcpy (s, cmd);
	Tcl_Eval(theInterp,s);

	CView::OnDestroy();
	
	// TODO: Add your message handler code here

}

LRESULT CHierarchyView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// pass some messages thru to Tk child window
	if (m_tkhwnd) {
		switch (message) {
			case WM_KEYDOWN:
			case WM_CHAR:
			case WM_KEYUP:
			case WM_SETFOCUS:
 				::SendMessage(m_tkhwnd,message,wParam,lParam);
				break;
		}
	}

	return CView::WindowProc(message, wParam, lParam);
}


void CHierarchyView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	SwitchGraph(m_tkname); 
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
