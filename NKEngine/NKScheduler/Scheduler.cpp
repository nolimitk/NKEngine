#include "Scheduler.h"
#include "../NKEngineLog.h"
#include "Serializer.h"

using namespace NKScheduler;
using namespace std;

bool NKScheduler::SchedulerThread::onStart(void)
{
	timeBeginPeriod(1);
	return true;
}

bool SchedulerThread::onRun(void)
{
	return Scheduler::getInstance()->execute();
}

bool NKScheduler::SchedulerThread::onEnd(void)
{
	timeEndPeriod(1);
	return true;
}

const std::chrono::microseconds Scheduler::SCHEDULER_TIME_UNIT = 50000us;

Scheduler::Scheduler(void)
//	:_container(size)
//	, _sizeShortTermSlot(size)
//	, _executionIndex(0)
{
}

Scheduler::~Scheduler(void)
{
	//Stop();
	
	//_container.Destroy();
}

bool Scheduler::start(void)
{
	//if( _sizeShortTermSlot == 0 ) return false;

	if (_thread.create() == false)
	{
		return false;
	}

	return true;
}

bool Scheduler::stop(void)
{
	if (_thread.join() == false)
	{
		return false;
	}
	return true;
};

bool Scheduler::addSerializer(const std::shared_ptr<Serializer>& serializer, const std::chrono::milliseconds& interval)
{
	if (serializer == nullptr) { _ASSERT(0); return false; }
	// @TODO tick�� max���� ���Ѵ�.
	if (interval == 0ms) { _ASSERT(0); return false; }

	// index 50~99 -> 0, 100~149 -> 1, 150->199 -> 2, 200->249 -> 3, , , 950~999 -> 18
	int slice = static_cast<int>((interval.count() / 50) - 1);
	uint64_t execution_index = _clock.executionIndex();
	uint64_t rotateIndex = execution_index % RECOMMAND_SHORTTERMJOB_SIZE;
	uint64_t add_index = (rotateIndex + slice) % RECOMMAND_SHORTTERMJOB_SIZE;

	if (_shortterm_slot[add_index].push(serializer) == false)
	{
		NKENGINELOG_ERROR(L"[SCHEDULER], %I64u, scheduler addslot failed, tick %I64u, want %d, index %I64u", execution_index, interval.count(), slice, add_index);
		return false;
	}

	NKENGINELOG_INFO(L"[SCHEDULER],0,add serializer, tick %u, slot %d", interval.count(), slice);

	return true;
}

bool Scheduler::execute(void)
{
	// update execution index
	uint64_t execution_index = _clock.executionIndex();
	_clock.increaseIndex();
	///

	// pop queue
	uint64_t rotateIndex = execution_index % RECOMMAND_SHORTTERMJOB_SIZE;
	SerializerSP job = _shortterm_slot[rotateIndex].popQueue();
	SerializerSP next_job = nullptr;
	
	// execution
	while(job != nullptr )
	{
		NKENGINELOG_INFO(L"[SCHEDULER],%I64u,execution, slot %I64u", execution_index, rotateIndex);

		// @nolimitk slot�� �����߿� �ٽ� ����� �� �ֵ��� �ʱ�ȭ�� ���� �Ѵ�.
		next_job = job->getNext();
		//pExecuteSlot->UnRegister();
		//pExecuteSlot->ReleaseReserve();
			
		NKNetwork::IOCPManager::getInstance()->postEvent(job, execution_index);
						
		job = next_job;
	}
		
	// instant execution
	/*
	pExecuteSlot = _container.PopInstantQueue();
	while( pExecuteSlot != NULL )
	{
		while( pExecuteSlot != NULL )
		{
			// instant execution�� index�� ���� ����� �� �ִ�.
			if( pExecuteSlot->getExecuteSlotIndex() != _executionIndex &&
				pExecuteSlot->getExecuteSlotIndex() != _executionIndex-1 )
			{
				NE_WARNINGLOG( L"%I64u, slot instant execution delayed, slot %I64u", _executionIndex, pExecuteSlot->getExecuteSlotIndex() );
			}

			pNextSlot = pExecuteSlot->getNext();
			pExecuteSlot->UnRegister();
			pExecuteSlot->ReleaseReserve();
			
			IOCPManager::getInstance()->PostEvent( pExecuteSlot, _executionIndex );
				
			pExecuteSlot = pNextSlot;
		}

		// instant execution�� ������ �߻��� �� �����Ƿ� �ð� ������ �д�.
		instantTime = nextTime + SCHEDULER_TIME_UNIT;
		currentTime = clock.getElapsedMicroSec();
		if( currentTime > instantTime )
		{
			NE_WARNINGLOG( L"%I64u, instant execution delayed, limit %lf, execution %lf", _executionIndex, instantTime, currentTime );
			break;
		}

		// instant execution ���߿� instant execution��û�� �ٽ� �� �� �ִ�.
		pExecuteSlot = _container.PopInstantQueue();
	}
	*/

	//////////////////////////////////////////////////////////////////////////////////////////
	// wait
	static std::chrono::microseconds next_tick = 0us;
	static std::chrono::microseconds last_tick = 0us;

	std::chrono::microseconds current_tick = _clock.getElapsedTime();
	std::chrono::milliseconds wait_time = 0ms;

	//int64_t instantTime = 0;

	next_tick += SCHEDULER_TIME_UNIT;
	// Slotó���� �ð��� �ʹ� ���� �ɸ� ����̴�.
	if(current_tick > next_tick)
	{			
		// @TODO ó���� Slot�� ���� ������ ���ܾ� �Ѵ�.
#ifndef _DEBUG
		NE_WARNINGLOG( L"scheduler lack, next %lf, current %lf, wait %lf, tick %I64u", nextTime, currentTime, waitTime, _executionIndex );
#endif
	}
	else
	{
		wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(next_tick - current_tick);
		if(wait_time > SCHEDULER_TIME_UNIT )
		{
			// realtime�� �ƴ� ��쿡�� ������ ����ð��� �������� �ʴ´�. [2014/12/10/ nolimitk]
			//if( true == _realtime )
			{
				// @nolimitk waitTime�� SCHEDULER_TIME_UNIT ���� ū ���� �Ͼ���� �ȵȴ�.
				//NE_ERRORLOG( L"scheduler accuracy is not match, next %lf, current %lf, wait %lf, tick %I64u", nextTime, currentTime, waitTime, _executionIndex );
				next_tick = current_tick + SCHEDULER_TIME_UNIT;
				wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(SCHEDULER_TIME_UNIT);
			}
		}
	}

	if(wait_time > 0ms)
	{
		NKENGINELOG_INFO(L"[SCHEDULER],%I64u,wait, time %I64d", execution_index, wait_time);
		//if( true == _realtime )
		{
			// @nolimitk ���е��� ���� sleep, slot�� ��� cpu�� �����ϰ� �ȴ�.
			_clock.sleep(wait_time);
		}
		//else
		{
		//	Sleep( (DWORD)(waitTime/1000) );
		}
	}

	// @nolimitk ����ð��� �׻� SCHEDULER_TIME_UNIT �̾�� �Ѵ�.
	// execution_time�� SCHEDULER_TIME_UNIT ���� ���� ��� Sleep�� ���� WaitTime ���� ���ڶ�� �߻����� ����.(����)
	// execution_time�� SCHEDULER_TIME_UNIT ���� Ŭ ��� Sleep�� WaitTime�� over�ؼ� �߻��߰ų�(����)
	// ���������� ����sleep�� ����ص� 1ms�� ������ �߻��Ѵ�. 2%�� �̹��� �����̹Ƿ� ����Ѵ�.
	{
		std::chrono::microseconds execution_time = _clock.getElapsedTime() - last_tick;
		last_tick = _clock.getElapsedTime();
		// 49.4 �̸�(48.4�̸����� ����)
		if(execution_time < SCHEDULER_TIME_UNIT-500us )
		{
			NKENGINELOG_INFO( L"[SCHEDULER],%I64u, not enough sleep, %I64u, %I64u", execution_index, execution_time, wait_time);
		}
		// 50.5 �̻�(51.5�̻����� ����)
		else if(execution_time > SCHEDULER_TIME_UNIT+500us )
		{
			//NE_WARNINGLOG( L"%I64u, over sleep, %u, %lf", _executionIndex, executeTime, waitTime );
		}
		else
		{
			NKENGINELOG_INFO(L"[SCHEDULER],%I64u, execution time, %I64u, %I64u", execution_index, execution_time, wait_time);
		}
	}
		
	//////////////////////////////////////////////////////////////////////////////////////////

	return true;
}



/*


bool Scheduler::DeleteSlot(EventSlot *pSlot)
{
	NE_DEBUGLOG( L"%I64u, scheduler delete slot, slot %d, %d, %I64u",
		_executionIndex, pSlot->getSlotType(), pSlot->getSlotIndex(), pSlot->getExecuteSlotIndex() );

	if( _container.Remove( pSlot ) == false )
	{
		NE_ERRORLOG( L"%I64u, scheduler delete slot failed, slot %d, %d, %I64u",
			_executionIndex, pSlot->getSlotType(), pSlot->getSlotIndex(), pSlot->getExecuteSlotIndex() );
		return false;
	}

	pSlot->Unref();

	return true;
}

bool Scheduler::ResetSlot(EventSlot *pSlot, uint32_t tick)
{
	 @TODO tick�� max���� ���Ѵ�.
	
	 index 50~99 -> 1, 100~149 -> 2, 150->199 -> 3, 200->249 -> 4
	int slice = (int)(tick / SCHEDULER_TIME_UNIT);

	NE_DEBUGLOG( L"%I64u, scheduler remove and insert, tick %u, want %d, slot %d, %d, %I64u",
		_executionIndex, tick, slice, pSlot->getSlotType(), pSlot->getSlotIndex(), pSlot->getExecuteSlotIndex()  );

	if( _container.RemoveAndInsert( slice, pSlot ) == false )
	{
		NE_ERRORLOG( L"%I64u, scheduler resetslot failed, tick %u, want %d", _executionIndex, tick, slice );
		return false;
	}

	return true;
}
*/