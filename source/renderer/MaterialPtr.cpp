#include <typeinfo>

#include "MaterialPtr.hpp"
#include "common/Logger.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"

using namespace mce;

MaterialPtr MaterialPtr::NONE = MaterialPtr();

MaterialPtr::MaterialPtr()
{
    m_group = nullptr;
    m_material = nullptr;
}

MaterialPtr::MaterialPtr(MaterialPtr&& other)
{
    _move(std::move(other));
}

MaterialPtr::MaterialPtr(RenderMaterialGroup& group, const std::string& name)
    : m_group(&group)
    , m_material(nullptr)
    , m_name(name)
{
    m_group->_addRef(*this);
    onGroupReloaded();
}

MaterialPtr::~MaterialPtr()
{
    _deref();
}

void MaterialPtr::_move(MaterialPtr&& other)
{
    m_group = other.m_group;
    m_material = other.m_material;
    std::swap(m_name, other.m_name);
    other._deref();
    m_group->_addRef(*this);
}

void MaterialPtr::_deref()
{
    if (m_group != nullptr)
    {
        m_group->_removeRef(*this);
        m_group = nullptr;
    }
    m_material = nullptr;
}

void MaterialPtr::onGroupReloaded()
{
    if (m_group == nullptr)
    {
        LOG_E("Null ptrs may never be registered!");
        throw std::bad_cast();
    }

    m_material = m_group->_getMaterial(m_name);
}

MaterialPtr& MaterialPtr::operator=(MaterialPtr&& other)
{
    _move(std::move(other));
    return *this;
}