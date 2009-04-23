#include "StdPch.h"
#include <comdef.h>
#include "ComSignat.h"


STDMETHODIMP CComSignat::get_FormatForPrintf(BSTR *pVal)
{
	// TODO: Add your implementation code here
	_bstr_t t = m_pSignat->sFrmt;
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComSignat::get_Name(BSTR *pVal)
{
	// TODO: Add your implementation code here
	_bstr_t t = m_pSignat->FormatName;
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComSignat::put_Name(BSTR newVal)
{
	// TODO: Add your implementation code here
	_bstr_t t = newVal;
    strcpy (m_pSignat->FormatName, t);

	return S_OK;
}

STDMETHODIMP CComSignat::get_DomainString(BSTR *pVal)
{
	// TODO: Add your implementation code here
    _bstr_t t = m_pSignat->FormatStr;
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComSignat::put_DomainString(BSTR newVal)
{
	// TODO: Add your implementation code here
    _bstr_t t = newVal;
    strcpy (m_pSignat->FormatStr, t);
	return S_OK;
}

STDMETHODIMP CComSignat::get_OrderNo(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_pSignat->OrderNo; 

	return S_OK;
}

STDMETHODIMP CComSignat::put_OrderNo(long newVal)
{
	m_pSignat->OrderNo = newVal;
	return S_OK;
}

STDMETHODIMP CComSignat::get_DomsWithDelims(BYTE index, BYTE *pVal)
{
	*pVal = m_pSignat->DomsWithDelims[index].m_DomNo;
	return S_OK;
}

STDMETHODIMP CComSignat::put_DomsWithDelims(BYTE index, BYTE newVal)
{	
	m_pSignat->DomsWithDelims[index].m_DomNo = newVal;
	return S_OK;
}

STDMETHODIMP CComSignat::get_DomsWithDelimsCount(BYTE *pVal)
{
	*pVal = m_pSignat->DomsWithDelims.size();
	return S_OK;
}

STDMETHODIMP CComSignat::put_DomsWithDelimsCount(BYTE newVal)
{
	m_pSignat->DomsWithDelims.resize(newVal);
	return S_OK;
}

STDMETHODIMP CComSignat::get_Doms(BYTE index, BYTE *pVal)
{
	*pVal = m_pSignat->Doms[index];
	return S_OK;
}

STDMETHODIMP CComSignat::put_Doms(BYTE index, BYTE newVal)
{
	m_pSignat->Doms[index] = newVal;
	return S_OK;
}

STDMETHODIMP CComSignat::get_DomsCount(BYTE *pVal)
{
	*pVal = m_pSignat->Doms.size();
	return S_OK;
}

STDMETHODIMP CComSignat::put_DomsCount(BYTE newVal)
{
	m_pSignat->Doms.resize(newVal);
	return S_OK;
}


STDMETHODIMP CComSignat::get_SignatId(long *pVal)
{
	*pVal = m_pSignat->SignatId;

	return S_OK;
}

STDMETHODIMP CComSignat::put_SignatId(long newVal)
{
	m_pSignat->SignatId = newVal;
	return S_OK;
}

STDMETHODIMP CComSignat::get_FormatForDomain(BSTR *pVal)
{
	_bstr_t t = m_pSignat->sFrmtWithotSpaces;
	*pVal = t.copy();
	return S_OK;
}

STDMETHODIMP CComSignat::CreateTempSignat()
{
	m_bTemporary =  true;
	m_pSignat = new CSignat;

	return S_OK;
}
