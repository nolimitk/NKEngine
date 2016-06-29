#include "SerializeContainer.h"
//#include "Scheduler.h"
//#include "Serializer.h"
//#include "../NKEngineLog.h"

using namespace NKScheduler;



SerializeContainer::SerializeContainer(void)
{

}

//SerializeContainer::SerializeContainer(uint size)
//	:_shortTermJob(size)
//	, _sizeShortTermSlot(size)
//{
//}


SerializeContainer::~SerializeContainer(void)
{
}
//
//bool SerializeContainer::Destroy(void)
//{
//	RealTimeJob *pDeleteJob = NULL;
//	RealTimeJob *pNextJob = NULL;
//	
//	// instant queue
//	pNextJob = PopInstantQueue();
//	while( pNextJob != NULL )
//	{
//		pDeleteJob = pNextJob;
//		pNextJob = pNextJob->getNext();
//
//		pDeleteJob->Unref();
//	}
//
//	// short term queue
//	for( uint i = 0 ; i < _sizeShortTermSlot ; i++ )
//	{
//		pNextJob = _shortTermJob.popQueue(i);
//
//		while( pNextJob != NULL )
//		{
//			pDeleteJob = pNextJob;
//			pNextJob = pNextJob->getNext();
//
//			pDeleteJob->Unref();
//		}
//	}
//	
//	// long term list
//	pNextJob = _longTermJob.pop();
//	while( pNextJob != NULL )
//	{
//		pDeleteJob = pNextJob;
//		pNextJob = pNextJob->getNext();
//
//		pDeleteJob->Unref();
//	}
//
//	return true;
//}
//
//bool SerializeContainer::insert(int index, RealTimeJob *pJob)
//{
//	// @TODO index의 max값이 필요하다. 검증도 필요하다.
//
//	if( pJob->TryReserve() == false )
//	{
//		NKENGINELOG_ERROR( L"serializer container, insert, job is already reserved" );
//		_ASSERT(0);
//		return false;
//	}
//
//	// instant execution
//	// it cannot be added at execution index.
//	if( index == 0 )
//	{
//		std::lock_guard<std::mutex> _lock(_mutexInstant);
//		pJob->registerAsInstant( getScheduler()->GetExecutionIndex() );
//		_instantQueue.push( pJob );
//		return true;
//	}
//	//
//
//	if( index < (int)_sizeShortTermSlot )
//	{
//		//NE_DEBUGLOG( L"SerializeContainer insert, add index %d, %I64u, %d, %u", addIndex, schedulerExecutionIndex, index, _sizeShortTermSlot );
//
//		std::lock_guard<std::mutex> _lock(_mutexShortTerm);
//		
//		uint64 schedulerExecutionIndex = getScheduler()->GetExecutionIndex();
//		int addIndex = (schedulerExecutionIndex + index) % _sizeShortTermSlot;
//
//		if( _shortTermJob.push( addIndex, pJob ) == false )
//		{
//			pJob->ReleaseReserve();
//			return false;
//		}
//
//		pJob->registerAsShort( addIndex, (schedulerExecutionIndex + index) );
//	}
//	else
//	{
//		std::lock_guard<std::mutex> _lock(_mutexLongTerm);
//		
//		uint64 schedulerExecutionIndex = getScheduler()->GetExecutionIndex();
//		// @nolimitk execution index값으로 정렬하므로 반드시 미리 설정
//		pJob->registerAsLong( (schedulerExecutionIndex + index) );
//		if( _longTermJob.insert( pJob ) == false )
//		{
//			pJob->UnRegister();
//			pJob->ReleaseReserve();
//			return false;
//		}
//		
//	}
//
//	return true;
//}
//
//RealTimeJob *SerializeContainer::PopQueue(int64 index)
//{
//	if( index < 0 ) return NULL;
//
//	//RealTimeJob *pExecuteJob = NULL;
//	NKCore::TQueue<RealTimeJob> executeQueue;
//	
//	int rotateIndex = (int)(index % _sizeShortTermSlot);
//		
//	{
//		std::lock_guard<std::mutex> _lock(_mutexShortTerm);
//		if( _shortTermJob.popQueue(rotateIndex, executeQueue) == false )
//		{
//			// @TODO error handling을 일단 하지 않는다.
//			NKENGINELOG_ERROR( L"serialize container, pop queue error, %d", rotateIndex );
//		}
//	}
//	
//	// relocation
//	RealTimeJob *plongTermJob = NULL;
//	uint64 originalIndex = index + _sizeShortTermSlot;
//	
//	{
//		std::lock_guard<std::mutex> _lock(_mutexLongTerm);
//		plongTermJob = _longTermJob.pop( originalIndex );
//
//		if( plongTermJob == NULL )
//		{
//			return executeQueue.popQueue();
//		}
//	}
//
//	RealTimeJob *pTraverseJob = NULL;
//	int gapIndex = 0;
//	
//	{
//		std::lock_guard<std::mutex> _lock(_mutexShortTerm);
//		while( plongTermJob != NULL )
//		{
//			// job을 옮길 수 있으므로 임시 변수에 저장한다.
//			pTraverseJob = plongTermJob;
//			plongTermJob = plongTermJob->getNext();
//
//			if( pTraverseJob->getExecuteSlotIndex() == (uint64)index )
//			{
//				// long term에 즉시 실행해야 하는 job이 있을수 있다.
//				executeQueue.push( pTraverseJob );
//				
//				NE_DEBUGLOG( L"%I64d, serializer execute longterm job, slot %I64u", index, pTraverseJob->getExecuteSlotIndex() );
//			}
//			else
//			{
//				if( pTraverseJob->getExecuteSlotIndex() > originalIndex )
//				{
//					NKENGINELOG_ERROR( L"%I64d, serializer relocation bug, slot %I64u, %I64u", index, pTraverseJob->getExecuteSlotIndex(), originalIndex );
//				}
//				else
//				{
//					gapIndex = (int)(pTraverseJob->getExecuteSlotIndex() % _sizeShortTermSlot);
//
//					if( _shortTermJob.push( gapIndex, pTraverseJob ) == false )
//					{
//						NKENGINELOG_ERROR( L"relocation failed, org %I64u, trg %d, slot %I64u, rotate %d", originalIndex, gapIndex, pTraverseJob->getExecuteSlotIndex(), rotateIndex );
//						pTraverseJob->ReleaseReserve();
//					}
//					else
//					{
//						pTraverseJob->registerAsShort( gapIndex, pTraverseJob->getExecuteSlotIndex() );
//						NE_DEBUGLOG( L"%I64d, serialize relocation, org %I64u, trg %d, slot %I64u, rotate %d",
//							index, originalIndex, gapIndex, pTraverseJob->getExecuteSlotIndex(), rotateIndex );
//					}
//				}
//			}
//		}
//	}
//
//	return executeQueue.popQueue();
//}
//
//RealTimeJob *SerializeContainer::PickQueue(int64 index)
//{
//	RealTimeJob *pJob = NULL;
//
//	int rotateIndex = (int)(index % _sizeShortTermSlot);
//
//	
//	{
//		std::lock_guard<std::mutex> _lock(_mutexShortTerm);
//		for( int i = 0 ; i < (int)_sizeShortTermSlot ; i++ )
//		{
//			pJob = _shortTermJob.pickQueue( (rotateIndex+i+1)%_sizeShortTermSlot );
//			if( pJob != NULL )
//			{
//				return pJob;
//			}
//		}
//	}
//
//	// long term job
//	
//	{
//		std::lock_guard<std::mutex> _lock(_mutexLongTerm);
//		pJob = _longTermJob.pick();
//		if( pJob != NULL )
//		{
//			return pJob;
//		}
//	}
//	
//	return NULL;
//}
//
//bool SerializeContainer::Remove(RealTimeJob *pJob)
//{
//	/*_Synchronized( _lockContainer )
//	{
//		return remove( pJob );
//	}*/
//
//	uint64 schedulerExecutionIndex = getScheduler()->GetExecutionIndex();
//
//	if( pJob->GetReserved() == false )
//	{
//		NKENGINELOG_ERROR(L"%I64u, serializer container, remove, job is not reserved, slot %d, %d, %I64u",
//			schedulerExecutionIndex, pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex());
//		return false;
//	}
//
//	int rotateIndex = (int)(schedulerExecutionIndex % _sizeShortTermSlot);
//
//	// @nolimitk you cannot delete instant execution or slot has already executed.
//	if( (pJob->getSlotType() == 1 && pJob->getSlotIndex() == rotateIndex) ||
//		(pJob->getSlotType() == 2 && pJob->getExecuteSlotIndex() == schedulerExecutionIndex) ||
//		pJob->getSlotType() == 3 )
//	{
//		NKENGINELOG_ERROR(L"%I64u, serializer container, remove, job is doing to execute, slot %d, %d, %I64u",
//			schedulerExecutionIndex, pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex());
//		return false;
//	}
//		
//	if( pJob->getSlotType() == 1 && pJob->getSlotIndex() < (int)_sizeShortTermSlot )
//	{		
//		std::lock_guard<std::mutex> _lock(_mutexShortTerm);
//		_shortTermJob.erase( pJob->getSlotIndex(), pJob );
//		pJob->UnRegister();
//	}
//	else if( pJob->getSlotType() == 2 )
//	{
//		std::lock_guard<std::mutex> _lock(_mutexLongTerm);
//		
//		_longTermJob.erase(pJob);
//		pJob->UnRegister();
//		
//	}
//	else
//	{
//		NKENGINELOG_ERROR(L"%I64u, serializer container, remove, job slot info is wrong, slot %d, %d, %I64u",
//			schedulerExecutionIndex, pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex());
//		return false;
//	}
//		
//	pJob->ReleaseReserve();
//
//	return true;
//}
//
//bool SerializeContainer::RemoveAndInsert(int index, RealTimeJob *pJob)
//{
//	if( Remove( pJob ) == false )
//	{
//		NKENGINELOG_ERROR( L"serialize container, remove and insert, remove a job failed" );
//		return false;
//	}
//
//	if( Insert(index,pJob) == false )
//	{
//		NKENGINELOG_ERROR( L"serialize container, remove and insert, insert a job failed" );
//		return false;
//	}
//	
//	return true;
//}
//
//RealTimeJob *SerializeContainer::PopInstantQueue(void)
//{
//	std::lock_guard<std::mutex> _lock(_mutexInstant);
//	return _instantQueue.popQueue();
//}