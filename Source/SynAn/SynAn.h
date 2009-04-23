

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Dec 11 18:50:02 2008
 */
/* Compiler settings for .\SynAn.idl:
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

#ifndef __SynAn_h__
#define __SynAn_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IClauseRoot_FWD_DEFINED__
#define __IClauseRoot_FWD_DEFINED__
typedef interface IClauseRoot IClauseRoot;
#endif 	/* __IClauseRoot_FWD_DEFINED__ */


#ifndef __ISyntaxUnit_FWD_DEFINED__
#define __ISyntaxUnit_FWD_DEFINED__
typedef interface ISyntaxUnit ISyntaxUnit;
#endif 	/* __ISyntaxUnit_FWD_DEFINED__ */


#ifndef __IHomonym_FWD_DEFINED__
#define __IHomonym_FWD_DEFINED__
typedef interface IHomonym IHomonym;
#endif 	/* __IHomonym_FWD_DEFINED__ */


#ifndef __IWord_FWD_DEFINED__
#define __IWord_FWD_DEFINED__
typedef interface IWord IWord;
#endif 	/* __IWord_FWD_DEFINED__ */


#ifndef __IRelation_FWD_DEFINED__
#define __IRelation_FWD_DEFINED__
typedef interface IRelation IRelation;
#endif 	/* __IRelation_FWD_DEFINED__ */


#ifndef __IGroup_FWD_DEFINED__
#define __IGroup_FWD_DEFINED__
typedef interface IGroup IGroup;
#endif 	/* __IGroup_FWD_DEFINED__ */


#ifndef __IClauseVariant_FWD_DEFINED__
#define __IClauseVariant_FWD_DEFINED__
typedef interface IClauseVariant IClauseVariant;
#endif 	/* __IClauseVariant_FWD_DEFINED__ */


#ifndef __IClause_FWD_DEFINED__
#define __IClause_FWD_DEFINED__
typedef interface IClause IClause;
#endif 	/* __IClause_FWD_DEFINED__ */


#ifndef __ISentence_FWD_DEFINED__
#define __ISentence_FWD_DEFINED__
typedef interface ISentence ISentence;
#endif 	/* __ISentence_FWD_DEFINED__ */


#ifndef __ISentencesCollection_FWD_DEFINED__
#define __ISentencesCollection_FWD_DEFINED__
typedef interface ISentencesCollection ISentencesCollection;
#endif 	/* __ISentencesCollection_FWD_DEFINED__ */


#ifndef __IRelationsIterator_FWD_DEFINED__
#define __IRelationsIterator_FWD_DEFINED__
typedef interface IRelationsIterator IRelationsIterator;
#endif 	/* __IRelationsIterator_FWD_DEFINED__ */


#ifndef __SentencesCollection_FWD_DEFINED__
#define __SentencesCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class SentencesCollection SentencesCollection;
#else
typedef struct SentencesCollection SentencesCollection;
#endif /* __cplusplus */

#endif 	/* __SentencesCollection_FWD_DEFINED__ */


#ifndef __Sentence_FWD_DEFINED__
#define __Sentence_FWD_DEFINED__

#ifdef __cplusplus
typedef class Sentence Sentence;
#else
typedef struct Sentence Sentence;
#endif /* __cplusplus */

#endif 	/* __Sentence_FWD_DEFINED__ */


#ifndef __Word_FWD_DEFINED__
#define __Word_FWD_DEFINED__

#ifdef __cplusplus
typedef class Word Word;
#else
typedef struct Word Word;
#endif /* __cplusplus */

#endif 	/* __Word_FWD_DEFINED__ */


#ifndef __Clause_FWD_DEFINED__
#define __Clause_FWD_DEFINED__

#ifdef __cplusplus
typedef class Clause Clause;
#else
typedef struct Clause Clause;
#endif /* __cplusplus */

#endif 	/* __Clause_FWD_DEFINED__ */


#ifndef __Group_FWD_DEFINED__
#define __Group_FWD_DEFINED__

#ifdef __cplusplus
typedef class Group Group;
#else
typedef struct Group Group;
#endif /* __cplusplus */

#endif 	/* __Group_FWD_DEFINED__ */


#ifndef __ClauseVariant_FWD_DEFINED__
#define __ClauseVariant_FWD_DEFINED__

#ifdef __cplusplus
typedef class ClauseVariant ClauseVariant;
#else
typedef struct ClauseVariant ClauseVariant;
#endif /* __cplusplus */

#endif 	/* __ClauseVariant_FWD_DEFINED__ */


#ifndef __Relation_FWD_DEFINED__
#define __Relation_FWD_DEFINED__

#ifdef __cplusplus
typedef class Relation Relation;
#else
typedef struct Relation Relation;
#endif /* __cplusplus */

#endif 	/* __Relation_FWD_DEFINED__ */


#ifndef __Homonym_FWD_DEFINED__
#define __Homonym_FWD_DEFINED__

#ifdef __cplusplus
typedef class Homonym Homonym;
#else
typedef struct Homonym Homonym;
#endif /* __cplusplus */

#endif 	/* __Homonym_FWD_DEFINED__ */


#ifndef __ClauseRoot_FWD_DEFINED__
#define __ClauseRoot_FWD_DEFINED__

#ifdef __cplusplus
typedef class ClauseRoot ClauseRoot;
#else
typedef struct ClauseRoot ClauseRoot;
#endif /* __cplusplus */

#endif 	/* __ClauseRoot_FWD_DEFINED__ */


#ifndef __SyntaxUnit_FWD_DEFINED__
#define __SyntaxUnit_FWD_DEFINED__

#ifdef __cplusplus
typedef class SyntaxUnit SyntaxUnit;
#else
typedef struct SyntaxUnit SyntaxUnit;
#endif /* __cplusplus */

#endif 	/* __SyntaxUnit_FWD_DEFINED__ */


#ifndef __RelationsIterator_FWD_DEFINED__
#define __RelationsIterator_FWD_DEFINED__

#ifdef __cplusplus
typedef class RelationsIterator RelationsIterator;
#else
typedef struct RelationsIterator RelationsIterator;
#endif /* __cplusplus */

#endif 	/* __RelationsIterator_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_SynAn_0000_0000 */
/* [local] */ 







extern RPC_IF_HANDLE __MIDL_itf_SynAn_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_SynAn_0000_0000_v0_0_s_ifspec;

#ifndef __IClauseRoot_INTERFACE_DEFINED__
#define __IClauseRoot_INTERFACE_DEFINED__

/* interface IClauseRoot */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IClauseRoot;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E022CF53-037F-11D3-8C22-00105A68ADF3")
    IClauseRoot : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RootWordNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RootHomonymNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClauseRootVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClauseRoot * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClauseRoot * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClauseRoot * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IClauseRoot * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IClauseRoot * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IClauseRoot * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IClauseRoot * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IClauseRoot * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RootWordNo )( 
            IClauseRoot * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RootHomonymNo )( 
            IClauseRoot * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IClauseRoot * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IClauseRootVtbl;

    interface IClauseRoot
    {
        CONST_VTBL struct IClauseRootVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClauseRoot_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClauseRoot_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClauseRoot_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClauseRoot_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IClauseRoot_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IClauseRoot_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IClauseRoot_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IClauseRoot_get_Type(This,pVal)	\
    ( (This)->lpVtbl -> get_Type(This,pVal) ) 

#define IClauseRoot_get_RootWordNo(This,pVal)	\
    ( (This)->lpVtbl -> get_RootWordNo(This,pVal) ) 

#define IClauseRoot_get_RootHomonymNo(This,pVal)	\
    ( (This)->lpVtbl -> get_RootHomonymNo(This,pVal) ) 

#define IClauseRoot_get_Description(This,pVal)	\
    ( (This)->lpVtbl -> get_Description(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClauseRoot_INTERFACE_DEFINED__ */


#ifndef __ISyntaxUnit_INTERFACE_DEFINED__
#define __ISyntaxUnit_INTERFACE_DEFINED__

/* interface ISyntaxUnit */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISyntaxUnit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7794ED72-F74F-11D3-8DC5-00105A68ADF3")
    ISyntaxUnit : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WordNum( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveHomonymNum( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseNum( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveClauseTypeNum( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Grammems( 
            /* [retval][out] */ hyper *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gramcodes( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GrammemsStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SimplePrepsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SimplePrepId( 
            /* [in] */ long ii,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SimplePrepStr( 
            /* [in] */ long ii,
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyntaxUnitVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISyntaxUnit * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISyntaxUnit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISyntaxUnit * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISyntaxUnit * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISyntaxUnit * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISyntaxUnit * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISyntaxUnit * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            ISyntaxUnit * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordNum )( 
            ISyntaxUnit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveHomonymNum )( 
            ISyntaxUnit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseNum )( 
            ISyntaxUnit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveClauseTypeNum )( 
            ISyntaxUnit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Grammems )( 
            ISyntaxUnit * This,
            /* [retval][out] */ hyper *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Gramcodes )( 
            ISyntaxUnit * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GrammemsStr )( 
            ISyntaxUnit * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SimplePrepsCount )( 
            ISyntaxUnit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SimplePrepId )( 
            ISyntaxUnit * This,
            /* [in] */ long ii,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SimplePrepStr )( 
            ISyntaxUnit * This,
            /* [in] */ long ii,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } ISyntaxUnitVtbl;

    interface ISyntaxUnit
    {
        CONST_VTBL struct ISyntaxUnitVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyntaxUnit_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISyntaxUnit_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISyntaxUnit_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISyntaxUnit_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISyntaxUnit_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISyntaxUnit_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISyntaxUnit_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISyntaxUnit_get_Type(This,pVal)	\
    ( (This)->lpVtbl -> get_Type(This,pVal) ) 

#define ISyntaxUnit_get_WordNum(This,pVal)	\
    ( (This)->lpVtbl -> get_WordNum(This,pVal) ) 

#define ISyntaxUnit_get_ActiveHomonymNum(This,pVal)	\
    ( (This)->lpVtbl -> get_ActiveHomonymNum(This,pVal) ) 

#define ISyntaxUnit_get_ClauseNum(This,pVal)	\
    ( (This)->lpVtbl -> get_ClauseNum(This,pVal) ) 

#define ISyntaxUnit_get_ActiveClauseTypeNum(This,pVal)	\
    ( (This)->lpVtbl -> get_ActiveClauseTypeNum(This,pVal) ) 

#define ISyntaxUnit_get_Grammems(This,pVal)	\
    ( (This)->lpVtbl -> get_Grammems(This,pVal) ) 

#define ISyntaxUnit_get_Gramcodes(This,pVal)	\
    ( (This)->lpVtbl -> get_Gramcodes(This,pVal) ) 

#define ISyntaxUnit_get_GrammemsStr(This,pVal)	\
    ( (This)->lpVtbl -> get_GrammemsStr(This,pVal) ) 

#define ISyntaxUnit_get_SimplePrepsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_SimplePrepsCount(This,pVal) ) 

#define ISyntaxUnit_get_SimplePrepId(This,ii,pVal)	\
    ( (This)->lpVtbl -> get_SimplePrepId(This,ii,pVal) ) 

#define ISyntaxUnit_get_SimplePrepStr(This,ii,pVal)	\
    ( (This)->lpVtbl -> get_SimplePrepStr(This,ii,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISyntaxUnit_INTERFACE_DEFINED__ */


#ifndef __IHomonym_INTERFACE_DEFINED__
#define __IHomonym_INTERFACE_DEFINED__

/* interface IHomonym */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IHomonym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1EC-F8C1-11D2-8C0E-00105A68ADF3")
    IHomonym : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Grammems( 
            /* [retval][out] */ hyper *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Lemma( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LemSign( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Poses( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GramDescriptionStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParadigmID( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GramCodes( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_POSStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsOb1( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsOb2( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsInOb( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SimplePrepsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOborDictIdOfSimplePrep( 
            /* [in] */ long iNum,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOborotId( 
            /* [retval][out] */ long *OborotId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOborotGF( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CommonGrammemsStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHomonymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHomonym * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHomonym * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHomonym * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHomonym * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHomonym * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHomonym * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHomonym * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Grammems )( 
            IHomonym * This,
            /* [retval][out] */ hyper *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Lemma )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LemSign )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Poses )( 
            IHomonym * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GramDescriptionStr )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParadigmID )( 
            IHomonym * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GramCodes )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_POSStr )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsOb1 )( 
            IHomonym * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsOb2 )( 
            IHomonym * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsInOb )( 
            IHomonym * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SimplePrepsCount )( 
            IHomonym * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOborDictIdOfSimplePrep )( 
            IHomonym * This,
            /* [in] */ long iNum,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOborotId )( 
            IHomonym * This,
            /* [retval][out] */ long *OborotId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOborotGF )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CommonGrammemsStr )( 
            IHomonym * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IHomonymVtbl;

    interface IHomonym
    {
        CONST_VTBL struct IHomonymVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHomonym_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHomonym_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHomonym_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHomonym_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHomonym_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHomonym_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHomonym_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IHomonym_get_Grammems(This,pVal)	\
    ( (This)->lpVtbl -> get_Grammems(This,pVal) ) 

#define IHomonym_get_Lemma(This,pVal)	\
    ( (This)->lpVtbl -> get_Lemma(This,pVal) ) 

#define IHomonym_get_LemSign(This,pVal)	\
    ( (This)->lpVtbl -> get_LemSign(This,pVal) ) 

#define IHomonym_get_Poses(This,pVal)	\
    ( (This)->lpVtbl -> get_Poses(This,pVal) ) 

#define IHomonym_get_GramDescriptionStr(This,pVal)	\
    ( (This)->lpVtbl -> get_GramDescriptionStr(This,pVal) ) 

#define IHomonym_get_ParadigmID(This,pVal)	\
    ( (This)->lpVtbl -> get_ParadigmID(This,pVal) ) 

#define IHomonym_get_GramCodes(This,pVal)	\
    ( (This)->lpVtbl -> get_GramCodes(This,pVal) ) 

#define IHomonym_get_POSStr(This,pVal)	\
    ( (This)->lpVtbl -> get_POSStr(This,pVal) ) 

#define IHomonym_get_IsOb1(This,pVal)	\
    ( (This)->lpVtbl -> get_IsOb1(This,pVal) ) 

#define IHomonym_get_IsOb2(This,pVal)	\
    ( (This)->lpVtbl -> get_IsOb2(This,pVal) ) 

#define IHomonym_get_IsInOb(This,pVal)	\
    ( (This)->lpVtbl -> get_IsInOb(This,pVal) ) 

#define IHomonym_get_SimplePrepsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_SimplePrepsCount(This,pVal) ) 

#define IHomonym_GetOborDictIdOfSimplePrep(This,iNum,pVal)	\
    ( (This)->lpVtbl -> GetOborDictIdOfSimplePrep(This,iNum,pVal) ) 

#define IHomonym_GetOborotId(This,OborotId)	\
    ( (This)->lpVtbl -> GetOborotId(This,OborotId) ) 

#define IHomonym_GetOborotGF(This,pVal)	\
    ( (This)->lpVtbl -> GetOborotGF(This,pVal) ) 

#define IHomonym_get_CommonGrammemsStr(This,pVal)	\
    ( (This)->lpVtbl -> get_CommonGrammemsStr(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHomonym_INTERFACE_DEFINED__ */


#ifndef __IWord_INTERFACE_DEFINED__
#define __IWord_INTERFACE_DEFINED__

/* interface IWord */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWord;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1E2-F8C1-11D2-8C0E-00105A68ADF3")
    IWord : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WordStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Register( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomonymsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Homonym( 
            /* [in] */ long lHomNum,
            /* [retval][out] */ IHomonym **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsInThesaurusEntry( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThesaurusEntryID( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsLastInThesaurusEntry( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsFirstInThesaurusEntry( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThesType( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GraphDescrs( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bArtificialCreated( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bDeleted( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MainVerbsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MainVerbs( 
            /* [in] */ long lVerbNo,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWordVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWord * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWord * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWord * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWord * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWord * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWord * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWord * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordStr )( 
            IWord * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Register )( 
            IWord * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HomonymsCount )( 
            IWord * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Homonym )( 
            IWord * This,
            /* [in] */ long lHomNum,
            /* [retval][out] */ IHomonym **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsInThesaurusEntry )( 
            IWord * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThesaurusEntryID )( 
            IWord * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsLastInThesaurusEntry )( 
            IWord * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFirstInThesaurusEntry )( 
            IWord * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThesType )( 
            IWord * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseNo )( 
            IWord * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GraphDescrs )( 
            IWord * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bArtificialCreated )( 
            IWord * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bDeleted )( 
            IWord * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MainVerbsCount )( 
            IWord * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MainVerbs )( 
            IWord * This,
            /* [in] */ long lVerbNo,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IWordVtbl;

    interface IWord
    {
        CONST_VTBL struct IWordVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWord_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWord_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWord_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWord_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IWord_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IWord_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IWord_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IWord_get_WordStr(This,pVal)	\
    ( (This)->lpVtbl -> get_WordStr(This,pVal) ) 

#define IWord_get_Register(This,pVal)	\
    ( (This)->lpVtbl -> get_Register(This,pVal) ) 

#define IWord_get_HomonymsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_HomonymsCount(This,pVal) ) 

#define IWord_get_Homonym(This,lHomNum,pVal)	\
    ( (This)->lpVtbl -> get_Homonym(This,lHomNum,pVal) ) 

#define IWord_get_IsInThesaurusEntry(This,pVal)	\
    ( (This)->lpVtbl -> get_IsInThesaurusEntry(This,pVal) ) 

#define IWord_get_ThesaurusEntryID(This,pVal)	\
    ( (This)->lpVtbl -> get_ThesaurusEntryID(This,pVal) ) 

#define IWord_get_IsLastInThesaurusEntry(This,pVal)	\
    ( (This)->lpVtbl -> get_IsLastInThesaurusEntry(This,pVal) ) 

#define IWord_get_IsFirstInThesaurusEntry(This,pVal)	\
    ( (This)->lpVtbl -> get_IsFirstInThesaurusEntry(This,pVal) ) 

#define IWord_get_ThesType(This,pVal)	\
    ( (This)->lpVtbl -> get_ThesType(This,pVal) ) 

#define IWord_get_ClauseNo(This,pVal)	\
    ( (This)->lpVtbl -> get_ClauseNo(This,pVal) ) 

#define IWord_get_GraphDescrs(This,pVal)	\
    ( (This)->lpVtbl -> get_GraphDescrs(This,pVal) ) 

#define IWord_get_bArtificialCreated(This,pVal)	\
    ( (This)->lpVtbl -> get_bArtificialCreated(This,pVal) ) 

#define IWord_get_bDeleted(This,pVal)	\
    ( (This)->lpVtbl -> get_bDeleted(This,pVal) ) 

#define IWord_get_MainVerbsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_MainVerbsCount(This,pVal) ) 

#define IWord_get_MainVerbs(This,lVerbNo,pVal)	\
    ( (This)->lpVtbl -> get_MainVerbs(This,lVerbNo,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWord_INTERFACE_DEFINED__ */


#ifndef __IRelation_INTERFACE_DEFINED__
#define __IRelation_INTERFACE_DEFINED__

/* interface IRelation */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IRelation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1EA-F8C1-11D2-8C0E-00105A68ADF3")
    IRelation : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceItemType( 
            /* [retval][out] */ int *type) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TargetItemType( 
            /* [retval][out] */ int *type) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceClauseNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TargetClauseNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceItemNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TargetItemNo( 
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRelationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRelation * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRelation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRelation * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRelation * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRelation * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRelation * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRelation * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SourceItemType )( 
            IRelation * This,
            /* [retval][out] */ int *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TargetItemType )( 
            IRelation * This,
            /* [retval][out] */ int *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IRelation * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SourceClauseNo )( 
            IRelation * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TargetClauseNo )( 
            IRelation * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SourceItemNo )( 
            IRelation * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TargetItemNo )( 
            IRelation * This,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IRelationVtbl;

    interface IRelation
    {
        CONST_VTBL struct IRelationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRelation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRelation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRelation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRelation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRelation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRelation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRelation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IRelation_get_SourceItemType(This,type)	\
    ( (This)->lpVtbl -> get_SourceItemType(This,type) ) 

#define IRelation_get_TargetItemType(This,type)	\
    ( (This)->lpVtbl -> get_TargetItemType(This,type) ) 

#define IRelation_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IRelation_get_SourceClauseNo(This,pVal)	\
    ( (This)->lpVtbl -> get_SourceClauseNo(This,pVal) ) 

#define IRelation_get_TargetClauseNo(This,pVal)	\
    ( (This)->lpVtbl -> get_TargetClauseNo(This,pVal) ) 

#define IRelation_get_SourceItemNo(This,pVal)	\
    ( (This)->lpVtbl -> get_SourceItemNo(This,pVal) ) 

#define IRelation_get_TargetItemNo(This,pVal)	\
    ( (This)->lpVtbl -> get_TargetItemNo(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRelation_INTERFACE_DEFINED__ */


#ifndef __IGroup_INTERFACE_DEFINED__
#define __IGroup_INTERFACE_DEFINED__

/* interface IGroup */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1E5-F8C1-11D2-8C0E-00105A68ADF3")
    IGroup : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TypeStr( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirstWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MainGroupFirstWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MainGroupLastWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Grammems( 
            /* [retval][out] */ hyper *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MainWord( 
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGroup * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGroup * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGroup * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGroup * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGroup * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeStr )( 
            IGroup * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FirstWord )( 
            IGroup * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastWord )( 
            IGroup * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MainGroupFirstWord )( 
            IGroup * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MainGroupLastWord )( 
            IGroup * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Grammems )( 
            IGroup * This,
            /* [retval][out] */ hyper *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MainWord )( 
            IGroup * This,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IGroupVtbl;

    interface IGroup
    {
        CONST_VTBL struct IGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGroup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGroup_get_TypeStr(This,pVal)	\
    ( (This)->lpVtbl -> get_TypeStr(This,pVal) ) 

#define IGroup_get_FirstWord(This,pVal)	\
    ( (This)->lpVtbl -> get_FirstWord(This,pVal) ) 

#define IGroup_get_LastWord(This,pVal)	\
    ( (This)->lpVtbl -> get_LastWord(This,pVal) ) 

#define IGroup_get_MainGroupFirstWord(This,pVal)	\
    ( (This)->lpVtbl -> get_MainGroupFirstWord(This,pVal) ) 

#define IGroup_get_MainGroupLastWord(This,pVal)	\
    ( (This)->lpVtbl -> get_MainGroupLastWord(This,pVal) ) 

#define IGroup_get_Grammems(This,pVal)	\
    ( (This)->lpVtbl -> get_Grammems(This,pVal) ) 

#define IGroup_get_MainWord(This,pVal)	\
    ( (This)->lpVtbl -> get_MainWord(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGroup_INTERFACE_DEFINED__ */


#ifndef __IClauseVariant_INTERFACE_DEFINED__
#define __IClauseVariant_INTERFACE_DEFINED__

/* interface IClauseVariant */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IClauseVariant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1E8-F8C1-11D2-8C0E-00105A68ADF3")
    IClauseVariant : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Group( 
            /* [in] */ long lGroupNum,
            /* [retval][out] */ IGroup **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UnitsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Unit( 
            /* [in] */ long lUnitNum,
            /* [retval][out] */ ISyntaxUnit **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubjectsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Subjects( 
            /* [in] */ long SubjectNo,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Predicate( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseRootNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VariantWeight( 
            /* [retval][out] */ int *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClauseVariantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClauseVariant * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClauseVariant * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClauseVariant * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IClauseVariant * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IClauseVariant * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IClauseVariant * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IClauseVariant * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GroupsCount )( 
            IClauseVariant * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Group )( 
            IClauseVariant * This,
            /* [in] */ long lGroupNum,
            /* [retval][out] */ IGroup **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UnitsCount )( 
            IClauseVariant * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Unit )( 
            IClauseVariant * This,
            /* [in] */ long lUnitNum,
            /* [retval][out] */ ISyntaxUnit **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubjectsCount )( 
            IClauseVariant * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Subjects )( 
            IClauseVariant * This,
            /* [in] */ long SubjectNo,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Predicate )( 
            IClauseVariant * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseRootNo )( 
            IClauseVariant * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VariantWeight )( 
            IClauseVariant * This,
            /* [retval][out] */ int *pVal);
        
        END_INTERFACE
    } IClauseVariantVtbl;

    interface IClauseVariant
    {
        CONST_VTBL struct IClauseVariantVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClauseVariant_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClauseVariant_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClauseVariant_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClauseVariant_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IClauseVariant_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IClauseVariant_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IClauseVariant_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IClauseVariant_get_GroupsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_GroupsCount(This,pVal) ) 

#define IClauseVariant_get_Group(This,lGroupNum,pVal)	\
    ( (This)->lpVtbl -> get_Group(This,lGroupNum,pVal) ) 

#define IClauseVariant_get_UnitsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_UnitsCount(This,pVal) ) 

#define IClauseVariant_get_Unit(This,lUnitNum,pVal)	\
    ( (This)->lpVtbl -> get_Unit(This,lUnitNum,pVal) ) 

#define IClauseVariant_get_SubjectsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_SubjectsCount(This,pVal) ) 

#define IClauseVariant_get_Subjects(This,SubjectNo,pVal)	\
    ( (This)->lpVtbl -> get_Subjects(This,SubjectNo,pVal) ) 

#define IClauseVariant_get_Predicate(This,pVal)	\
    ( (This)->lpVtbl -> get_Predicate(This,pVal) ) 

#define IClauseVariant_get_ClauseRootNo(This,pVal)	\
    ( (This)->lpVtbl -> get_ClauseRootNo(This,pVal) ) 

#define IClauseVariant_get_VariantWeight(This,pVal)	\
    ( (This)->lpVtbl -> get_VariantWeight(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClauseVariant_INTERFACE_DEFINED__ */


#ifndef __IClause_INTERFACE_DEFINED__
#define __IClause_INTERFACE_DEFINED__

/* interface IClause */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IClause;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1E3-F8C1-11D2-8C0E-00105A68ADF3")
    IClause : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirstWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AntecedentWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RelativeWord( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VariantsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseVariant( 
            /* [in] */ long lVarNum,
            /* [retval][out] */ IClauseVariant **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseRootsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClauseRoots( 
            /* [in] */ long lNum,
            /* [retval][out] */ IClauseRoot **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConjsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConjStr( 
            /* [in] */ int iNum,
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClauseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClause * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClause * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClause * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IClause * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IClause * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IClause * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IClause * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FirstWord )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastWord )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AntecedentWord )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RelativeWord )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VariantsCount )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseVariant )( 
            IClause * This,
            /* [in] */ long lVarNum,
            /* [retval][out] */ IClauseVariant **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseRootsCount )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClauseRoots )( 
            IClause * This,
            /* [in] */ long lNum,
            /* [retval][out] */ IClauseRoot **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IClause * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConjsCount )( 
            IClause * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConjStr )( 
            IClause * This,
            /* [in] */ int iNum,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IClauseVtbl;

    interface IClause
    {
        CONST_VTBL struct IClauseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClause_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClause_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClause_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClause_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IClause_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IClause_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IClause_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IClause_get_FirstWord(This,pVal)	\
    ( (This)->lpVtbl -> get_FirstWord(This,pVal) ) 

#define IClause_get_LastWord(This,pVal)	\
    ( (This)->lpVtbl -> get_LastWord(This,pVal) ) 

#define IClause_get_AntecedentWord(This,pVal)	\
    ( (This)->lpVtbl -> get_AntecedentWord(This,pVal) ) 

#define IClause_get_RelativeWord(This,pVal)	\
    ( (This)->lpVtbl -> get_RelativeWord(This,pVal) ) 

#define IClause_get_VariantsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_VariantsCount(This,pVal) ) 

#define IClause_get_ClauseVariant(This,lVarNum,pVal)	\
    ( (This)->lpVtbl -> get_ClauseVariant(This,lVarNum,pVal) ) 

#define IClause_get_ClauseRootsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_ClauseRootsCount(This,pVal) ) 

#define IClause_get_ClauseRoots(This,lNum,pVal)	\
    ( (This)->lpVtbl -> get_ClauseRoots(This,lNum,pVal) ) 

#define IClause_get_Description(This,pVal)	\
    ( (This)->lpVtbl -> get_Description(This,pVal) ) 

#define IClause_get_ConjsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_ConjsCount(This,pVal) ) 

#define IClause_get_ConjStr(This,iNum,pVal)	\
    ( (This)->lpVtbl -> get_ConjStr(This,iNum,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClause_INTERFACE_DEFINED__ */


#ifndef __ISentence_INTERFACE_DEFINED__
#define __ISentence_INTERFACE_DEFINED__

/* interface ISentence */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISentence;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB81A1E0-F8C1-11D2-8C0E-00105A68ADF3")
    ISentence : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PrimitiveClausesCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PrimitiveClause( 
            /* [in] */ long lClauseNum,
            /* [retval][out] */ IClause **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WordsNum( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Word( 
            /* [in] */ long lWordNum,
            /* [retval][out] */ IWord **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClausesCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Clause( 
            /* [in] */ long lClauseNum,
            /* [retval][out] */ IClause **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateRelationsIterator( 
            /* [retval][out] */ IRelationsIterator **piRelsIt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHomonymByClauseVar( 
            /* [in] */ long iWord,
            /* [in] */ long iVar,
            /* [in] */ long iClause,
            /* [retval][out] */ IHomonym **piHom) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClauseNoByPrimitiveClauseNo( 
            /* [in] */ long lPrClauseNo,
            /* [retval][out] */ long *lClauseNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PrimitiveClauseNoByClauseNo( 
            /* [in] */ long lClauseNo,
            /* [retval][out] */ long *lPrClauseNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOborotStrByOborotId( 
            /* [in] */ long OborotId,
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISentenceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISentence * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISentence * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISentence * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISentence * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISentence * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISentence * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISentence * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PrimitiveClausesCount )( 
            ISentence * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PrimitiveClause )( 
            ISentence * This,
            /* [in] */ long lClauseNum,
            /* [retval][out] */ IClause **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordsNum )( 
            ISentence * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Word )( 
            ISentence * This,
            /* [in] */ long lWordNum,
            /* [retval][out] */ IWord **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClausesCount )( 
            ISentence * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Clause )( 
            ISentence * This,
            /* [in] */ long lClauseNum,
            /* [retval][out] */ IClause **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateRelationsIterator )( 
            ISentence * This,
            /* [retval][out] */ IRelationsIterator **piRelsIt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHomonymByClauseVar )( 
            ISentence * This,
            /* [in] */ long iWord,
            /* [in] */ long iVar,
            /* [in] */ long iClause,
            /* [retval][out] */ IHomonym **piHom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClauseNoByPrimitiveClauseNo )( 
            ISentence * This,
            /* [in] */ long lPrClauseNo,
            /* [retval][out] */ long *lClauseNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PrimitiveClauseNoByClauseNo )( 
            ISentence * This,
            /* [in] */ long lClauseNo,
            /* [retval][out] */ long *lPrClauseNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOborotStrByOborotId )( 
            ISentence * This,
            /* [in] */ long OborotId,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } ISentenceVtbl;

    interface ISentence
    {
        CONST_VTBL struct ISentenceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISentence_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISentence_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISentence_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISentence_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISentence_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISentence_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISentence_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISentence_get_PrimitiveClausesCount(This,pVal)	\
    ( (This)->lpVtbl -> get_PrimitiveClausesCount(This,pVal) ) 

#define ISentence_get_PrimitiveClause(This,lClauseNum,pVal)	\
    ( (This)->lpVtbl -> get_PrimitiveClause(This,lClauseNum,pVal) ) 

#define ISentence_get_WordsNum(This,pVal)	\
    ( (This)->lpVtbl -> get_WordsNum(This,pVal) ) 

#define ISentence_get_Word(This,lWordNum,pVal)	\
    ( (This)->lpVtbl -> get_Word(This,lWordNum,pVal) ) 

#define ISentence_get_ClausesCount(This,pVal)	\
    ( (This)->lpVtbl -> get_ClausesCount(This,pVal) ) 

#define ISentence_get_Clause(This,lClauseNum,pVal)	\
    ( (This)->lpVtbl -> get_Clause(This,lClauseNum,pVal) ) 

#define ISentence_CreateRelationsIterator(This,piRelsIt)	\
    ( (This)->lpVtbl -> CreateRelationsIterator(This,piRelsIt) ) 

#define ISentence_GetHomonymByClauseVar(This,iWord,iVar,iClause,piHom)	\
    ( (This)->lpVtbl -> GetHomonymByClauseVar(This,iWord,iVar,iClause,piHom) ) 

#define ISentence_ClauseNoByPrimitiveClauseNo(This,lPrClauseNo,lClauseNo)	\
    ( (This)->lpVtbl -> ClauseNoByPrimitiveClauseNo(This,lPrClauseNo,lClauseNo) ) 

#define ISentence_PrimitiveClauseNoByClauseNo(This,lClauseNo,lPrClauseNo)	\
    ( (This)->lpVtbl -> PrimitiveClauseNoByClauseNo(This,lClauseNo,lPrClauseNo) ) 

#define ISentence_GetOborotStrByOborotId(This,OborotId,pVal)	\
    ( (This)->lpVtbl -> GetOborotStrByOborotId(This,OborotId,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISentence_INTERFACE_DEFINED__ */


#ifndef __ISentencesCollection_INTERFACE_DEFINED__
#define __ISentencesCollection_INTERFACE_DEFINED__

/* interface ISentencesCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISentencesCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7907CB45-2A26-11d3-8C52-00105A68ADF3")
    ISentencesCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitializeProcesser( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessData( 
            /* [in] */ IUnknown *piPlmLine) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SentencesCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Sentence( 
            /* [in] */ long i,
            /* [retval][out] */ ISentence **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearSentences( void) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableThesauri( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableThesauri( 
            /* [retval][out] */ BOOL *Val) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SilentMode( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KillHomonymsMode( 
            /* [retval][out] */ UINT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KillHomonymsMode( 
            /* [in] */ UINT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLemmatizer( 
            /* [in] */ IUnknown *Interf) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOborDic( 
            /* [in] */ IUnknown *Interf) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeleteEqualPrimitiveClauseVariants( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeleteEqualPrimitiveClauseVariants( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SyntaxLanguage( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SyntaxLanguage( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableProgressBar( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableProgressBar( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISentencesCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISentencesCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISentencesCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISentencesCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISentencesCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISentencesCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISentencesCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISentencesCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitializeProcesser )( 
            ISentencesCollection * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessData )( 
            ISentencesCollection * This,
            /* [in] */ IUnknown *piPlmLine);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SentencesCount )( 
            ISentencesCollection * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Sentence )( 
            ISentencesCollection * This,
            /* [in] */ long i,
            /* [retval][out] */ ISentence **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearSentences )( 
            ISentencesCollection * This);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EnableThesauri )( 
            ISentencesCollection * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EnableThesauri )( 
            ISentencesCollection * This,
            /* [retval][out] */ BOOL *Val);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SilentMode )( 
            ISentencesCollection * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KillHomonymsMode )( 
            ISentencesCollection * This,
            /* [retval][out] */ UINT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KillHomonymsMode )( 
            ISentencesCollection * This,
            /* [in] */ UINT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLemmatizer )( 
            ISentencesCollection * This,
            /* [in] */ IUnknown *Interf);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOborDic )( 
            ISentencesCollection * This,
            /* [in] */ IUnknown *Interf);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeleteEqualPrimitiveClauseVariants )( 
            ISentencesCollection * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DeleteEqualPrimitiveClauseVariants )( 
            ISentencesCollection * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SyntaxLanguage )( 
            ISentencesCollection * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SyntaxLanguage )( 
            ISentencesCollection * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EnableProgressBar )( 
            ISentencesCollection * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EnableProgressBar )( 
            ISentencesCollection * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } ISentencesCollectionVtbl;

    interface ISentencesCollection
    {
        CONST_VTBL struct ISentencesCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISentencesCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISentencesCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISentencesCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISentencesCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISentencesCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISentencesCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISentencesCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISentencesCollection_InitializeProcesser(This)	\
    ( (This)->lpVtbl -> InitializeProcesser(This) ) 

#define ISentencesCollection_ProcessData(This,piPlmLine)	\
    ( (This)->lpVtbl -> ProcessData(This,piPlmLine) ) 

#define ISentencesCollection_get_SentencesCount(This,pVal)	\
    ( (This)->lpVtbl -> get_SentencesCount(This,pVal) ) 

#define ISentencesCollection_get_Sentence(This,i,pVal)	\
    ( (This)->lpVtbl -> get_Sentence(This,i,pVal) ) 

#define ISentencesCollection_ClearSentences(This)	\
    ( (This)->lpVtbl -> ClearSentences(This) ) 

#define ISentencesCollection_put_EnableThesauri(This,newVal)	\
    ( (This)->lpVtbl -> put_EnableThesauri(This,newVal) ) 

#define ISentencesCollection_get_EnableThesauri(This,Val)	\
    ( (This)->lpVtbl -> get_EnableThesauri(This,Val) ) 

#define ISentencesCollection_put_SilentMode(This,newVal)	\
    ( (This)->lpVtbl -> put_SilentMode(This,newVal) ) 

#define ISentencesCollection_get_KillHomonymsMode(This,pVal)	\
    ( (This)->lpVtbl -> get_KillHomonymsMode(This,pVal) ) 

#define ISentencesCollection_put_KillHomonymsMode(This,newVal)	\
    ( (This)->lpVtbl -> put_KillHomonymsMode(This,newVal) ) 

#define ISentencesCollection_SetLemmatizer(This,Interf)	\
    ( (This)->lpVtbl -> SetLemmatizer(This,Interf) ) 

#define ISentencesCollection_SetOborDic(This,Interf)	\
    ( (This)->lpVtbl -> SetOborDic(This,Interf) ) 

#define ISentencesCollection_get_DeleteEqualPrimitiveClauseVariants(This,pVal)	\
    ( (This)->lpVtbl -> get_DeleteEqualPrimitiveClauseVariants(This,pVal) ) 

#define ISentencesCollection_put_DeleteEqualPrimitiveClauseVariants(This,newVal)	\
    ( (This)->lpVtbl -> put_DeleteEqualPrimitiveClauseVariants(This,newVal) ) 

#define ISentencesCollection_get_SyntaxLanguage(This,pVal)	\
    ( (This)->lpVtbl -> get_SyntaxLanguage(This,pVal) ) 

#define ISentencesCollection_put_SyntaxLanguage(This,newVal)	\
    ( (This)->lpVtbl -> put_SyntaxLanguage(This,newVal) ) 

#define ISentencesCollection_get_EnableProgressBar(This,pVal)	\
    ( (This)->lpVtbl -> get_EnableProgressBar(This,pVal) ) 

#define ISentencesCollection_put_EnableProgressBar(This,newVal)	\
    ( (This)->lpVtbl -> put_EnableProgressBar(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISentencesCollection_INTERFACE_DEFINED__ */


#ifndef __IRelationsIterator_INTERFACE_DEFINED__
#define __IRelationsIterator_INTERFACE_DEFINED__

/* interface IRelationsIterator */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IRelationsIterator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("23D97177-087F-11D4-8DE4-00105A68ADF3")
    IRelationsIterator : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddClauseNoAndVariantNo( 
            /* [in] */ long iClauseNo,
            /* [in] */ long iVarNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BuildRelations( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RelationsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Relation( 
            /* [in] */ long iRel,
            /* [retval][out] */ IRelation **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirmGroupsCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirmGroup( 
            /* [in] */ long iGroup,
            /* [retval][out] */ IGroup **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRelationsIteratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRelationsIterator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRelationsIterator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRelationsIterator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRelationsIterator * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRelationsIterator * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRelationsIterator * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRelationsIterator * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddClauseNoAndVariantNo )( 
            IRelationsIterator * This,
            /* [in] */ long iClauseNo,
            /* [in] */ long iVarNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildRelations )( 
            IRelationsIterator * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RelationsCount )( 
            IRelationsIterator * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Relation )( 
            IRelationsIterator * This,
            /* [in] */ long iRel,
            /* [retval][out] */ IRelation **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FirmGroupsCount )( 
            IRelationsIterator * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FirmGroup )( 
            IRelationsIterator * This,
            /* [in] */ long iGroup,
            /* [retval][out] */ IGroup **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IRelationsIterator * This);
        
        END_INTERFACE
    } IRelationsIteratorVtbl;

    interface IRelationsIterator
    {
        CONST_VTBL struct IRelationsIteratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRelationsIterator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRelationsIterator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRelationsIterator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRelationsIterator_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRelationsIterator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRelationsIterator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRelationsIterator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IRelationsIterator_AddClauseNoAndVariantNo(This,iClauseNo,iVarNo)	\
    ( (This)->lpVtbl -> AddClauseNoAndVariantNo(This,iClauseNo,iVarNo) ) 

#define IRelationsIterator_BuildRelations(This)	\
    ( (This)->lpVtbl -> BuildRelations(This) ) 

#define IRelationsIterator_get_RelationsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_RelationsCount(This,pVal) ) 

#define IRelationsIterator_get_Relation(This,iRel,pVal)	\
    ( (This)->lpVtbl -> get_Relation(This,iRel,pVal) ) 

#define IRelationsIterator_get_FirmGroupsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_FirmGroupsCount(This,pVal) ) 

#define IRelationsIterator_get_FirmGroup(This,iGroup,pVal)	\
    ( (This)->lpVtbl -> get_FirmGroup(This,iGroup,pVal) ) 

#define IRelationsIterator_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRelationsIterator_INTERFACE_DEFINED__ */



#ifndef __SYNANLib_LIBRARY_DEFINED__
#define __SYNANLib_LIBRARY_DEFINED__

/* library SYNANLib */
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
	rAllNumbers	= ( ( 1 << rSingular )  | ( 1 << rPlural )  ) ,
	rNominativ	= 2,
	rGenitiv	= 3,
	rDativ	= 4,
	rAccusativ	= 5,
	rInstrumentalis	= 6,
	rLocativ	= 7,
	rVocativ	= 8,
	rAllCases	= ( ( ( ( ( ( ( 1 << rNominativ )  | ( 1 << rGenitiv )  )  | ( 1 << rDativ )  )  | ( 1 << rAccusativ )  )  | ( 1 << rInstrumentalis )  )  | ( 1 << rVocativ )  )  | ( 1 << rLocativ )  ) ,
	rMasculinum	= 9,
	rFeminum	= 10,
	rNeutrum	= 11,
	rMascFem	= 12,
	rAllGenders	= ( ( ( 1 << rMasculinum )  | ( 1 << rFeminum )  )  | ( 1 << rNeutrum )  ) ,
	rPresentTense	= 13,
	rFutureTense	= 14,
	rPastTense	= 15,
	rAllTimes	= ( ( ( 1 << rPresentTense )  | ( 1 << rFutureTense )  )  | ( 1 << rPastTense )  ) ,
	rFirstPerson	= 16,
	rSecondPerson	= 17,
	rThirdPerson	= 18,
	rAllPersons	= ( ( ( 1 << rFirstPerson )  | ( 1 << rSecondPerson )  )  | ( 1 << rThirdPerson )  ) ,
	rImperative	= 19,
	rAnimative	= 20,
	rNonAnimative	= 21,
	rAllAnimative	= ( ( 1 << rAnimative )  | ( 1 << rNonAnimative )  ) ,
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
enum __MIDL___MIDL_itf_SynAn_0000_0011_0001
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
enum __MIDL___MIDL_itf_SynAn_0000_0011_0002
    {	VERBSATZ_T	= 0,
	PARTIZIPIALSATZ_T	= 1,
	INFINITIVSATZ_T	= 2
    } 	gClauseTypeEnum;


EXTERN_C const IID LIBID_SYNANLib;

EXTERN_C const CLSID CLSID_SentencesCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("CF509F75-2981-11D3-8C51-00105A68ADF3")
SentencesCollection;
#endif

EXTERN_C const CLSID CLSID_Sentence;

#ifdef __cplusplus

class DECLSPEC_UUID("CE8F3450-E101-11D2-8BE1-00105A68ADF3")
Sentence;
#endif

EXTERN_C const CLSID CLSID_Word;

#ifdef __cplusplus

class DECLSPEC_UUID("CE8F3452-E101-11D2-8BE1-00105A68ADF3")
Word;
#endif

EXTERN_C const CLSID CLSID_Clause;

#ifdef __cplusplus

class DECLSPEC_UUID("CB81A1E4-F8C1-11D2-8C0E-00105A68ADF3")
Clause;
#endif

EXTERN_C const CLSID CLSID_Group;

#ifdef __cplusplus

class DECLSPEC_UUID("CB81A1E6-F8C1-11D2-8C0E-00105A68ADF3")
Group;
#endif

EXTERN_C const CLSID CLSID_ClauseVariant;

#ifdef __cplusplus

class DECLSPEC_UUID("CB81A1E9-F8C1-11D2-8C0E-00105A68ADF3")
ClauseVariant;
#endif

EXTERN_C const CLSID CLSID_Relation;

#ifdef __cplusplus

class DECLSPEC_UUID("CB81A1EB-F8C1-11D2-8C0E-00105A68ADF3")
Relation;
#endif

EXTERN_C const CLSID CLSID_Homonym;

#ifdef __cplusplus

class DECLSPEC_UUID("CB81A1ED-F8C1-11D2-8C0E-00105A68ADF3")
Homonym;
#endif

EXTERN_C const CLSID CLSID_ClauseRoot;

#ifdef __cplusplus

class DECLSPEC_UUID("E022CF54-037F-11D3-8C22-00105A68ADF3")
ClauseRoot;
#endif

EXTERN_C const CLSID CLSID_SyntaxUnit;

#ifdef __cplusplus

class DECLSPEC_UUID("7794ED73-F74F-11D3-8DC5-00105A68ADF3")
SyntaxUnit;
#endif

EXTERN_C const CLSID CLSID_RelationsIterator;

#ifdef __cplusplus

class DECLSPEC_UUID("23D97178-087F-11D4-8DE4-00105A68ADF3")
RelationsIterator;
#endif
#endif /* __SYNANLib_LIBRARY_DEFINED__ */

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


