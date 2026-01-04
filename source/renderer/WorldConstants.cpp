#include "MatrixStack.hpp"
#include "GlobalConstantBufferManager.hpp"
#include "RenderContextImmediate.hpp"
#include "WorldConstants.hpp"

using namespace mce;

WorldConstants::WorldConstants()
{
    WORLDVIEWPROJ = nullptr;
    WORLD = nullptr;
    WORLDVIEW = nullptr;
    PROJ = nullptr;
}

void WorldConstants::refreshWorldConstants()
{
#ifdef FEATURE_GFX_SHADERS
    if (!MatrixStack::Projection.isDirty() &&
        !MatrixStack::View.isDirty() &&
        !MatrixStack::World.isDirty())
    {
        // Exit early if no matrices have been updated since the last frame.
        return;
    }

    // Get the current transformation matrices from the global stacks.
    const Matrix& projMatrix  = MatrixStack::Projection.top();
    const Matrix& viewMatrix  = MatrixStack::View.top();
    const Matrix& worldMatrix = MatrixStack::World.top();

    // Order matters!
    Matrix worldViewProjMatrix = projMatrix * viewMatrix * worldMatrix;
    Matrix worldViewMatrix = viewMatrix * worldMatrix;

    if (WORLDVIEWPROJ)
        WORLDVIEWPROJ->setData(&worldViewProjMatrix);

    if (WORLD)
        WORLD->setData(&worldMatrix);

    if (WORLDVIEW)
        WORLDVIEW->setData(&worldViewMatrix);

    if (PROJ)
        PROJ->setData(&projMatrix);

    // Mark the stacks as clean to prevent redundant calculations in subsequent calls.
    MatrixStack::Projection.makeClean();
    MatrixStack::View.makeClean();
    MatrixStack::World.makeClean();

    // Sync the updated constant buffer data to the GPU.
    sync();
#else
    RenderContext& renderContext = RenderContextImmediate::get();

    if (MatrixStack::Projection.isDirty())
    {
        const Matrix& projMatrix = MatrixStack::Projection.top();

        renderContext.loadMatrix(MATRIX_PROJECTION, projMatrix);

        MatrixStack::Projection.makeClean();
    }

    if (MatrixStack::World.isDirty() || MatrixStack::View.isDirty())
    {
        const Matrix& worldMatrix = MatrixStack::World.top();
        const Matrix& viewMatrix = MatrixStack::View.top();
        // Order matters!
        Matrix modelViewMatrix = viewMatrix * worldMatrix;

        renderContext.loadMatrix(MATRIX_VIEW, modelViewMatrix);

        MatrixStack::World.makeClean();
        MatrixStack::View.makeClean();
    }
#endif
}

void WorldConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("WorldConstants");

    ShaderConstantBase* pWorldViewProj = m_constantBuffer->getUnspecializedShaderConstant("WORLDVIEWPROJ");
    if (pWorldViewProj)
    {
        if (pWorldViewProj->getType() == SHADER_PRIMITIVE_MATRIX4x4)
        {
            WORLDVIEWPROJ = (ShaderConstantMatrix4x4*)pWorldViewProj;
        }
        else
        {
            WORLDVIEWPROJ = nullptr;
        }
    }

    ShaderConstantBase* pWorld = m_constantBuffer->getUnspecializedShaderConstant("WORLD");
    if (pWorld)
    {
        if (pWorld->getType() == SHADER_PRIMITIVE_MATRIX4x4)
        {
            WORLD = (ShaderConstantMatrix4x4*)pWorld;
        }
        else
        {
            WORLD = nullptr;
        }
    }


    ShaderConstantBase* pWorldView = m_constantBuffer->getUnspecializedShaderConstant("WORLDVIEW");
    if (pWorldView)
    {
        if (pWorldView->getType() == SHADER_PRIMITIVE_MATRIX4x4)
        {
            WORLDVIEW = (ShaderConstantMatrix4x4*)pWorldView;
        }
        else
        {
            WORLDVIEW = nullptr;
        }
    }

    ShaderConstantBase* pProj = m_constantBuffer->getUnspecializedShaderConstant("PROJ");
    if (pProj)
    {
        if (pProj->getType() == SHADER_PRIMITIVE_MATRIX4x4)
        {
            PROJ = (ShaderConstantMatrix4x4*)pProj;
        }
        else
        {
            PROJ = nullptr;
        }
    }
}