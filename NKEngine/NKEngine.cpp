#include "NKEngine.h"
#include "NKEngineLog.h"

bool NKEngine::StartEngine(void)
{
	setlocale(LC_ALL, "korean");

	if (NKNetwork::IOCPManager::getInstance()->create() == false)
	{
		// @TODO 이렇게 되면 error를 두번 찍고 assert도 두번 걸린다. exception으로 처리하도록 변경하자
		NKENGINELOG_ERROR_ASSERT(L"NKNetwork failed to start.");
		return false;
	}

	class SchedulerCallback : public NKNetwork::WorkerCallback
	{
	public:
		virtual void onUpdate(int64_t delta) override
		{
			NKScheduler::_scheduler._in_workerthread = true;
			NKScheduler::_scheduler.execute();
		}
	};

	std::shared_ptr<SchedulerCallback> _callback = std::make_shared< SchedulerCallback>();
	NKNetwork::IOCPManager::getInstance()->registerWorkerEvent(_callback);

	/*if (NKScheduler::Scheduler::getInstance()->start() == false)
	{
		return false;
	}*/

	return true;
}

void NKEngine::ReleaseEngine(void)
{
	//NKScheduler::Scheduler::getInstance()->stop();
	NKNetwork::IOCPManager::getInstance()->close();

	//NKScheduler::Scheduler::getInstance()->destroy();
	NKEngineLogSingleton::destroy();
	NKLog::AsyncLogSingleton::destroy();
}