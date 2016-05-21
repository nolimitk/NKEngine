#ifndef __NKTHREAD_HEADER__
#define __NKTHREAD_HEADER__
// @nolimitk
// 16.05.11
// thread

#include <thread>

namespace NKCore
{
	class NKThread
	{
	public:
		bool create(void);
		bool join(void);
		// return false if to join is timeout
		bool join(std::chrono::milliseconds tick);

	public:
		std::thread::id getThreadID(void) const { return _id;  }

		// event
	public:
		virtual bool onStart(void) { return true; }
		virtual bool onRun(void) = 0;
		virtual bool onEnd(void) { return true; }
		///

	protected:
		static bool run(void *pParam);
		
	protected:
		std::thread _t;
		std::thread::id _id;
		volatile bool _run;

	public:
		NKThread(void);
		virtual ~NKThread(void);
	};
}

#endif // __NKTHREAD_HEADER__