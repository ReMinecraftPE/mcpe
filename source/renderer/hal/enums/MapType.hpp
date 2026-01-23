#pragma once

namespace mce
{
	enum MapType
	{
		MAP_READ,
		MAP_WRITE,
		MAP_READ_WRITE,
		MAP_WRITE_DISCARD,

		// The application promises not to overwrite any data that will be read by pending draw calls.
		// Specifying this flag allows the driver to return immediately and continue rendering, using this buffer.
		// If this flag is not used, the driver must finish rendering before returning from locking.
		MAP_WRITE_NO_OVERWRITE,

		MAP_UNKNOWN,
	};
}
