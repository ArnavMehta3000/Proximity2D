#include "enginepch.h"
#include "Engine/Modules/MaterialLibrary.h"
#include <execution>

namespace Proximity::Modules
{
    void MaterialLibrary::AddMaterial(const std::shared_ptr<Graphics::Material>& material)
    {
        auto& name = material->GetName();
        if (Exists(name))
        {
            PRX_LOG_WARN("Material with the same name [%s] already exists in shader library", name.c_str());
            return;
        }
        m_materials[name] = material;
    }

    void MaterialLibrary::RemoveMaterial(const std::string& materialName)
    {
        if (m_materials.erase(materialName) == 1)
            PRX_LOG_INFO("Successfully removed material [%s] from library", materialName.c_str());
        else
            PRX_LOG_INFO("Failed to remove material [%s] from library", materialName.c_str());
    }

    bool MaterialLibrary::Exists(const std::string& materialName)
    {
        return (m_materials.find(materialName) != m_materials.end());
    }

    std::shared_ptr<Graphics::Material> MaterialLibrary::Get(const std::string& materialName)
    {
        if (!Exists(materialName))
        {
            PRX_LOG_ERROR("Material [%s] not found", materialName.c_str());
            return nullptr;
        }
        else
            return m_materials[materialName];
    }
    void MaterialLibrary::Release()
    {
        // Release all materials
        std::for_each(std::execution::par, m_materials.begin(), m_materials.end(),
            [](std::pair<std::string, std::shared_ptr<Graphics::Material>> pair)
            {
                pair.second->Release();
            });
    }
}