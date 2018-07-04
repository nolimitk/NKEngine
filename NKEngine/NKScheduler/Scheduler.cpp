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

const std::chrono::microseconds Scheduler::SCHEDULER_INTERVAL_MARGIN = 500us;
const std::chrono::microseconds Scheduler::SCHEDULER_INTERVAL_UNIT = 50ms;

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

uint64_t Scheduler::convertToExecutionIndex(const std::chrono::milliseconds& interval)
{
	// @TODO interval�� max���� ���Ѵ�.
	// index 0~49 -> 0, 50~99 -> 0, 100~149 -> 1, 150->199 -> 2, 200->249 -> 3,,, 950~999 -> 4
	if (interval < 50ms) return 0;
	int64_t slice = (interval / SCHEDULER_INTERVAL_UNIT) - 1;
	if (slice < 0 || slice >= DEFAULT_JOBSLOT_SHORTTERM_SIZE)
	{
		_ASSERT(false);
		return false;
	}

	uint64_t execution_index = _clock.executionIndex();
	return execution_index + slice;
}

bool Scheduler::addSerializer(const SerializerSP& serializer, uint64_t reserve_execution_index)
{
	if (serializer == nullptr) { _ASSERT(false); return false; }
	if (serializer->canReserve(reserve_execution_index) == false)
	{
		return false;
	}

	uint32_t round_slice = reserve_execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;

	if (_shortterm_slot[round_slice].push(serializer) == false)
	{
		//NKENGINELOG_ERROR(L"[SCHEDULER], %I64u, scheduler addslot failed, tick %I64u, want %d, index %I64u", execution_index, interval.count(), slice, round_slice);
		return false;
	}

	serializer->setReserve(reserve_execution_index);

	//NKENGINELOG_INFO(L"[SCHEDULER],0,add serializer, tick %u, slot %d", interval.count(), slice);

	return true;
}

bool Scheduler::execute(void)
{
	/// update execution index
	uint64_t execution_index = _clock.executionIndex();
	_clock.increaseIndex();
	///

	{
		/// pop queue
		uint64_t round_slice = execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;
		NKCore::TWaitFreeQueue2<SerializerSP>::iterator_type node = _shortterm_slot[round_slice].popQueue();
		NKCore::TWaitFreeQueue2<SerializerSP>::iterator_type next_node = nullptr;
		///

		/// execution
		while (node != nullptr)
		{
			NKENGINELOG_INFO(L"[SCHEDULER],%I64u,execution,slot %I64u", execution_index, round_slice);

			// @nolimitk slot�� �����߿� �ٽ� ����� �� �ֵ��� �ʱ�ȭ�� ���� �Ѵ�.
			next_node = node->getNext();
			//pExecuteSlot->UnRegister();
			//pExecuteSlot->ReleaseReserve();

			//job->setReserveFalse();
			NKNetwork::IOCPManager::getInstance()->postEvent(node->getValue(), execution_index);

			node = next_node;
		}
		///
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

	next_tick += SCHEDULER_INTERVAL_UNIT;
	// Slotó���� �ð��� �ʹ� ���� �ɸ� ����̴�.
	if(current_tick > next_tick)
	{			
		// @TODO ó���� Slot�� ���� ������ ���ܾ� �Ѵ�.
#ifndef _DEBUG
		//NE_WARNINGLOG( L"scheduler lack, next %lf, current %lf, wait %lf, tick %I64u", nextTime, currentTime, waitTime, _executionIndex );
#endif
	}
	else
	{
		wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(next_tick - current_tick);
		if(wait_time > SCHEDULER_INTERVAL_UNIT)
		{
			// realtime�� �ƴ� ��쿡�� ������ ����ð��� �������� �ʴ´�. [2014/12/10/ nolimitk]
			//if( true == _realtime )
			{
				// @nolimitk waitTime�� SCHEDULER_TIME_UNIT ���� ū ���� �Ͼ���� �ȵȴ�.
				//NE_ERRORLOG( L"scheduler accuracy is not match, next %lf, current %lf, wait %lf, tick %I64u", nextTime, currentTime, waitTime, _executionIndex );
				next_tick = current_tick + SCHEDULER_INTERVAL_UNIT;
				wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(SCHEDULER_INTERVAL_UNIT);
			}
		}
	}

	if(wait_time > 0ms)
	{
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
		if (execution_time < SCHEDULER_INTERVAL_UNIT - SCHEDULER_INTERVAL_MARGIN)
		{
			NKENGINELOG_INFO(L"[SCHEDULER],%I64u,not enough sleep,execution %I64u us,wait %I64u ms", execution_index, execution_time, wait_time);
		}
		// 50.5 �̻�(51.5�̻����� ����)
		else if (execution_time > SCHEDULER_INTERVAL_UNIT + SCHEDULER_INTERVAL_MARGIN)
		{
			NKENGINELOG_INFO(L"[SCHEDULER],%I64u,over sleep,execution %I64u us,wait %I64u ms", execution_index, execution_time, wait_time);
		}
		else
		{
			NKENGINELOG_INFO(L"[SCHEDULER],%I64u,execution time,execution %I64u us,wait %I64u ms", execution_index, execution_time, wait_time);
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