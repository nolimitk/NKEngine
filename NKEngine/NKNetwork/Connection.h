#pragma once
#ifndef __CONNECTION_HEADER__
#define __CONNECTION_HEADER__
// @nolimitk
// 18.06.05
// connection

namespace NKNetwork
{
	//class Session;
	//class Service;

	class Connection
	{		
	public:
		//bool connect(const HANDLE completion_port, const NKWString& address, uint16_t port);
		//bool connect(const HANDLE completion_port, const NKString& address, uint16_t port);
		//bool reconnect(void);

	public:
		/*inline void set(Service *pService, const std::shared_ptr<NetworkEvent>& network_event)
		{
			_pService = pService; _network_event = network_event;
		}
		inline Service *getService(void) const { return _pService; }*/

		// session
	/*public:
		inline bool bind(Session *pSession) { return BindPointer<Session>(pSession, &_pSession); }
		inline bool Unbind(void) { return UnbindPointer<Session>(&_pSession); }
		inline SmartPointer<Session> GetSession(void) { return _pSession; }*/

		// heartbeat
	/*public:
		inline void SetHeartBeat(bool flag) { _bHeartBeat = flag; if (flag == true) { SetPongTime(); } }
		inline bool GetHeartBeat(void) { return _bHeartBeat; }
		inline void SetPongTime(void) { _lastPongTime = GetTickCount64(); _bSentPing = false; }
		inline ULONGLONG GetPongTime(void) { return _lastPongTime; }
		inline void SetSentPing(void) { _bSentPing = true; }
		inline bool GetSentPing(void) { return _bSentPing; }*/

		/*
	protected:
		inline void setPongTime(void) { _last_pongtime = GetTickCount64(); _sent_ping = false; }
		*/

	protected:
		//Service *_pService;

		// 상속받은 class에서 직접 접근을 할 수 없도록 막는다. [2014/11/11/ nolimitk]
		// 반드시 bind, unbind 함수를 통해 접근해야 한다.
	private:
		//Session *_pSession;

		// feature : heartbeat
		// possible to take on/off
		/*
	protected:
		bool _heart_beat;
		ULONGLONG _last_pongtime;
		bool _sent_ping;

		// for reconnect
	protected:
		NKString _address;
		uint16_t _port;
		bool _reconnect;
		*/
		
	public:
		Connection(void);
		virtual ~Connection(void);
	};
}

#endif // __CONNECTION_HEADER__