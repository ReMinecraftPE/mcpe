#include "Tag.hpp"
#include "Int8Tag.hpp"
#include "Int16Tag.hpp"
#include "Int32Tag.hpp"
#include "Int64Tag.hpp"
#include "FloatTag.hpp"
#include "DoubleTag.hpp"
#include "Int8ArrayTag.hpp"
#include "StringTag.hpp"
#include "ListTag.hpp"
#include "CompoundTag.hpp"
#include "Int32ArrayTag.hpp"
#include "Int64ArrayTag.hpp"
#include "EndTag.hpp"

std::string Tag::getTagName(Tag::Type type)
{
	switch (type)
	{
		case TAG_TYPE_END:         return "TAG_End";
		case TAG_TYPE_INT8:        return "TAG_Byte";
		case TAG_TYPE_INT16:       return "TAG_Short";
		case TAG_TYPE_INT32:       return "TAG_Int";
		case TAG_TYPE_INT64:       return "TAG_Long";
		case TAG_TYPE_FLOAT:       return "TAG_Float";
		case TAG_TYPE_DOUBLE:      return "TAG_Double";
		case TAG_TYPE_INT8_ARRAY:  return "TAG_Byte_Array";
		case TAG_TYPE_STRING:      return "TAG_String";
		case TAG_TYPE_LIST:        return "TAG_List";
		case TAG_TYPE_COMPOUND:    return "TAG_Compound";
		case TAG_TYPE_INT32_ARRAY: return "TAG_Int_Array";
		default:                   return "UNKNOWN";
	}
}

Tag* Tag::newTag(Tag::Type type) 
{
    switch (type)
	{
    case TAG_TYPE_END:         return new EndTag();
    case TAG_TYPE_INT8:        return new Int8Tag();
    case TAG_TYPE_INT16:       return new Int16Tag();
    case TAG_TYPE_INT32:       return new Int32Tag();
    case TAG_TYPE_INT64:       return new Int64Tag();
    case TAG_TYPE_FLOAT:       return new FloatTag();
    case TAG_TYPE_DOUBLE:      return new DoubleTag();
    case TAG_TYPE_INT8_ARRAY:  return new Int8ArrayTag();
    case TAG_TYPE_STRING:      return new StringTag();
    case TAG_TYPE_LIST:        return new ListTag();
    case TAG_TYPE_COMPOUND:    return new CompoundTag();
    case TAG_TYPE_INT32_ARRAY: return new Int32ArrayTag();
    case TAG_TYPE_INT64_ARRAY: return new Int64ArrayTag();
    default: return nullptr;
    }
}

Tag* Tag::readNamedTag(IDataInput& dis, std::string& name)
{
    Tag::Type type = (Tag::Type)dis.readInt8();
	if (type == TAG_TYPE_END)
		return new EndTag();

	name = dis.readString();

    Tag* tag = Tag::newTag(type);
	if (tag)
		tag->load(dis);

    return tag;
}

void Tag::writeNamedTag(const std::string& name, const Tag& tag, IDataOutput& dos)
{
    dos.writeInt8(tag.getId());

    if (tag.getId() != TAG_TYPE_END)
	{
		dos.writeString(name);
		tag.write(dos);
    }
}

TagMemoryChunk::TagMemoryChunk()
{
	m_elements = 0;
	m_size = 0;
	m_buffer = nullptr;
}

TagMemoryChunk::~TagMemoryChunk()
{
	if (m_buffer)
		delete m_buffer;
}

TagMemoryChunk TagMemoryChunk::copy() const
{
	TagMemoryChunk result = TagMemoryChunk();
	result.m_elements = m_elements;
	result.m_size = m_size;
	result.alloc<int8_t>(m_size);
	memcpy(result.m_buffer, m_buffer, m_size);
	return result;
}

bool TagMemoryChunk::operator!=(const TagMemoryChunk& other) const
{
	if (m_size != other.m_size )
		return true;

	if (m_size > 0)
		return memcmp(m_buffer, other.m_buffer, m_size) != 0;

	return false;
}