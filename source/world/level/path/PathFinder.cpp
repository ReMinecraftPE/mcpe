/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "PathFinder.hpp"
#include "world/level/Level.hpp"
#include "world/tile/DoorTile.hpp"
#include "world/entity/Entity.hpp"

static int dword_1CD868;
static int dword_1CD86C;
static int dword_1CD870;

constexpr int MakeNodeHash(int x, int y, int z)
{
	// NOTE: Same as in Java Edition Beta 1.3_01
	return (y & 0xFF) | 
		((x & 0x7FFF) << 8) | 
		((z & 0x7FFF) << 24) | 
		(x < 0 ? 0x80000000 : 0) | 
		(z < 0 ? 0x8000 : 0);
}

PathFinder::PathFinder()
{
	m_pLevel = nullptr;
}

PathFinder::~PathFinder()
{
	// NOTE: In v0.2.0, this is it. We're going to do more:
	for (size_t i = 0; i < m_nodeSpillover.size(); i++)
		delete m_nodeSpillover[i];

	m_nodeSpillover.clear();
}

int PathFinder::isFree(Entity* pEntity, int x, int y, int z, const Node* node)
{
	for (int x1 = x; x1 < x + node->m_x; x1++)
	{
		for (int y1 = y; y1 < y + node->m_y; y1++)
		{
			for (int z1 = z; z1 < z + node->m_z; z1++)
			{
				TileID id = m_pLevel->getTile(x1, y1, z1);
				if (id < 0)
					continue;

				if (id == Tile::door_iron->m_ID || id == Tile::door_wood->m_ID)
				{
					if (!DoorTile::isOpen(m_pLevel->getData(x1, y1, z1)))
						return 0;

					continue;
				}

				Material* pMtl = Tile::tiles[id]->m_pMaterial;
				if (pMtl->blocksMotion())
					return 0;

				if (pMtl == Material::water)
					return -1;
				if (pMtl == Material::lava)
					return -2;
			}
		}
	}

	return 1; // Totally free!
}

Node* PathFinder::getNode(Entity* pEntity, int x, int y, int z, const Node* node, int a)
{
	Node* pNode = nullptr;
	if (isFree(pEntity, x, y, z, node) == 1)
		pNode = getNode(x, y, z);

	if (a > 0 && !pNode && isFree(pEntity, x, y + a, z, node) == 1)
	{
		y += a;
		pNode = getNode(x, y, z);
	}

	if (!pNode || y < 0)
		return nullptr;

	int limit = y - 4;
	while (true)
	{
		int is_free = isFree(pEntity, x, --y, z, node);

		if (is_free != 1)
		{
			if (is_free == -2)
				pNode = nullptr;
			break;
		}

		if (y == limit)
		{
			pNode = nullptr;
			break;
		}

		if (!y)
			break;

		pNode = getNode(x, y, z);
	}

	return pNode;
}

Node* PathFinder::getNode(int x, int y, int z)
{
	NodeMap::iterator iter = m_nodeMap.find(MakeNodeHash(x, y, z));

	if (iter != m_nodeMap.end())
		return iter->second;

	Node* pNode = new_Node(x, y, z);
	dword_1CD868++;
	m_nodeMap.insert_or_assign(MakeNodeHash(x, y, z), pNode);

	return pNode;
}

int PathFinder::getNeighbors(Entity* pEntity, Node* node1, const Node* node2, Node* node3, float maxDist)
{
	int nr = 0;
	bool isf = isFree(pEntity, node1->m_x, node1->m_y, node1->m_z, node2) == 1;

	Node* n1 = getNode(pEntity, node1->m_x, node1->m_y, node1->m_z + 1, node2, isf);
	Node* n2 = getNode(pEntity, node1->m_x - 1, node1->m_y, node1->m_z, node2, isf);
	Node* n3 = getNode(pEntity, node1->m_x + 1, node1->m_y, node1->m_z, node2, isf);
	Node* n4 = getNode(pEntity, node1->m_x, node1->m_y, node1->m_z - 1, node2, isf);

	if (n1 && !n1->field_1A && n1->distanceTo(node3) < maxDist) field_10038[nr++] = n1;
	if (n2 && !n2->field_1A && n2->distanceTo(node3) < maxDist) field_10038[nr++] = n2;
	if (n3 && !n3->field_1A && n3->distanceTo(node3) < maxDist) field_10038[nr++] = n3;
	if (n4 && !n4->field_1A && n4->distanceTo(node3) < maxDist) field_10038[nr++] = n4;

	return nr;
}

bool PathFinder::inlined_0(Path& path, Node* nodeEnd)
{
	if (dword_1CD870 < dword_1CD868)
		dword_1CD870 = dword_1CD868;

	int number = 1;
	Node* temp = nodeEnd;
	while (temp->field_10)
	{
		temp = temp->field_10;
		number++;
	}

	Node** pathNodes = new Node*[number];
	int index = number - 1;

	pathNodes[index--] = nodeEnd;

	while (nodeEnd->field_10)
	{
		pathNodes[index--] = nodeEnd->field_10;
		nodeEnd = nodeEnd->field_10;
	}

	path.setNodes(pathNodes, number);
	return true;
}

bool PathFinder::findPath(Path& path, Entity* pEntity, Node* nodeStart, Node* nodeEnd, const Node* node3, float fp)
{
	dword_1CD868 = 0;

	nodeStart->field_4 = 0;
	nodeStart->field_C = nodeStart->field_8 = nodeStart->distanceTo(nodeEnd);

	m_binaryHeap.clear();
	m_binaryHeap.insert(nodeStart);

	Node* nodep = nodeStart;

	while (true)
	{
		if (!m_binaryHeap.size())
			break;

		Node* pNode = m_binaryHeap.removeTop();
		if (pNode->equals(nodeEnd))
			return inlined_0(path, nodeEnd);

		if (nodep->distanceTo(nodeEnd) > pNode->distanceTo(nodeEnd))
			nodep = pNode;

		pNode->field_1A = true;

		int numNeighbors = getNeighbors(pEntity, pNode, node3, nodeEnd, fp);
		for (int i = 0; i < numNeighbors; i++)
		{
			Node* otherNode = field_10038[i];

			if (!otherNode->field_1A)
			{
				float dist = pNode->field_4 + pNode->distanceTo(otherNode);
				if (otherNode->field_0 < 0 || otherNode->field_4 > dist)
				{
					otherNode->field_10 = pNode;
					otherNode->field_4 = dist;
					otherNode->field_8 = otherNode->distanceTo(nodeEnd);

					if (otherNode->field_0 < 0)
					{
						otherNode->field_C = otherNode->field_4 + otherNode->field_8;
						m_binaryHeap.insert(otherNode);
					}
					else
					{
						// Update distance
						m_binaryHeap.setDistance(otherNode, otherNode->field_4 + otherNode->field_8);
					}
				}
			}
		}
	}

	if (nodep != nodeStart)
		return inlined_0(path, nodeEnd);

	return false; // no path found
}

bool PathFinder::findPath(Path& path, Entity* pEntity, float x, float y, float z, float d)
{
	// uh?
	m_nodeMap.clear();

	m_nodeCount = 0;
	// not treating spillover btw? or what

	int x1 = Mth::floor(pEntity->m_hitbox.min.x);
	int y1 = Mth::floor(pEntity->m_hitbox.min.y);
	int z1 = Mth::floor(pEntity->m_hitbox.min.z);
	Node* node1 = getNode(x1, y1, z1);

	int x2 = Mth::floor(x - 0.5f * pEntity->field_88);
	int y2 = Mth::floor(y);
	int z2 = Mth::floor(z - 0.5f * pEntity->field_88);
	Node* node2 = nullptr;

	if (!m_pLevel->getTile(x2, y2 - 1, z2))
	{
		for (int x3 = x2; x3 <= Mth::floor(x + 0.5f * pEntity->field_88); x3++)
		{
			for (int z3 = z2; z3 <= Mth::floor(y + 0.5f * pEntity->field_88); z3++)
			{
				if (m_pLevel->getTile(x3, y2 - 1, z3))
				{
					node2 = getNode(x3, y2, z3);
					break; // breaking out of the z3 loop only.  Intended to break out of x3 too?
				}
			}
		}
	}
	
	if (!node2)
		node2 = getNode(x2, y2, z2);

	int x4 = Mth::floor(pEntity->field_88 + 1.0f);
	int y4 = Mth::floor(pEntity->field_8C + 1.0f);
	int z4 = Mth::floor(pEntity->field_88 + 1.0f);

	Node node3;
	node3.setPos(x4, y4, z4);
	node3.setHash(MakeNodeHash(x4, y4, z4));

	bool foundPath = findPath(path, pEntity, node1, node2, &node3, d);

	if (m_nodeCount > 2048)
	{
		// huh.
		for (size_t i = 0; i < m_nodeSpillover.size(); i++)
			delete m_nodeSpillover[i];

		m_nodeSpillover.clear();
	}

	return foundPath;
}

Node* PathFinder::new_Node(int x, int y, int z)
{
	int nodeID = m_nodeCount++;

	Node* pNode;

	if (m_nodeCount < MAX_NODE_COUNT)
	{
		// Allocate from node reserve
		pNode = &m_nodeReserve[nodeID];
		pNode->init();
		pNode->setPos(x, y, z);
		pNode->setHash(MakeNodeHash(x, y, z));
	}
	else
	{
		pNode = new Node;
		pNode->setPos(x, y, z);
		pNode->setHash(MakeNodeHash(x, y, z));
		m_nodeSpillover.push_back(pNode);
	}

	return pNode;
}
