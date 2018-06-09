// @nolimitk
// 16.04.16
// core test

#include "NKEngine.h"
#include "NKUnittest.h"

NKTEST(Buffer_ByteStream_Test)
{
	NKCore::ByteStream stream(std::make_shared<NKCore::Buffer>(1024));
	_ASSERT(stream.getLength() == 0);
	_ASSERT(stream.write<int>(100) == true);
	struct Data
	{
		int _i;
		int64_t _i64;
	};
	Data d;
	d._i = 10;
	d._i64 = 1000000000;
	_ASSERT(stream.write((byte*)&d, sizeof(d)) == true);
	_ASSERT(stream.getLength() == sizeof(int) + sizeof(d));
	_ASSERT(stream.read<int>() == 100);
	Data* r = nullptr;
	_ASSERT(stream.read((byte**)&r, sizeof(r)) == true);
	_ASSERT(r != nullptr);
	_ASSERT(r->_i == 10);
	_ASSERT(r->_i64 == 1000000000);
	
	return true;
}

NKTEST(NKClock_Test)
{
	NKCore::NKClock clock;
	clock.sleepAccurate(1000);
	{
		int64_t sleep_tick = clock.getElapsedMicroSec();
		_TEST(sleep_tick > 0 && sleep_tick < 2000);
	}
	clock.sleepAccurate(20*1000);
	{
		int64_t sleep_tick = clock.getElapsedMicroSec();
		_TEST(sleep_tick > 20*1000 && sleep_tick < 22*1000);
	}

	return true;
}

NKTEST(Singleton_Test)
{
	class SingletonMock
	{
	public:
		int _v;

	public:
		SingletonMock() :_v(10) {}
	};
	SingletonMock* singleton = NKCore::Singleton<SingletonMock>::getInstance();
	_ASSERT(singleton->_v == 10);

	NKCore::Singleton<SingletonMock>::destroy();
	
	return true;
}

NKTEST(UniqueID_Test)
{
	NKCore::UniqueID uid;
	NKCore::UniqueID uid2;
	_ASSERT(uid2 == uid+1);

	std::vector<NKCore::UniqueID> uniqueid_vector;
	{
		const int thread_count = 10;
		NKUnitTest::thread_test(thread_count,
			[&uniqueid_vector]()
		{
			NKCore::UniqueID uid;
			uniqueid_vector.push_back(uid);
		}
		);

		_ASSERT(uniqueid_vector.size() == thread_count);

		for (int i = 0; i < thread_count; ++i)
		{
			for (int j = 0; j < thread_count; ++j)
			{
				if (i != j)
				{
					_ASSERT(uniqueid_vector[i] != uniqueid_vector[j]);
				}
			}
		}
	}

	NKCore::UniqueIDSetSafe uid_set;
	_ASSERT(uid_set.registerID(uid) == true);
	_ASSERT(uid_set.isRegisteredID(uid) == true);
	_ASSERT(uid_set.unregisterID(uid) == true);
	_ASSERT(uid_set.isRegisteredID(uid) == false);
	_ASSERT(uid_set.registerID(uid) == true);
	_ASSERT(uid_set.isRegisteredID(uid) == true);
	_ASSERT(uid_set.unregisterID(uid) == true);
	_ASSERT(uid_set.isRegisteredID(uid) == false);

	{
		const int thread_count = 10;
		NKUnitTest::thread_test(thread_count,
			[&uid, &uid_set]()
		{
			for (int i = 0; i < 100; ++i)
			{
				NKCore::UniqueID uid;
				_ASSERT(uid_set.registerID(uid) == true);
			}
		}
		);

		_ASSERT(uid_set.size() == 10 * 100);
	}

	return true;
}

NKTEST(NKString_Test)
{
	{
		NKString str = nullptr;
		NKString str2(nullptr);
		NKString str3("nolimitk");
	}

	{
		NKWString str = nullptr;
		NKWString str2(nullptr);
		NKWString str3(L"nolimitk");
	}

	{
		NKString source = "nolimitk";
		NKWString destnation = MultibyteToUnicode(source);
		_ASSERT(destnation.empty() == false);
	}
	
	return true;
}

NKTEST(NKThread_Test)
{
	class MockThread : public NKCore::NKThread
	{
	public:
		int _v;

		bool onRun(void) override
		{
			_v = 10;
			return true;
		}

		MockThread(void):_v(10){}
	};

	{
		MockThread t;
		_ASSERT(t.create() == true);
		_ASSERT(t.join() == true);
		_ASSERT(t._v == 10);
	}
	
	return true;
}