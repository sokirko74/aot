#ifndef __COMDICTIONARY_H_
#define __COMDICTIONARY_H_

#include "../StructDictLib/ross.h"
#include "../common/cortege.h"
#include "../common/utilit.h"


#ifndef PURE_CPP

#include "resource.h"       // main symbols
#include "StructDict.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CComDictionary
class ATL_NO_VTABLE CComDictionary: 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComDictionary, &CLSID_Dictionary>,
	public IDispatchImpl<IDictionary, &IID_IDictionary, &LIBID_STRUCTDICTLib>, 
	public CDictionary
{
public:
	
	
//DECLARE_CLASSFACTORY2(CDialingLicense) 
DECLARE_REGISTRY_RESOURCEID(IDR_DICTIONARY)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComDictionary)
	COM_INTERFACE_ENTRY(IDictionary)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IDictionary
public:
	STDMETHOD(get_bDontLoadExamples)( BOOL *pVal);
	STDMETHOD(put_bDontLoadExamples)(BOOL newVal);
	STDMETHOD(LocateUnit_Inproc)(unsigned char* UnitStr,  BYTE MeanNum,   WORD* UnitNo);
	STDMETHOD(get_UnitEditor)(WORD UnitNo, BSTR *pVal);
	STDMETHOD(put_UnitEditor)(WORD UnitNo, BSTR newVal);
	STDMETHOD(SetUnitModifTimeStr)(WORD UnitNo, BSTR TimeStr);
	STDMETHOD(GetCortegeBracketLeafId)(long CortegeNo,  long* BracketLeafId);
	STDMETHOD(get_Name)( BSTR *pVal);
	STDMETHOD(DelAllItemsInTheDomain)(BYTE DomNo);
	STDMETHOD(CheckArticleError)(BSTR Article,  BSTR* Error,  BOOL* Result);
	STDMETHOD(ClearUnit)(WORD UnitNo);
	STDMETHOD(get_MaxNumDom)( BYTE *pVal);
	STDMETHOD(InitRusConsts)();
	
	STDMETHOD(UpdateConstDomains)();
	STDMETHOD(IncludeArticle)(WORD UnitNo, BSTR Article,  BOOL* result);
	STDMETHOD(get_Fields)(BYTE FieldNo,  IField **pVal);
	STDMETHOD(put_Fields)(BYTE FieldNo, IField* newVal);
	STDMETHOD(get_Domains)(BYTE DomNo,  IDomain* *pVal);
	STDMETHOD(put_Domains)(BYTE DomNo, IDomain* newVal);
	STDMETHOD(SetUnitStr)(WORD UnitNo,  BSTR UnitStr);
	STDMETHOD(GetWildCardDomItemNo)( LONG* result);
	STDMETHOD(GetUnitCommentStr)(WORD UnitNo,  BSTR* result);
	STDMETHOD(DelUnit)(WORD UnitNo);
	STDMETHOD(UnitsLowerBound)(BSTR UnitStr,  WORD* UniNo);
	STDMETHOD(SetUnitAuthor)(WORD UnitNo, BSTR Author);
	STDMETHOD(GetUnitModifTimeStr)(WORD UnitNo, BSTR* result);
	STDMETHOD(GetSelectedUnitNo)(WORD UnitNo, WORD* result);
	STDMETHOD(BuildFormats)();
	STDMETHOD(GetDomainsSize)( BYTE* result);
	STDMETHOD(GetFieldDomainNo)( BYTE* FieldNo);
	STDMETHOD(LoadUnitComments)();
	STDMETHOD(Save)();
	STDMETHOD(SetUnitSelected)(WORD UnitNo, BOOL Selected);
	STDMETHOD(SetUnitCommentStr)(WORD UnitNo, BSTR Str);
	STDMETHOD(InsertUnit)(BSTR UnitStr, BYTE MeanNum,  LONG* result);
	STDMETHOD(GetDomainNoByDomStr)(BSTR DomStr,  BYTE* result);
	STDMETHOD(GetFieldsSize)(BYTE* result);
	STDMETHOD(DelDomItem)(LONG ItemNo);
	STDMETHOD(BuildSignat)(ISignat* Signat, BSTR DoemnStrng, BSTR Name);
	STDMETHOD(InsertDomItem)(BSTR ItemStr, BYTE DomNo,  int* ItemNo);
	STDMETHOD(GetDomItemDomNo)(LONG ItemNo,  BYTE* result);
	STDMETHOD(GetDomItemsSize)( LONG* result);
	STDMETHOD(GetLexPlusDomNo)( BYTE* result);
	STDMETHOD(GetFieldNoByFieldStr)(BSTR Str, BYTE* result);
	STDMETHOD(SetUnitCurrentTime)(WORD UnitNo);
	STDMETHOD(SetUnitEndPos)(WORD UnitNo, LONG NewValue);
	STDMETHOD(SetUnitStartPos)(WORD UnitNo, LONG NewValue);
	STDMETHOD(GetCortegesSize)( LONG* result);
	STDMETHOD(DelCorteges)(LONG StartPos, LONG EndPos);
	STDMETHOD(GetDomItemStr)(LONG ItemNo,  BSTR* result);
	STDMETHOD(GetDomItemStr_Inproc)(LONG ItemNo,  LONG* result);	
	STDMETHOD(GetItemNoByItemStr)(BSTR ItemStr, BYTE DomNo,  LONG* result);
	STDMETHOD(LocateUnit)(BSTR UnitStr, BYTE MeanNum,  WORD* result);
	STDMETHOD(GetSelectedUnitsSize)( LONG* result);
	STDMETHOD(GetCortegeItem)(LONG CortegeNo, BYTE PositionInCortege, LONG* ItemId);
	STDMETHOD(GetCortegeLeafId)(LONG CortegeNo,  BYTE* result);
	STDMETHOD(GetCortegeLevelId)(LONG CortegeNo,  BYTE* result);
	STDMETHOD(GetCortegeSignatNo)(LONG CortegeNo,  BYTE* result);
	STDMETHOD(GetCortegeFieldNo)(LONG CortegeNo,  BYTE* result);
	STDMETHOD(IsEmptyArticle)(WORD UnitNo,  BOOL* result);
	STDMETHOD(GetUnitAuthor)(WORD UnitNo,  BSTR* result);
	STDMETHOD(GetUnitSelected)(WORD UnitNo,  BOOL* result);
	STDMETHOD(GetUnitEndPos)(WORD UnitNo,  LONG* result);
	STDMETHOD(GetUnitStartPos)(WORD UnitNo,  LONG*  result);
	STDMETHOD(GetUnitMeanNum)(WORD UnitNo,  BYTE* MeanNum);
	STDMETHOD(GetUnitStr)(WORD UnitNo,  BSTR* result);
	STDMETHOD(GetUnitsSize)( WORD* result);
	STDMETHOD(LoadWithoutComments)(BSTR Path);
	STDMETHOD(get_RussianFields)(BOOL* pVal);
	STDMETHOD(put_RussianFields)(BOOL newVal);
	STDMETHOD(ImportFromText)(BSTR FileName, BOOL Simulating, int ConflictSolver, int MaxEntriesToRead, BSTR* Messages);
	STDMETHOD(GetUnitTextHeader)(WORD UnitNo, BSTR* Result);
};


#endif // COM
#endif //__DICTIONARY_H_
