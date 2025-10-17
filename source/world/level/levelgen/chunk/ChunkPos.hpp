#pragma once

#ifndef __VEC3_HPP
class Vec3;
#endif
#ifndef __TILEPOS_HPP
struct TilePos;
#endif

#define __CHUNKPOS_HPP

struct ChunkPos
{
	int x, z;
    
private:
    void _init(int _x, int _z);
    void _init(const Vec3& pos);
	void _init(const TilePos& pos);
    
public:
	ChunkPos();
	ChunkPos(int _x, int _z);
	ChunkPos(int _x, int _y, int _z);
	ChunkPos(float _x, float _y, float _z);
	ChunkPos(const Vec3& pos) { _init(pos); }
	ChunkPos(const TilePos& pos) { _init(pos); }

	int lengthSqr() const
	{
		return x * x + z * z;
	}

	bool operator<(const ChunkPos& b) const;
	bool operator>(const ChunkPos& b) const;
	bool operator<=(const ChunkPos& b) const;
	bool operator>=(const ChunkPos& b) const;
	ChunkPos operator+(const ChunkPos& b) const;
	ChunkPos operator+(int i) const;
	ChunkPos operator-(const ChunkPos& b) const;
	ChunkPos operator-(int i) const;
	void operator+=(const ChunkPos& b);
	void operator-=(const ChunkPos& b);
	void operator*=(int i);
	ChunkPos operator-() const;
	ChunkPos operator*(int i) const;
	ChunkPos operator/(int i) const;
	bool operator==(const ChunkPos& other) const;
	bool operator!=(const ChunkPos& other) const;

	operator TilePos() const;
};
