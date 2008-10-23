
// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#ifndef graspace_h
 #define graspace_h

   #include "utilit.h"

   #define   SEOLN        (unsigned char) '´'
   #define   SSpace       (unsigned char) 'Å'
   #define   STab         '\x10'

   inline bool is_gra_space (int c)
    { return c==SEOLN || c == SSpace || c == STab;}

   // max length of the token
   const size_t CriticalTokenLength = 255;

   // length of the token and its graphematical descriptors 
   const size_t CriticalGraphemLineLength = CriticalTokenLength*2;

#endif
