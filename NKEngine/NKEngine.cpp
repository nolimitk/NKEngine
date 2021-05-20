#include "NKEngine.h"
#include "NKEngineLog.h"

bool NKEngine::StartEngine(void)
{
	setlocale(LC_ALL, "korean");

	if (NKNetwork::IOCPManager::getInstance()->create() == false)
	{
		// @TODO �̷��� �Ǹ� error�� �ι� ��� assert�� �ι� �ɸ���. exception���� ó���ϵ��� ��������
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