#ifndef  GramInfo_h
#define  GramInfo_h


//! CGramInfo represents one morphological pattern, for example "[SUB pl, sg]"
struct CGramInfo 
{
	//! the language of the pattern 
	MorphLanguageEnum	m_Language;
	//! the part of speech mask  (SUB, ART,ADJ...)
	uint32_t				m_PartOfSpeechMask;
	//! morphological features (plu, sig, nom...)
	uint64_t				m_Grammems;

	CGramInfo () {
		m_PartOfSpeechMask = 0;
	    m_Grammems = 0;;
		m_Language = morphUnknown;

	};
	CGramInfo (MorphLanguageEnum Language, uint32_t PartOfSpeechMask, uint64_t Grammems)
	{ 
		m_PartOfSpeechMask = PartOfSpeechMask;
	    m_Grammems = Grammems;
		m_Language = Language;

	};
	

	bool  EqualOrLess (const CGramInfo& X) const
	{ 
		return 
			   (X.m_Language  == m_Language ) 
			&& ( (m_PartOfSpeechMask &  X.m_PartOfSpeechMask) > 0)
			&& ( (m_Grammems &  X.m_Grammems) == X.m_Grammems)
				;
	};

	bool operator ==  (const CGramInfo& X) const
	{
		return		(m_PartOfSpeechMask == X.m_PartOfSpeechMask)
				&&  (X.m_Language  == m_Language ) 
				&&	(m_Grammems == X.m_Grammems);
	};
	
	bool operator <  (const CGramInfo& X) const
	{
		if (m_Language != X.m_Language)
			return m_Language < X.m_Language;

		if (m_PartOfSpeechMask != X.m_PartOfSpeechMask)
			return m_PartOfSpeechMask < X.m_PartOfSpeechMask;

		return m_Grammems < X.m_Grammems;
	};

	void AddGramInfo (const CGramInfo& X) 
	{
		m_PartOfSpeechMask |= X.m_PartOfSpeechMask;
		m_Grammems |= X.m_Grammems;
	};

};

inline size_t get_size_in_bytes (const CGramInfo& i)
{
	return		1/*get_size_in_bytes(i.m_Language)*/ 
			+	get_size_in_bytes(i.m_PartOfSpeechMask) 
			+	get_size_in_bytes(i.m_Grammems);

};

inline size_t save_to_bytes(const CGramInfo& i, BYTE* buf)
{
	BYTE t = i.m_Language;
	buf += save_to_bytes(t, buf);
	buf += save_to_bytes(i.m_PartOfSpeechMask, buf);
	buf += save_to_bytes(i.m_Grammems, buf);
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(CGramInfo& i, const BYTE* buf)
{
	BYTE t;
	buf += restore_from_bytes(t, buf);
	i.m_Language = (MorphLanguageEnum)t;
	buf += restore_from_bytes(i.m_PartOfSpeechMask, buf);
	buf += restore_from_bytes(i.m_Grammems, buf);
	return get_size_in_bytes(i);
}


struct CGramInfoAndGraphemUnitOffset
{
	CGramInfo	m_GramInfo;
	int			m_UnitOffset;
	bool operator < (const CGramInfoAndGraphemUnitOffset& X) const
	{
		return m_UnitOffset< X.m_UnitOffset;
	};
};

inline size_t get_size_in_bytes (const CGramInfoAndGraphemUnitOffset& i)
{
	return get_size_in_bytes(i.m_GramInfo) + get_size_in_bytes(i.m_UnitOffset);

};

inline size_t save_to_bytes(const CGramInfoAndGraphemUnitOffset& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_GramInfo, buf);
	buf += save_to_bytes(i.m_UnitOffset, buf);
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(CGramInfoAndGraphemUnitOffset& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_GramInfo, buf);
	buf += restore_from_bytes(i.m_UnitOffset, buf);
	return get_size_in_bytes(i);
}

#endif
