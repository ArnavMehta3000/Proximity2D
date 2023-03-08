#include "enginepch.h"
#include "Engine/Game/OrthographicCamera.h"

namespace Proximity::Core
{
	OrthographicCamera::OrthographicCamera()
		:
		m_nearPlane(1.0f),
		m_farPlane(1000.0f),
		m_zRotation(0.0f)
	{
		Core::Globals::g_vpResizeEvent += PRX_ACTION_FUNC(OrthographicCamera::OnResizeCallback);
	}

	OrthographicCamera::OrthographicCamera(float width, float height, float nearPlane, float farPlane)
		: 
		m_position(Vector3(0.0f, 0.0f, -5.0f)),
		m_zRotation(0.0f),
		m_orthoScale(100.0f),
		m_nearPlane(nearPlane),
		m_farPlane(farPlane)

	{
		Core::Globals::g_vpResizeEvent += PRX_ACTION_FUNC(OrthographicCamera::OnResizeCallback);
		m_projectionMatrix = DirectX::XMMatrixOrthographicLH(width, height, nearPlane, farPlane);
		UpdateMatrices();
	}

	OrthographicCamera::~OrthographicCamera()
	{
		Core::Globals::g_vpResizeEvent -= PRX_ACTION_FUNC(OrthographicCamera::OnResizeCallback);
	}

	void OrthographicCamera::UpdateMatrices()
	{
		// S*R*T format
		//m_viewMatrix = Matrix::CreateScale(m_orthoScale) * Matrix::CreateRotationZ(XMConvertToRadians(m_zRotation)) * XMMatrixLookToLH(m_position, Vector3(0.0f, 0.0f, 1.f), Vector3(0.0f, 1.0f, 0.0f));

		m_viewMatrix = XMMatrixLookToLH(m_position, Vector3(0.0f, 0.0f, 1.f), Vector3(0.0f, 1.0f, 0.0f));
		m_viewMatrix *= DirectX::XMMatrixScaling(m_orthoScale, m_orthoScale, m_orthoScale) * DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_zRotation));

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
	}

	void OrthographicCamera::OnResizeCallback(int w, int h)
	{
		if (w == 0 || h == 0)
			return;

		m_projectionMatrix = DirectX::XMMatrixOrthographicLH((float)w, (float)h, m_nearPlane, m_farPlane);
	}
}