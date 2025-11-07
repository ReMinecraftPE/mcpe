#pragma once

#include "enums/CullMode.hpp"

namespace mce
{
	struct RasterizerStateDescription
	{
        float depthBias;
        CullMode cullMode;
        bool enableScissorTest;
	
		RasterizerStateDescription();

		bool operator==(const RasterizerStateDescription& other) const;
		bool operator!=(const RasterizerStateDescription& other) const;
	};
}
