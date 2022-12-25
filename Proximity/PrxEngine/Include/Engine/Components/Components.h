#pragma once
#include "Engine/Components/BaseComponent.h"
#include "Graphics/Material.h"
#include "Graphics/VertexStructures.h"
#include "../External/entt/entt.hpp"

namespace Proximity::Core
{
	struct NameComponent : public BaseComponent
	{
		NameComponent();
		NameComponent(const std::string& name);
		NameComponent(const NameComponent&) = default;

		std::string m_EntityName;
	};

	struct TransformComponent : public BaseComponent
	{
		TransformComponent();
		TransformComponent(Math::Vec3 pos);
		TransformComponent(Math::Vec3 pos, Math::F32 rot);
		TransformComponent(Math::Vec3 pos, Math::F32 rot, Math::Vec3 scale);
		TransformComponent(const TransformComponent&) = default;
		
		Math::Vector3 m_Position;
		Math::F32     m_Rotation;
		Math::Vector3 m_Scale;
		entt::entity  m_parentHandle;

		operator Math::Matrix () noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationZ(m_Rotation) * DX::XMMatrixTranslationFromVector(m_Position);
		}

		operator const Math::Matrix() noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationZ(m_Rotation) * DX::XMMatrixTranslationFromVector(m_Position);
		}
	};

	// Anon namespace to prevent creation/access of more textures
	namespace
	{
		struct WhiteTexture
		{
			WhiteTexture()
				:
				m_created(false),
				Texture(nullptr),
				SRV(nullptr)
			{}
			bool m_created;
			ComPtr<ID3D11Texture2D> Texture;
			ComPtr<ID3D11ShaderResourceView> SRV;
		};
	}

	struct SpriteRendererComponent : public BaseComponent
	{
		SpriteRendererComponent();

		static void Release();
		
		DX::XMFLOAT4 Tint;
		Graphics::Material Material;

	private:	
		static WhiteTexture s_whiteTexture;
	};
}