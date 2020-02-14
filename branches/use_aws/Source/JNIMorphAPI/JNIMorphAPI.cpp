#include "./ru_aot_morph_JavaMorphAPI.h"

#ifndef JNIEXPORT
#define JNIEXPORT
#endif

#ifndef JNIIMPORT
#define JNIIMPORT
#endif

#ifndef JNICALL
#define JNICALL
#endif

#ifndef __GNUC__
#pragma warning(disable:4786)
#pragma warning(disable:4530)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>
#include <malloc.h>

#include "../common/utilit.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/Paradigm.h"

//jni infrastructure stuff
static jclass setClazz=0;
static jmethodID setConstructor=0;
static jclass JNIAPIExceptionClass=0;
static jmethodID method_convertFromCharsetCp1251ToJavaString=0;
static jmethodID method_grammemSetAddGrammem=0;
static jmethodID method_paradigmsetAddNewParadigm=0;
static jmethodID method_wordresult_new=0;

void throwEx(JNIEnv* env, char* message){
	if(env->ExceptionOccurred())return;
	jint retcode;//Returns 0 on success; a negative value on failure.
	if(message==0)retcode=env->ThrowNew(JNIAPIExceptionClass,"null message");
	else{
		retcode=env->ThrowNew(JNIAPIExceptionClass,message);
		free(message);
	}
}
//jni infrastructure stuff end


enum jni_language {Russian,English,German};

enum rus_pos{
	noun,  // 0
	adjective, // 1
	verb, // 2
	mestoim_noun, // 3
	mestoim_adjective, // 4
	mestoim_predikativ, // 5
	chislitelnoeKolichestv, // 6
	chislitelnoePoryadkovoe, // 7
	narechie, // 8
	predikativ, //9
	predlog, // 10
	posl, // 11
	soyuz, // 12
	mejdometie, // 13
	vvodnoe_slovo,// 14
	fraz, // 15
	chastica, // 16
	kratkoePrilagat,  // 17
	prichastie, //18
	deeprichastie, //19
	kratkoePrichastie, // 20
	verbInfinitive  //21
};

enum rus_grammems{
	// 0..1
	plural, singular,
	// 2..8
	padejImen, padejRodit, padejDatel, padejVinit, padejTvor, padejPredl, padejZvateln,
	// род 9-12
	rodMuj, rodJen, rodSred, rodMujJen,
	// 13..15
	present, future, past,
	// 16..18
	lico1, lico2, lico3,
	// 19
	povelitelnFormaGlagola,
	// 20..21
	odush, neodush,
	// 22
	sravnitelnFormaPrilagat,
	// 23..24
	vidSov, vidNesov,
	// 25..26
	neperehodnyiGlagol, perehodnyiGlagol,
	// 27..28
	deistvitZalog, stradatZalog,
	// 29-31
	neizmenyaemoe, abbrev, otchestvo,
	// 32-33
	lokativnost, organizaciya,
	// 34-35
	kachestv, neImeetMnojestvChisla,
	// 36-37 (наречия)
	voprositNarech, ukazat,
	// 38..39
	firstName, lastName,
	// 40
	bezlichnGlagol,
	// 41,42
	jargon, opechatka,
	// 43,44,45
	razgovorn, posessive, archaic,
	// для второго родительного и второго предложного
	padej2,
	poet, professionalizm,
	prev, poloj
};

struct jni_dictionary{
	jni_language 		lang;
	MorphLanguageEnum	Language;
	CAgramtab*			pAgramtab;
	CLemmatizer*		pLemmatizer;
};

char* str_compose(const char *fmt, ...){
   /* Guess we need no more than 100 bytes. */
   int n, size = 100;
   char *p, *np;
   va_list ap;

  if ((p = (char*)malloc (size)) == NULL){
	  //out_of_memory();
	  return 0;
  }

  while (1) {
	  /* Try to print in the allocated space. */
	  va_start(ap, fmt);
	  n = vsnprintf (p, size, fmt, ap);
	  va_end(ap);
	  /* If that worked, return the string. */
	  if (n > -1 && n < size){
		 //return p;
		  break;
	  }
	  /* Else try again with more space. */
	  if (n > -1)    /* glibc 2.1 */
		 size = n+1; /* precisely what is needed */
	  else           /* glibc 2.0 */
		 size *= 2;  /* twice the old size */
	  if ((np = (char*)realloc (p, size)) == NULL) {
		 //out_of_memory();
		 //free(p);
		 return p;//partial string
	  } else {
		 p = np;
	  }
   }

   return p;
}


/* returns true if ok */
bool GetGrammems (jni_dictionary& dic, const char* tab_str, QWORD& grammems){
	return dic.pAgramtab->GetGrammems(tab_str, grammems);
}

/* cp1251 charset on input string - for Russian */
jobject GetMorphInfo(JNIEnv *env, jclass clazz, jni_dictionary& dic, string& Form){
	jobject paradigmset=NULL;
	jobject grammemset=NULL;
	jstring jbaseForm=NULL;
	jobject wordresult=NULL;
	jbyteArray baseFormBytes=NULL;
//	try{
	bool bCapital   = is_upper_alpha((BYTE)Form[0], dic.Language);
	vector<CFormInfo> Paradigms;
	dic.pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);
	paradigmset=env->NewObject(setClazz, setConstructor);
	if(env->ExceptionOccurred())return NULL;
	if(paradigmset==0){
		throwEx(env, strdup("Out of memory"));
		return NULL;
	}
	for(int i=0; i < Paradigms.size(); i++){
		const CFormInfo& F = Paradigms[i];
		bool found=F.m_bFound;
		string baseForm=F.GetWordForm(0);
		const char* chars=baseForm.c_str();
		jsize length=(jsize)strlen(chars);
		baseFormBytes=env->NewByteArray(length);
		if(env->ExceptionOccurred()){
			//paradigmset is a local ref, no need to release.
			return NULL;
		}
		if(baseFormBytes==0){
			throwEx(env, strdup("Out of memory"));
			//paradigmset is a local ref, no need to release.
			return NULL;
		}
		env->SetByteArrayRegion(baseFormBytes, (jsize)0, length, (jbyte*)chars);
		if(env->ExceptionOccurred()){
			//paradigmset is a local ref, no need to release.
			//baseFormBytes is a local ref, no need to release.
			return NULL;
		}
		//for Russian bytes
		jbaseForm=(jstring)env->CallStaticObjectMethod(clazz, method_convertFromCharsetCp1251ToJavaString, baseFormBytes);
		if(env->ExceptionOccurred()){
			//paradigmset is a local ref, no need to release.
			//baseFormBytes is a local ref, no need to release.
			return NULL;
		}
		if(jbaseForm==0){
			throwEx(env, strdup("Out of memory"));
			//paradigmset is a local ref, no need to release.
			//baseFormBytes is a local ref, no need to release.
			return NULL;
		}
		//baseFormBytes is a local ref, no need to release.
		string GramCodes=F.GetSrcAncode();
		BYTE  PartOfSpeech = dic.pAgramtab->GetPartOfSpeech(GramCodes.c_str());
		//assert(dic.lang==Russian);
		rus_pos pos=(rus_pos)PartOfSpeech;

		string CommonAncode=F.GetCommonAncode();
		QWORD grammems=0;
		QWORD grammems1;
		bool ok=GetGrammems(dic, CommonAncode.c_str(), grammems1);
		if(ok)grammems=grammems1;
		for (long i=0; i < GramCodes.length(); i+=2){
			QWORD grammems2;
			bool ok2=GetGrammems(dic, GramCodes.c_str()+i, grammems2);
			if(ok2)grammems|=grammems2;
		}
		//assert(dic.lang==Russian);
		size_t GrammemsCountInLanguage = dic.pAgramtab->GetGrammemsCount();
		grammemset=env->NewObject(setClazz, setConstructor);
		if(env->ExceptionOccurred()){
			//paradigmset is a local ref, no need to release.
			//jbaseForm is a local ref, no need to release.
			return NULL;
		}
		if(grammemset==0){
			throwEx(env, strdup("Out of memory"));
			//paradigmset is a local ref, no need to release.
			//jbaseForm is a local ref, no need to release.
			return NULL;
		}
		for (int i = GrammemsCountInLanguage-1; i>=0; i--){
			if ((((QWORD)1)<<i) & grammems){
				env->CallStaticVoidMethod(clazz, method_grammemSetAddGrammem, grammemset, (jint)(rus_grammems)i);
				if(env->ExceptionOccurred()){
					//paradigmset is a local ref, no need to release.
					//jbaseForm is a local ref, no need to release.
					//grammemset is a local ref, no need to release.
					return NULL;
				}
			}
		}
		env->CallStaticVoidMethod(clazz, method_paradigmsetAddNewParadigm,
				paradigmset, grammemset, jbaseForm, (jboolean)found, (jint)pos);
		//jbaseForm is a local ref, no need to release.
		//grammemset is a local ref, no need to release.
		if(env->ExceptionOccurred()){
			//paradigmset is a local ref, no need to release.
			return NULL;
		}
	}
	wordresult=env->CallStaticObjectMethod(clazz, method_wordresult_new, paradigmset);
	//paradigmset is a local ref, no need to release.
	if(env->ExceptionOccurred()){
		return NULL;
	}
	if(wordresult==0){
		throwEx(env, strdup("Out of memory"));
		return NULL;
	}
	return wordresult;
//	}catch(CExpc e){
//		char* err=str_compose("C++ exception: CExpc: %s",e.m_strCause.c_str());
//		throwEx(env, err);
		//paradigmset is a local ref, no need to release.
		//jbaseForm is a local ref, no need to release.
		//grammemset is a local ref, no need to release.
		//wordresult is a local ref, no need to release.
//	}catch(int e){
//		string errstr("C++ exception: int: ");
//		errstr+=e;
//		errstr+=".";
//		throwEx(env, strdup(errstr.c_str()));
		//paradigmset is a local ref, no need to release.
		//jbaseForm is a local ref, no need to release.
		//grammemset is a local ref, no need to release.
		//wordresult is a local ref, no need to release.
//	}catch(...){
		//throwEx(env, strdup("Unknown C++ exception."));
		//paradigmset is a local ref, no need to release.
		//jbaseForm is a local ref, no need to release.
		//grammemset is a local ref, no need to release.
		//wordresult is a local ref, no need to release.
//	}
//	return NULL;
}

template <class  T, class Y>
bool InitMorphologySystem(JNIEnv *env, jni_dictionary &dic){
	switch (dic.Language){
		case morphRussian:
			dic.lang=Russian;
			break;
		case morphEnglish :
		case morphGerman:
		default:
			throwEx(env, strdup("assertion error: A1"));
			return false;
	}

	string langua_str = GetStringByLanguage(dic.Language);
	dic.pLemmatizer = new T;
	string strError;
	if (!dic.pLemmatizer->LoadDictionariesRegistry(strError)){
   		char* err=str_compose("Cannot load %s morphological dictionary. Error details: %s", langua_str.c_str(), strError.c_str());
		throwEx(env, err);
		return false;
	}
	dic.pAgramtab = new Y;
	if (!dic.pAgramtab->LoadFromRegistry()){
   		char* err=str_compose("Cannot load %s gramtab.", langua_str.c_str());
		throwEx(env, err);
		return false;
	}
	return true;
}

static bool inited=0;
static jni_dictionary dic;

//cp1251 charset for <code>word</code> - for Russian.
/*
 * Class:     ru_aot_morph_JavaMorphAPI
 * Method:    lookupWordImpl
 * Signature: (I[B)Lru/aot/morph/JavaMorphAPI/WordResult;
 */
JNIEXPORT jobject JNICALL Java_ru_aot_morph_JavaMorphAPI_lookupWordImpl
  (JNIEnv *env, jclass clazz, jint languageId, jbyteArray word){
	jbyte* bytes=NULL;
	char* chars=NULL;
	try{
		if(!inited||dic.pAgramtab==0||dic.pLemmatizer==0){
			throwEx(env, strdup("Dictionaries are not loaded. Call JavaMorphAPI.initDictionaries() first!"));
			return NULL;
		}
		if(languageId!=0){
			throwEx(env, strdup("The only language implemented is Russian."));
			return NULL;
		}
		if(word==NULL){
			throwEx(env, strdup("word is null"));
			return NULL;
		}

		jsize len=env->GetArrayLength(word);
		bytes=env->GetByteArrayElements(word,NULL);
		if(bytes==0){
			throwEx(env, strdup("Out of memory"));
			return NULL;
		}
		chars=(char*)malloc(len+1);
		if(chars==0){
			throwEx(env, strdup("Out of memory"));
			return NULL;
		}
		for(jsize i=0;i<len;i++){chars[i]=(char)bytes[i];}
		chars[len]=(char)0;
		string s = chars;
		free(chars);chars=0;
		env->ReleaseByteArrayElements(word,bytes,JNI_ABORT);
		bytes=NULL;
		Trim(s);
		if (s.empty()){
			throwEx(env, strdup("Empty or whitespace-only string instead of a word."));
			return NULL;
		}
		return GetMorphInfo(env, clazz, dic, s);
	}catch(CExpc& e){
		char* err=str_compose("C++ exception: CExpc: %s",e.m_strCause.c_str());
		throwEx(env, err);
		if(chars!=0){free(chars);chars=0;}
		if(bytes!=NULL){env->ReleaseByteArrayElements(word,bytes,JNI_ABORT);bytes=NULL;}
	}catch(int e){
		string errstr("C++ exception: int: ");
		errstr+=e;
		errstr+=".";
		throwEx(env, strdup(errstr.c_str()));
		if(chars!=0){free(chars);chars=0;}
		if(bytes!=NULL){env->ReleaseByteArrayElements(word,bytes,JNI_ABORT);bytes=NULL;}
	}catch(...){
		throwEx(env, strdup("Unknown C++ exception."));
		if(chars!=0){free(chars);chars=0;}
		if(bytes!=NULL){env->ReleaseByteArrayElements(word,bytes,JNI_ABORT);bytes=NULL;}
	}
	return NULL;
}

/*
 * Class:     ru_aot_morph_JavaMorphAPI
 * Method:    initImpl
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ru_aot_morph_JavaMorphAPI_initImpl
  (JNIEnv *env, jclass clazz, jint languagesBitSet){
	dic.pAgramtab=0;
	dic.pLemmatizer=0;
	setClazz=NULL;
	setConstructor=NULL;
	JNIAPIExceptionClass=NULL;
	method_convertFromCharsetCp1251ToJavaString=NULL;
	method_grammemSetAddGrammem=NULL;
	method_paradigmsetAddNewParadigm=NULL;
	method_wordresult_new=NULL;
	JNIAPIExceptionClass=env->FindClass("ru/aot/morph/JavaMorphAPI$JavaMorphAPIException");if(JNIAPIExceptionClass==NULL||env->ExceptionOccurred()){if(!env->ExceptionOccurred())env->FatalError("JNIMorphAPI JNI: Cannot resolve exception class");return;}
	JNIAPIExceptionClass=(jclass)env->NewGlobalRef(JNIAPIExceptionClass);if(JNIAPIExceptionClass==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	setClazz=env->FindClass("java/util/HashSet");if(setClazz==NULL||env->ExceptionOccurred()){throwEx(env,strdup("Out of memory"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	setClazz=(jclass)env->NewGlobalRef(setClazz);if(setClazz==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	setConstructor=env->GetMethodID(setClazz, "<init>", "()V");if(setConstructor==NULL||env->ExceptionOccurred()){throwEx(env,strdup("Out of memory"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	//setConstructor=(jmethodID)env->NewGlobalRef(setConstructor);if(setConstructor==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	method_convertFromCharsetCp1251ToJavaString=env->GetStaticMethodID(clazz, "convertFromCP1251", "([B)Ljava/lang/String;");if(method_convertFromCharsetCp1251ToJavaString==NULL||env->ExceptionOccurred()){throwEx(env,strdup("Out of memory"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	//method_convertFromCharsetCp1251ToJavaString=(jmethodID)env->NewGlobalRef(method_convertFromCharsetCp1251ToJavaString);if(method_convertFromCharsetCp1251ToJavaString==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	method_grammemSetAddGrammem=env->GetStaticMethodID(clazz, "addGrammemToSet", "(Ljava/util/HashSet;I)V");if(method_grammemSetAddGrammem==NULL||env->ExceptionOccurred()){throwEx(env,strdup("Out of memory"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	//method_grammemSetAddGrammem=(jmethodID)env->NewGlobalRef(method_grammemSetAddGrammem);if(method_grammemSetAddGrammem==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	method_paradigmsetAddNewParadigm=env->GetStaticMethodID(clazz, "addParadigmToSet", "(Ljava/util/HashSet;Ljava/util/HashSet;Ljava/lang/String;ZI)V");if(method_paradigmsetAddNewParadigm==NULL||env->ExceptionOccurred()){throwEx(env,strdup("Out of memory"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	//method_paradigmsetAddNewParadigm=(jmethodID)env->NewGlobalRef(method_paradigmsetAddNewParadigm);if(method_paradigmsetAddNewParadigm==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	method_wordresult_new=env->GetStaticMethodID(clazz, "createWordResult", "(Ljava/util/HashSet;)Lru/aot/morph/JavaMorphAPI$WordResult;");if(method_wordresult_new==NULL||env->ExceptionOccurred()){throwEx(env,strdup("method_wordresult_new is null"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}
	//method_wordresult_new=(jmethodID)env->NewGlobalRef(method_wordresult_new);if(method_wordresult_new==NULL||env->ExceptionOccurred()){throwEx(env,strdup("global ref error"));Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);return;}

	inited=false;
	if(languagesBitSet==0){
		throwEx(env, strdup("The set of languages is empty."));
		return;
	}
	if(languagesBitSet!=1){
		throwEx(env, strdup("Russian is the only language supported by JavaMorphAPI."));
		return;
	}
	dic.Language=morphRussian;
	try{
		//dic.Language=morphEnglish;
		//dic.Language=morphGerman;

		//LOADING DICTS
		bool bResult = false;
		switch (dic.Language){
			case morphRussian:
				bResult = InitMorphologySystem<CLemmatizerRussian, CRusGramTab>(env,dic);
				break;
/*			case morphEnglish :
				bResult = InitMorphologySystem<CLemmatizerEnglish, CEngGramTab>(env,dic);
				break;
			case morphGerman:
				bResult = InitMorphologySystem<CLemmatizerGerman, CGerGramTab>(env,dic);
				break; */
			default:
				throwEx(env,strdup("assertion error: A2."));
				return;
		};
		if (!bResult){
			Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);
			return;//exception was thrown by InitMorphologySystem
		}

		inited=true;
		return;//ok
	}catch(CExpc& e){
		const char* ca=e.m_strCause.c_str();
		char* err=str_compose("C++ exception: CExpc: %s",ca);
		throwEx(env, err);
		Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);
		return;
	}catch(int e){
		string errstr("C++ exception: int: ");
		errstr+=e;
		errstr+=".";
		throwEx(env, strdup(errstr.c_str()));
		Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);
		return;
	}catch(...){
		throwEx(env, strdup("Unknown C++ exception."));
		Java_ru_aot_morph_JavaMorphAPI_closeImpl(env,clazz);
		return;
	}
}

/*
 * Class:     ru_aot_morph_JavaMorphAPI
 * Method:    closeImpl
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ru_aot_morph_JavaMorphAPI_closeImpl
  (JNIEnv *env, jclass clazz){
	try{
		//dispose of dics
		if(dic.pLemmatizer!=0){delete dic.pLemmatizer;dic.pLemmatizer=0;}
		if(dic.pAgramtab!=0){delete dic.pAgramtab;dic.pAgramtab=0;}

		if(setClazz!=NULL){env->DeleteGlobalRef(setClazz);setClazz=NULL;}
		//if(setConstructor!=NULL){env->DeleteGlobalRef(setConstructor);setConstructor=NULL;}
		if(JNIAPIExceptionClass!=NULL){env->DeleteGlobalRef(JNIAPIExceptionClass);JNIAPIExceptionClass=NULL;}
		//if(method_convertFromCharsetCp1251ToJavaString!=NULL){env->DeleteGlobalRef(method_convertFromCharsetCp1251ToJavaString);method_convertFromCharsetCp1251ToJavaString=NULL;}
		//if(method_grammemSetAddGrammem!=NULL){env->DeleteGlobalRef(method_grammemSetAddGrammem);method_grammemSetAddGrammem=NULL;}
		//if(method_paradigmsetAddNewParadigm!=NULL){env->DeleteGlobalRef(method_paradigmsetAddNewParadigm);method_paradigmsetAddNewParadigm=NULL;}
		//if(method_wordresult_new!=NULL){env->DeleteGlobalRef(method_wordresult_new);method_wordresult_new=NULL;}

		inited=false;
		return;//ok
	}catch(CExpc& e){
		char* err=str_compose("C++ exception: CExpc: %s",e.m_strCause.c_str());
		throwEx(env, err);
		return;
	}catch(int e){
		string errstr("C++ exception: int: ");
		errstr+=e;
		errstr+=".";
		throwEx(env, strdup(errstr.c_str()));
		return;
	}catch(...){
		throwEx(env, strdup("Unknown C++ exception."));
		return;
	}
}
