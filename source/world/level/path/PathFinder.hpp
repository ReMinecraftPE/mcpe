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

class Level;
class Entity;

#define MAX_NODE_COUNT (2048)
#define FIELD_10038_SIZE (32)

typedef std::map <int, Node*> NodeMap;

class PathFinder
{
public:
	PathFinder();
	~PathFinder();

	int isFree(Entity*, int x, int y, int z, const Node* node);
	Node* getNode(Entity*, int x, int y, int z, const Node* node, int a);
	Node* getNode(int x, int y, int z);
	int getNeighbors(Entity*, Node*, const Node*, Node*, float);
	bool findPath(Path&, Entity*, Node*, Node*, const Node*, float);
	bool findPath(Path&, Entity*, float, float, float, float);

private:
	Node* new_Node(int x, int y, int z);
	bool inlined_0(Path& path, Node* node2);

private:
	Level* m_pLevel;
	BinaryHeap m_binaryHeap;
	NodeMap m_nodeMap;
	Node m_nodeReserve[MAX_NODE_COUNT];
	std::vector<Node*> m_nodeSpillover;
	int m_nodeCount;
	Node* field_10038[FIELD_10038_SIZE];
};
