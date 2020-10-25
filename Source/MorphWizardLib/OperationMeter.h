// OperationMeter.h: interface for the COperationMeter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __OPERATION_METER_RML_H__
#define __OPERATION_METER_RML_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef  WIN32
	typedef  const char* LPCSTR;
#else
	#define NOMINMAX 
	#include "windows.h"
#endif

#include "assert.h"

template <class _Ty>
const _Ty& my_max(const _Ty& _Left, const _Ty& _Right) {
	return _Left < _Right ? _Right : _Left;
}

//----------------------------------------------------------------------------
// COperationMeter abstract class
//----------------------------------------------------------------------------
class COperationMeterRML
{
public:
	virtual ~COperationMeterRML()						{}

	uint32_t GetMaxPos() const 						{ return m_maxPos; } 
	uint32_t GetPos() const 							{ return m_curPos; } 
	
	bool SetMaxPos(uint32_t pos, uint32_t count=50 )
	{
		if( pos!=m_maxPos && pos>=0 || m_curPos!=0 ) 
		{
			m_maxPos = pos;
			SetStepCount(count);	// коррекция шага
			UpdateMaxPos();
	//		if( m_curPos>m_maxPos ) 
			{
	//			m_curPos = m_maxPos;
				m_curPos = m_pos = 0;	//!!!
				UpdatePos();
			}
			return true;
		}
		return false;
	}

	bool SetPos( uint32_t pos )
	{
		if( pos!=m_curPos && pos<=m_maxPos )
		{
			m_curPos = pos;
			if( pos>=m_pos+GetStep() || pos<=m_pos-GetStep() || pos==m_maxPos ) 
			{
				m_pos = pos;
				UpdatePos();
				return true;
			}
		}
		return false;
	}

	uint32_t GetStep() const 							
	{ 
		assert(m_step!=0 && m_step<=m_maxPos); 
		return m_step; 
	} 

	void SetStep( uint32_t step = 0)
	{
		auto a = my_max(m_maxPos / 50, (uint32_t)1);
		m_step = (step==0 || step>=m_maxPos) ? a : step;
	}

	void SetStepCount( uint32_t count )
	{
		assert(count>0);
		m_step = my_max(m_maxPos/count,(uint32_t)1);
	}

	bool AddPos( uint32_t pos  = 1)
	{
		assert(pos>0);
		return SetPos(pos+m_curPos);
	}
	
	virtual void SetInfo( LPCSTR info )		{}
	
	//__declspec(property(get=GetMaxPos,	put=SetMaxPos))		uint32_t 	MaxPos;
	//__declspec(property(get=GetPos,		put=SetPos))		uint32_t 	Pos;
protected:
	COperationMeterRML()		: m_maxPos(0), m_curPos(0), m_step(0), m_pos(0) {}
	
	virtual	void UpdateMaxPos()=0;
	virtual	void UpdatePos()=0;
private:
	uint32_t 	m_maxPos;
	uint32_t 	m_curPos;
	uint32_t 	m_pos;
	uint32_t	m_step;

};

//----------------------------------------------------------------------------
class CSimpleMeterRML: public COperationMeterRML
{
public:
	CSimpleMeterRML()	{}
	
	virtual void SetInfo( LPCSTR info )		{ printf("\n%s",info); }
protected:
	virtual	void UpdateMaxPos() {}
	virtual	void UpdatePos()	{ 	printf("\r %i%%  ",GetPos()*100/GetMaxPos()); }
};

//----------------------------------------------------------------------------
class CFileMeterRML: public COperationMeterRML
{
public:
	CFileMeterRML() : m_fp(0)	{}

	bool SetFileMaxPos( FILE* fp )
	{
		if( fp!=NULL )
		{
			m_fp = fp;
			int fileLen = 0; 
			if( fseek(fp,0,SEEK_END)==0 )
			{
				fileLen = ftell(fp);
				if( fileLen>0 ) 
				{
					rewind(fp);
					return SetMaxPos(fileLen);
				}
			}
		}
		return false;
	}

	bool SetFilePos()
	{
		if(		GetMaxPos()>0  &&	m_fp!=NULL )
			return  SetPos(ftell(m_fp));
		return false;
	}

private:

	FILE* m_fp;
};


#endif 
