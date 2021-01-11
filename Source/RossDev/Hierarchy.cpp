#include "stdafx.h"

#include "HierarchyDoc.h"
#include "Hierarchy.h"
#include "..\rossdev\resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHierarchyView, CView)

BEGIN_MESSAGE_MAP(CHierarchyView, CView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CHierarchyView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()


bool CHierarchyView::FindRelation(size_t  node1, size_t node2)
{
	for (size_t i = 0; i < GetDocument()->Relations.size(); i++)
		if ((node1 == GetDocument()->Relations[i].node1)
			&& (node2 == GetDocument()->Relations[i].node2))
			return true;
	return false;

};


bool CHierarchyView::FindOmniDirectionRelation(size_t  node1, size_t node2)
{
	return     FindRelation(node1, node2)
		|| FindRelation(node2, node1);


};


CHierarchyView::CHierarchyView() : m_TclInterp(((CRossDevApp*)AfxGetApp())->m_TclInterp)
{
	// TODO: add construction code here
	m_NewNodeCount = 0;

}

CHierarchyView::~CHierarchyView()
{
}

void CHierarchyView::OnDraw(CDC* pDC)
{
	CHierarchyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
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


void CHierarchyView::CreateRelation(int node1, int node2, bool bWork)
{
	GetDocument()->Relations.push_back(CHierarchyRelation(node1, node2, "", bWork));
	m_TclInterp.RunTcl(Format("$GT($main,graph) create edge $nods(%i) $nods(%u)", node1, node2));
};


void CHierarchyView::CreateNode(const char* Name, CPoint p)
{
	CNodePlace P;
	P.x = p.x;
	P.y = p.y;
	GetDocument()->Nodes.push_back(CHierarchyNode(Name, P));
	m_TclInterp.RunTcl(Format("set nods(%i) [$GT($main,graph) create node]", GetDocument()->Nodes.size() - 1));
	m_TclInterp.RunTcl(Format("$GT($main,graph) set $nods(%i)  -x %u -y %u -label \"%s\"", GetDocument()->Nodes.size() - 1, p.x, p.y, Name));
};


void CHierarchyView::ShowGraph()
{
	m_TclInterp.RunTcl("$GT($main,graph) draw");
};


BOOL CHierarchyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_TopTKWindow = m_TclInterp.UseTopWindow(m_hWnd);
	SwitchMainTkWindow();
	m_TclInterp.RunTcl("source $env(GRAPHLET_DIR)/lib/graphscript/ross/graph.tcl");
	return CView::OnCreate(lpCreateStruct);
}


void CHierarchyView::SwitchMainTkWindow()
{
	m_TclInterp.RunTcl(Format("set main %s", m_TopTKWindow.m_tkname.c_str()));
};

void CHierarchyView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_TopTKWindow.m_tkwin) {
		CRect clrect;
		GetClientRect(clrect);
		Tk_MoveResizeWindow(m_TopTKWindow.m_tkwin, 0, 0, clrect.Width(), clrect.Height());
		m_TclInterp.RunTcl(Format("place  configure $main.drawing -x 1 -y 1 -height %u -width %u", clrect.Height(), clrect.Width()));;
		m_TclInterp.RunTcl(Format("place configure $GT($main,canvas) -x 1 -y 1 -height %u -width %u", clrect.Height(), clrect.Width()));
	}

}

BOOL CHierarchyView::OnEraseBkgnd(CDC* pDC)
{
	return 0; // let tk do it
}

void CHierarchyView::OnDestroy()
{
	m_TclInterp.RunTcl(Format("destroy %s", m_TopTKWindow.m_tkname.c_str()));
	CView::OnDestroy();
}

LRESULT CHierarchyView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// pass some messages thru to Tk child window
	if (m_TopTKWindow.m_tkhwnd) {
		switch (message) {
		case WM_KEYDOWN:
		case WM_CHAR:
		case WM_KEYUP:
		case WM_SETFOCUS:
			::SendMessage(m_TopTKWindow.m_tkhwnd, message, wParam, lParam);
			break;
		}
	}

	return CView::WindowProc(message, wParam, lParam);
}


void CHierarchyView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	SwitchMainTkWindow();
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
