#pragma once

#include "StencilFaceDescription.hpp"
#include "renderer/StencilRefObject.hpp"
#include "enums/ComparisonFunc.hpp"
#include "enums/DepthWriteMask.hpp"

namespace mce
{
	struct DepthStencilStateDescription
	{
        bool depthTestEnabled;
        bool stencilTestEnabled;
        ComparisonFunc depthFunc;
        StencilFaceDescription frontFace;
        StencilFaceDescription backFace;
        DepthWriteMask depthWriteMask;
        unsigned int stencilReadMask;
        unsigned int stencilWriteMask;
        StencilRefObject stencilRef;
        bool overwroteStencilRef;
		
		DepthStencilStateDescription();

		bool operator==(const DepthStencilStateDescription& other) const;
		bool operator!=(const DepthStencilStateDescription& other) const;
	};
}
