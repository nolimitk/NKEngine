#include "RealTimeJob.h"
//#include "Serializer.h"
#include "../NKEngineLog.h"

using namespace NKScheduler;

RealTimeJob::RealTimeJob(void)
	:_reserved(0)
	,_slotType(0)
	,_slotIndex(-1)
	,_executeSlotIndex(0)
	,_pContainer(nullptr)
	,_tick(0)
{
}

RealTimeJob::~RealTimeJob(void)
{
}

bool RealTimeJob::ReExecute(uint32_t tick)
{
	//return _pContainer->AddTimeJob(this,tick);
	return false;
}

bool RealTimeJob::ReExecute(void)
{
	//return _pContainer->AddTimeJob(this,_tick);
	return false;
}

void RealTimeJob::UnRegister(void)
{
	// @nolimitk slot type은 왜 reset 안했었더라...?
	//ReleaseReserve();
	//_bReserved = false;
	_slotIndex = -1;
	_executeSlotIndex = 0;
	
	setPrev(nullptr);
	setNext(nullptr);
}

bool RealTimeJob::TryReserve(void)
{
	int init = InterlockedExchange( &_reserved, 1 );
	if( init == 1 ) return false;
	return true;
}

void RealTimeJob::ReleaseReserve(void)
{
	int init = InterlockedExchange( &_reserved, 0 );
	if( init == 0 )
	{
		NKENGINELOG_ERROR( L"realtimejob, release reserve, it would be bug." );
	}
}