#ifndef _Valency_H
#define _Valency_H

#include "morph_dict/common/utilit.h"
#include "common/cortege.h"
#include "dicts/StructDictLib/StructDictConsts.h"

enum Valency_Direction { A_C, C_A};
typedef TCortege TCortege;

class CRossHolder;
struct CValency 
{
		Valency_Direction m_Direction;
		std::string            m_RelationStr; 
		long              m_RelationId;
        BYTE              m_LeafId;
		BYTE              m_BracketLeafId;
		uint16_t              m_UnitNo;
		bool              m_IsAddition;
		const CRossHolder*      m_RossHolder;
		bool              m_InstrAgent;
		// это валентность помечена !
		bool			  m_bObligatory;
		// это валентность считается обязательной, потому что
		// она первая в валентной структуре
		bool			  m_bObligatoryBecauseFirst;

		// это валентность помечена ?
		bool			  m_bOptional;
		
		
		bool IsFromDict () const;
		bool operator == ( const CValency& X) const;
		
		CValency (std::string  RelationStr, Valency_Direction Direction, const CRossHolder* Ross = 0, BYTE LeafId = 0, BYTE BracketLeafId = 0, uint16_t UnitNo = ErrUnitNo);
		CValency();
		CValency(	const TCortege C, 
					const long MainWordVarNo, 
					const CRossHolder* Ross, 
					unsigned short UnitNo = ErrUnitNo
					);
};


#endif
