#include <sstream>

#include "ListTag.hpp"
#include "common/Util.hpp"
#include "FloatTag.hpp"
#include "StringTag.hpp"
#include "Int32Tag.hpp"

ListTag::ListTag() : m_type(TAG_TYPE_END) {}
ListTag::ListTag(Tag::Type type) : m_type(type) {}

ListTag::~ListTag()
{
	deleteChildren();
}

void ListTag::write(IDataOutput& dos) const
{
    dos.writeInt8(m_type);
    dos.writeInt32(m_list.size());

    for (std::vector<Tag*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
        (*it)->write(dos);
	}
}

void ListTag::load(IDataInput& dis)
{
    m_type = (Tag::Type)dis.readInt8();
	uint32_t length = dis.readInt32();

    m_list.clear();

	if (length <= 0 || m_type == TAG_TYPE_END )
		return;

	for (uint32_t i = 0; i < length; ++i )
	{
		if (dis.numBytesLeft() == 0) break;

		Tag* tag = Tag::newTag(m_type);
		if (!tag) break;

		tag->load(dis);
        m_list.push_back(tag);
	}
}

std::string ListTag::toString() const
{
	std::ostringstream sstream;
	sstream << m_list.size() << " entries of type " << Tag::getTagName(m_type);
	return sstream.str();
}

void ListTag::add(Tag* tag)
{
    m_type = tag->getId();
    m_list.push_back(tag);
}

float ListTag::getFloat(unsigned int index) const
{
	if (index < m_list.size()
		&& index >= 0
		&& m_list[index] != nullptr
		&& m_list[index]->getId() == TAG_TYPE_FLOAT)
	{
		return ((FloatTag*)m_list[index])->m_data;
	}
	else
	{
		return 0.0f;
	}
}

int32_t ListTag::getInt32(unsigned int index) const
{
	if (index < m_list.size()
		&& index >= 0
		&& m_list[index] != nullptr
		&& m_list[index]->getId() == TAG_TYPE_INT32)
	{
		return ((FloatTag*)m_list[index])->m_data;
	}
	else
	{
		return 0.0f;
	}
}

const std::string& ListTag::getString(unsigned int index) const
{
	if (index >= m_list.size())
		return Util::EMPTY_STRING;
	if (index < 0)
		return Util::EMPTY_STRING;
	if (!m_list[index])
		return Util::EMPTY_STRING;
	if (m_list[index]->getId() == TAG_TYPE_STRING)
		return Util::EMPTY_STRING;

	return ((StringTag*)m_list[index])->m_data;
}

const CompoundTag* ListTag::getCompound(unsigned int index) const
{
	if (index < m_list.size()
		&& index >= 0
		&& m_list[index]->getId() == TAG_TYPE_COMPOUND)
	{
		return (const CompoundTag *)m_list[index];
	}
	else
	{
		return nullptr;
	}
}

Tag* ListTag::copy() const
{
	return copyList();
}

ListTag* ListTag::copyList() const
{
    ListTag* result = new ListTag();
    result->m_type = m_type;

    for (std::vector<Tag*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
        Tag* tag = *it;
		Tag* copiedTag = tag->copy();
        result->m_list.push_back(copiedTag);
    }

    return result;
}

void ListTag::deleteChildren()
{
	for (std::vector<Tag*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		Tag* tag = *it;
        tag->deleteChildren();
		delete tag;
	}

	m_list.clear();
}

bool ListTag::operator==(const Tag& other) const
{
	const ListTag& other2 = (const ListTag&)(other);
	if (getId() == other2.getId() && m_type == other2.m_type)
	{
		if (m_list.size() == other2.m_list.size())
		{
			if (m_list.empty())
				return true;

			for (size_t i = 0; i < m_list.size(); i++)
			{
				if (m_list[i] != other2.m_list[i])
					return false;
			}
		}
	}

	return false;
}

ListTagFloatAdder::ListTagFloatAdder(float f)
{
	m_tag = nullptr;
	this->operator()(f);
}

ListTagFloatAdder& ListTagFloatAdder::operator()(float f)
{
	if (!m_tag)
		m_tag = new ListTag();

	Tag* newTag = new FloatTag(f);

	m_tag->add(newTag);

	return *this;
}

ListTagInt32Adder::ListTagInt32Adder(int32_t i)
{
	m_tag = nullptr;
	this->operator()(i);
}

ListTagInt32Adder& ListTagInt32Adder::operator()(int32_t i)
{
	if (!m_tag)
		m_tag = new ListTag();

	Tag* newTag = new Int32Tag(i);

	m_tag->add(newTag);

	return *this;
}