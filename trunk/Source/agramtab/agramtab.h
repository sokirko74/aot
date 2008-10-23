

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Oct 23 10:48:47 2008
 */
/* Compiler settings for .\agramtab.idl:
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

#ifndef __agramtab_h__
#define __agramtab_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGramTab_FWD_DEFINED__
#define __IGramTab_FWD_DEFINED__
typedef interface IGramTab IGramTab;
#endif 	/* __IGramTab_FWD_DEFINED__ */


#ifndef __RusGramTab_FWD_DEFINED__
#define __RusGramTab_FWD_DEFINED__

#ifdef __cplusplus
typedef class RusGramTab RusGramTab;
#else
typedef struct RusGramTab RusGramTab;
#endif /* __cplusplus */

#endif 	/* __RusGramTab_FWD_DEFINED__ */


#ifndef __EngGramTab_FWD_DEFINED__
#define __EngGramTab_FWD_DEFINED__

#ifdef __cplusplus
typedef class EngGramTab EngGramTab;
#else
typedef struct EngGramTab EngGramTab;
#endif /* __cplusplus */

#endif 	/* __EngGramTab_FWD_DEFINED__ */


#ifndef __GerGramTab_FWD_DEFINED__
#define __GerGramTab_FWD_DEFINED__

#ifdef __cplusplus
typedef class GerGramTab GerGramTab;
#else
typedef struct GerGramTab GerGramTab;
#endif /* __cplusplus */

#endif 	/* __GerGramTab_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IGramTab_INTERFACE_DEFINED__
#define __IGramTab_INTERFACE_DEFINED__

/* interface IGramTab */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGramTab;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECD62F7D-19B6-11D3-A77A-2679A7000000")
    IGramTab : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPartOfSpeech( 
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ BYTE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGrammems( 
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ hyper *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClauseTypeByName( 
            /* [in] */ BSTR bstrTypeName,
            /* [retval][out] */ long *lType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClauseTypeByNameIfCan( 
            /* [in] */ BSTR bstrTypeName,
            /* [out] */ long *lType,
            /* [retval][out] */ BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClauseNameByType( 
            /* [in] */ long type,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGramCodeByGrammemsAndPartofSpeechIfCan( 
            /* [in] */ BYTE Pos,
            /* [in] */ hyper grammems,
            /* [out] */ BSTR *gramcodes,
            /* [retval][out] */ BOOL *bRes) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PartOfSpeechCount( 
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GrammemsCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GrammemsToStr( 
            /* [in] */ hyper grammems,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPartOfSpeechStr( 
            /* [in] */ BYTE PartOfSpeech,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HaveEqualPartOfSpeech( 
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessPOSAndGrammems( 
            /* [in] */ BSTR in,
            /* [out] */ BYTE *PartOfSpeech,
            /* [out] */ hyper *Grammems) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GleicheGenderNumberCase( 
            /* [in] */ BSTR common_noun_gram_code,
            /* [in] */ BSTR noun_gram_code,
            /* [in] */ BSTR adj_gram_code,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GleicheCaseNumber( 
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllGrammems( 
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ hyper *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GleicheGenderNumber( 
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindGrammem( 
            /* [in] */ BSTR gram_codes,
            /* [in] */ hyper grammems,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GleicheSubjectPredicate( 
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *bRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessPOSAndGrammemsIfCan( 
            /* [in] */ BSTR in,
            /* [out] */ BYTE *PartOfSpeech,
            /* [out] */ hyper *Grammems,
            /* [retval][out] */ BOOL *Result) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGramTabVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGramTab * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGramTab * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGramTab * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGramTab * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGramTab * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGramTab * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGramTab * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPartOfSpeech )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ BYTE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGrammems )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ hyper *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClauseTypeByName )( 
            IGramTab * This,
            /* [in] */ BSTR bstrTypeName,
            /* [retval][out] */ long *lType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClauseTypeByNameIfCan )( 
            IGramTab * This,
            /* [in] */ BSTR bstrTypeName,
            /* [out] */ long *lType,
            /* [retval][out] */ BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClauseNameByType )( 
            IGramTab * This,
            /* [in] */ long type,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGramCodeByGrammemsAndPartofSpeechIfCan )( 
            IGramTab * This,
            /* [in] */ BYTE Pos,
            /* [in] */ hyper grammems,
            /* [out] */ BSTR *gramcodes,
            /* [retval][out] */ BOOL *bRes);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PartOfSpeechCount )( 
            IGramTab * This,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GrammemsCount )( 
            IGramTab * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GrammemsToStr )( 
            IGramTab * This,
            /* [in] */ hyper grammems,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPartOfSpeechStr )( 
            IGramTab * This,
            /* [in] */ BYTE PartOfSpeech,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            IGramTab * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *HaveEqualPartOfSpeech )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessPOSAndGrammems )( 
            IGramTab * This,
            /* [in] */ BSTR in,
            /* [out] */ BYTE *PartOfSpeech,
            /* [out] */ hyper *Grammems);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GleicheGenderNumberCase )( 
            IGramTab * This,
            /* [in] */ BSTR common_noun_gram_code,
            /* [in] */ BSTR noun_gram_code,
            /* [in] */ BSTR adj_gram_code,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GleicheCaseNumber )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAllGrammems )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code,
            /* [out][retval] */ hyper *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GleicheGenderNumber )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindGrammem )( 
            IGramTab * This,
            /* [in] */ BSTR gram_codes,
            /* [in] */ hyper grammems,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GleicheSubjectPredicate )( 
            IGramTab * This,
            /* [in] */ BSTR gram_code1,
            /* [in] */ BSTR gram_code2,
            /* [out][retval] */ BOOL *bRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessPOSAndGrammemsIfCan )( 
            IGramTab * This,
            /* [in] */ BSTR in,
            /* [out] */ BYTE *PartOfSpeech,
            /* [out] */ hyper *Grammems,
            /* [retval][out] */ BOOL *Result);
        
        END_INTERFACE
    } IGramTabVtbl;

    interface IGramTab
    {
        CONST_VTBL struct IGramTabVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGramTab_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGramTab_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGramTab_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGramTab_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGramTab_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGramTab_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGramTab_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGramTab_GetPartOfSpeech(This,gram_code,result)	\
    (This)->lpVtbl -> GetPartOfSpeech(This,gram_code,result)

#define IGramTab_GetGrammems(This,gram_code,result)	\
    (This)->lpVtbl -> GetGrammems(This,gram_code,result)

#define IGramTab_GetClauseTypeByName(This,bstrTypeName,lType)	\
    (This)->lpVtbl -> GetClauseTypeByName(This,bstrTypeName,lType)

#define IGramTab_GetClauseTypeByNameIfCan(This,bstrTypeName,lType,Result)	\
    (This)->lpVtbl -> GetClauseTypeByNameIfCan(This,bstrTypeName,lType,Result)

#define IGramTab_GetClauseNameByType(This,type,name)	\
    (This)->lpVtbl -> GetClauseNameByType(This,type,name)

#define IGramTab_GetGramCodeByGrammemsAndPartofSpeechIfCan(This,Pos,grammems,gramcodes,bRes)	\
    (This)->lpVtbl -> GetGramCodeByGrammemsAndPartofSpeechIfCan(This,Pos,grammems,gramcodes,bRes)

#define IGramTab_get_PartOfSpeechCount(This,pVal)	\
    (This)->lpVtbl -> get_PartOfSpeechCount(This,pVal)

#define IGramTab_get_GrammemsCount(This,pVal)	\
    (This)->lpVtbl -> get_GrammemsCount(This,pVal)

#define IGramTab_GrammemsToStr(This,grammems,result)	\
    (This)->lpVtbl -> GrammemsToStr(This,grammems,result)

#define IGramTab_GetPartOfSpeechStr(This,PartOfSpeech,result)	\
    (This)->lpVtbl -> GetPartOfSpeechStr(This,PartOfSpeech,result)

#define IGramTab_Load(This)	\
    (This)->lpVtbl -> Load(This)

#define IGramTab_HaveEqualPartOfSpeech(This,gram_code1,gram_code2,result)	\
    (This)->lpVtbl -> HaveEqualPartOfSpeech(This,gram_code1,gram_code2,result)

#define IGramTab_ProcessPOSAndGrammems(This,in,PartOfSpeech,Grammems)	\
    (This)->lpVtbl -> ProcessPOSAndGrammems(This,in,PartOfSpeech,Grammems)

#define IGramTab_GleicheGenderNumberCase(This,common_noun_gram_code,noun_gram_code,adj_gram_code,result)	\
    (This)->lpVtbl -> GleicheGenderNumberCase(This,common_noun_gram_code,noun_gram_code,adj_gram_code,result)

#define IGramTab_GleicheCaseNumber(This,gram_code1,gram_code2,result)	\
    (This)->lpVtbl -> GleicheCaseNumber(This,gram_code1,gram_code2,result)

#define IGramTab_GetAllGrammems(This,gram_code,result)	\
    (This)->lpVtbl -> GetAllGrammems(This,gram_code,result)

#define IGramTab_GleicheGenderNumber(This,gram_code1,gram_code2,result)	\
    (This)->lpVtbl -> GleicheGenderNumber(This,gram_code1,gram_code2,result)

#define IGramTab_FindGrammem(This,gram_codes,grammems,result)	\
    (This)->lpVtbl -> FindGrammem(This,gram_codes,grammems,result)

#define IGramTab_GleicheSubjectPredicate(This,gram_code1,gram_code2,bRes)	\
    (This)->lpVtbl -> GleicheSubjectPredicate(This,gram_code1,gram_code2,bRes)

#define IGramTab_ProcessPOSAndGrammemsIfCan(This,in,PartOfSpeech,Grammems,Result)	\
    (This)->lpVtbl -> ProcessPOSAndGrammemsIfCan(This,in,PartOfSpeech,Grammems,Result)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetPartOfSpeech_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code,
    /* [out][retval] */ BYTE *result);


void __RPC_STUB IGramTab_GetPartOfSpeech_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetGrammems_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code,
    /* [out][retval] */ hyper *result);


void __RPC_STUB IGramTab_GetGrammems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetClauseTypeByName_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR bstrTypeName,
    /* [retval][out] */ long *lType);


void __RPC_STUB IGramTab_GetClauseTypeByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetClauseTypeByNameIfCan_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR bstrTypeName,
    /* [out] */ long *lType,
    /* [retval][out] */ BOOL *Result);


void __RPC_STUB IGramTab_GetClauseTypeByNameIfCan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetClauseNameByType_Proxy( 
    IGramTab * This,
    /* [in] */ long type,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IGramTab_GetClauseNameByType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetGramCodeByGrammemsAndPartofSpeechIfCan_Proxy( 
    IGramTab * This,
    /* [in] */ BYTE Pos,
    /* [in] */ hyper grammems,
    /* [out] */ BSTR *gramcodes,
    /* [retval][out] */ BOOL *bRes);


void __RPC_STUB IGramTab_GetGramCodeByGrammemsAndPartofSpeechIfCan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGramTab_get_PartOfSpeechCount_Proxy( 
    IGramTab * This,
    /* [retval][out] */ BYTE *pVal);


void __RPC_STUB IGramTab_get_PartOfSpeechCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGramTab_get_GrammemsCount_Proxy( 
    IGramTab * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IGramTab_get_GrammemsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GrammemsToStr_Proxy( 
    IGramTab * This,
    /* [in] */ hyper grammems,
    /* [out][retval] */ BSTR *result);


void __RPC_STUB IGramTab_GrammemsToStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetPartOfSpeechStr_Proxy( 
    IGramTab * This,
    /* [in] */ BYTE PartOfSpeech,
    /* [out][retval] */ BSTR *result);


void __RPC_STUB IGramTab_GetPartOfSpeechStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_Load_Proxy( 
    IGramTab * This);


void __RPC_STUB IGramTab_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_HaveEqualPartOfSpeech_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code1,
    /* [in] */ BSTR gram_code2,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGramTab_HaveEqualPartOfSpeech_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_ProcessPOSAndGrammems_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR in,
    /* [out] */ BYTE *PartOfSpeech,
    /* [out] */ hyper *Grammems);


void __RPC_STUB IGramTab_ProcessPOSAndGrammems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GleicheGenderNumberCase_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR common_noun_gram_code,
    /* [in] */ BSTR noun_gram_code,
    /* [in] */ BSTR adj_gram_code,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGramTab_GleicheGenderNumberCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GleicheCaseNumber_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code1,
    /* [in] */ BSTR gram_code2,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGramTab_GleicheCaseNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GetAllGrammems_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code,
    /* [out][retval] */ hyper *result);


void __RPC_STUB IGramTab_GetAllGrammems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GleicheGenderNumber_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code1,
    /* [in] */ BSTR gram_code2,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGramTab_GleicheGenderNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_FindGrammem_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_codes,
    /* [in] */ hyper grammems,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGramTab_FindGrammem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_GleicheSubjectPredicate_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR gram_code1,
    /* [in] */ BSTR gram_code2,
    /* [out][retval] */ BOOL *bRes);


void __RPC_STUB IGramTab_GleicheSubjectPredicate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGramTab_ProcessPOSAndGrammemsIfCan_Proxy( 
    IGramTab * This,
    /* [in] */ BSTR in,
    /* [out] */ BYTE *PartOfSpeech,
    /* [out] */ hyper *Grammems,
    /* [retval][out] */ BOOL *Result);


void __RPC_STUB IGramTab_ProcessPOSAndGrammemsIfCan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGramTab_INTERFACE_DEFINED__ */



#ifndef __AGRAMTABLib_LIBRARY_DEFINED__
#define __AGRAMTABLib_LIBRARY_DEFINED__

/* library AGRAMTABLib */
/* [helpstring][version][uuid] */ 


enum rPartOfSpeeches
    {	NOUN	= 0,
	ADJ_FULL	= 1,
	VERB	= 2,
	PRONOUN	= 3,
	PRONOUN_P	= 4,
	PRONOUN_PREDK	= 5,
	NUMERAL	= 6,
	NUMERAL_P	= 7,
	ADV	= 8,
	PREDK	= 9,
	PREP	= 10,
	POSL	= 11,
	CONJ	= 12,
	INTERJ	= 13,
	INP	= 14,
	PHRASE	= 15,
	PARTICLE	= 16,
	ADJ_SHORT	= 17,
	PARTICIPLE	= 18,
	ADVERB_PARTICIPLE	= 19,
	PARTICIPLE_SHORT	= 20,
	INFINITIVE	= 21
    } ;

enum rGrammems
    {	rPlural	= 0,
	rSingular	= 1,
	rAllNumbers	= 1 << rSingular | 1 << rPlural,
	rNominativ	= 2,
	rGenitiv	= 3,
	rDativ	= 4,
	rAccusativ	= 5,
	rInstrumentalis	= 6,
	rLocativ	= 7,
	rVocativ	= 8,
	rAllCases	= 1 << rNominativ | 1 << rGenitiv | 1 << rDativ | 1 << rAccusativ | 1 << rInstrumentalis | 1 << rVocativ | 1 << rLocativ,
	rMasculinum	= 9,
	rFeminum	= 10,
	rNeutrum	= 11,
	rMascFem	= 12,
	rAllGenders	= 1 << rMasculinum | 1 << rFeminum | 1 << rNeutrum,
	rPresentTense	= 13,
	rFutureTense	= 14,
	rPastTense	= 15,
	rAllTimes	= 1 << rPresentTense | 1 << rFutureTense | 1 << rPastTense,
	rFirstPerson	= 16,
	rSecondPerson	= 17,
	rThirdPerson	= 18,
	rAllPersons	= 1 << rFirstPerson | 1 << rSecondPerson | 1 << rThirdPerson,
	rImperative	= 19,
	rAnimative	= 20,
	rNonAnimative	= 21,
	rAllAnimative	= 1 << rAnimative | 1 << rNonAnimative,
	rComparative	= 22,
	rPerfective	= 23,
	rNonPerfective	= 24,
	rNonTransitive	= 25,
	rTransitive	= 26,
	rActiveVoice	= 27,
	rPassiveVoice	= 28,
	rIndeclinable	= 29,
	rInitialism	= 30,
	rPatronymic	= 31,
	rToponym	= 32,
	rOrganisation	= 33,
	rQualitative	= 34,
	rDeFactoSingTantum	= 35,
	rInterrogative	= 36,
	rDemonstrative	= 37,
	rName	= 38,
	rSurName	= 39,
	rImpersonal	= 40,
	rSlang	= 41,
	rMisprint	= 42,
	rColloquial	= 43,
	rPossessive	= 44,
	rArchaism	= 45,
	rSecondCase	= 46,
	rPoetry	= 47,
	rProfession	= 48,
	rSuperlative	= 49,
	rPositive	= 50
    } ;
typedef /* [public] */ 
enum __MIDL___MIDL_itf_agramtab_0262_0001
    {	VERB_PERS_T	= 0,
	ADVERB_PARTICIPLE_T	= 1,
	PARTICIPLE_SHORT_T	= 2,
	ADJ_SHORT_T	= 3,
	PREDK_T	= 4,
	PARTICIPLE_T	= 5,
	INFINITIVE_T	= 6,
	INP_T	= 7,
	DASH_T	= 8,
	UNDETACHED_ADJ_PATIC	= 9,
	COMPARATIVE_T	= 10,
	COPUL_T	= 11
    } 	rClauseTypeEnum;


enum gPartOfSpeeches
    {	gART	= 0,
	gADJ	= 1,
	gADV	= 2,
	gEIG	= 3,
	gSUB	= 4,
	gVER	= 5,
	gPA1	= 6,
	gPA2	= 7,
	gPRONOMEN	= 8,
	gPRP	= 9,
	gKON	= 10,
	gNEG	= 11,
	gINJ	= 12,
	gZAL	= 13,
	gZUS	= 14,
	gPRO_BEG	= 15,
	gZU_INFINITIV	= 16
    } ;

enum gGrammems
    {	gNoaUnk	= 0,
	gPredikBenutz	= 1,
	gProUnk	= 2,
	gTmpUnk	= 3,
	gNac	= 4,
	gMou	= 5,
	gCou	= 6,
	gGeo	= 7,
	gWasser	= 8,
	gGeb	= 9,
	gStd	= 10,
	gLok	= 11,
	gVor	= 12,
	gSichAcc	= 13,
	gSichDat	= 14,
	gSchwach	= 15,
	gNichtSchwach	= 16,
	gModal	= 17,
	gAuxiliar	= 18,
	gKonj1	= 19,
	gKonj2	= 20,
	gPartizip1	= 21,
	gPartizip2	= 22,
	gZuVerbForm	= 23,
	gImperativ	= 24,
	gPraeteritum	= 25,
	gPrasens	= 26,
	gGrundform	= 27,
	gKomparativ	= 28,
	gSuperlativ	= 29,
	gProportionalKonjunktion	= 30,
	gInfinitiv	= 31,
	gVergleichsKonjunktion	= 32,
	gNebenordnende	= 33,
	gUnterordnende	= 34,
	gPersonal	= 35,
	gDemonstrativ	= 36,
	gInterrogativ	= 37,
	gPossessiv	= 38,
	gReflexiv	= 39,
	gRinPronomen	= 40,
	gAlgPronomen	= 41,
	gAdjektiveOhneArtikel	= 42,
	gAdjektiveMitUnbestimmte	= 43,
	gAdjektiveMitBestimmte	= 44,
	gErstePerson	= 45,
	gZweitePerson	= 46,
	gDrittePerson	= 47,
	gFeminin	= 48,
	gMaskulin	= 49,
	gNeutrum	= 50,
	gPlural	= 51,
	gSingular	= 52,
	gNominativ	= 53,
	gGenitiv	= 54,
	gDativ	= 55,
	gAkkusativ	= 56,
	gAbbreviation	= 57,
	gEinwohner	= 58,
	gTransitiv	= 59,
	gIntransitiv	= 60,
	gImpersonal	= 61
    } ;
typedef /* [public] */ 
enum __MIDL___MIDL_itf_agramtab_0262_0002
    {	VERBSATZ_T	= 0,
	PARTIZIPIALSATZ_T	= 1,
	INFINITIVSATZ_T	= 2
    } 	gClauseTypeEnum;


enum ePartOfSpeeches
    {	eNOUN	= 0,
	eADJ	= 1,
	eVERB	= 2,
	eVBE	= 3,
	eMOD	= 4,
	eNUMERAL	= 5,
	eCONJ	= 6,
	eINTERJ	= 7,
	ePREP	= 8,
	ePARTICLE	= 9,
	eART	= 10,
	eADV	= 11,
	ePN	= 12,
	eORDNUM	= 13,
	ePRON	= 14,
	ePOSS	= 15,
	ePN_ADJ	= 16
    } ;

enum eGrammems
    {	eSingular	= 0,
	ePlural	= 1,
	eMasculinum	= 2,
	eFeminum	= 3,
	eAnimative	= 4,
	ePerfective	= 5,
	eNominative	= 6,
	eObjectCase	= 7,
	eNarrative	= 8,
	eGeographics	= 9,
	eProper	= 10,
	ePersonalPronoun	= 11,
	ePossessive	= 12,
	ePredicative	= 13,
	eUncountable	= 14,
	eReflexivePronoun	= 15,
	eDemonstrativePronoun	= 16,
	eMass	= 17,
	eComparativ	= 18,
	eSupremum	= 19,
	eFirstPerson	= 20,
	eSecondPerson	= 21,
	eThirdPerson	= 22,
	ePresentIndef	= 23,
	eInfinitive	= 24,
	ePastIndef	= 25,
	ePastParticiple	= 26,
	eGerund	= 27,
	eFuturum	= 28,
	eConditional	= 29,
	eApostropheS	= 30,
	eApostrophe	= 31,
	eNames	= 32,
	eOrganisation	= 33
    } ;

EXTERN_C const IID LIBID_AGRAMTABLib;

EXTERN_C const CLSID CLSID_RusGramTab;

#ifdef __cplusplus

class DECLSPEC_UUID("ECD62F7E-19B6-11D3-A77A-2679A7000000")
RusGramTab;
#endif

EXTERN_C const CLSID CLSID_EngGramTab;

#ifdef __cplusplus

class DECLSPEC_UUID("3B09AA0A-27A7-11D3-A791-BCD3B4000000")
EngGramTab;
#endif

EXTERN_C const CLSID CLSID_GerGramTab;

#ifdef __cplusplus

class DECLSPEC_UUID("2B33F700-3899-4FB3-9EF6-C714926BC700")
GerGramTab;
#endif
#endif /* __AGRAMTABLib_LIBRARY_DEFINED__ */

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


