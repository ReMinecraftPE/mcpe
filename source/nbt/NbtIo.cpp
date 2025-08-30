#include "NbtIo.hpp"

CompoundTag* NbtIo::read(IDataInput& dis)
{
    std::string tagName = std::string();

    return (CompoundTag*)Tag::readNamedTag(dis, tagName);
}

void NbtIo::write(const CompoundTag& tag, IDataOutput& dos)
{
    // I have no idea how they intend for this to work without writing the Tag name,
    // considering the name of the format is fucking Named Binary Tag
    Tag::writeNamedTag("", tag, dos);
}