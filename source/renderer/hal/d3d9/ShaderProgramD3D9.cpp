#include <cstring>

#include "API_D3D9.hpp"
#include "API_D3D9Compiler.hpp"

#include "ShaderProgramD3D9.hpp"

#include "common/Util.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

ShaderProgramD3D9::ShaderProgramD3D9(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramBase(header, shaderPath, shaderType)
{
    m_shaderSource = shaderSource;
}

ShaderProgramD3D9::~ShaderProgramD3D9()
{
}

std::string _getShaderModelType(ShaderType shaderType)
{
    switch (shaderType)
    {
    case SHADER_TYPE_VERTEX:   return "vs";
    case SHADER_TYPE_FRAGMENT: return "ps";
    case SHADER_TYPE_GEOMETRY: return "gs";

    default:
        LOG_E("Unknown shader type: %d", shaderType);
        throw std::bad_cast();
    }
}

std::string _getShaderTarget(ShaderType shaderType)
{
    std::string shaderModelType = _getShaderModelType(shaderType);

    return shaderModelType + "_3_0";
}

void _translateShaderSource(std::string& source)
{
    // Dirty hack, since D3D9 doesn't support the lack of an underscore
    Util::stringReplace(source, "TEXCOORD_", "TEXCOORD");
}

void ShaderProgramD3D9::compileShaderProgram()
{
    if (!m_shaderBytecode.empty())
        return;

    if (m_shaderSource.empty())
    {
        m_bValid = false;
        return;
    }

    _translateShaderSource(m_shaderSource);

    RenderContext& renderContext = RenderContextImmediate::get();
    std::string shaderTarget = _getShaderTarget(m_shaderType);

    HRESULT hResult;
    ComInterface<ID3DXBuffer> code, errorMsgs;
    hResult = D3DXCompileShader(
        m_shaderSource.c_str(), m_shaderSource.size(),
        NULL,
        NULL,
        "main",
        shaderTarget.c_str(),
        0x0,
        *code, *errorMsgs,
        NULL);
    if (hResult != S_OK)
    {
        LPVOID errors = errorMsgs->GetBufferPointer();
        LOG_E("Error compiling shader \"%s\":", m_shaderPath.c_str());
		LOG_E((const char*)errors);
        m_bValid = false;
        return;
    }

    D3DDevice d3dDevice = renderContext.getD3DDevice();
    bool bSuccess = false;

    switch (m_shaderType)
    {
    case SHADER_TYPE_VERTEX:
        m_vertexShader.release();
        hResult = d3dDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), *m_vertexShader);
        ErrorHandlerD3D9::checkForErrors(hResult);
        bSuccess = m_vertexShader;
        break;
    case SHADER_TYPE_FRAGMENT:
        m_pixelShader.release();
        hResult = d3dDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), *m_pixelShader);
        ErrorHandlerD3D9::checkForErrors(hResult);
        bSuccess = m_pixelShader;
        break;
    case SHADER_TYPE_GEOMETRY:
        LOG_W("Geometry shaders are not supported on D3D9!");
        break;
    default:
        LOG_W("Unknown shaderType: %d", m_shaderType);
        break;
    }

    m_shaderBytecode = std::string((const char*)code->GetBufferPointer(), code->GetBufferSize());
    m_bValid = bSuccess;
}
