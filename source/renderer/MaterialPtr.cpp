#include <typeinfo>

#include "MaterialPtr.hpp"
#include "common/Logger.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"

using namespace mce;

MaterialPtr MaterialPtr::NONE = MaterialPtr();

MaterialPtr::MaterialPtr()
{
    m_pGroup = nullptr;
    m_pMaterial = nullptr;
}

MaterialPtr::MaterialPtr(MaterialPtr&& other)
{
    _move(std::move(other));
}

MaterialPtr::MaterialPtr(RenderMaterialGroup& group, const std::string& name)
    : m_pGroup(&group)
    , m_pMaterial(nullptr)
    , m_name(name)
{
    m_pGroup->_addRef(*this);
    onGroupReloaded();
}

MaterialPtr::~MaterialPtr()
{
    _deref();
}

void MaterialPtr::_move(MaterialPtr&& other)
{
    m_pGroup = other.m_pGroup;
    m_pMaterial = other.m_pMaterial;
    std::swap(m_name, other.m_name);
    other._deref();
    m_pGroup->_addRef(*this);
}

void MaterialPtr::_deref()
{
    if (m_pGroup != nullptr)
    {
        m_pGroup->_removeRef(*this);
        m_pGroup = nullptr;
    }
    m_pMaterial = nullptr;
}

void MaterialPtr::onGroupReloaded()
{
    if (m_pGroup == nullptr)
    {
        LOG_E("Null ptrs may never be registered!");
        throw std::bad_cast();
    }

    m_pMaterial = m_pGroup->_getMaterial(m_name);
}

RenderMaterial* MaterialPtr::operator->() const
{
    if (!m_pMaterial)
    {
        LOG_E("Invalid dereference");
        throw std::bad_cast();
    }
    return m_pMaterial;
}

MaterialPtr::operator bool() const
{
    return *this != MaterialPtr::NONE;
}

MaterialPtr& MaterialPtr::operator=(MaterialPtr&& other)
{
    _move(std::move(other));
    return *this;
}

bool MaterialPtr::operator==(const MaterialPtr& other) const
{
    return this->m_pMaterial == other.m_pMaterial;
}

bool MaterialPtr::operator!=(const MaterialPtr& other) const
{
    return this->m_pMaterial != other.m_pMaterial;
}