#pragma once
#ifndef __CONNECTION_HEADER__
#define __CONNECTION_HEADER__
// @nolimitk
// 13.09.12
// connection

#include <memory>
#include "AsyncSocket.h"
#include "NetworkEvent.h"

namespace NKNetwork
{
	class Session;
	class Service;

	class Connection : public AsyncSocket
	{
	protected:
		AsyncSocket::open;
		AsyncSocket::connect;
		//AsyncSocket::getHandle;
		AsyncSocket::setAddress;
		
	public:
		bool connect(const HANDLE completion_port, const NKWString& address, uint16_t port);
		bool connect(const HANDLE completion_port, const NKString& address, uint16_t port);
		bool reconnect(void);

	public:
		inline NKCore::UniqueID getID(void) const { return _id; }
		/*inline std::shared_ptr<NetworkEvent> getNetworkEvent(void) const { return _network_event; }
		inline void setNetworkEvent(const std::shared_ptr<NetworkEvent>& network_event) { _network_event = network_event; }*/
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

	protected:
		inline void setPongTime(void) { _last_pongtime = GetTickCount64(); _sent_ping = false; }

		// network event
	public:
		virtual bool onClosed(void) override;
		virtual bool onConnected(void) override;
		virtual bool onConnectFailed(void) override;
		virtual bool onReceived(const Packet& packet) override;
		virtual bool onSent(void) override;

	protected:
		NKCore::UniqueID _id;

		std::unique_ptr<NetworkEvent> _network_event;
		//Service *_pService;

		// ��ӹ��� class���� ���� ������ �� �� ������ ���´�. [2014/11/11/ nolimitk]
		// �ݵ�� bind, unbind �Լ��� ���� �����ؾ� �Ѵ�.
	private:
		//Session *_pSession;

		// feature : heartbeat
		// possible to take on/off
	protected:
		bool _heart_beat;
		ULONGLONG _last_pongtime;
		bool _sent_ping;

		// for reconnect
	protected:
		NKString _address;
		uint16_t _port;
		bool _reconnect;
		
	public:
		Connection(std::unique_ptr<NetworkEvent>&& network_event, bool reconnect = false);
		virtual ~Connection(void);
		
	private:
		void* operator new(size_t size) {}
	};
}

#endif // __CONNECTION_HEADER__