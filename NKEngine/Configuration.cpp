#include "Configuration.h"
#include <string>
#include <fstream>

using namespace NKEngine;
using namespace std;

Configuration::Configuration(void)
	:_parse_errorcode(0)
{
}

Configuration::~Configuration(void)
{
}

bool Configuration::open(const NKString& filename)
{
	wifstream fp;
	std::wstring contents;
	
	fp.open(filename.c_str(), ios::in | ios::_Nocreate );

	if( fp.is_open() == false )
	{
		_parse_errorcode = errno;
		return false;
	}

	std::getline( fp, contents, (wchar_t)EOF);

	_doc.Parse<0>( contents.c_str() );
	if( _doc.HasParseError() == true )
	{
		_parse_errorcode = _doc.GetParseError();
		return false;
	}
	return true;
}