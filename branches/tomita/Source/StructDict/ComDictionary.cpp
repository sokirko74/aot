#include "StdPch.h"
#include "ComDictionary.h"
#include "ComTempArticle.h"
#include "ComDomen.h"
#include "ComSignat.h"
#include "ComField.h"

STDMETHODIMP CComDictionary::LoadWithoutComments(BSTR Path)
{
	_bstr_t t = Path;

	if (!CDictionary::Load( (const char*) t) )
		return  E_FAIL;
	else
		return   S_OK;
}


STDMETHODIMP CComDictionary::GetUnitsSize(WORD *result)
{
	*result = m_Units.size();
	return S_OK;
}



STDMETHODIMP CComDictionary::GetUnitStr(WORD UnitNo, BSTR *result)
{
	_bstr_t t = GetUnits()[UnitNo].m_EntryStr;
	*result = t.copy();
	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitMeanNum(WORD UnitNo, BYTE* result)
{
	// TODO: Add your implementation code here
	*result = GetUnits()[UnitNo].m_MeanNum; 
	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitStartPos(WORD UnitNo, LONG *result)
{
	// TODO: Add your implementation code here
	*result = GetUnits()[UnitNo].m_StartCortegeNo; 

	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitEndPos(WORD UnitNo, LONG *result)
{
	*result  = GetUnits()[UnitNo].m_LastCortegeNo; 

	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitSelected(WORD UnitNo, BOOL *result)
{
	*result  = GetUnits()[UnitNo].m_bSelected; 
	return S_OK;
}


STDMETHODIMP CComDictionary::GetUnitAuthor(WORD UnitNo, BSTR *result)
{
	_bstr_t t = GetUnits()[UnitNo].m_AuthorStr;
	*result = t.copy() ; 
	return S_OK;
}

STDMETHODIMP CComDictionary::IsEmptyArticle(WORD UnitNo, BOOL *result)
{
	*result = GetUnits()[UnitNo].HasEmptyArticle() ? TRUE : FALSE;
	return S_OK;
}



STDMETHODIMP CComDictionary::GetCortegeFieldNo(LONG CortegeNo, BYTE *result)
{
	*result = GetCortege(CortegeNo)->m_FieldNo;
	return S_OK;
}

STDMETHODIMP CComDictionary::GetCortegeSignatNo(LONG CortegeNo, BYTE *result)
{
	// TODO: Add your implementation code here
	*result = GetCortege(CortegeNo)->m_SignatNo;
	return S_OK;
}

STDMETHODIMP CComDictionary::GetCortegeLevelId(LONG CortegeNo, BYTE *result)
{
	// TODO: Add your implementation code here
	*result = GetCortege(CortegeNo)->m_LevelId;

	return S_OK;
}

STDMETHODIMP CComDictionary::GetCortegeLeafId(LONG CortegeNo, BYTE *result)
{
	*result = GetCortege(CortegeNo)->m_LeafId;
	return S_OK;
}

STDMETHODIMP CComDictionary::GetCortegeBracketLeafId(long CortegeNo, long* BracketLeafId)
{
	*BracketLeafId = GetCortege(CortegeNo)->m_BracketLeafId;

	return S_OK;
}


STDMETHODIMP CComDictionary::GetCortegeItem(LONG CortegeNo, BYTE PositionInCortege, LONG* ItemId)
{
	*ItemId = GetCortege(CortegeNo)->GetItem(PositionInCortege);

	return S_OK;
}

STDMETHODIMP CComDictionary::GetSelectedUnitsSize(LONG *result)
{
	*result = TRoss::GetSelectedUnitsSize();
	return S_OK;
}

STDMETHODIMP CComDictionary::LocateUnit(BSTR UnitStr, BYTE MeanNum, WORD *result)
{
	// TODO: Add your implementation code here
	_bstr_t t = UnitStr;
	*result =  TRoss::LocateUnit((const char*)t, MeanNum);
	return S_OK;
}


STDMETHODIMP CComDictionary::GetItemNoByItemStr(BSTR ItemStr, BYTE DomNo, LONG* result)
{
	// TODO: Add your implementation code here
	_bstr_t t = ItemStr;
	if (DomNo == ErrUChar)
      *result = -1;
	else
		*result = TRoss::GetItemNoByItemStr((const char*)t, DomNo);
	return S_OK;
}

STDMETHODIMP CComDictionary::GetDomItemStr(LONG ItemNo, BSTR *result)
{
	// TODO: Add your implementation code here
	_bstr_t t = (ItemNo == - 1) ? "" : GetDomItemStrInner(ItemNo);
	*result = t.copy();
	return S_OK;
}

const char NullStr[] = {'\0'};
STDMETHODIMP CComDictionary::GetDomItemStr_Inproc(LONG ItemNo, LONG *result)
{
	// TODO: Add your implementation code here
	*result = (ItemNo == - 1) ? (LONG)NullStr : (LONG)GetDomItemStrInner(ItemNo);
	return S_OK;
}


STDMETHODIMP CComDictionary::DelCorteges(LONG StartPos, LONG EndPos)
{
	// TODO: Add your implementation code here
	TRoss::DelCorteges(StartPos, EndPos);
	return S_OK;
}

STDMETHODIMP CComDictionary::GetCortegesSize(LONG *result)
{
	// TODO: Add your implementation code here
	*result = TRoss::_GetCortegesSize();

	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitStartPos(WORD UnitNo, LONG NewValue)
{
	m_Units[UnitNo].m_StartCortegeNo = NewValue;

	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitEndPos(WORD UnitNo, LONG NewValue)
{
	// TODO: Add your implementation code here
	m_Units[UnitNo].m_LastCortegeNo = NewValue;

	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitCurrentTime(WORD UnitNo)
{
	// TODO: Add your implementation code here
	CDictionary::SetUnitCurrentTime (UnitNo);
	TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	C->modif_tm = RmlGetCurrentTime();
	return S_OK;
}

STDMETHODIMP CComDictionary::GetFieldNoByFieldStr(BSTR Str, BYTE* result)
{
	// TODO: Add your implementation code here
	_bstr_t t = Str;
	*result = TRoss::GetFieldNoByFieldStrInner((char*)t);
	return S_OK;
}


STDMETHODIMP CComDictionary::GetLexPlusDomNo(BYTE *result)
{
	*result = TRoss::LexPlusDomNo;

	return S_OK;
}


STDMETHODIMP CComDictionary::GetDomItemsSize(LONG *result)
{
	// TODO: Add your implementation code here
	*result = TRoss::GetDomItemsSize();

	return S_OK;
}

STDMETHODIMP CComDictionary::GetDomItemDomNo(LONG ItemNo, BYTE *result)
{
	*result = m_DomItems[ItemNo].GetDomNo();
	return S_OK;
}

STDMETHODIMP CComDictionary::InsertDomItem(BSTR ItemStr, BYTE DomNo, int *result)
{
	// TODO: Add your implementation code here
	_bstr_t t = ItemStr;

	if (!TRoss::InsertDomItem((const char*)t, DomNo, *result))
	{
		ErrorMessage(m_LastError);
		return E_FAIL;
	};

	return S_OK;
}





STDMETHODIMP CComDictionary::BuildSignat(ISignat* Signat, BSTR Domainstring, BSTR Name)
{
	_bstr_t t1 = Domainstring;
	_bstr_t t2 = Name;
    CComObject<CComSignat>* It = reinterpret_cast<CComObject<CComSignat>*>(Signat); 
	if (!CDictionary::BuildOneFieldFormat (*It->m_pSignat, (char*)t1, (char*)t2, m_MaxNumDom))
		ErrorMessage(m_LastError);
	return S_OK;
}


STDMETHODIMP CComDictionary::DelDomItem(LONG ItemNo)
{
	TRoss::DelDomItem(ItemNo);
	return S_OK;
}

STDMETHODIMP CComDictionary::GetFieldsSize(BYTE* result)
{
	*result = Fields.size();
    return S_OK;

};

STDMETHODIMP CComDictionary::GetDomainNoByDomStr(BSTR DomStr, BYTE *result)
{
	// TODO: Add your implementation code here
	_bstr_t t = DomStr;
	*result = TRoss::GetDomenNoByDomStr(t);
	return S_OK;
}

STDMETHODIMP CComDictionary::InsertUnit(BSTR UnitStr, BYTE MeanNum, LONG *result)
{
	*result = TRoss::InsertUnit((const char*)(_bstr_t)UnitStr, MeanNum);
	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitCommentStr(WORD UnitNo, BSTR Str)
{
	TRoss::SetUnitCommentStr(UnitNo, (const char*) _bstr_t(Str));
	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitSelected(WORD UnitNo, BOOL Selected)
{
	m_Units[UnitNo].m_bSelected = (Selected) ? true : false;
	return S_OK;
}

STDMETHODIMP CComDictionary::Save()
{
	TRoss::Save();
	return S_OK;
}

STDMETHODIMP CComDictionary::LoadUnitComments()
{
	try  
	{
	   ReadUnitComments();
	}
	catch (...)
	{
	};
	return S_OK;
}

STDMETHODIMP CComDictionary::GetFieldDomainNo(BYTE *FieldNo)
{
	*FieldNo = FieldDomNo;
	return S_OK;
}

STDMETHODIMP CComDictionary::GetDomainsSize(BYTE *result)
{
	*result = m_Domens.size();
	return S_OK;
}

STDMETHODIMP CComDictionary::BuildFormats()
{
	if (!TRoss::BuildFormats(m_MaxNumDom))
	{
		ErrorMessage(m_LastError);
		return E_FAIL;
	};

	return S_OK;
}

STDMETHODIMP CComDictionary::GetSelectedUnitNo(WORD UnitNo, WORD *result)
{
	*result = TRoss::GetSelectedUnitNo(UnitNo);
	return S_OK;
}


STDMETHODIMP CComDictionary::GetUnitModifTimeStr(WORD UnitNo, BSTR* result)
{
	_bstr_t t = TRoss::GetUnitModifTimeStr(UnitNo).c_str();
	*result = t.copy();
	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitAuthor(WORD UnitNo, BSTR Author)
{
	TRoss::SetUnitAuthor(UnitNo, (const char*) (_bstr_t) Author);
	return S_OK;
}

STDMETHODIMP CComDictionary::UnitsLowerBound(BSTR UnitStr, WORD *result)
{
	// TODO: Add your implementation code here
	_bstr_t t = UnitStr;
	*result = TRoss::UnitsLowerBound(t);
	return S_OK;
}

STDMETHODIMP CComDictionary::DelUnit(WORD UnitNo)
{
	TRoss::DelUnit(m_Units.begin() + UnitNo);
	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitCommentStr(WORD UnitNo, BSTR *result)
{
    TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	_bstr_t t = C->Comments;
	*result = t.copy();
	return S_OK;
}

STDMETHODIMP CComDictionary::GetWildCardDomItemNo(LONG *result)
{
	*result  = WildCardDomItemNo;
	return S_OK;
}

STDMETHODIMP CComDictionary::SetUnitStr(WORD UnitNo, BSTR UnitStr)
{
	CDictionary::SetUnitStr(UnitNo, (const  char*)_bstr_t(UnitStr));
	return S_OK;
}


STDMETHODIMP CComDictionary::get_Domains(BYTE DomNo, IDomain** pVal)
{
	CComObject<CComDomen>* It = new CComObject<CComDomen>; 
	It->m_pDomen = &( m_Domens[DomNo] );
	It->AddRef();
	*pVal = It;
	return S_OK;
}

STDMETHODIMP CComDictionary::put_Domains(BYTE DomNo, IDomain* newVal)
{
    CComObject<CComDomen>* It = reinterpret_cast<CComObject<CComDomen>*>(newVal); 
    m_Domens[DomNo] = *It->m_pDomen;
	return S_OK;
}


STDMETHODIMP CComDictionary::get_Fields(BYTE FieldNo, IField **pVal)
{
	CComObject<CComField>* It = new CComObject<CComField>; 
	It->m_pField = & ( Fields[FieldNo] );
    It->AddRef();
	*pVal = It;
	return S_OK;
}

STDMETHODIMP CComDictionary::put_Fields(BYTE FieldNo, IField* newVal)
{
	CComObject<CComField>* It = reinterpret_cast<CComObject<CComField>*>(newVal); 
	Fields[FieldNo] = *(It->m_pField);
	return S_OK;
}



STDMETHODIMP CComDictionary::IncludeArticle(WORD UnitNo, BSTR Article, BOOL *result)
{
	CComObject<CComTempArticle>* A1 = new CComObject<CComTempArticle>;
	A1->AddRef();
   
	A1->AttachToDictionary(this);
	A1->ReadFromDictionary(UnitNo, FALSE, TRUE);

	CComObject<CComTempArticle>* A2 = new CComObject<CComTempArticle>;
	A2->AddRef();
	A2->AttachToDictionary(this);
	A2->put_ArticleStr(Article);
	A2->MarkUp();
	A2->BuildCortegeList();
	A2->IsPartOf(A1, TRUE, result);
	A1->Release();
	A2->Release();
	return S_OK;
}

STDMETHODIMP CComDictionary::UpdateConstDomains()
{
	TRoss::UpdateConstDomens();
	return S_OK;
}

STDMETHODIMP CComDictionary::InitRusConsts()
{
	if (!TRoss::InitDomensConsts()) return E_FAIL;
	return S_OK;
}

STDMETHODIMP CComDictionary::get_MaxNumDom(BYTE *pVal)
{
	*pVal = m_MaxNumDom;
	return S_OK;
}


STDMETHODIMP CComDictionary::ClearUnit(WORD UnitNo)
{
	TRoss::ClearUnit(UnitNo);

	return S_OK;
}



STDMETHODIMP CComDictionary::CheckArticleError(BSTR Article, BSTR *Error, BOOL *Result)
{
	CComObject<CComTempArticle>* A = new CComObject<CComTempArticle>;
	A->AddRef();
	A->AttachToDictionary(this);
	*Result =  TRUE;
	if (A->put_ArticleStr(Article) == E_FAIL)
	{
		_bstr_t t = m_LastError.c_str();
		 *Error = t.copy();	
		 *Result = FALSE;
	};
	
	return S_OK;
}

STDMETHODIMP CComDictionary::DelAllItemsInTheDomain(BYTE DomNo)
{
	size_t count = TRoss::GetDomItemsSize();
	for (size_t i = 0; i<count; i++)
	{
		BYTE CurrDomNo  = TRoss::m_DomItems[i].GetDomNo();
		if (   CurrDomNo ==  DomNo  )
			{
				TRoss::DelDomItem (i); 
				count--;
				i--;
			};

	};
	return S_OK;
}


STDMETHODIMP CComDictionary::get_Name(BSTR *pVal)
{
	_bstr_t t = m_DictName.c_str();
	*pVal = t.copy();
	return S_OK;
}


STDMETHODIMP CComDictionary::SetUnitModifTimeStr(WORD UnitNo, BSTR TimeStr)
{
	TRoss::SetUnitModifTimeStr(UnitNo, (const char*)(_bstr_t) TimeStr);
	return S_OK;
}


STDMETHODIMP CComDictionary::get_UnitEditor(WORD UnitNo, BSTR *pVal)
{
	string Editor =  GetUnitEditor(UnitNo);
	_bstr_t t = Editor.c_str();
	*pVal = t.copy();
	return S_OK;	
}


STDMETHODIMP CComDictionary::put_UnitEditor(WORD UnitNo, BSTR newVal)
{
	TRoss::SetUnitEditor(UnitNo, (const char*)(_bstr_t) newVal);
	return S_OK;
}



STDMETHODIMP CComDictionary::LocateUnit_Inproc(unsigned char *UnitStr, BYTE MeanNum, WORD *UnitNo)
{
	const char* t = (const char*)UnitStr;
	*UnitNo =  TRoss::LocateUnit((const char*)t, MeanNum);
	return S_OK;
}



STDMETHODIMP CComDictionary::get_bDontLoadExamples(BOOL *pVal)
{
	*pVal = m_bDontLoadExamples ? true : false;
	return S_OK;
}

STDMETHODIMP CComDictionary::put_bDontLoadExamples(BOOL newVal)
{
	m_bDontLoadExamples = newVal ? TRUE : FALSE;
	return S_OK;
}


STDMETHODIMP CComDictionary::get_RussianFields(BOOL* pVal)
{
	*pVal = m_bRussianFields ? true : false;

	return S_OK;
}

STDMETHODIMP CComDictionary::put_RussianFields(BOOL newVal)
{
	m_bRussianFields = newVal ? TRUE : FALSE;

	return S_OK;
}

STDMETHODIMP CComDictionary::ImportFromText(BSTR FileName, BOOL Simulating, int ConflictSolver, int StartEntry, BSTR* Messages)
{
	string sMessages;
	CDictionary::ImportFromText((const char*)_bstr_t(FileName),
		Simulating==TRUE,
		(ImportConflictEnum)ConflictSolver,
		StartEntry,
		sMessages);
	_bstr_t bMessages = sMessages.c_str();
	*Messages = bMessages.copy();
	return S_OK;
}

STDMETHODIMP CComDictionary::GetUnitTextHeader(WORD UnitNo, BSTR* Result)
{
	_bstr_t q = TRoss::GetUnitTextHeader(UnitNo).c_str();
	*Result = q.copy();
	return S_OK;
}
