#pragma once

#include "CompoundTag.hpp"
#include "common/DataIO.hpp"

class NbtIo
{
public:
	static CompoundTag* read(IDataInput& dis);
	static void write(const CompoundTag& tag, IDataOutput& dos);
};