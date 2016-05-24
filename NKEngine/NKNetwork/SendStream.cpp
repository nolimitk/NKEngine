#include "SendStream.h"

using namespace NKNetwork;

SendStream::SendStream(const std::shared_ptr<NKCore::Buffer>& buffer)
	:ByteStream(buffer)
{
}

SendStream::~SendStream(void)
{
}