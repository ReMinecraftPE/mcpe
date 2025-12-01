#pragma once

#include "EntityRenderer.hpp"
#include "client/app/AppPlatformListener.hpp"
#include "renderer/Mesh.hpp"

class ArrowRenderer : public EntityRenderer, public AppPlatformListener
{
public:
	ArrowRenderer();
	~ArrowRenderer();

private:
	void _buildMesh();

public:
	void onAppSuspended() override;
	void render(Entity* ent, const Vec3& pos, float rot, float a) override;

private:
	mce::Mesh m_mesh;
};
