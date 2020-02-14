//a .cpp file to test linkage of a .so
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char **argv) {
	//Java_ru_aot_morph_JavaMorphAPI_initImpl(0,0,0);
	//Java_ru_aot_morph_JavaMorphAPI_lookupWordImpl(0,0,0,0);
	//Java_ru_aot_morph_JavaMorphAPI_closeImpl(0,0);
   void *lib_handle;
   void *fn;
   int x;
   char *error;

   //RTLD_LAZY: If specified, Linux is not concerned about unresolved symbols until they are referenced.
   //RTLD_NOW: All unresolved symbols resolved when dlopen() is called.
   //RTLD_GLOBAL: Make symbol libraries visible.

   lib_handle = dlopen("JNIMorphAPI.so", RTLD_NOW);
   if (!lib_handle) {
      fprintf(stderr, "dlopen: %s\n", dlerror());
      return 1;
   }

   fn = dlsym(lib_handle, "Java_ru_aot_morph_JavaMorphAPI_initImpl");
   if ((error = dlerror()) != NULL){fprintf(stderr, "init: %s\n", error);return 1;}

   fn = dlsym(lib_handle, "Java_ru_aot_morph_JavaMorphAPI_lookupWordImpl");
   if ((error = dlerror()) != NULL){fprintf(stderr, "lookup: %s\n", error);return 1;}

   fn = dlsym(lib_handle, "Java_ru_aot_morph_JavaMorphAPI_closeImpl");
   if ((error = dlerror()) != NULL){fprintf(stderr, "close: %s\n", error);return 1;}

   printf("TEST PASSED\n");
   dlclose(lib_handle);
   return 0;
}
