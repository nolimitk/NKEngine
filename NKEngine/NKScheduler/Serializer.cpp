#include "Serializer.h"
//#include <crtdbg.h>
#include "../NKEngineLog.h"
#include "RealTimeJob.h"

using namespace NKScheduler;

Serializer::Serializer(void)
//	:_lastReservedTick(0)
//	, _container(RECOMMAND_SHORTTERMJOB_SIZE)
{
}

Serializer::~Serializer(void)
{
//	_container.Destroy();
}

bool Serializer::addJob(const RealTimeJobSP& job, uint32_t tick)
{
	if (job == nullptr)
	{
		_ASSERT(0);
		return false;
	}
	// @TODO tick�� max���� ���Ѵ�.

	// index 50~99 -> 0, 100~149 -> 1, 150->199 -> 2, 200->249 -> 3,,, 950~999 -> 18
	int slice = (int)((tick / SCHEDULER_TIME_UNIT) - 1);

	if (_shortterm_slot[slice].push(job) == false)
	{
		NKENGINELOG_ERROR(L"[SERIALIZER], failed to push the job into queue,tick %u, slice %d", tick, slice);
		return false;
	}
	
#if defined _SCHEDULER_DEBUG_LOG_
	NKENGINELOG_INFO( L"[SERIALIZER],add job, tick %u, slice %d, slot %I64u", tick, slice, pJob->getExecuteSlotIndex() );
#endif

	// scheduler slice�� �־�д�... �̹� �־�ξ����� �� ���� �ʿ䰡 ���µ�...
	// �׳� serializer queue�� �־�ΰ� loop���� ó���ص� ���� �ʳ�?
	//RegisterScheduler(tick);
	return true;
}

/*


bool Serializer::DeleteTimeJob(RealTimeJob *pJob)
{
	NE_DEBUGLOG( L"serializer delete slot, type %d, slot %d, executeslot %I64u", pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex() );

	if( _container.Remove( pJob ) == false )
	{
		NKENGINELOG_ERROR( L"serializer delete slot failed, type %d, slot %d, executeslot %I64u", pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex() );
		return false;
	}

	pJob->SetContainer(nullptr);
	pJob->SetTick(0);
	pJob->Unref();

	// @TODO �ش� slot�� job�� ���� ���� ��ϵ� tick�� ��� ����ؾ� �Ѵ�.
	// @TODO ������� ���� ��� ������ �ø���������� �ѹ� ����� �� �ִ�.

	return true;
}

bool Serializer::ResetTimeJob(RealTimeJob *pJob, uint tick)
{
	// @TODO tick�� max���� ���Ѵ�.
	if( tick == 0 ) tick = 1;
	
	// index 50~99 -> 1, 100~149 -> 2, 150->199 -> 3, 200->249 -> 4
	int slice = (int)(tick / SCHEDULER_TIME_UNIT);

	NE_DEBUGLOG( L"serializer remove and insert, tick %u, want %d, type %d, slot %d, executeslot %I64u",
		tick, slice, pJob->getSlotIndex(), pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex() );

	if( _container.RemoveAndInsert( slice, pJob ) == false )
	{
		NKENGINELOG_ERROR( L"serializer resetjob failed, tick %u, want %d", tick, slice );
		return false;
	}

	if( GetReserved() == false )
	{
		// ����� �ȵǾ� ������ ���״�.
		NKENGINELOG_ERROR( L"serializer resetjob error, tick %u, want %d", tick, slice );
	}
	else
	{
		if( tick < _lastReservedTick )
		{
			_lastReservedTick = tick;
			if( getScheduler()->ResetSlot( this, tick ) == true )
			{
				// execution index to start serializer
				_lastExecutionIndex = _executeSlotIndex;
				return true;
			}

			return false;
		}

		return true;
	}

	return false;
}

bool Serializer::RegisterScheduler(uint tick)
{
	bool result = false;
	if( TryReserve() == true )
	{
		_lastReservedTick = tick;
		result = getScheduler()->AddSlot( this, tick );
	}
	else
	{
		if( tick < _lastReservedTick )
		{
			_lastReservedTick = tick;
			result = getScheduler()->ResetSlot( this, tick );
		}
	}

	if( result == true )
	{		
		// execution index to start serializer
		_lastExecutionIndex = _executeSlotIndex;
	}
	return true;
}
*/

bool Serializer::execute(uint64_t execute_index)
{
#if defined _SCHEDULER_DEBUG_LOG_
	//NE_DEBUGLOG( L"serializer executed, %I64u", execute_index );
#endif

	// @TODO job ó�� �ð��� �����Ͽ� monitoring �Ѵ�.
	NKCore::NKClock clock;
	int64_t execute_time = 0;

	//_lastReservedTick = (uint32_t)(-1);

	int64_t rotateIndex = execute_index % RECOMMAND_SHORTTERMJOB_SIZE;

	RealTimeJobSP job = _shortterm_slot[rotateIndex].popQueue();
	RealTimeJobSP next_job = nullptr;
	
	/// execution
	while (job != nullptr)
	{
		// @nolimitk job�� �����߿� �ٽ� ����� �� �ֵ��� �ʱ�ȭ�� ���� �Ѵ�.
		next_job = job->getNext();
#if defined _SCHEDULER_DEBUG_LOG_
		//NE_DEBUGLOG( L"job executed, job %I64u, last %I64u, execute %I64u", pJob->getExecuteSlotIndex(), _lastExecutionIndex, execute_index  );
#endif
		//job->UnRegister();
		//job->ReleaseReserve();

		job->onExecute(execute_index);

		job = next_job;
	}
	///

	//_lastExecutionIndex = execute_index;

	// instant execution
	/*
	pJob = _container.PopInstantQueue();
	while (pJob != NULL)
	{
		while (pJob != NULL)
		{
			// instant execution�� �ٷ� ����ǰų� 1tick�� ������ ����� �� �ִ�.
			if (pJob->getExecuteSlotIndex() != (uint)(execute_index - 1) &&
				pJob->getExecuteSlotIndex() != (uint)execute_index)
			{
				NKENGINELOG_WARNING(L"%I64u, job instant execution delayed, slot %I64u", execute_index, pJob->getExecuteSlotIndex());
			}

			pNextJob = pJob->getNext();
			pJob->UnRegister();
			pJob->ReleaseReserve();

			pJob->Process((uint64)execute_index);
			pJob->Unref();

			pJob = pNextJob;
		}

		// instant execution�� ������ �߻��� �� �����Ƿ� �ð� ������ �д�.
		executeTime = clock.getElapsedMicroSec();
		if (executeTime > SCHEDULER_TIME_UNIT)
		{
			NKENGINELOG_WARNING(L"%I64u, instant execution limited, execution %td", execute_index, executeTime);
			break;
		}

		// instant execution ���߿� instant execution��û�� �ٽ� �� �� �ִ�.
		pJob = _container.PopInstantQueue();
	}
	*/
	//

	// @nolimitk ���� ������ ���� ���� tick�� ã�´�.
	// 14.06.09 �����߿� ������� ��쿡�� �̹� ��ϵ� �� ���� �����ؾ��� job�� ���� �� �ִ�.
	// ���� ���� ���ɼ��� �����Ѵ�.
	/*
	pJob = _container.PickQueue(execute_index);
	if (pJob != NULL)
	{
		// 14.06.09 �����߿� ������� ��쿡�� �̹� ��ϵ� �� ���� �����ؾ��� job�� ���� �� �ִ�.
		if (_executeSlotIndex == 0 || pJob->getExecuteSlotIndex() < _executeSlotIndex)
		{
			int rotateIndex = (int)(execute_index % RECOMMAND_SHORTTERMJOB_SIZE);
			int tick = 0;

			int slotType = pJob->getSlotType();
			if (slotType == 1)
			{
				// short term slot
				int slotIndex = pJob->getSlotIndex();

				if (slotIndex == rotateIndex) tick = RECOMMAND_SHORTTERMJOB_SIZE * SCHEDULER_TIME_UNIT;
				else if (slotIndex > rotateIndex) tick = (int)(slotIndex - rotateIndex) * SCHEDULER_TIME_UNIT;
				else if (slotIndex < rotateIndex) tick = (RECOMMAND_SHORTTERMJOB_SIZE - rotateIndex + (int)slotIndex) * SCHEDULER_TIME_UNIT;
			}
			else
			{
				// long term slot
				tick = (int)(pJob->getExecuteSlotIndex() - execute_index) * SCHEDULER_TIME_UNIT;
			}

			NE_DEBUGLOG(L"%I64u, next serializer, slot %d, %d, execute %I64d, rotate %d, tick %d, serializer execute %I64u",
				execute_index, pJob->getSlotType(), pJob->getSlotIndex(), pJob->getExecuteSlotIndex(), rotateIndex, tick, _executeSlotIndex);

			RegisterScheduler(tick);
		}
	}
	*/

	return true;
}

bool Serializer::onProcess(NKNetwork::EventContext& event_context, uint32_t transferred)
{
	_ASSERT(event_context._type == NKNetwork::EVENTCONTEXTTYPE::SCHEDULER);

	NKNetwork::SchedulerContext& scheduler_context = static_cast<NKNetwork::SchedulerContext&>(event_context);
	NKENGINELOG_INFO(L"slot executed, %I64u", scheduler_context._param);

	execute(scheduler_context._param);

	//SAFE_DELETE(pEventContext);

	//Unref();
	
	return true;
}