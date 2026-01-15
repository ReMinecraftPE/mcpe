#include "ShaderConstantWithDataD3D9.hpp"

using namespace mce;

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_FLOAT1>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_FLOAT2>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_FLOAT3>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_FLOAT4>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_INT1>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantI(location, (const int*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantI(location, (const int*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_INT2>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantI(location, (const int*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantI(location, (const int*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_INT3>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantI(location, (const int*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantI(location, (const int*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_INT4>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantI(location, (const int*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantI(location, (const int*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_MATRIX2x2>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 1);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 1);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_MATRIX3x3>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 3);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 3);
		break;
	}
}

template <>
void ShaderConstantWithDataD3D9<SHADER_PRIMITIVE_MATRIX4x4>::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
	D3DDevice d3dDevice = context.getD3DDevice();
	switch (shaderType)
	{
	case SHADER_TYPE_VERTEX:
		d3dDevice->SetVertexShaderConstantF(location, (const float*)m_data, 4);
		break;
	case SHADER_TYPE_FRAGMENT:
		d3dDevice->SetPixelShaderConstantF(location, (const float*)m_data, 4);
		break;
	}
}