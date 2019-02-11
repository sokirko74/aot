#include "StdPch.h"
#include "ComDomen.h"
#include "../StructDictLib/ItemsContainer.h"
#include "../common/utilit.h"

STDMETHODIMP CComDomen::get_DomStr(BSTR *pVal)
{
	_bstr_t t = m_pDomen->DomStr;
	*pVal  = t.copy();

	return S_OK;
}

STDMETHODIMP CComDomen::put_DomStr(BSTR newVal)
{
	_bstr_t t = newVal;
	strcpy (m_pDomen->DomStr, (const char*)t);
	return S_OK;
}

STDMETHODIMP CComDomen::get_Source(BYTE *pVal)
{
	*pVal =  m_pDomen->Source;

	return S_OK;
}

STDMETHODIMP CComDomen::put_Source(BYTE newVal)
{
	m_pDomen->Source = newVal;
	return S_OK;
}

STDMETHODIMP CComDomen::get_PartsSize(BYTE *pVal)
{
	*pVal = m_pDomen->PartsSize;
	return S_OK;
}

STDMETHODIMP CComDomen::get_Parts(BYTE PartNo, BYTE *pVal)
{
	*pVal = m_pDomen->Parts[PartNo];
	return S_OK;
}

STDMETHODIMP CComDomen::put_Parts(BYTE PartNo, BYTE newVal)
{
    m_pDomen->Parts[PartNo] = newVal;
	return S_OK;
}

STDMETHODIMP CComDomen::get_Format(BSTR *pVal)
{
	_bstr_t t = m_pDomen->Format;
	*pVal = t.copy();

	return S_OK;
}

STDMETHODIMP CComDomen::put_Format(BSTR newVal)
{
	_bstr_t t = newVal;
	strcpy (m_pDomen->Format, (const char*) t);
	return S_OK;
}

STDMETHODIMP CComDomen::get_IsFree(BOOL *pVal)
{
	*pVal = m_pDomen->IsFree ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComDomen::put_IsFree(BOOL newVal)
{
	m_pDomen->IsFree = newVal ? true : false;

	return S_OK;
}

STDMETHODIMP CComDomen::get_Items(LONG index, BSTR *pVal)
{
	_bstr_t t =  m_pDomen->m_pParent->GetDomItemStr(m_pDomen->m_pParent->m_DomItems[index+m_pDomen->m_StartDomItem]);
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComDomen::GetItemsCount(LONG *result)
{
	
	*result = m_pDomen->m_EndDomItem - m_pDomen->m_StartDomItem;

	return S_OK;
}

STDMETHODIMP CComDomen::FindItem(BSTR ItemStr, LONG *index)
{
	string t = (const char*)_bstr_t(ItemStr);
	int GlobalIndex = m_pDomen->m_pParent->GetItemNoByItemStr(t.c_str(), m_pDomen->m_DomNo);

	if (GlobalIndex == -1) 
		*index = -1;
	else
		*index = GlobalIndex - m_pDomen->m_StartDomItem;
	return S_OK;
}

STDMETHODIMP CComDomen::get_IsDelim(BOOL *pVal)
{
	*pVal = (m_pDomen->IsDelim) ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComDomen::put_IsDelim(BOOL newVal)
{
	m_pDomen->IsDelim= (newVal) ? true : false;
	return S_OK;
}

