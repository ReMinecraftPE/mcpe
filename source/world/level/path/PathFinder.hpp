/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <vector>
#include <map>
#include "Path.hpp"
#include "BinaryHeap.hpp"
#include "world/level/TilePos.hpp"

class LevelSource;
class Entity;

#define MAX_NODE_COUNT (2048)
#define NEIGHBORS_SIZE (32)

typedef std::map <int, Node*> NodeMap;

class PathFinder
{
public:
	PathFinder();
	~PathFinder();

	int isFree(Entity*, const TilePos& pos, const Node* node);
	Node* getNode(Entity*, const TilePos& pos, const Node* node, int a);
	Node* getNode(const TilePos& pos);
	int getNeighbors(Entity*, Node*, const Node*, Node*, float);
	bool findPath(Path&, Entity*, Node*, Node*, const Node*, float);
	bool findPath(Path&, Entity*, const Vec3& pos, float);
	bool findPath(Path&, Entity*, const Entity*, float);
	bool findPath(Path&, Entity*, const TilePos& tilePos, float);

	void setLevel(LevelSource* pLevel)
	{
		m_pLevel = pLevel;
	}

private:
	Node* new_Node(const TilePos& pos);
	bool reconstructPath(Path& path, Node* node2);

private:
	LevelSource* m_pLevel;
	BinaryHeap m_binaryHeap;
	NodeMap m_nodeMap;
	Node m_nodeReserve[MAX_NODE_COUNT];
	std::vector<Node*> m_nodeSpillover;
	int m_nodeCount;
	Node* m_neighbors[NEIGHBORS_SIZE];
	bool m_bEntityIsDoorBreaker;
};
