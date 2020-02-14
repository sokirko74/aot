

// FormulaEditor.cpp : implementation file
//

#include "stdafx.h"


#pragma warning (disable : 4786)

#include "FormulaEditor.h"
#include "ValueDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CFormulaEditor::CFormulaEditor(CRossDoc* pRossDoc,
					   vector<TCortege>& OldValue,
		               vector<TCortege>& NewValue,
			           CRossDevTextField& F,
					   CWnd* pParent /*=NULL*/)
	: CDialog(CFormulaEditor::IDD, pParent),
	  m_OldValue (OldValue),
	  m_NewValue (NewValue),
	  m_Field (F),
	  m_pRossDoc(pRossDoc)
{
	//{{AFX_DATA_INIT(CSetEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CFormulaEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormulaEditor)
	DDX_GridControl(pDX, IDC_GRID, m_Grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormulaEditor, CDialog)
	//{{AFX_MSG_MAP(CFormulaEditor)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_FORMULAEDITOR_VALUE, OnFormulaEditorValue)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormulaEditor message handlers

bool CFormulaEditor::SetRowNumbers() 
{

	 m_Grid.SetRowCount(m_RowsCount+1);
	 m_Grid.SetColumnCount(m_ColumnsCount+1);
   	 m_Grid.SetFixedColumnCount(1);

     for (size_t i=0; i<m_RowsCount+1; i++)
	 {
       CString S;
	   S.Format ("%i", i+1);
	   m_Grid.SetItemText(i, 0, S);
	   m_Grid.SetItemText(i, m_ColumnsCount, "                  ");
	 };
	 m_Grid.AutoSize();

	 return false;
};


size_t  CFormulaEditor::GetEmptyCellNo (size_t RowNo)
{
	size_t i=1;
	for (; i < m_ColumnsCount; i++)
		if (m_Grid.GetItemText(RowNo,i).IsEmpty())
			break;

    return i;
};

BOOL CFormulaEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_RowsCount = 0; 
	m_ColumnsCount = 0;
	int LevelId =0;
	int k = 0 ;

    for (size_t i=0; i<m_OldValue.size(); i++)
		if (LevelId != m_OldValue [i].m_LevelId)
		{
			LevelId = m_OldValue [i].m_LevelId;
			if (m_ColumnsCount < k) m_ColumnsCount = k;
			k = 1;
			if (LevelId > m_RowsCount)
				m_RowsCount = LevelId;
		}
		else
			k++;

     if (m_ColumnsCount < k) m_ColumnsCount = k;

     //для номеров строк
	 m_ColumnsCount++;

	 // установка ширины формулы
	 SetRowNumbers();

	 // запись значений полей в таблицу 
	 for (size_t i=0; i<m_OldValue.size(); i++)
	 {
			CString Q = WriteToString(GetRoss(), GetRoss()->Fields[m_OldValue[i].m_FieldNo].m_Signats[m_OldValue[i].GetSignatNo()].sFrmt, m_OldValue[i]).c_str();
			m_Grid.SetItemText(m_OldValue[i].m_LevelId-1, GetEmptyCellNo(m_OldValue[i].m_LevelId-1), Q);
	 };

	 m_Grid.AutoSize();
     
 	return TRUE;  
	              
}

void CFormulaEditor::OnOK() 
{
	CString S = ConstructFldName(GetRoss(), m_Field.FieldNo, m_Field.LeafId, m_Field.BracketLeafId) + CString(" = ");
	int LevelId = 1;
	for (size_t i=0; i<m_RowsCount; i++)
	{
		CString Line;

		// printing level id
		Line.Format(" %i ",LevelId);

		// printing values 
		for (size_t k=1; k<m_ColumnsCount; k++)
		{
			CString Q =  m_Grid.GetItemText(i, k);
			Q.TrimLeft();
			if (!Q.IsEmpty())
			Line += Q + CString ("\r\n");
		};
		Line.TrimLeft();;
		if (Line.IsEmpty()) 
		{
			continue;
		}
		S += Line;
		LevelId++;
   };

   try {
	   CTempArticle A;
	   A.m_pRoss = GetRoss();
	   A.SetArticleStr( (const char*)S );
	   size_t Count = A.GetCortegesSize();
	   m_NewValue.clear();
	   for (int i=0;  i < Count; i++)
		   m_NewValue.push_back(A.GetCortege(i));
   }
   catch (...)
   {
	   AfxMessageBox ("Cannot parse " + S);
	   return;

   };
	
   CDialog::OnOK();
}

void CFormulaEditor::GetSelectedItem(int& nCol, int& nRow) const
{
	for (nRow =0; nRow <= m_RowsCount; nRow++)
		for (nCol =0; nCol <= m_ColumnsCount; nCol++)
			if (m_Grid.GetItemState(nRow,nCol) & GVIS_SELECTED)
				return;

	nCol  = -1;
	nRow = -1;
};

// deleting one line
void CFormulaEditor::OnButton1() 
{
	if (m_RowsCount == 0)  return;
	int nRow, nCol;
	GetSelectedItem(nCol, nRow);
    m_Grid.DeleteRow(nRow);
	m_RowsCount--;
	SetRowNumbers();
}


// add new line
void CFormulaEditor::OnButton2() 
{
	m_RowsCount++;
	SetRowNumbers();
}


// добавить один столбец 
void CFormulaEditor::OnButton3() 
{
	m_ColumnsCount++;
    SetRowNumbers();
}

// удалить один столбец 
void CFormulaEditor::OnButton4() 
{
	int nRow, nCol;
	GetSelectedItem(nCol, nRow);
	if (nRow == -1)
	{
		AfxMessageBox ("No selection");
		return;
	};
	m_Grid.SetItemText(nRow, nCol, " ");
	UpdateData(FALSE);
}

// редактировать один кортеж
void CFormulaEditor::OnFormulaEditorValue() 
{
	
	TCortege OldValue;
	int nRow, nCol;
	GetSelectedItem(nCol, nRow);

	if (nRow == -1)
	{
		AfxMessageBox ("No selection");
		return;
	};

	CString OneValue =  m_Grid.GetItemText(nRow, nCol);
    CString S  = ConstructFldName(GetRoss(), m_Field.FieldNo, m_Field.LeafId, m_Field.BracketLeafId) + CString(" = 1 ") + OneValue; 
	try
	{
		CTempArticle A;
		A.m_pRoss = GetRoss();
		A.SetArticleStr( (const char*)S );
        OldValue = A.GetCortege(0);
	}
	catch (...)
	{
	};

	TCortege NewValue;
	OldValue.m_FieldNo = m_Field.FieldNo;
  	CValueDlg Dlg (m_pRossDoc, OldValue, NewValue);
	try 
	{
		  if (Dlg.DoModal() != IDOK) return;
	}
	catch (...)
	{
			return;
	};


	OneValue = WriteToString(GetRoss(), GetRoss()->Fields[NewValue.m_FieldNo].m_Signats[NewValue.GetSignatNo()].sFrmt, NewValue).c_str();
	m_Grid.SetItemText(nRow, nCol, OneValue);

	if (!OneValue.IsEmpty())
	{
		if (nRow == m_RowsCount)
	 		m_RowsCount++;
		if (nCol == m_ColumnsCount)
	 		m_ColumnsCount++;

		SetRowNumbers();
	};


}



