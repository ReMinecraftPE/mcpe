#include <cstring>

#include "API_D3D11.hpp"
#include "API_D3D11Compiler.hpp"

#include "ShaderProgramD3D11.hpp"

#include "common/Util.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

ShaderProgramD3D11::ShaderProgramD3D11(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramBase(header, shaderPath, shaderType)
{
    m_shaderSource = shaderSource;
}

ShaderProgramD3D11::~ShaderProgramD3D11()
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

void _getShaderModelVersion(D3D_FEATURE_LEVEL featureLevel, int& major, int& minor)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-downlevel-intro
    switch (featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1: // 5.0
    case D3D_FEATURE_LEVEL_11_0: // 5.0
        major = 5;
        minor = 0;
        break;

    case D3D_FEATURE_LEVEL_10_1: // 4.x
        major = 4;
        minor = 1;
        break;

    case D3D_FEATURE_LEVEL_10_0: // 4.0
    case D3D_FEATURE_LEVEL_9_3:  // 4_0_level_9_3
    case D3D_FEATURE_LEVEL_9_2:  // 4_0_level_9_1
    case D3D_FEATURE_LEVEL_9_1:  // 4_0_level_9_1
        major = 4;
        minor = 0;
        break;

    default:
        LOG_W("Unsupported D3D feature level: %d", featureLevel);
        break;
    }
}

std::string _getShaderModelVersionSuffix(D3D_FEATURE_LEVEL featureLevel)
{
    switch (featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    case D3D_FEATURE_LEVEL_10_1:
    case D3D_FEATURE_LEVEL_10_0:
        return "";

    case D3D_FEATURE_LEVEL_9_3:
        return "_level_9_3";

    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
        return "_level_9_1";

    default:
        return "";
    }
}

std::string _getShaderTarget(ShaderType shaderType, D3D_FEATURE_LEVEL featureLevel)
{
    int shaderModelMajor = -1;
    int shaderModelMinor = -1;
    _getShaderModelVersion(featureLevel, shaderModelMajor, shaderModelMinor);

    std::string shaderModelSuffix = _getShaderModelVersionSuffix(featureLevel);
    std::string shaderModelType = _getShaderModelType(shaderType);

    return shaderModelType + "_" + Util::toString(shaderModelMajor) + "_" + Util::toString(shaderModelMinor) + shaderModelSuffix;
}

void ShaderProgramD3D11::compileShaderProgram()
{
    if (!m_shaderBytecode.empty())
        return;

    if (m_shaderSource.empty())
    {
        m_bValid = false;
        return;
    }

    RenderContext& renderContext = RenderContextImmediate::get();
    std::string shaderTarget = _getShaderTarget(m_shaderType, renderContext.m_featureLevel);
    std::string shaderSourceName = "assets/" + m_shaderPath;

    HRESULT hResult;
    ComInterface<ID3DBlob> code, errorMsgs;
    hResult = D3DCompile(
        m_shaderSource.c_str(), m_shaderSource.size(),
        shaderSourceName.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        shaderTarget.c_str(),
        D3DCOMPILE_OPTIMIZATION_LEVEL3, 0x0,
        *code, *errorMsgs
    );
    if (hResult != S_OK)
    {
        LPVOID errors = errorMsgs->GetBufferPointer();
        LOG_E("Error compiling shader: %s", errors);
        m_bValid = false;
        return;
    }

    D3DDevice d3dDevice = renderContext.getD3DDevice();
    bool bSuccess = false;

    switch (m_shaderType)
    {
    case SHADER_TYPE_VERTEX:
        m_vertexShader.release();
        hResult = d3dDevice->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, *m_vertexShader);
        ErrorHandlerDXGI::checkForErrors(hResult);
        bSuccess = m_vertexShader;
        break;
    case SHADER_TYPE_FRAGMENT:
        m_pixelShader.release();
        hResult = d3dDevice->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, *m_pixelShader);
        ErrorHandlerDXGI::checkForErrors(hResult);
        bSuccess = m_pixelShader;
        break;
    case SHADER_TYPE_GEOMETRY:
        m_geometryShader.release();
        hResult = d3dDevice->CreateGeometryShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, *m_geometryShader);
        ErrorHandlerDXGI::checkForErrors(hResult);
        bSuccess = m_geometryShader;
        break;
    default:
        LOG_W("Unknown shaderType: %d", m_shaderType);
        break;
    }

    m_shaderBytecode = std::string((const char*)code->GetBufferPointer(), code->GetBufferSize());
    m_bValid = bSuccess;
}
