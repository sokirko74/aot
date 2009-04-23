

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Dec 11 18:47:17 2008
 */
/* Compiler settings for .\StructDict.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __StructDict_h__
#define __StructDict_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDomain_FWD_DEFINED__
#define __IDomain_FWD_DEFINED__
typedef interface IDomain IDomain;
#endif 	/* __IDomain_FWD_DEFINED__ */


#ifndef __ISignat_FWD_DEFINED__
#define __ISignat_FWD_DEFINED__
typedef interface ISignat ISignat;
#endif 	/* __ISignat_FWD_DEFINED__ */


#ifndef __IField_FWD_DEFINED__
#define __IField_FWD_DEFINED__
typedef interface IField IField;
#endif 	/* __IField_FWD_DEFINED__ */


#ifndef __ITextField_FWD_DEFINED__
#define __ITextField_FWD_DEFINED__
typedef interface ITextField ITextField;
#endif 	/* __ITextField_FWD_DEFINED__ */


#ifndef __ITempArticle_FWD_DEFINED__
#define __ITempArticle_FWD_DEFINED__
typedef interface ITempArticle ITempArticle;
#endif 	/* __ITempArticle_FWD_DEFINED__ */


#ifndef __IDictionary_FWD_DEFINED__
#define __IDictionary_FWD_DEFINED__
typedef interface IDictionary IDictionary;
#endif 	/* __IDictionary_FWD_DEFINED__ */


#ifndef __Dictionary_FWD_DEFINED__
#define __Dictionary_FWD_DEFINED__

#ifdef __cplusplus
typedef class Dictionary Dictionary;
#else
typedef struct Dictionary Dictionary;
#endif /* __cplusplus */

#endif 	/* __Dictionary_FWD_DEFINED__ */


#ifndef __Signat_FWD_DEFINED__
#define __Signat_FWD_DEFINED__

#ifdef __cplusplus
typedef class Signat Signat;
#else
typedef struct Signat Signat;
#endif /* __cplusplus */

#endif 	/* __Signat_FWD_DEFINED__ */


#ifndef __Domain_FWD_DEFINED__
#define __Domain_FWD_DEFINED__

#ifdef __cplusplus
typedef class Domain Domain;
#else
typedef struct Domain Domain;
#endif /* __cplusplus */

#endif 	/* __Domain_FWD_DEFINED__ */


#ifndef __Field_FWD_DEFINED__
#define __Field_FWD_DEFINED__

#ifdef __cplusplus
typedef class Field Field;
#else
typedef struct Field Field;
#endif /* __cplusplus */

#endif 	/* __Field_FWD_DEFINED__ */


#ifndef __TempArticle_FWD_DEFINED__
#define __TempArticle_FWD_DEFINED__

#ifdef __cplusplus
typedef class TempArticle TempArticle;
#else
typedef struct TempArticle TempArticle;
#endif /* __cplusplus */

#endif 	/* __TempArticle_FWD_DEFINED__ */


#ifndef __TextField_FWD_DEFINED__
#define __TextField_FWD_DEFINED__

#ifdef __cplusplus
typedef class TextField TextField;
#else
typedef struct TextField TextField;
#endif /* __cplusplus */

#endif 	/* __TextField_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IDomain_INTERFACE_DEFINED__
#define __IDomain_INTERFACE_DEFINED__

/* interface IDomain */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IDomain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD4C1F31-2608-11D3-A78E-9089CA000000")
    IDomain : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DomStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DomStr( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PartsSize( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parts( 
            /* [in] */ BYTE PartNo,
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Parts( 
            /* [in] */ BYTE PartNo,
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Format( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Format( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsFree( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsFree( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Items( 
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemsCount( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindItem( 
            /* [in] */ BSTR ItemStr,
            /* [out][retval] */ LONG *index) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsDelim( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsDelim( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDomainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDomain * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDomain * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDomain * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDomain * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDomain * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDomain * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDomain * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomStr )( 
            IDomain * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DomStr )( 
            IDomain * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IDomain * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Source )( 
            IDomain * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PartsSize )( 
            IDomain * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parts )( 
            IDomain * This,
            /* [in] */ BYTE PartNo,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Parts )( 
            IDomain * This,
            /* [in] */ BYTE PartNo,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Format )( 
            IDomain * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Format )( 
            IDomain * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFree )( 
            IDomain * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IsFree )( 
            IDomain * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Items )( 
            IDomain * This,
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemsCount )( 
            IDomain * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindItem )( 
            IDomain * This,
            /* [in] */ BSTR ItemStr,
            /* [out][retval] */ LONG *index);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsDelim )( 
            IDomain * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IsDelim )( 
            IDomain * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IDomainVtbl;

    interface IDomain
    {
        CONST_VTBL struct IDomainVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDomain_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDomain_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDomain_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDomain_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDomain_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDomain_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDomain_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IDomain_get_DomStr(This,pVal)	\
    ( (This)->lpVtbl -> get_DomStr(This,pVal) ) 

#define IDomain_put_DomStr(This,newVal)	\
    ( (This)->lpVtbl -> put_DomStr(This,newVal) ) 

#define IDomain_get_Source(This,pVal)	\
    ( (This)->lpVtbl -> get_Source(This,pVal) ) 

#define IDomain_put_Source(This,newVal)	\
    ( (This)->lpVtbl -> put_Source(This,newVal) ) 

#define IDomain_get_PartsSize(This,pVal)	\
    ( (This)->lpVtbl -> get_PartsSize(This,pVal) ) 

#define IDomain_get_Parts(This,PartNo,pVal)	\
    ( (This)->lpVtbl -> get_Parts(This,PartNo,pVal) ) 

#define IDomain_put_Parts(This,PartNo,newVal)	\
    ( (This)->lpVtbl -> put_Parts(This,PartNo,newVal) ) 

#define IDomain_get_Format(This,pVal)	\
    ( (This)->lpVtbl -> get_Format(This,pVal) ) 

#define IDomain_put_Format(This,newVal)	\
    ( (This)->lpVtbl -> put_Format(This,newVal) ) 

#define IDomain_get_IsFree(This,pVal)	\
    ( (This)->lpVtbl -> get_IsFree(This,pVal) ) 

#define IDomain_put_IsFree(This,newVal)	\
    ( (This)->lpVtbl -> put_IsFree(This,newVal) ) 

#define IDomain_get_Items(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Items(This,index,pVal) ) 

#define IDomain_GetItemsCount(This,result)	\
    ( (This)->lpVtbl -> GetItemsCount(This,result) ) 

#define IDomain_FindItem(This,ItemStr,index)	\
    ( (This)->lpVtbl -> FindItem(This,ItemStr,index) ) 

#define IDomain_get_IsDelim(This,pVal)	\
    ( (This)->lpVtbl -> get_IsDelim(This,pVal) ) 

#define IDomain_put_IsDelim(This,newVal)	\
    ( (This)->lpVtbl -> put_IsDelim(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDomain_INTERFACE_DEFINED__ */


#ifndef __ISignat_INTERFACE_DEFINED__
#define __ISignat_INTERFACE_DEFINED__

/* interface ISignat */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISignat;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F269911-246D-11D3-A78C-067993000000")
    ISignat : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FormatForPrintf( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DomainString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DomainString( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DomsWithDelims( 
            /* [in] */ BYTE index,
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DomsWithDelims( 
            /* [in] */ BYTE index,
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DomsWithDelimsCount( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DomsWithDelimsCount( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Doms( 
            /* [in] */ BYTE index,
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Doms( 
            /* [in] */ BYTE index,
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DomsCount( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DomsCount( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SignatId( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SignatId( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FormatForDomain( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTempSignat( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISignatVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISignat * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISignat * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISignat * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISignat * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISignat * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISignat * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISignat * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FormatForPrintf )( 
            ISignat * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISignat * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISignat * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomainString )( 
            ISignat * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DomainString )( 
            ISignat * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OrderNo )( 
            ISignat * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OrderNo )( 
            ISignat * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomsWithDelims )( 
            ISignat * This,
            /* [in] */ BYTE index,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DomsWithDelims )( 
            ISignat * This,
            /* [in] */ BYTE index,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomsWithDelimsCount )( 
            ISignat * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DomsWithDelimsCount )( 
            ISignat * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Doms )( 
            ISignat * This,
            /* [in] */ BYTE index,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Doms )( 
            ISignat * This,
            /* [in] */ BYTE index,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomsCount )( 
            ISignat * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DomsCount )( 
            ISignat * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SignatId )( 
            ISignat * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SignatId )( 
            ISignat * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FormatForDomain )( 
            ISignat * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTempSignat )( 
            ISignat * This);
        
        END_INTERFACE
    } ISignatVtbl;

    interface ISignat
    {
        CONST_VTBL struct ISignatVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISignat_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISignat_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISignat_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISignat_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISignat_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISignat_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISignat_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISignat_get_FormatForPrintf(This,pVal)	\
    ( (This)->lpVtbl -> get_FormatForPrintf(This,pVal) ) 

#define ISignat_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISignat_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISignat_get_DomainString(This,pVal)	\
    ( (This)->lpVtbl -> get_DomainString(This,pVal) ) 

#define ISignat_put_DomainString(This,newVal)	\
    ( (This)->lpVtbl -> put_DomainString(This,newVal) ) 

#define ISignat_get_OrderNo(This,pVal)	\
    ( (This)->lpVtbl -> get_OrderNo(This,pVal) ) 

#define ISignat_put_OrderNo(This,newVal)	\
    ( (This)->lpVtbl -> put_OrderNo(This,newVal) ) 

#define ISignat_get_DomsWithDelims(This,index,pVal)	\
    ( (This)->lpVtbl -> get_DomsWithDelims(This,index,pVal) ) 

#define ISignat_put_DomsWithDelims(This,index,newVal)	\
    ( (This)->lpVtbl -> put_DomsWithDelims(This,index,newVal) ) 

#define ISignat_get_DomsWithDelimsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_DomsWithDelimsCount(This,pVal) ) 

#define ISignat_put_DomsWithDelimsCount(This,newVal)	\
    ( (This)->lpVtbl -> put_DomsWithDelimsCount(This,newVal) ) 

#define ISignat_get_Doms(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Doms(This,index,pVal) ) 

#define ISignat_put_Doms(This,index,newVal)	\
    ( (This)->lpVtbl -> put_Doms(This,index,newVal) ) 

#define ISignat_get_DomsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_DomsCount(This,pVal) ) 

#define ISignat_put_DomsCount(This,newVal)	\
    ( (This)->lpVtbl -> put_DomsCount(This,newVal) ) 

#define ISignat_get_SignatId(This,pVal)	\
    ( (This)->lpVtbl -> get_SignatId(This,pVal) ) 

#define ISignat_put_SignatId(This,newVal)	\
    ( (This)->lpVtbl -> put_SignatId(This,newVal) ) 

#define ISignat_get_FormatForDomain(This,pVal)	\
    ( (This)->lpVtbl -> get_FormatForDomain(This,pVal) ) 

#define ISignat_CreateTempSignat(This)	\
    ( (This)->lpVtbl -> CreateTempSignat(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISignat_INTERFACE_DEFINED__ */


#ifndef __IField_INTERFACE_DEFINED__
#define __IField_INTERFACE_DEFINED__

/* interface IField */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD4C1F35-2608-11D3-A78E-9089CA000000")
    IField : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FieldStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FieldStr( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderId( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderId( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsApplicToActant( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsApplicToActant( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Signats( 
            /* [in] */ BYTE SignatNo,
            /* [retval][out] */ ISignat **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Signats( 
            /* [in] */ BYTE SignatNo,
            /* [in] */ ISignat *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SignatCount( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteLastSignat( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IField * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IField * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IField * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IField * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FieldStr )( 
            IField * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FieldStr )( 
            IField * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IField * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Type )( 
            IField * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OrderId )( 
            IField * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OrderId )( 
            IField * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsApplicToActant )( 
            IField * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IsApplicToActant )( 
            IField * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Signats )( 
            IField * This,
            /* [in] */ BYTE SignatNo,
            /* [retval][out] */ ISignat **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Signats )( 
            IField * This,
            /* [in] */ BYTE SignatNo,
            /* [in] */ ISignat *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SignatCount )( 
            IField * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteLastSignat )( 
            IField * This);
        
        END_INTERFACE
    } IFieldVtbl;

    interface IField
    {
        CONST_VTBL struct IFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IField_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IField_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IField_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IField_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IField_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IField_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IField_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IField_get_FieldStr(This,pVal)	\
    ( (This)->lpVtbl -> get_FieldStr(This,pVal) ) 

#define IField_put_FieldStr(This,newVal)	\
    ( (This)->lpVtbl -> put_FieldStr(This,newVal) ) 

#define IField_get_Type(This,pVal)	\
    ( (This)->lpVtbl -> get_Type(This,pVal) ) 

#define IField_put_Type(This,newVal)	\
    ( (This)->lpVtbl -> put_Type(This,newVal) ) 

#define IField_get_OrderId(This,pVal)	\
    ( (This)->lpVtbl -> get_OrderId(This,pVal) ) 

#define IField_put_OrderId(This,newVal)	\
    ( (This)->lpVtbl -> put_OrderId(This,newVal) ) 

#define IField_get_IsApplicToActant(This,pVal)	\
    ( (This)->lpVtbl -> get_IsApplicToActant(This,pVal) ) 

#define IField_put_IsApplicToActant(This,newVal)	\
    ( (This)->lpVtbl -> put_IsApplicToActant(This,newVal) ) 

#define IField_get_Signats(This,SignatNo,pVal)	\
    ( (This)->lpVtbl -> get_Signats(This,SignatNo,pVal) ) 

#define IField_put_Signats(This,SignatNo,newVal)	\
    ( (This)->lpVtbl -> put_Signats(This,SignatNo,newVal) ) 

#define IField_get_SignatCount(This,pVal)	\
    ( (This)->lpVtbl -> get_SignatCount(This,pVal) ) 

#define IField_DeleteLastSignat(This)	\
    ( (This)->lpVtbl -> DeleteLastSignat(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IField_INTERFACE_DEFINED__ */


#ifndef __ITextField_INTERFACE_DEFINED__
#define __ITextField_INTERFACE_DEFINED__

/* interface ITextField */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITextField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B3C7D666-2C57-11D3-A7A1-80F9E5000000")
    ITextField : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartLine( 
            /* [retval][out] */ DWORD *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartLine( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndLine( 
            /* [retval][out] */ DWORD *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndLine( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FieldNo( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FieldNo( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LeafId( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LeafId( 
            /* [in] */ BYTE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BracketLeafId( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BracketLeafId( 
            /* [in] */ BYTE newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITextFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITextField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITextField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITextField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITextField * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITextField * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITextField * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITextField * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartLine )( 
            ITextField * This,
            /* [retval][out] */ DWORD *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartLine )( 
            ITextField * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndLine )( 
            ITextField * This,
            /* [retval][out] */ DWORD *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EndLine )( 
            ITextField * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FieldNo )( 
            ITextField * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FieldNo )( 
            ITextField * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LeafId )( 
            ITextField * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LeafId )( 
            ITextField * This,
            /* [in] */ BYTE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BracketLeafId )( 
            ITextField * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BracketLeafId )( 
            ITextField * This,
            /* [in] */ BYTE newVal);
        
        END_INTERFACE
    } ITextFieldVtbl;

    interface ITextField
    {
        CONST_VTBL struct ITextFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITextField_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITextField_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITextField_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITextField_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITextField_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITextField_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITextField_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITextField_get_StartLine(This,pVal)	\
    ( (This)->lpVtbl -> get_StartLine(This,pVal) ) 

#define ITextField_put_StartLine(This,newVal)	\
    ( (This)->lpVtbl -> put_StartLine(This,newVal) ) 

#define ITextField_get_EndLine(This,pVal)	\
    ( (This)->lpVtbl -> get_EndLine(This,pVal) ) 

#define ITextField_put_EndLine(This,newVal)	\
    ( (This)->lpVtbl -> put_EndLine(This,newVal) ) 

#define ITextField_get_FieldNo(This,pVal)	\
    ( (This)->lpVtbl -> get_FieldNo(This,pVal) ) 

#define ITextField_put_FieldNo(This,newVal)	\
    ( (This)->lpVtbl -> put_FieldNo(This,newVal) ) 

#define ITextField_get_LeafId(This,pVal)	\
    ( (This)->lpVtbl -> get_LeafId(This,pVal) ) 

#define ITextField_put_LeafId(This,newVal)	\
    ( (This)->lpVtbl -> put_LeafId(This,newVal) ) 

#define ITextField_get_BracketLeafId(This,pVal)	\
    ( (This)->lpVtbl -> get_BracketLeafId(This,pVal) ) 

#define ITextField_put_BracketLeafId(This,newVal)	\
    ( (This)->lpVtbl -> put_BracketLeafId(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITextField_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_StructDict_0000_0004 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_StructDict_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_StructDict_0000_0004_v0_0_s_ifspec;

#ifndef __ITempArticle_INTERFACE_DEFINED__
#define __ITempArticle_INTERFACE_DEFINED__

/* interface ITempArticle */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITempArticle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B3C7D664-2C57-11D3-A7A1-80F9E5000000")
    ITempArticle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UnitStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MeanNum( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Fields( 
            /* [in] */ long FieldNo,
            /* [retval][out] */ ITextField **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadFromDictionary( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BOOL VisualOrder,
            /* [in] */ BOOL ReadOnly) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ArticleStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ArticleStr( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MarkUp( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFieldsSize( 
            /* [out][retval] */ WORD *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BuildCortegeList( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteToDictionary( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsModified( 
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortege( 
            /* [in] */ LONG i,
            /* [out] */ BYTE *FieldNo,
            /* [out] */ BYTE *SignatNo,
            /* [out] */ BYTE *LevelId,
            /* [out] */ BYTE *LeafId,
            /* [out] */ BYTE *BracketLeafId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegesSize( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IntersectByFields( 
            /* [in] */ ITempArticle *Article,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddArticle( 
            /* [in] */ ITempArticle *Article) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsPartOf( 
            /* [in] */ ITempArticle *Article,
            /* [in] */ BOOL UseWildCards,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AttachToDictionary( 
            /* [in] */ IDictionary *Dictionary) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeItem( 
            /* [in] */ LONG CortegeNo,
            /* [in] */ BYTE PositionInCortege,
            /* [out][retval] */ LONG *ItemId) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorLine( 
            /* [retval][out] */ int *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITempArticleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITempArticle * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITempArticle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITempArticle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITempArticle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITempArticle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITempArticle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITempArticle * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UnitStr )( 
            ITempArticle * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MeanNum )( 
            ITempArticle * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Fields )( 
            ITempArticle * This,
            /* [in] */ long FieldNo,
            /* [retval][out] */ ITextField **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadFromDictionary )( 
            ITempArticle * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BOOL VisualOrder,
            /* [in] */ BOOL ReadOnly);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ArticleStr )( 
            ITempArticle * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ArticleStr )( 
            ITempArticle * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MarkUp )( 
            ITempArticle * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFieldsSize )( 
            ITempArticle * This,
            /* [out][retval] */ WORD *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildCortegeList )( 
            ITempArticle * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteToDictionary )( 
            ITempArticle * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsModified )( 
            ITempArticle * This,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortege )( 
            ITempArticle * This,
            /* [in] */ LONG i,
            /* [out] */ BYTE *FieldNo,
            /* [out] */ BYTE *SignatNo,
            /* [out] */ BYTE *LevelId,
            /* [out] */ BYTE *LeafId,
            /* [out] */ BYTE *BracketLeafId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegesSize )( 
            ITempArticle * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IntersectByFields )( 
            ITempArticle * This,
            /* [in] */ ITempArticle *Article,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddArticle )( 
            ITempArticle * This,
            /* [in] */ ITempArticle *Article);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsPartOf )( 
            ITempArticle * This,
            /* [in] */ ITempArticle *Article,
            /* [in] */ BOOL UseWildCards,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AttachToDictionary )( 
            ITempArticle * This,
            /* [in] */ IDictionary *Dictionary);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeItem )( 
            ITempArticle * This,
            /* [in] */ LONG CortegeNo,
            /* [in] */ BYTE PositionInCortege,
            /* [out][retval] */ LONG *ItemId);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            ITempArticle * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ErrorLine )( 
            ITempArticle * This,
            /* [retval][out] */ int *pVal);
        
        END_INTERFACE
    } ITempArticleVtbl;

    interface ITempArticle
    {
        CONST_VTBL struct ITempArticleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITempArticle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITempArticle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITempArticle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITempArticle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITempArticle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITempArticle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITempArticle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITempArticle_get_UnitStr(This,pVal)	\
    ( (This)->lpVtbl -> get_UnitStr(This,pVal) ) 

#define ITempArticle_get_MeanNum(This,pVal)	\
    ( (This)->lpVtbl -> get_MeanNum(This,pVal) ) 

#define ITempArticle_get_Fields(This,FieldNo,pVal)	\
    ( (This)->lpVtbl -> get_Fields(This,FieldNo,pVal) ) 

#define ITempArticle_ReadFromDictionary(This,UnitNo,VisualOrder,ReadOnly)	\
    ( (This)->lpVtbl -> ReadFromDictionary(This,UnitNo,VisualOrder,ReadOnly) ) 

#define ITempArticle_get_ArticleStr(This,pVal)	\
    ( (This)->lpVtbl -> get_ArticleStr(This,pVal) ) 

#define ITempArticle_put_ArticleStr(This,newVal)	\
    ( (This)->lpVtbl -> put_ArticleStr(This,newVal) ) 

#define ITempArticle_MarkUp(This)	\
    ( (This)->lpVtbl -> MarkUp(This) ) 

#define ITempArticle_GetFieldsSize(This,result)	\
    ( (This)->lpVtbl -> GetFieldsSize(This,result) ) 

#define ITempArticle_BuildCortegeList(This)	\
    ( (This)->lpVtbl -> BuildCortegeList(This) ) 

#define ITempArticle_WriteToDictionary(This)	\
    ( (This)->lpVtbl -> WriteToDictionary(This) ) 

#define ITempArticle_IsModified(This,result)	\
    ( (This)->lpVtbl -> IsModified(This,result) ) 

#define ITempArticle_GetCortege(This,i,FieldNo,SignatNo,LevelId,LeafId,BracketLeafId)	\
    ( (This)->lpVtbl -> GetCortege(This,i,FieldNo,SignatNo,LevelId,LeafId,BracketLeafId) ) 

#define ITempArticle_GetCortegesSize(This,result)	\
    ( (This)->lpVtbl -> GetCortegesSize(This,result) ) 

#define ITempArticle_IntersectByFields(This,Article,result)	\
    ( (This)->lpVtbl -> IntersectByFields(This,Article,result) ) 

#define ITempArticle_AddArticle(This,Article)	\
    ( (This)->lpVtbl -> AddArticle(This,Article) ) 

#define ITempArticle_IsPartOf(This,Article,UseWildCards,result)	\
    ( (This)->lpVtbl -> IsPartOf(This,Article,UseWildCards,result) ) 

#define ITempArticle_AttachToDictionary(This,Dictionary)	\
    ( (This)->lpVtbl -> AttachToDictionary(This,Dictionary) ) 

#define ITempArticle_GetCortegeItem(This,CortegeNo,PositionInCortege,ItemId)	\
    ( (This)->lpVtbl -> GetCortegeItem(This,CortegeNo,PositionInCortege,ItemId) ) 

#define ITempArticle_get_LastError(This,pVal)	\
    ( (This)->lpVtbl -> get_LastError(This,pVal) ) 

#define ITempArticle_get_ErrorLine(This,pVal)	\
    ( (This)->lpVtbl -> get_ErrorLine(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITempArticle_INTERFACE_DEFINED__ */


#ifndef __IDictionary_INTERFACE_DEFINED__
#define __IDictionary_INTERFACE_DEFINED__

/* interface IDictionary */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IDictionary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FCD1516D-23C4-11D3-A78B-0EEA0E000000")
    IDictionary : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadWithoutComments( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitsSize( 
            /* [out][retval] */ WORD *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitStr( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitMeanNum( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BYTE *MeanNum) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitStartPos( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitEndPos( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitSelected( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitAuthor( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEmptyArticle( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeFieldNo( 
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeSignatNo( 
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeLevelId( 
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeLeafId( 
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeItem( 
            /* [in] */ LONG CortegeNo,
            /* [in] */ BYTE PositionInCortege,
            /* [out][retval] */ LONG *ItemId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSelectedUnitsSize( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LocateUnit( 
            /* [in] */ BSTR UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [out][retval] */ WORD *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemNoByItemStr( 
            /* [in] */ BSTR ItemStr,
            /* [in] */ BYTE DomNo,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomItemStr( 
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DelCorteges( 
            /* [in] */ LONG StartPos,
            /* [in] */ LONG EndPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegesSize( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitStartPos( 
            /* [in] */ WORD UnitNo,
            /* [in] */ LONG NewValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitEndPos( 
            /* [in] */ WORD UnitNo,
            /* [in] */ LONG NewValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitCurrentTime( 
            /* [in] */ WORD UnitNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFieldNoByFieldStr( 
            /* [in] */ BSTR Str,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLexPlusDomNo( 
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomItemsSize( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomItemDomNo( 
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertDomItem( 
            /* [in] */ BSTR ItemStr,
            /* [in] */ BYTE DomNo,
            /* [out][retval] */ int *ItemNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BuildSignat( 
            /* [in] */ ISignat *Signat,
            /* [in] */ BSTR DomainString,
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DelDomItem( 
            /* [in] */ LONG ItemNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFieldsSize( 
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomainNoByDomStr( 
            /* [in] */ BSTR DomStr,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertUnit( 
            /* [in] */ BSTR UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitCommentStr( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Str) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitSelected( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BOOL Selected) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadUnitComments( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFieldDomainNo( 
            /* [out][retval] */ BYTE *FieldNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomainsSize( 
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BuildFormats( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSelectedUnitNo( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ WORD *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitModifTimeStr( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitAuthor( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Author) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnitsLowerBound( 
            /* [in] */ BSTR UnitStr,
            /* [out][retval] */ WORD *UniNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DelUnit( 
            /* [in] */ WORD UnitNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitCommentStr( 
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWildCardDomItemNo( 
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitStr( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR UnitStr) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Domains( 
            /* [in] */ BYTE DomNo,
            /* [retval][out] */ IDomain **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Domains( 
            /* [in] */ BYTE DomNo,
            /* [in] */ IDomain *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Fields( 
            /* [in] */ BYTE FieldNo,
            /* [retval][out] */ IField **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Fields( 
            /* [in] */ BYTE FieldNo,
            /* [in] */ IField *newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IncludeArticle( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Article,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateConstDomains( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitRusConsts( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxNumDom( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearUnit( 
            /* [in] */ WORD UnitNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckArticleError( 
            /* [in] */ BSTR Article,
            /* [out] */ BSTR *Error,
            /* [retval][out] */ BOOL *Result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCortegeBracketLeafId( 
            /* [in] */ long CortegeNo,
            /* [retval][out] */ long *BracketLeafId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUnitModifTimeStr( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR TimeStr) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UnitEditor( 
            /* [in] */ WORD UnitNo,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UnitEditor( 
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LocateUnit_Inproc( 
            /* [in] */ unsigned char *UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [retval][out] */ WORD *UnitNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DelAllItemsInTheDomain( 
            /* [in] */ BYTE DomNo) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bDontLoadExamples( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bDontLoadExamples( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDomItemStr_Inproc( 
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RussianFields( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RussianFields( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportFromText( 
            /* [in] */ BSTR FileName,
            /* [in] */ BOOL Simulating,
            /* [in] */ int ConflictSolver,
            /* [in] */ int StartEntry,
            /* [retval][out] */ BSTR *Messages) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitTextHeader( 
            /* [in] */ WORD UnitNo,
            /* [retval][out] */ BSTR *Result) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDictionary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDictionary * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDictionary * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDictionary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDictionary * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadWithoutComments )( 
            IDictionary * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitsSize )( 
            IDictionary * This,
            /* [out][retval] */ WORD *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitMeanNum )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BYTE *MeanNum);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitStartPos )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitEndPos )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitSelected )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitAuthor )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsEmptyArticle )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeFieldNo )( 
            IDictionary * This,
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeSignatNo )( 
            IDictionary * This,
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeLevelId )( 
            IDictionary * This,
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeLeafId )( 
            IDictionary * This,
            /* [in] */ LONG CortegeNo,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeItem )( 
            IDictionary * This,
            /* [in] */ LONG CortegeNo,
            /* [in] */ BYTE PositionInCortege,
            /* [out][retval] */ LONG *ItemId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSelectedUnitsSize )( 
            IDictionary * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LocateUnit )( 
            IDictionary * This,
            /* [in] */ BSTR UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [out][retval] */ WORD *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemNoByItemStr )( 
            IDictionary * This,
            /* [in] */ BSTR ItemStr,
            /* [in] */ BYTE DomNo,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomItemStr )( 
            IDictionary * This,
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DelCorteges )( 
            IDictionary * This,
            /* [in] */ LONG StartPos,
            /* [in] */ LONG EndPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegesSize )( 
            IDictionary * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitStartPos )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ LONG NewValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitEndPos )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ LONG NewValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitCurrentTime )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFieldNoByFieldStr )( 
            IDictionary * This,
            /* [in] */ BSTR Str,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLexPlusDomNo )( 
            IDictionary * This,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomItemsSize )( 
            IDictionary * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomItemDomNo )( 
            IDictionary * This,
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertDomItem )( 
            IDictionary * This,
            /* [in] */ BSTR ItemStr,
            /* [in] */ BYTE DomNo,
            /* [out][retval] */ int *ItemNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildSignat )( 
            IDictionary * This,
            /* [in] */ ISignat *Signat,
            /* [in] */ BSTR DomainString,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DelDomItem )( 
            IDictionary * This,
            /* [in] */ LONG ItemNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFieldsSize )( 
            IDictionary * This,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomainNoByDomStr )( 
            IDictionary * This,
            /* [in] */ BSTR DomStr,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertUnit )( 
            IDictionary * This,
            /* [in] */ BSTR UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitCommentStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Str);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitSelected )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BOOL Selected);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IDictionary * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadUnitComments )( 
            IDictionary * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFieldDomainNo )( 
            IDictionary * This,
            /* [out][retval] */ BYTE *FieldNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomainsSize )( 
            IDictionary * This,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildFormats )( 
            IDictionary * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSelectedUnitNo )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ WORD *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitModifTimeStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitAuthor )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Author);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnitsLowerBound )( 
            IDictionary * This,
            /* [in] */ BSTR UnitStr,
            /* [out][retval] */ WORD *UniNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DelUnit )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitCommentStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWildCardDomItemNo )( 
            IDictionary * This,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR UnitStr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Domains )( 
            IDictionary * This,
            /* [in] */ BYTE DomNo,
            /* [retval][out] */ IDomain **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Domains )( 
            IDictionary * This,
            /* [in] */ BYTE DomNo,
            /* [in] */ IDomain *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Fields )( 
            IDictionary * This,
            /* [in] */ BYTE FieldNo,
            /* [retval][out] */ IField **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Fields )( 
            IDictionary * This,
            /* [in] */ BYTE FieldNo,
            /* [in] */ IField *newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IncludeArticle )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR Article,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateConstDomains )( 
            IDictionary * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitRusConsts )( 
            IDictionary * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaxNumDom )( 
            IDictionary * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearUnit )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckArticleError )( 
            IDictionary * This,
            /* [in] */ BSTR Article,
            /* [out] */ BSTR *Error,
            /* [retval][out] */ BOOL *Result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IDictionary * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCortegeBracketLeafId )( 
            IDictionary * This,
            /* [in] */ long CortegeNo,
            /* [retval][out] */ long *BracketLeafId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUnitModifTimeStr )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR TimeStr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UnitEditor )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UnitEditor )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LocateUnit_Inproc )( 
            IDictionary * This,
            /* [in] */ unsigned char *UnitStr,
            /* [in] */ BYTE MeanNum,
            /* [retval][out] */ WORD *UnitNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DelAllItemsInTheDomain )( 
            IDictionary * This,
            /* [in] */ BYTE DomNo);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bDontLoadExamples )( 
            IDictionary * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bDontLoadExamples )( 
            IDictionary * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDomItemStr_Inproc )( 
            IDictionary * This,
            /* [in] */ LONG ItemNo,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RussianFields )( 
            IDictionary * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RussianFields )( 
            IDictionary * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ImportFromText )( 
            IDictionary * This,
            /* [in] */ BSTR FileName,
            /* [in] */ BOOL Simulating,
            /* [in] */ int ConflictSolver,
            /* [in] */ int StartEntry,
            /* [retval][out] */ BSTR *Messages);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitTextHeader )( 
            IDictionary * This,
            /* [in] */ WORD UnitNo,
            /* [retval][out] */ BSTR *Result);
        
        END_INTERFACE
    } IDictionaryVtbl;

    interface IDictionary
    {
        CONST_VTBL struct IDictionaryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDictionary_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDictionary_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDictionary_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDictionary_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDictionary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDictionary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDictionary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IDictionary_LoadWithoutComments(This,Path)	\
    ( (This)->lpVtbl -> LoadWithoutComments(This,Path) ) 

#define IDictionary_GetUnitsSize(This,result)	\
    ( (This)->lpVtbl -> GetUnitsSize(This,result) ) 

#define IDictionary_GetUnitStr(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitStr(This,UnitNo,result) ) 

#define IDictionary_GetUnitMeanNum(This,UnitNo,MeanNum)	\
    ( (This)->lpVtbl -> GetUnitMeanNum(This,UnitNo,MeanNum) ) 

#define IDictionary_GetUnitStartPos(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitStartPos(This,UnitNo,result) ) 

#define IDictionary_GetUnitEndPos(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitEndPos(This,UnitNo,result) ) 

#define IDictionary_GetUnitSelected(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitSelected(This,UnitNo,result) ) 

#define IDictionary_GetUnitAuthor(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitAuthor(This,UnitNo,result) ) 

#define IDictionary_IsEmptyArticle(This,UnitNo,result)	\
    ( (This)->lpVtbl -> IsEmptyArticle(This,UnitNo,result) ) 

#define IDictionary_GetCortegeFieldNo(This,CortegeNo,result)	\
    ( (This)->lpVtbl -> GetCortegeFieldNo(This,CortegeNo,result) ) 

#define IDictionary_GetCortegeSignatNo(This,CortegeNo,result)	\
    ( (This)->lpVtbl -> GetCortegeSignatNo(This,CortegeNo,result) ) 

#define IDictionary_GetCortegeLevelId(This,CortegeNo,result)	\
    ( (This)->lpVtbl -> GetCortegeLevelId(This,CortegeNo,result) ) 

#define IDictionary_GetCortegeLeafId(This,CortegeNo,result)	\
    ( (This)->lpVtbl -> GetCortegeLeafId(This,CortegeNo,result) ) 

#define IDictionary_GetCortegeItem(This,CortegeNo,PositionInCortege,ItemId)	\
    ( (This)->lpVtbl -> GetCortegeItem(This,CortegeNo,PositionInCortege,ItemId) ) 

#define IDictionary_GetSelectedUnitsSize(This,result)	\
    ( (This)->lpVtbl -> GetSelectedUnitsSize(This,result) ) 

#define IDictionary_LocateUnit(This,UnitStr,MeanNum,result)	\
    ( (This)->lpVtbl -> LocateUnit(This,UnitStr,MeanNum,result) ) 

#define IDictionary_GetItemNoByItemStr(This,ItemStr,DomNo,result)	\
    ( (This)->lpVtbl -> GetItemNoByItemStr(This,ItemStr,DomNo,result) ) 

#define IDictionary_GetDomItemStr(This,ItemNo,result)	\
    ( (This)->lpVtbl -> GetDomItemStr(This,ItemNo,result) ) 

#define IDictionary_DelCorteges(This,StartPos,EndPos)	\
    ( (This)->lpVtbl -> DelCorteges(This,StartPos,EndPos) ) 

#define IDictionary_GetCortegesSize(This,result)	\
    ( (This)->lpVtbl -> GetCortegesSize(This,result) ) 

#define IDictionary_SetUnitStartPos(This,UnitNo,NewValue)	\
    ( (This)->lpVtbl -> SetUnitStartPos(This,UnitNo,NewValue) ) 

#define IDictionary_SetUnitEndPos(This,UnitNo,NewValue)	\
    ( (This)->lpVtbl -> SetUnitEndPos(This,UnitNo,NewValue) ) 

#define IDictionary_SetUnitCurrentTime(This,UnitNo)	\
    ( (This)->lpVtbl -> SetUnitCurrentTime(This,UnitNo) ) 

#define IDictionary_GetFieldNoByFieldStr(This,Str,result)	\
    ( (This)->lpVtbl -> GetFieldNoByFieldStr(This,Str,result) ) 

#define IDictionary_GetLexPlusDomNo(This,result)	\
    ( (This)->lpVtbl -> GetLexPlusDomNo(This,result) ) 

#define IDictionary_GetDomItemsSize(This,result)	\
    ( (This)->lpVtbl -> GetDomItemsSize(This,result) ) 

#define IDictionary_GetDomItemDomNo(This,ItemNo,result)	\
    ( (This)->lpVtbl -> GetDomItemDomNo(This,ItemNo,result) ) 

#define IDictionary_InsertDomItem(This,ItemStr,DomNo,ItemNo)	\
    ( (This)->lpVtbl -> InsertDomItem(This,ItemStr,DomNo,ItemNo) ) 

#define IDictionary_BuildSignat(This,Signat,DomainString,Name)	\
    ( (This)->lpVtbl -> BuildSignat(This,Signat,DomainString,Name) ) 

#define IDictionary_DelDomItem(This,ItemNo)	\
    ( (This)->lpVtbl -> DelDomItem(This,ItemNo) ) 

#define IDictionary_GetFieldsSize(This,result)	\
    ( (This)->lpVtbl -> GetFieldsSize(This,result) ) 

#define IDictionary_GetDomainNoByDomStr(This,DomStr,result)	\
    ( (This)->lpVtbl -> GetDomainNoByDomStr(This,DomStr,result) ) 

#define IDictionary_InsertUnit(This,UnitStr,MeanNum,result)	\
    ( (This)->lpVtbl -> InsertUnit(This,UnitStr,MeanNum,result) ) 

#define IDictionary_SetUnitCommentStr(This,UnitNo,Str)	\
    ( (This)->lpVtbl -> SetUnitCommentStr(This,UnitNo,Str) ) 

#define IDictionary_SetUnitSelected(This,UnitNo,Selected)	\
    ( (This)->lpVtbl -> SetUnitSelected(This,UnitNo,Selected) ) 

#define IDictionary_Save(This)	\
    ( (This)->lpVtbl -> Save(This) ) 

#define IDictionary_LoadUnitComments(This)	\
    ( (This)->lpVtbl -> LoadUnitComments(This) ) 

#define IDictionary_GetFieldDomainNo(This,FieldNo)	\
    ( (This)->lpVtbl -> GetFieldDomainNo(This,FieldNo) ) 

#define IDictionary_GetDomainsSize(This,result)	\
    ( (This)->lpVtbl -> GetDomainsSize(This,result) ) 

#define IDictionary_BuildFormats(This)	\
    ( (This)->lpVtbl -> BuildFormats(This) ) 

#define IDictionary_GetSelectedUnitNo(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetSelectedUnitNo(This,UnitNo,result) ) 

#define IDictionary_GetUnitModifTimeStr(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitModifTimeStr(This,UnitNo,result) ) 

#define IDictionary_SetUnitAuthor(This,UnitNo,Author)	\
    ( (This)->lpVtbl -> SetUnitAuthor(This,UnitNo,Author) ) 

#define IDictionary_UnitsLowerBound(This,UnitStr,UniNo)	\
    ( (This)->lpVtbl -> UnitsLowerBound(This,UnitStr,UniNo) ) 

#define IDictionary_DelUnit(This,UnitNo)	\
    ( (This)->lpVtbl -> DelUnit(This,UnitNo) ) 

#define IDictionary_GetUnitCommentStr(This,UnitNo,result)	\
    ( (This)->lpVtbl -> GetUnitCommentStr(This,UnitNo,result) ) 

#define IDictionary_GetWildCardDomItemNo(This,result)	\
    ( (This)->lpVtbl -> GetWildCardDomItemNo(This,result) ) 

#define IDictionary_SetUnitStr(This,UnitNo,UnitStr)	\
    ( (This)->lpVtbl -> SetUnitStr(This,UnitNo,UnitStr) ) 

#define IDictionary_get_Domains(This,DomNo,pVal)	\
    ( (This)->lpVtbl -> get_Domains(This,DomNo,pVal) ) 

#define IDictionary_put_Domains(This,DomNo,newVal)	\
    ( (This)->lpVtbl -> put_Domains(This,DomNo,newVal) ) 

#define IDictionary_get_Fields(This,FieldNo,pVal)	\
    ( (This)->lpVtbl -> get_Fields(This,FieldNo,pVal) ) 

#define IDictionary_put_Fields(This,FieldNo,newVal)	\
    ( (This)->lpVtbl -> put_Fields(This,FieldNo,newVal) ) 

#define IDictionary_IncludeArticle(This,UnitNo,Article,result)	\
    ( (This)->lpVtbl -> IncludeArticle(This,UnitNo,Article,result) ) 

#define IDictionary_UpdateConstDomains(This)	\
    ( (This)->lpVtbl -> UpdateConstDomains(This) ) 

#define IDictionary_InitRusConsts(This)	\
    ( (This)->lpVtbl -> InitRusConsts(This) ) 

#define IDictionary_get_MaxNumDom(This,pVal)	\
    ( (This)->lpVtbl -> get_MaxNumDom(This,pVal) ) 

#define IDictionary_ClearUnit(This,UnitNo)	\
    ( (This)->lpVtbl -> ClearUnit(This,UnitNo) ) 

#define IDictionary_CheckArticleError(This,Article,Error,Result)	\
    ( (This)->lpVtbl -> CheckArticleError(This,Article,Error,Result) ) 

#define IDictionary_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IDictionary_GetCortegeBracketLeafId(This,CortegeNo,BracketLeafId)	\
    ( (This)->lpVtbl -> GetCortegeBracketLeafId(This,CortegeNo,BracketLeafId) ) 

#define IDictionary_SetUnitModifTimeStr(This,UnitNo,TimeStr)	\
    ( (This)->lpVtbl -> SetUnitModifTimeStr(This,UnitNo,TimeStr) ) 

#define IDictionary_get_UnitEditor(This,UnitNo,pVal)	\
    ( (This)->lpVtbl -> get_UnitEditor(This,UnitNo,pVal) ) 

#define IDictionary_put_UnitEditor(This,UnitNo,newVal)	\
    ( (This)->lpVtbl -> put_UnitEditor(This,UnitNo,newVal) ) 

#define IDictionary_LocateUnit_Inproc(This,UnitStr,MeanNum,UnitNo)	\
    ( (This)->lpVtbl -> LocateUnit_Inproc(This,UnitStr,MeanNum,UnitNo) ) 

#define IDictionary_DelAllItemsInTheDomain(This,DomNo)	\
    ( (This)->lpVtbl -> DelAllItemsInTheDomain(This,DomNo) ) 

#define IDictionary_get_bDontLoadExamples(This,pVal)	\
    ( (This)->lpVtbl -> get_bDontLoadExamples(This,pVal) ) 

#define IDictionary_put_bDontLoadExamples(This,newVal)	\
    ( (This)->lpVtbl -> put_bDontLoadExamples(This,newVal) ) 

#define IDictionary_GetDomItemStr_Inproc(This,ItemNo,result)	\
    ( (This)->lpVtbl -> GetDomItemStr_Inproc(This,ItemNo,result) ) 

#define IDictionary_get_RussianFields(This,pVal)	\
    ( (This)->lpVtbl -> get_RussianFields(This,pVal) ) 

#define IDictionary_put_RussianFields(This,newVal)	\
    ( (This)->lpVtbl -> put_RussianFields(This,newVal) ) 

#define IDictionary_ImportFromText(This,FileName,Simulating,ConflictSolver,StartEntry,Messages)	\
    ( (This)->lpVtbl -> ImportFromText(This,FileName,Simulating,ConflictSolver,StartEntry,Messages) ) 

#define IDictionary_GetUnitTextHeader(This,UnitNo,Result)	\
    ( (This)->lpVtbl -> GetUnitTextHeader(This,UnitNo,Result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDictionary_INTERFACE_DEFINED__ */



#ifndef __STRUCTDICTLib_LIBRARY_DEFINED__
#define __STRUCTDICTLib_LIBRARY_DEFINED__

/* library STRUCTDICTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_STRUCTDICTLib;

EXTERN_C const CLSID CLSID_Dictionary;

#ifdef __cplusplus

class DECLSPEC_UUID("FCD1516E-23C4-11D3-A78B-0EEA0E000000")
Dictionary;
#endif

EXTERN_C const CLSID CLSID_Signat;

#ifdef __cplusplus

class DECLSPEC_UUID("3F269912-246D-11D3-A78C-067993000000")
Signat;
#endif

EXTERN_C const CLSID CLSID_Domain;

#ifdef __cplusplus

class DECLSPEC_UUID("FD4C1F32-2608-11D3-A78E-9089CA000000")
Domain;
#endif

EXTERN_C const CLSID CLSID_Field;

#ifdef __cplusplus

class DECLSPEC_UUID("FD4C1F36-2608-11D3-A78E-9089CA000000")
Field;
#endif

EXTERN_C const CLSID CLSID_TempArticle;

#ifdef __cplusplus

class DECLSPEC_UUID("B3C7D665-2C57-11D3-A7A1-80F9E5000000")
TempArticle;
#endif

EXTERN_C const CLSID CLSID_TextField;

#ifdef __cplusplus

class DECLSPEC_UUID("B3C7D667-2C57-11D3-A7A1-80F9E5000000")
TextField;
#endif
#endif /* __STRUCTDICTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


