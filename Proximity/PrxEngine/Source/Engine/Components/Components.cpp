#include "enginepch.h"
#include "Engine/Components/Components.h"
#include "Graphics/D3DManager.h"

namespace Proximity::Core
{
#pragma region Transform Component
	TransformComponent::TransformComponent()
		:
		BaseComponent("Transform"),
		m_Position(0.0f),
		m_Rotation(0.0f),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(0.0f),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::F32 rot)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::F32 rot, Math::Vec3 scale)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(scale),
		m_parentHandle(entt::null)
	{}
#pragma endregion


#pragma region Name Component
	NameComponent::NameComponent()
		:
		BaseComponent("Name"),
		m_EntityName("Unnamed Entity")
	{}

	NameComponent::NameComponent(const std::string& name)
		:
		BaseComponent("Name"),
		m_EntityName(name)
	{}
#pragma endregion


#pragma region Sprite Renderer Component

	WhiteTexture SpriteRendererComponent::s_whiteTexture = WhiteTexture();

	SpriteRendererComponent::SpriteRendererComponent()
		:
		BaseComponent("Sprite Renderer"),
		Tint(1, 1, 1, 1),
		Material()
	{
		// If white texture has not been created
		if (s_whiteTexture.m_created == false)
		{
			const uint32_t pixel = 0xffffffff;
			auto d3d = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
			if (d3d == nullptr)
			{
				PRX_LOG_ERROR("Failed to get D3D Manager to create white texture (Sprite Renderer Component)");
				return;
			}
			else
			{
				D3D11_SUBRESOURCE_DATA initData = { &pixel, sizeof(uint32_t), 0 };

				CREATE_ZERO(D3D11_TEXTURE2D_DESC, desc);
				desc.Width            = desc.Height = desc.MipLevels = desc.ArraySize = 1;
				desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.Usage            = D3D11_USAGE_IMMUTABLE;
				desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
				
				PRX_ASSERT_HR(d3d->GetDevice()->CreateTexture2D(&desc, &initData, s_whiteTexture.Texture.ReleaseAndGetAddressOf()),
					"Failed to create 1px white texture in sprite component");


				CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, SRVDesc);
				SRVDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
				SRVDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Texture2D.MipLevels = 1;

				PRX_ASSERT_HR(d3d->GetDevice()->CreateShaderResourceView(s_whiteTexture.Texture.Get(), &SRVDesc, s_whiteTexture.SRV.ReleaseAndGetAddressOf()),
					"Failed to create shader resource view of 1px white texture in sprite component")
			}
		}
	}

	void SpriteRendererComponent::Release()
	{
		COM_RELEASE(s_whiteTexture.Texture);
		COM_RELEASE(s_whiteTexture.SRV);
	}
#pragma endregion
}