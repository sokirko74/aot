#include "StdPch.h"
#include "StructDict.h"
#include "ComTextField.h"

/////////////////////////////////////////////////////////////////////////////


STDMETHODIMP CComTextField::get_StartLine(DWORD *pVal)
{
	*pVal = m_pField->StartLine;
	return S_OK;
}

STDMETHODIMP CComTextField::put_StartLine(DWORD newVal)
{
	m_pField->StartLine = newVal;
	return S_OK;
}

STDMETHODIMP CComTextField::get_EndLine(DWORD *pVal)
{
	*pVal = m_pField->EndLine;
	return S_OK;
}

STDMETHODIMP CComTextField::put_EndLine(DWORD newVal)
{
	m_pField->EndLine = newVal;
	return S_OK;
}

STDMETHODIMP CComTextField::get_FieldNo(BYTE *pVal)
{
	*pVal = m_pField->FieldNo;
	return S_OK;
}

STDMETHODIMP CComTextField::put_FieldNo(BYTE newVal)
{
	m_pField->FieldNo = newVal;
	return S_OK;
}

STDMETHODIMP CComTextField::get_LeafId(BYTE *pVal)
{
	*pVal = m_pField->LeafId;
	return S_OK;
}

STDMETHODIMP CComTextField::put_LeafId(BYTE newVal)
{
	m_pField->LeafId = newVal;
	return S_OK;
}

STDMETHODIMP CComTextField::get_BracketLeafId(BYTE *pVal)
{
	*pVal = m_pField->BracketLeafId;
	return S_OK;
}

STDMETHODIMP CComTextField::put_BracketLeafId(BYTE newVal)
{
	m_pField->BracketLeafId = newVal;
	return S_OK;
}
