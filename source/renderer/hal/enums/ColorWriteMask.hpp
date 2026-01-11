#pragma once

namespace mce
{
	enum ColorWriteMask
	{
		COLOR_WRITE_MASK_NONE = 0x0,
		COLOR_WRITE_MASK_RED = (1<<0),
		COLOR_WRITE_MASK_GREEN = (1<<1),
		COLOR_WRITE_MASK_BLUE = (1<<2),
		COLOR_WRITE_MASK_ALPHA = (1<<3),
		COLOR_WRITE_MASK_ALL = 0xF
	};
}