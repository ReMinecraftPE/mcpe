#include "RenderMaterialGroup.hpp"

#include "thirdparty/rapidjson/error/en.h"

#include "common/Util.hpp"
#include "common/utility/InheritanceTree.hpp"

#include "renderer/RenderMaterial.hpp"

using namespace mce;

RenderMaterialGroup RenderMaterialGroup::common;
RenderMaterialGroup RenderMaterialGroup::switchable;

struct MaterialParent
{
    std::string parentName;
    const rapidjson::Value* json;

    MaterialParent()
    {
        json = nullptr;
    }
};

typedef InheritanceTree<MaterialParent> MaterialTree;

RenderMaterialGroup::RenderMaterialGroup()
    : AppPlatformListener(false)
{
}

RenderMaterialGroup::~RenderMaterialGroup()
{
    _fireGroupDestroyed();
}

void RenderMaterialGroup::_fireGroupReloaded()
{
    for (std::set<MaterialPtr*>::iterator it = m_references.begin(); it != m_references.end(); it++)
    {
        (*it)->onGroupReloaded();
    }
}

void RenderMaterialGroup::_fireGroupDestroyed()
{
    m_references.clear();
}

RenderMaterial& RenderMaterialGroup::_material(const std::string& fileName, const std::string& tag)
{
    return m_materials[fileName + tag];
}

void _getParent(const std::string& name, const std::string& materialIdentifier, std::string& parentName, std::string& childName)
{
    size_t separatorPos = name.find_last_of(":");
    childName = std::string(name, 0, separatorPos);

    if (separatorPos == std::string::npos)
        parentName = Util::EMPTY_STRING;
    else
        parentName = name.substr(separatorPos + 1) + materialIdentifier;
}

struct MaterialTreePopulator
{
    RenderMaterial& groupBaseParent;
    RenderMaterialGroup& group;

    MaterialTreePopulator(RenderMaterialGroup& group, RenderMaterial& groupBaseParent)
        : groupBaseParent(groupBaseParent)
        , group(group)
    {
    }

	void operator()(const std::string& materialName, MaterialParent& materialParent)
	{
        // Construct material with parent and place in group
        RenderMaterial& parentMaterial = group._getMaterialOrDefault(materialParent.parentName, groupBaseParent);
        group.m_materials[materialName] = RenderMaterial(*materialParent.json, parentMaterial);
	}
};

void RenderMaterialGroup::_loadMaterialSet(const rapidjson::Value& root, RenderMaterial& groupBaseParent, const std::string& materialIdentifier)
{
    MaterialTree familyTree;

    for (rapidjson::Value::ConstMemberIterator it = root.MemberBegin(); it != root.MemberEnd(); it++)
    {
        std::string fullName = it->name.GetString();
        std::string parentName, childName;
        _getParent(fullName, materialIdentifier, parentName, childName);
        MaterialTree::Node& childNode = familyTree._node(childName);
        MaterialTree::Node& parentNode = familyTree._node(parentName);
        childNode.val.parentName = parentName;
        childNode.val.json = &it->value;

        parentNode.child.push_back(&childNode);
    }

    // Construct all materials from the tree in order
    MaterialTreePopulator visitor(*this, groupBaseParent);
    familyTree.visitBFS(visitor);
}

bool _isMaterialGroup(const rapidjson::Value& root)
{
    return !root.HasMember("vertexShader") || !root["vertexShader"].IsString();
}

void RenderMaterialGroup::_loadList()
{
    std::string fileContents = AppPlatform::singleton()->readAssetFileStr(m_listPath, false);
    rapidjson::Document document;
    document.Parse(fileContents.c_str());
    const rapidjson::Value& root = document.GetArray();

    for (rapidjson::Value::ConstValueIterator it = root.Begin(); it != root.End(); it++)
    {
        const rapidjson::Value& value = (*it).GetObj();
        std::string path = value["path"].GetString();

        RenderMaterial material;

        if (value.HasMember("defines"))
        {
            const rapidjson::Value& defines = value["defines"].GetArray();
            for (rapidjson::Value::ConstValueIterator it = defines.Begin(); it != defines.End(); it++)
            {
                material.m_defines.insert(it->GetString());
            }
        }

        std::string tag;
        if (value.HasMember("tag"))
        {
            tag = value["tag"].GetString();
        }

        fileContents = AppPlatform::singleton()->readAssetFileStr(path, false);
        rapidjson::Document doc;
        rapidjson::ParseResult ok = doc.Parse(fileContents.c_str());
        if (!ok)
        {
            LOG_E("Error parsing \"%s\", offset %zu: %s", path.c_str(), ok.Offset(), rapidjson::GetParseError_En(ok.Code()));
            continue;
        }

        const rapidjson::Value& root = doc.GetObj();
        if (_isMaterialGroup(root))
        {
            _loadMaterialSet(root, material, tag);
        }
        else
        {
            RenderMaterial& materialRef = _material(Util::getFileName(path), tag);
            materialRef = RenderMaterial(root, materialRef);
        }
    }

    for (std::map<const std::string, RenderMaterial>::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        RenderMaterial& material = it->second;
        if (material.m_pShader)
        {
            material.m_pShader->finalizeShaderUniforms();
        }
    }

    Shader::freeCompilerResources();
}

void RenderMaterialGroup::_addRef(MaterialPtr& reference)
{
    m_references.insert(&reference);
}

void RenderMaterialGroup::_removeRef(MaterialPtr& reference)
{
    m_references.erase(&reference);
}

RenderMaterial* RenderMaterialGroup::_getMaterialPtr(const std::string& name)
{
    std::map<const std::string, RenderMaterial>::iterator it = m_materials.find(name);
    if (it != m_materials.end())
        return &it->second;

    return nullptr;
}

RenderMaterial* RenderMaterialGroup::_getMaterial(const std::string& name)
{
    RenderMaterial* materialPtr = _getMaterialPtr(name);
    if (!materialPtr)
    {
        LOG_W("Material: %s not found", name.c_str());
    }

    return materialPtr;
}

RenderMaterial& RenderMaterialGroup::_getMaterialOrDefault(const std::string& name, RenderMaterial& defaultMaterial)
{
    RenderMaterial* materialPtr = _getMaterialPtr(name);
    if (materialPtr)
        return *materialPtr;
    else
        return defaultMaterial;
}

MaterialPtr RenderMaterialGroup::getMaterial(const std::string& name)
{
    return MaterialPtr(*this, name);
}

void RenderMaterialGroup::loadList(const std::string listPath)
{
    if (!m_listPath.empty())
    {
        m_materials.clear();
        m_listPath = listPath;
        _loadList();
        _fireGroupReloaded();
    }
    else
    {
        initListener();
        m_listPath = listPath;
        _loadList();
    }
}

void RenderMaterialGroup::compileMaterials()
{
    for (std::map<const std::string, RenderMaterial>::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        it->second.compileShader();
    }
}

void RenderMaterialGroup::onAppResumed()
{
    m_materials.clear();
    _loadList();
    _fireGroupReloaded();
}

void RenderMaterialGroup::onAppSuspended()
{
    for (std::map<const std::string, RenderMaterial>::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        it->second = RenderMaterial();
    }
}
