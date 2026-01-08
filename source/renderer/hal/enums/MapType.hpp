#pragma once

namespace mce
{
	enum MapType
	{
		MAP_READ,
		MAP_WRITE,
		MAP_READ_WRITE,
		MAP_WRITE_DISCARD,
		MAP_WRITE_NO_OVERWRITE,
		MAP_UNKNOWN,
	};
}