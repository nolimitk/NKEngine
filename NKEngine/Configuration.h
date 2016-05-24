#ifndef __CONFIGURATION_HEADER__
#define __CONFIGURATION_HEADER__
// @nolimitk
// 13.11.22
// configuration by json

#include "../external/rapidjson/inc/document.h"
#include "NKCore.h"

namespace NKEngine
{
	typedef rapidjson::GenericDocument<rapidjson::UTF16<> > WDocument;
	typedef rapidjson::GenericValue<rapidjson::UTF16<> > WValue;

	// JSON loader
	class Configuration
	{
	public:
		bool open(const NKString& filename);
		inline int getParseErrorString(void) const { return _parse_errorcode; }
		inline const WDocument &GetDoc(void) { return _doc; }

	protected:
		WDocument _doc;
		int _parse_errorcode;

	public:
		Configuration(void);
		virtual ~Configuration(void);
	};
}

#endif // __CONFIGURATION_HEADER__