// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#ifndef __RmlThes_h__
#define __RmlThes_h__

#ifndef __RML_THEs_CONSTS
#define __RML_THEs_CONSTS

	enum RelationTypes {
	  Coord,
	  Subord,
	  CoordSimilar,
	  Hyphen,
	  eSubord,
	  eParat
	};
	
	enum Domains
	{
	  Common = 0, 
	  Geographics = 2, 
	  People = 4, 
	  Computer = 64, 
	  Finance = 128
	};



#endif

#endif
