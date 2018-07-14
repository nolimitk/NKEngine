#include "Service.h"
#include "AsyncServerSocket.h"
#include "AsyncSocket.h"
#include "../NKEngineLog.h"

bool NKNetwork::Service::start(void)
{
	std::shared_ptr<ServiceServerCallback> server_callback = std::make_shared<ServiceServerCallback>(*this);
	std::shared_ptr<ServiceClientCallback> client_callback = std::make_shared<ServiceClientCallback>(*this);
	_server_socket = std::make_shared<NKNetwork::AsyncServerSocket>(server_callback, client_callback);

	if (_server_socket->open(_port) == false)
	{
		return false;
	}

	NKENGINELOG_INFO(L"service started. %d", _port);
	return true;
}

void NKNetwork::Service::close(void)
{
	_server_socket->close();
}

bool NKNetwork::Service::insertConnection(const ConnectionSP& connection)
{
	std::lock_guard<std::mutex> _lock(__mutex_connection_map);
	auto ret = _connection_map.insert(make_pair(connection->getID(), connection));
	return ret.second;
}

bool NKNetwork::Service::eraseConnection(const NKCore::UniqueID& id)
{
	std::lock_guard<std::mutex> _lock(__mutex_connection_map);
	auto count_erased = _connection_map.erase(id);
	return (count_erased == 1);
}

size_t NKNetwork::Service::getConnectionCount(void) const
{
	std::lock_guard<std::mutex> _lock(__mutex_connection_map);
	return _connection_map.size();
}

NKNetwork::ServiceServerCallback::ServiceServerCallback(Service& service)
	:_service(service)
{
}

void NKNetwork::ServiceServerCallback::onAccepted(const ConnectionSP& connection)
{
	if (_service.insertConnection(connection) == true)
	{
		NKENGINELOG_INFO(L"connection accepted, %I64u", connection->getID());
	}
}

void NKNetwork::ServiceClientCallback::onClosed(const ConnectionSP & connection)
{
	if (_service.eraseConnection(connection->getID()) == true)
	{
		NKENGINELOG_INFO(L"connection closed, %I64u", connection->getID());
	}
}

NKNetwork::ServiceClientCallback::ServiceClientCallback(Service& service)
	:_service(service)
{
}