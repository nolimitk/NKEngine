#include "Serializer.h"
#include "../NKEngineLog.h"
#include "RealTimeJob.h"

using namespace NKScheduler;

Serializer::Serializer(void)
	:_reserved(false)
	,_reserved_execution_index(0)
{
}

Serializer::~Serializer(void)
{
}

bool Serializer::addJob(const RealTimeJobSP& job, uint64_t reserve_execution_index)
{
	if (job == nullptr){ _ASSERT(false); return false;}
	//if (job->getNext() != nullptr) { _ASSERT(false); return false; }
	
	uint32_t round_slice = reserve_execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;

	if (_shortterm_slot[round_slice].push(job) == false)
	{
		//NKENGINELOG_ERROR(L"[SERIALIZER], failed to push the job into queue,tick %u, slice %d", tick, slice);
		return false;
	}
	
#if defined _SCHEDULER_DEBUG_LOG_
	NKENGINELOG_INFO( L"[SERIALIZER],add job, tick %u, slice %d, slot %I64u", tick, slice, pJob->getExecuteSlotIndex() );
#endif

	return true;
}

bool Serializer::execute(uint64_t execution_index)
{
#if defined _SCHEDULER_DEBUG_LOG_
	//NE_DEBUGLOG( L"serializer executed, %I64u", execute_index );
#endif

	// @TODO job ó�� �ð��� �����Ͽ� monitoring �Ѵ�.
	NKCore::NKClock clock;
	int64_t execute_time = 0;

	//_lastReservedTick = (uint32_t)(-1);

	int64_t round_slice = execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;

	NKCore::TWaitFreeQueue2<RealTimeJobSP>::iterator_type node = _shortterm_slot[round_slice].popQueue();
	NKCore::TWaitFreeQueue2<RealTimeJobSP>::iterator_type next_node = nullptr;
	
	/// execution
	while (node != nullptr)
	{
		// @nolimitk job�� �����߿� �ٽ� ����� �� �ֵ��� �ʱ�ȭ�� ���� �Ѵ�.
		next_node = node->getNext();
		
		//job->UnRegister();
		//job->ReleaseReserve();

		node->getValue()->onExecute(execution_index);
		NKENGINELOG_INFO(L"[SCHEDULER],%I64u,job executed,slot %I64u", execution_index, round_slice);

		node = next_node;
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
	NKENGINELOG_INFO(L"[SCHEDULER],slot executed, %I64u", scheduler_context._param);

	execute(scheduler_context._param);
		
	return true;
}