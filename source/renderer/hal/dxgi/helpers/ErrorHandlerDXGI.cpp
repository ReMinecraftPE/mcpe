#include <typeinfo>
#include "ErrorHandlerDXGI.hpp"
#include "common/Logger.hpp"

using namespace mce;

DWORD HRESULTToGfxError(HRESULT hResult)
{
    if (hResult == S_OK)
        return 0;

    switch (hResult)
    {
    case DXGI_ERROR_INVALID_CALL:          return 5;
    case DXGI_ERROR_NOT_FOUND:             return 7;
    case DXGI_ERROR_MORE_DATA:             return 6;
    case DXGI_ERROR_UNSUPPORTED:           return 8;
    case DXGI_ERROR_DEVICE_REMOVED:        return 2;
    case DXGI_ERROR_DEVICE_HUNG:           return 1;
    case DXGI_ERROR_DEVICE_RESET:          return 3;
    case DXGI_ERROR_WAS_STILL_DRAWING:     return 10;
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return 4;
    case DXGI_ERROR_ACCESS_LOST:           return 9;
    default:
        LOG_E("Unknown hResult: 0x%X", hResult);
        throw std::bad_cast();
    }

    return 0;
}

void ErrorHandlerDXGI::checkForErrors(HRESULT hResult)
{
#ifdef _DEBUG
	if (hResult != S_OK)
	{
        CHAR* lpMsgBuf = new CHAR[0x1000];
        lpMsgBuf[0] = '\0';
        DWORD tChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpMsgBuf, 0x1000, NULL);
        LOG_E("DXGI Error: %s", lpMsgBuf);
        delete[] lpMsgBuf;

        throw std::bad_cast();
	}
#endif
}
