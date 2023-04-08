#include "enginepch.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "yaml-cpp/yaml.h"
#include <execution>

namespace Proximity::Modules
{
#pragma region YAML Converting Operators
    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMINT2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMINT3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMINT4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMUINT2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMUINT3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMUINT4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }
#pragma endregion


    void MaterialLibrary::SerializeMaterials()
    {
        // Serialize material data in shaders directory
        auto outFilePath = Utils::DirectoryManager::s_appDirectories.MaterialDataFile;
        YAML::Emitter out;

        out << YAML::BeginMap;
        std::for_each(m_materials.begin(), m_materials.end(),
            [&](std::pair<std::string, std::shared_ptr<Graphics::Material>> pair)
            {
                // First is material name = material->GetName()
                // Second is pointer to material

                auto shaderPair = pair.second->GetShaderPair();

                out << YAML::Key << "Material" << YAML::Value << pair.second->GetName();
                out << YAML::Key << "Data" << YAML::Value;
                out << YAML::BeginMap;
                    
                // Reflect linked shaders
                out << YAML::Key << "Vertex Shader" << YAML::Value << shaderPair.first->GetName();
                out << YAML::Key << "Pixel Shader" << YAML::Value << shaderPair.second->GetName();

                // Reflect input slots if any
                if (pair.second->GetInputResourceCount() > 0)
                {
                    out << YAML::Key << "Input Slots" << YAML::Value;
                    out << YAML::BeginMap;
                    for (auto& inputSlot : pair.second->GetInputResourceList())
                    {
                        out << YAML::Key << "Name" << YAML::Value << inputSlot.Name;
                        out << YAML::Key << "Data" << YAML::Value;
                        out << YAML::BeginMap;
                        out << YAML::Key << "Bind Point" << YAML::Value << inputSlot.BindPoint;
                        out << YAML::Key << "Bind Count" << YAML::Value << inputSlot.BindCount;
                            
                            // Serialize what shader this input slot was reflected from
                            if (inputSlot.ShaderType == Graphics::GPUShaderType::Vertex)
                            {
                                out << YAML::Key << "Shader Type" << YAML::Value << "Vertex";
                                out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.first->GetName();
                            }
                            else if (inputSlot.ShaderType == Graphics::GPUShaderType::Pixel)
                            {
                                out << YAML::Key << "Shader Type" << YAML::Value << "Pixel";
                                out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.second->GetName();
                            }

                            // Serialize what resource tye this input slot is
                            if (inputSlot.Type == D3D_SIT_TEXTURE)
                            {
                                auto texture = *inputSlot.GetIf< std::shared_ptr<Graphics::Texture2D>>();

                                out << YAML::Key << "Slot Type" << YAML::Value << "Texture";

                                // Applied texture is not internal texture, save path
                                std::string name = "null";
                                if (texture != nullptr)
                                    name = (texture->Name == "_InteralTex") ? texture->Name : texture->Filepath;

                                out << YAML::Key << "Texture" << YAML::Value << name;
                            }
                            else if (inputSlot.Type == D3D_SIT_SAMPLER)
                            {
                                auto sampler = inputSlot.GetIf<Graphics::SamplerState>();
                                out << YAML::Key << "Slot Type" << YAML::Value << "Sampler";
                                out << YAML::Key << "Sampler Name" << YAML::Value << ((sampler == nullptr) ? "null" : sampler->Name);
                            }
                        out << YAML::EndMap;
                    }
                    out << YAML::EndMap;
                }

                // Reflect constant buffers if any
                if (pair.second->GetConstantBufferCount() > 0)
                {
                    out << YAML::Key << "Constant Buffers" << YAML::Value;
                    out << YAML::BeginMap;

                    for (auto& cb : pair.second->GetConstantBufferList())
                    {
                        out << YAML::Key << "Name" << YAML::Value << cb.Desc.Name;
                        out << YAML::Key << "Size" << YAML::Value << cb.Desc.Size;
                        out << YAML::Key << "Variable Count" << YAML::Value << cb.Desc.Variables;

                        // Serialize what shader this constant  was reflected from
                        if (cb.Type == Graphics::GPUShaderType::Vertex)
                        {
                            out << YAML::Key << "Shader Type" << YAML::Value << "Vertex";
                            out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.first->GetName();
                        }
                        else if (cb.Type == Graphics::GPUShaderType::Pixel)
                        {
                            out << YAML::Key << "Shader Type" << YAML::Value << "Pixel";
                            out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.second->GetName();
                        }

                        out << YAML::Key << "Variables" << YAML::Value;
                        out << YAML::BeginMap;
                        for (auto& vars : cb.Variables)
                        {
                            out << YAML::Key << "Name" << YAML::Value << vars.Name;
                            out << YAML::Key << "Data" << YAML::Value;
                            out << YAML::BeginMap;
                            out << YAML::Key << "Size" << YAML::Value << vars.Size;
                            out << YAML::Key << "Offset" << YAML::Value << vars.Offset;
                            
                            switch (vars.Type)
                            {
                            case Proximity::Graphics::MaterialVarType::BOOL:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<bool>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<bool>(vars.GetDefaultIf<bool>());
                                break;

                            case Proximity::Graphics::MaterialVarType::INT:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<int>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<int>(vars.GetDefaultIf<int>());
                                break;

                            case Proximity::Graphics::MaterialVarType::INT2:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMINT2>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMINT2>(vars.GetDefaultIf<DirectX::XMINT2>());
                                break;

                            case Proximity::Graphics::MaterialVarType::INT3:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMINT3>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMINT3>(vars.GetDefaultIf<DirectX::XMINT3>());
                                break;

                            case Proximity::Graphics::MaterialVarType::INT4:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMINT4>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMINT4>(vars.GetDefaultIf<DirectX::XMINT4>());
                                break;

                            case Proximity::Graphics::MaterialVarType::UINT:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<UINT>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<UINT>(vars.GetDefaultIf<UINT>());
                                break;

                            case Proximity::Graphics::MaterialVarType::UINT2:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMUINT2>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMUINT2>(vars.GetDefaultIf<DirectX::XMUINT2>());
                                break;

                            case Proximity::Graphics::MaterialVarType::UINT3:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMUINT3>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMUINT3>(vars.GetDefaultIf<DirectX::XMUINT3>());
                                break;

                            case Proximity::Graphics::MaterialVarType::UINT4:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMUINT4>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMUINT4>(vars.GetDefaultIf<DirectX::XMUINT4>());
                                break;

                            case Proximity::Graphics::MaterialVarType::FLOAT:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<float>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<float>(vars.GetDefaultIf<float>());
                                break;

                            case Proximity::Graphics::MaterialVarType::FLOAT2:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMFLOAT2>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMFLOAT2>(vars.GetDefaultIf<DirectX::XMFLOAT2>());
                                break;

                            case Proximity::Graphics::MaterialVarType::FLOAT3:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMFLOAT3>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMFLOAT3>(vars.GetDefaultIf<DirectX::XMFLOAT3>());
                                break;

                            case Proximity::Graphics::MaterialVarType::FLOAT4:
                                out << YAML::Key << "Value" << YAML::Value << *vars.GetIf<DirectX::XMFLOAT4>();
                                out << YAML::Key << "Default Value" << YAML::Value << static_cast<DirectX::XMFLOAT4>(vars.GetDefaultIf<DirectX::XMFLOAT4>());
                                break;
                            }
                            out << YAML::EndMap;
                        }
                        out << YAML::EndMap;
                    }
                    out << YAML::EndMap;
                }

                out << YAML::EndMap;
            });
        out << YAML::EndMap;

        std::ofstream fout(outFilePath);
        fout << out.c_str();
        fout.close();
    }

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