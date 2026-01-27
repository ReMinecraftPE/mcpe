#pragma once

#include <typeinfo>

#include "../API_D3D9.hpp"

#include "common/Logger.hpp"

namespace mce
{
	class ErrorHandlerD3D9
	{
	public:
		static inline void checkForErrors(HRESULT hResult)
		{
//#ifdef _DEBUG
			if (hResult != S_OK)
			{
				LOG_E("D3D9 Error: 0x%X", hResult); // doesn't work on newer compilers: DXGetErrorString(hResult)
				throw std::bad_cast();
			}
//#endif
		}
	};
}
