#include "StdPch.h"
#include "StructDict.h"
#include "ComField.h"
#include "ComSignat.h"



STDMETHODIMP CComField::get_FieldStr(BSTR *pVal)
{
	*pVal = _bstr_t((char*)m_pField->FieldStr).copy();
	return S_OK;
}

STDMETHODIMP CComField::put_FieldStr(BSTR newVal)
{
	_bstr_t t= newVal;
	if (t.length() > FieldStrLen-1) return E_FAIL;
	strcpy (m_pField->FieldStr, (const char*) t);
	return S_OK;
}

STDMETHODIMP CComField::get_Type(BYTE *pVal)
{
	*pVal = m_pField->TypeRes;
	return S_OK;
}

STDMETHODIMP CComField::put_Type(BYTE newVal)
{
	m_pField->TypeRes = newVal;
	return S_OK;
}

STDMETHODIMP CComField::get_OrderId(LONG *pVal)
{
	*pVal = m_pField->OrderId;
	return S_OK;
}

STDMETHODIMP CComField::put_OrderId(LONG newVal)
{
	m_pField->OrderId = newVal;
	return S_OK;
}

STDMETHODIMP CComField::get_IsApplicToActant(BOOL *pVal)
{
	*pVal = m_pField->IsApplicToActant ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CComField::put_IsApplicToActant(BOOL newVal)
{
	m_pField->IsApplicToActant = newVal ? true : false;
	return S_OK;
}

STDMETHODIMP CComField::get_Signats(BYTE SignatNo, ISignat **pVal)
{
	CComObject<CComSignat>* It = new CComObject<CComSignat>; 
	It->m_pSignat = &(m_pField->m_Signats[SignatNo]);
	It->AddRef();
	*pVal = It;
	return S_OK;
}

STDMETHODIMP CComField::put_Signats(BYTE SignatNo, ISignat *newVal)
{
	CComObject<CComSignat>* It = reinterpret_cast<CComObject<CComSignat>*>(newVal); 
	m_pField->m_Signats[SignatNo] = *(It->m_pSignat);
	return S_OK;
}

STDMETHODIMP CComField::get_SignatCount(BYTE *pVal)
{
	*pVal = m_pField->m_Signats.size();
	return S_OK;
}

STDMETHODIMP CComField::AddSignat()
{
	CSignat S;
	S.SignatId = 0;
	S.OrderNo = 0;
	strcpy(S.FormatName, "no_name");

	for (size_t i=0; i< m_pField->m_Signats.size(); i++)
		if (m_pField->m_Signats[i].SignatId > S.SignatId) 
			S.SignatId = m_pField->m_Signats[i].SignatId;

	S.FormatStr[0] = 0;
    S.SignatId ++;
	m_pField->m_Signats.push_back(S);
	return S_OK;
}

STDMETHODIMP CComField::DeleteLastSignat(void)
{
	if (!m_pField->m_Signats.empty())
		m_pField->m_Signats.pop_back();

	return S_OK;
}
