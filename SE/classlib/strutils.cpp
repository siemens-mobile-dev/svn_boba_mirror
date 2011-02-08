#include "strutils.hpp"
#include <vector>

const string tostring( STRID strid )
{
	vector<char> tmp;
	tmp.resize( TextGetLength( strid ) + 13 );
	StrID2Str( strid, &tmp[0], tmp.size() );
	return &tmp[0];
}

STRID tostrid( const string& str )
{
	return Str2ID( str.c_str(), 6, str.size() );
}
