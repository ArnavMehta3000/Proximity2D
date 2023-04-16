#include "enginepch.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "Engine/Modules/ShaderLibrary.h"
#include "Engine/Modules/TextureLibrary.h"
#include "Graphics/Rendering/Renderer2D.h"
#include <yaml-cpp/yaml.h>
#include <execution>


namespace YAML
{
    template<> 
    struct convert<DirectX::XMINT2>
    {
        static Node encode(const DirectX::XMINT2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMINT2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMINT3>
    {
        static Node encode(const DirectX::XMINT3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMINT3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            rhs.z = node[2].as<int>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMINT4>
    {
        static Node encode(const DirectX::XMINT4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMINT4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            rhs.z = node[2].as<int>();
            rhs.w = node[3].as<int>();

            return true;
        }
    };


    template<>
    struct convert<DirectX::XMUINT2>
    {
        static Node encode(const DirectX::XMUINT2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMUINT2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<UINT>();
            rhs.y = node[1].as<UINT>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMUINT3>
    {
        static Node encode(const DirectX::XMUINT3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMUINT3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<UINT>();
            rhs.y = node[1].as<UINT>();
            rhs.z = node[2].as<UINT>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMUINT4>
    {
        static Node encode(const DirectX::XMUINT4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMUINT4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<UINT>();
            rhs.y = node[1].as<UINT>();
            rhs.z = node[2].as<UINT>();
            rhs.w = node[3].as<UINT>();

            return true;
        }
    };


    template<>
    struct convert<DirectX::XMFLOAT2>
    {
        static Node encode(const DirectX::XMFLOAT2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMFLOAT2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMFLOAT3>
    {
        static Node encode(const DirectX::XMFLOAT3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMFLOAT3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();

            return true;
        }
    };

    template<>
    struct convert<DirectX::XMFLOAT4>
    {
        static Node encode(const DirectX::XMFLOAT4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, DirectX::XMFLOAT4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();

            return true;
        }
    };
}



namespace Proximity::Modules
{
#pragma region YAML Type Emitters
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








    void MaterialLibrary::InitProjectLib()
    {
        auto shaderLib = PRX_RESOLVE(Modules::ShaderLibrary);
        auto textureLib = PRX_RESOLVE(Modules::TextureLibrary);

        auto Deserilizer = [shaderLib, textureLib](const YAML::Node& material) -> Graphics::Material*
        {
            auto materialName     = material["Material"].as<std::string>();
            auto vertexShaderName = material["Vertex Shader"].as<std::string>();
            auto pixelShaderName  = material["Pixel Shader"].as<std::string>();

            
            auto output = new Graphics::Material(shaderLib->Get(vertexShaderName), shaderLib->Get(pixelShaderName), materialName);

            // Loop through all constant buffers and set their values
            if (material["Constant Buffers"])
            {
                for (auto cb : material["Constant Buffers"])
                {
                    auto bufferName = cb["Buffer Name"].as<std::string>();
                    auto bufferData = cb["Buffer Data"];

                    if (!bufferData)  // No buffer data
                        continue;
                    
                    auto variables = bufferData["Variables"];
                    if (!variables)  // No variables
                        continue;

                    for (auto var : variables)
                    {
                        auto varName = var["Variable Name"].as<std::string>();
                        auto varType = var["Variable Type"].as<std::string>();
                        auto varValue = var["Value"];

                        Graphics::ShaderVar_T shaderVar;
                        if (varType == "BOOL")
                            shaderVar = varValue.as<bool>();
                        else if (varType == "INT")
                            shaderVar = varValue.as<int>();
                        else if (varType == "XMINT2")
                            shaderVar = varValue.as<DirectX::XMINT2>();
                        else if (varType == "XMINT3")
                            shaderVar = varValue.as<DirectX::XMINT3>();
                        else if (varType == "XMINT4")
                            shaderVar = varValue.as<DirectX::XMINT4>();
                        else if (varType == "UINT")
                            shaderVar = varValue.as<UINT>();
                        else if (varType == "XMUINT2")
                            shaderVar = varValue.as<DirectX::XMUINT2>();
                        else if (varType == "XMUINT3")
                            shaderVar = varValue.as<DirectX::XMUINT3>();
                        else if (varType == "XMUINT4")
                            shaderVar = varValue.as<DirectX::XMUINT4>();
                        else if (varType == "FLOAT")
                            shaderVar = varValue.as<float>();
                        else if (varType == "XMFLOAT2")
                            shaderVar = varValue.as<DirectX::XMFLOAT2>();
                        else if (varType == "XMFLOAT3")
                            shaderVar = varValue.as<DirectX::XMFLOAT3>();
                        else if (varType == "XMFLOAT4")
                            shaderVar = varValue.as<DirectX::XMFLOAT4>();
                        else
                        {
                            PRX_LOG_ERROR("Shader variable type [%s] for buffer [%s], failed to deserialize from material [%s]",
                                varName.c_str(), bufferName.c_str(), materialName.c_str());
                            continue;
                        }

                        auto success = output->SetBufferVarByName(bufferName, varName, shaderVar);
                    }
                }
            }


            // Loop through all input slots and set their values
            if (material["Input Slots"])
            {
                for (auto& inputSlot : material["Input Slots"])
                {
                    auto slotName    = inputSlot["Slot Name"].as<std::string>();
                    auto slotData    = inputSlot["Slot Data"];
                    auto slotType    = slotData["Slot Type"].as<std::string>();
                    auto shaderName  = slotData["Shader Name"].as<std::string>();

                    output->ReflectInputSlotByName(slotName.c_str(), shaderName);

                    if (slotType == "Texture")
                    {
                        auto textureName = slotData["Texture"].as<std::string>();
                        auto texture = textureLib->Get(textureName);
                        if (!output->SetInputResourceByName(slotName, texture))
                            PRX_LOG_ERROR("Failed to set input slot [%s] texture resource", slotName.c_str());
                    }
                    else if (slotType == "Sampler")
                    {
                        auto samplerName = slotData["Sampler"].as<std::string>();
                        auto renderer = PRX_RESOLVE(Graphics::Renderer2D);

                        const auto& samplerList = renderer->GetSamplerList();
                        auto it = std::find_if(samplerList.begin(), samplerList.end(),
                            [samplerName](const Graphics::SamplerState& ss)
                            {
                                return ss.Name.compare(samplerName) == 0;
                            });

                        // Not found
                        if (it == std::end(samplerList))
                            PRX_LOG_ERROR("Failed to find sampler for input slot");

                        if (!output->SetInputResourceByName(slotName, *it))
                            PRX_LOG_ERROR("Failed to set input slot [%s] sampler resource", slotName.c_str());
                    }
                }
            }



            // Apply all the value changes
            for (auto& cb : output->GetConstantBufferList())
                cb.ApplyBufferChanges();

            return output;
        };

        YAML::Node data = YAML::LoadFile(Utils::DirectoryManager::s_appDirectories.MaterialDataFile.string());

        if (!data["Materials"])
        {
            PRX_LOG_ERROR("Invalid material data file");
            MSG_BOX("Invalid material data file", "Material Load Error");
            return;
        }

        for (auto material : data["Materials"])
        {
            auto deserializedMaterial = Deserilizer(material);

            if (deserializedMaterial == nullptr)
                PRX_LOG_ERROR("Failed to deserialize material");
            else
                AddMaterial(std::shared_ptr<Graphics::Material>(deserializedMaterial));
        }
    }

    void MaterialLibrary::SerializeMaterials()
    {
        auto SerializeMat = [](YAML::Emitter& out, std::pair<std::string, std::shared_ptr<Graphics::Material>>& pair) -> void
        {
            auto shaderPair = pair.second->GetShaderPair();

            out << YAML::BeginMap;

            out << YAML::Key << "Material" << YAML::Value << pair.second->GetName();
            out << YAML::Key << "Vertex Shader" << YAML::Value << shaderPair.first->GetName();
            out << YAML::Key << "Pixel Shader" << YAML::Value << shaderPair.second->GetName();

            // Reflect constant buffers if any
            if (pair.second->GetConstantBufferCount() > 0)
            {
                out << YAML::Key << "Constant Buffers" << YAML::Value;

                out << YAML::BeginSeq;
                for (auto& cb : pair.second->GetConstantBufferList())
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Buffer Name" << YAML::Value << cb.Desc.Name;
                        out << YAML::Key << "Buffer Data" << YAML::Value;

                        out << YAML::BeginMap;
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

                        // Add the values of the buffer variables if any
                        if (cb.Variables.size() > 0)
                        {
                            out << YAML::Key << "Variables" << YAML::Value;

                            out << YAML::BeginSeq;
                            for (auto& var : cb.Variables)
                            {
                                out << YAML::BeginMap;
                                    out << YAML::Key << "Variable Name" << YAML::Value << var.Name;

                                    switch (var.Type)
                                    {
                                    case Proximity::Graphics::MaterialVarType::BOOL:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "BOOL";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<bool>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::INT:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "INT";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<int>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::INT2:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMINT2";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMINT2>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::INT3:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMINT3";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMINT3>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::INT4:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMINT4";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMINT4>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::UINT:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "UINT";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<UINT>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::UINT2:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMUINT2";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMUINT2>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::UINT3:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMUINT3";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMUINT3>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::UINT4:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMUINT4";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMUINT4>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::FLOAT:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "FLOAT";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<float>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::FLOAT2:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMFLOAT2";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMFLOAT2>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::FLOAT3:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMFLOAT3";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMFLOAT3>();
                                        break;

                                    case Proximity::Graphics::MaterialVarType::FLOAT4:
                                        out << YAML::Key << "Variable Type" << YAML::Value << "XMFLOAT4";
                                        out << YAML::Key << "Value" << YAML::Value << *var.GetIf<DirectX::XMFLOAT4>();
                                        break;
                                    }

                                out << YAML::EndMap;
                            }
                            out << YAML::EndSeq;
                        }


                        out << YAML::EndMap;
                    out << YAML::EndMap;
                }
                out << YAML::EndSeq;
            }


            // Reflect input slots if any
            if (pair.second->GetInputResourceCount() > 0)
            {
                out << YAML::Key << "Input Slots" << YAML::Value;

                out << YAML::BeginSeq;
                for (auto& slot : pair.second->GetInputResourceList())
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Slot Name" << YAML::Value << slot.Name;
                        out << YAML::Key << "Slot Data" << YAML::Value;

                        out << YAML::BeginMap;
                            // Serialize what shader this input slot was reflected from
                            if (slot.ShaderType == Graphics::GPUShaderType::Vertex)
                            {
                                out << YAML::Key << "Shader Type" << YAML::Value << "Vertex";
                                out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.first->GetName();
                            }
                            else if (slot.ShaderType == Graphics::GPUShaderType::Pixel)
                            {
                                out << YAML::Key << "Shader Type" << YAML::Value << "Pixel";
                                out << YAML::Key << "Shader Name" << YAML::Value << shaderPair.second->GetName();
                            }

                            // Reflect slot data/value
                            if (slot.Type == D3D_SIT_TEXTURE)  // Slot is texture type
                            {
                                auto texture = *slot.GetIf< std::shared_ptr<Graphics::Texture2D>>();
                                out << YAML::Key << "Slot Type" << YAML::Value << "Texture";

                                // Applied texture is not internal texture, save path
                                std::string name = "null";
                                if (texture != nullptr)
                                    name = texture->Name;

                                out << YAML::Key << "Texture" << YAML::Value << name;
                            }
                            else if (slot.Type == D3D_SIT_SAMPLER)  // Slot is sampler type
                            {
                                auto sampler = slot.GetIf<Graphics::SamplerState>();
                                out << YAML::Key << "Slot Type" << YAML::Value << "Sampler";
                                out << YAML::Key << "Sampler" << YAML::Value << ((sampler == nullptr) ? "null" : sampler->Name);
                            }
                        out << YAML::EndMap;
                    out << YAML::EndMap;
                }

                out << YAML::EndSeq;
            }


            out << YAML::EndMap;
        };


        // Serialize material data in shaders directory
        std::filesystem::path outFilePath = Utils::DirectoryManager::s_appDirectories.MaterialDataFile;
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "Materials" << YAML::Value <<YAML::BeginSeq;

        // Loop over all created materials
        std::for_each(m_materials.begin(), m_materials.end(),
            [&](std::pair<std::string, std::shared_ptr<Graphics::Material>> pair)
            {
                SerializeMat(out, pair);
            });
        out << YAML::EndSeq;
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