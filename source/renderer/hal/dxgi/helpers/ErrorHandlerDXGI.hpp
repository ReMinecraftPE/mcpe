#pragma once

#include "../API_DXGI.hpp"

namespace mce
{
	class ErrorHandlerDXGI
	{
	public:
		static void checkForErrors(HRESULT hResult);
	};
}
