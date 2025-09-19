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

constexpr int MakeNodeHash(const TilePos& pos)
{
	// NOTE: Same as in Java Edition Beta 1.3_01
	return (pos.y & 0xFF) | 
		((pos.x & 0x7FFF) << 8) |
		((pos.z & 0x7FFF) << 24) |
		(pos.x < 0 ? 0x80000000 : 0) |
		(pos.z < 0 ? 0x8000 : 0);
}

PathFinder::PathFinder()
{
	m_pLevel = nullptr;
	m_nodeCount = 0;
	m_bEntityIsDoorBreaker = false;
}

PathFinder::~PathFinder()
{
	// NOTE: In v0.2.0, this is it. We're going to do more:
	for (size_t i = 0; i < m_nodeSpillover.size(); i++)
		delete m_nodeSpillover[i];

	m_nodeSpillover.clear();
}

int PathFinder::isFree(Entity* pEntity, const TilePos& pos, const Node* node)
{
	TilePos tp(pos);

	for (tp.x = pos.x; tp.x < pos.x + node->m_tilePos.x; tp.x++)
	{
		for (tp.y = pos.y; tp.y < pos.y + node->m_tilePos.y; tp.y++)
		{
			for (tp.z = pos.z; tp.z < pos.z + node->m_tilePos.z; tp.z++)
			{
				TileID id = m_pLevel->getTile(tp);
				if (id <= 0)
					continue;

				if (id == Tile::door_iron->m_ID || id == Tile::door_wood->m_ID)
				{
					if (id != Tile::door_wood->m_ID || !m_bEntityIsDoorBreaker)
					{
						if (!DoorTile::isOpen(m_pLevel->getData(tp)))
							return 0; // 4 on 0.2.1
					}

					continue;
				}

				// 0.2.1
				/*if (id == Tile::water->m_ID || id == Tile::calmWater->m_ID)
				{
					if (field_100B9) // offset from 0.7.0
						return 3;
				}
				else */if (id == Tile::fence->m_ID/* || id == Tile::fenceGate->m_ID*/)
				{
					return 0; // 1 on 0.2.1
				}

				Material* pMtl = Tile::tiles[id]->m_pMaterial;
				if (pMtl->blocksMotion())
					return 0; // 4 on 0.2.1

				if (pMtl == Material::water)
					return -1;
				if (pMtl == Material::lava)
					return -2; // 2 on 0.2.1
			}
		}
	}

	return 1; // Totally free! (5 on 0.2.1)
}

Node* PathFinder::getNode(Entity* pEntity, const TilePos& pos, const Node* node, int a)
{
	TilePos tp(pos);
	Node* pNode = nullptr;
	if (isFree(pEntity, tp, node) == 1)
		pNode = getNode(tp);

	if (!pNode && a > 0 && isFree(pEntity, TilePos(tp.x, tp.y + a, tp.z), node) == 1)
	{
		tp.y += a;
		pNode = getNode(tp);
	}

	if (!pNode || tp.y < 0)
		return nullptr;

	int limit = tp.y - 4;
	while (true)
	{
		int is_free = isFree(pEntity, TilePos(tp.x, --tp.y, tp.z), node);

		if (is_free != 1)
		{
			if (is_free == -2)
				pNode = nullptr;
			break;
		}

		if (tp.y == limit)
		{
			pNode = nullptr;
			break;
		}

		if (!tp.y)
			break;

		pNode = getNode(tp);
	}

	return pNode;
}

Node* PathFinder::getNode(const TilePos& pos)
{
	NodeMap::iterator iter = m_nodeMap.find(MakeNodeHash(pos));

	if (iter != m_nodeMap.end())
		return iter->second;

	Node* pNode = new_Node(pos);
	dword_1CD868++;
	m_nodeMap[MakeNodeHash(pos)] = pNode;

	return pNode;
}

int PathFinder::getNeighbors(Entity* pEntity, Node* node1, const Node* node2, Node* node3, float maxDist)
{
	int nr = 0;
	bool isf = isFree(pEntity, node1->m_tilePos.above(), node2) == 1; // this is > 0 on b1.2_02, but == 1 on 0.12.1

	Node* n1 = getNode(pEntity, node1->m_tilePos.south(), node2, isf);
	Node* n2 = getNode(pEntity, node1->m_tilePos.west(),  node2, isf);
	Node* n3 = getNode(pEntity, node1->m_tilePos.east(),  node2, isf);
	Node* n4 = getNode(pEntity, node1->m_tilePos.north(), node2, isf);

	if (n1 && !n1->m_bClosed && n1->distanceTo(node3) < maxDist) m_neighbors[nr++] = n1;
	if (n2 && !n2->m_bClosed && n2->distanceTo(node3) < maxDist) m_neighbors[nr++] = n2;
	if (n3 && !n3->m_bClosed && n3->distanceTo(node3) < maxDist) m_neighbors[nr++] = n3;
	if (n4 && !n4->m_bClosed && n4->distanceTo(node3) < maxDist) m_neighbors[nr++] = n4;

	return nr;
}

bool PathFinder::reconstructPath(Path& path, Node* nodeEnd)
{
	if (dword_1CD870 < dword_1CD868)
		dword_1CD870 = dword_1CD868;

	int number = 1;
	Node* temp = nodeEnd;
	while (temp->m_pCameFrom)
	{
		temp = temp->m_pCameFrom;
		number++;
	}

	Node** pathNodes = new Node*[number];
	int index = number - 1;

	pathNodes[index--] = nodeEnd;

	while (nodeEnd->m_pCameFrom)
	{
		pathNodes[index--] = nodeEnd->m_pCameFrom;
		nodeEnd = nodeEnd->m_pCameFrom;
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

	while (m_binaryHeap.size() > 0)
	{
		Node* pNode = m_binaryHeap.removeTop();
		if (pNode->equals(nodeEnd))
			return reconstructPath(path, nodeEnd);

		if (nodep->distanceTo(nodeEnd) > pNode->distanceTo(nodeEnd))
			nodep = pNode;

		pNode->m_bClosed = true;

		int numNeighbors = getNeighbors(pEntity, pNode, node3, nodeEnd, fp);
		for (int i = 0; i < numNeighbors; i++)
		{
			Node* otherNode = m_neighbors[i];

			if (!otherNode->m_bClosed)
			{
				float dist = pNode->field_4 + pNode->distanceTo(otherNode);
				if (otherNode->field_0 < 0 || otherNode->field_4 > dist)
				{
					otherNode->m_pCameFrom = pNode;
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
		return reconstructPath(path, nodeEnd);

	return false; // no path found
}

bool PathFinder::findPath(Path& path, Entity* pEntity, const Vec3& pos, float d)
{
	// uh?
	m_nodeMap.clear();

	m_nodeCount = 0;
	// not treating spillover btw? or what

	Node* node1 = getNode(pEntity->m_hitbox.min);

	TilePos tp(pos.x - (pEntity->m_bbWidth / 2),
		       pos.y,
		       pos.z - (pEntity->m_bbWidth / 2));
	Node* node2 = nullptr;

	// Not present in b1.2_02
	if (!m_pLevel->getTile(tp.below()))
	{
		TilePos tp2(tp), tp3(pos + (0.5f * pEntity->m_bbWidth));
		for (tp2.x = tp.x; tp2.x <= tp3.x; tp2.x++)
		{
			for (tp2.z = tp.z; tp2.z <= tp3.y; tp2.z++)
			{
				if (m_pLevel->getTile(TilePos(tp2.x, tp2.y - 1, tp2.z)))
				{
					node2 = getNode(tp2);
					break; // breaking out of the tp2.z loop only.  Intended to break out of tp2.x too?
				}
			}
		}
	}
	
	if (!node2)
		node2 = getNode(tp);

	tp = TilePos(pEntity->m_bbWidth, pEntity->m_bbHeight, pEntity->m_bbWidth) + 1;

	Node node3;
	node3.setPos(tp);
	node3.setHash(MakeNodeHash(tp));

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

bool PathFinder::findPath(Path& path, Entity* pEntity, const Entity* pTarget, float d)
{
	return findPath(path, pEntity, Vec3(pTarget->m_pos.x, pTarget->m_hitbox.min.y/*bb.y0*/, pTarget->m_pos.z), d);
}

bool PathFinder::findPath(Path& path, Entity* pEntity, const TilePos& tilePos, float d)
{
	Vec3 pos(tilePos);
	pos.x += 0.5f;
	pos.y += 0.01f; // was 0.5f on b1.2_02, 0.01f on 0.12.1
	pos.z += 0.5f;
	return findPath(path, pEntity, pos, d);
}

Node* PathFinder::new_Node(const TilePos& pos)
{
	int nodeID = m_nodeCount++;

	Node* pNode;

	if (m_nodeCount < MAX_NODE_COUNT)
	{
		// Allocate from node reserve
		pNode = &m_nodeReserve[nodeID];
		pNode->init();
		pNode->setPos(pos);
		pNode->setHash(MakeNodeHash(pos));
	}
	else
	{
		pNode = new Node;
		pNode->setPos(pos);
		pNode->setHash(MakeNodeHash(pos));
		m_nodeSpillover.push_back(pNode);
	}

	return pNode;
}
