#include "StdPch.h"
#include "StructDict.h"
#include "ComTempArticle.h"
#include "ComDictionary.h"
#include "ComTextField.h"

#include "../common/utilit.h"



STDMETHODIMP CComTempArticle::AttachToDictionary(IDictionary *Dictionary)
{
	CComObject<CComDictionary>* t = (CComObject<CComDictionary>*)Dictionary;
	m_pRoss = t;

	return S_OK;
}


STDMETHODIMP CComTempArticle::get_UnitStr(BSTR *pVal)
{
	_bstr_t t = m_EntryStr;
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComTempArticle::get_MeanNum(BYTE *pVal)
{
	*pVal = m_MeanNum;
	return S_OK;
}

STDMETHODIMP CComTempArticle::get_Fields(LONG FieldNo, ITextField **pVal)
{
	CComObject<CComTextField>* It = new CComObject<CComTextField>; 
	It->m_pField = &m_Fields[FieldNo];
	It->AddRef();
	*pVal = It;
	return S_OK;
}




STDMETHODIMP CComTempArticle::ReadFromDictionary(WORD UnitNo, BOOL VisualOrder, BOOL ReadOnly)
{
	CTempArticle::ReadFromDictionary(UnitNo, VisualOrder ? true : false, ReadOnly ? true : false);
	return S_OK;
}


STDMETHODIMP CComTempArticle::put_ArticleStr(BSTR newVal)
{
	_bstr_t t = newVal; 
	if (!CTempArticle::SetArticleStr((const char *)t))
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CComTempArticle::MarkUp()
{
  if (	!CTempArticle::MarkUp() ) 
	  return E_FAIL;

  return S_OK;
}

STDMETHODIMP CComTempArticle::GetFieldsSize(WORD *result)
{
	*result = m_Fields.size();
	return S_OK;
}




STDMETHODIMP CComTempArticle::BuildCortegeList()
{
	if (	!CTempArticle::BuildCortegeList() ) 
		return E_FAIL;
	return S_OK;
}


STDMETHODIMP CComTempArticle::WriteToDictionary()
{
	if (	!CTempArticle::WriteToDictionary() ) 
		return E_FAIL;
	return S_OK;
}





STDMETHODIMP CComTempArticle::IsModified(BOOL *result)
{
	if (m_ReadOnly) 
	{
		m_LastError = "Article is readonly";
		return E_FAIL;
	};

	*result = CTempArticle::IsModified() ? TRUE : FALSE;

	return S_OK;
}

STDMETHODIMP CComTempArticle::GetCortege(LONG CortegeNo, BYTE *FieldNo, BYTE *SignatNo, BYTE *LevelId, BYTE *LeafId, BYTE* BracketLeafId)
{
	const TCortege10& C = CTempArticle::GetCortege(CortegeNo);
	*FieldNo = C.m_FieldNo;
	*SignatNo = C.m_SignatNo;
    *LeafId = C.m_LeafId;
	*LevelId = C.m_LevelId;
	*BracketLeafId = C.m_BracketLeafId;
    return S_OK;
}

STDMETHODIMP CComTempArticle::GetCortegeItem(LONG CortegeNo, BYTE PositionInCortege, LONG* ItemId)
{
	const TCortege10& C = CTempArticle::GetCortege(CortegeNo);
	*ItemId = C.GetItem(PositionInCortege);

	return S_OK;
}


STDMETHODIMP CComTempArticle::GetCortegesSize(LONG *result)
{
	*result = CTempArticle::GetCortegesSize();
	return S_OK;
}


STDMETHODIMP CComTempArticle::get_LastError(BSTR *pVal)
{
	_bstr_t t = m_LastError.c_str();
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComTempArticle::IsPartOf(ITempArticle *Article, BOOL UseWildCards, BOOL *result)
{
    CComTempArticle* It = reinterpret_cast<CComTempArticle*>(Article);
	*result =  CTempArticle::IsPartOf(It, UseWildCards? true: false);
	return S_OK;
};

STDMETHODIMP CComTempArticle::AddArticle(ITempArticle *Article)
{
	if (m_ReadOnly) return E_FAIL;
    CComTempArticle* It = reinterpret_cast<CComTempArticle*>(Article);
	if (!CTempArticle::AddArticle(It)) return E_FAIL;
	return S_OK;
}


STDMETHODIMP CComTempArticle::IntersectByFields(ITempArticle *Article, LONG *result)
{
	CComTempArticle* It = reinterpret_cast<CComTempArticle*>(Article);
	*result = CTempArticle::IntersectByFields(It);
	return S_OK;
}

STDMETHODIMP CComTempArticle::get_ArticleStr(BSTR *pVal)
{
	_bstr_t t = GetArticleStr().c_str();;
    *pVal = t.copy();
	return S_OK;
}


STDMETHODIMP CComTempArticle::get_ErrorLine(int* pVal)
{
	*pVal = m_ErrorLine;
	return S_OK;
}
