#pragma once


namespace Proximity::Core
{
	

	class OrthographicCamera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float width, float height, float nearPlane = 1.0f, float farPlane = 1000.0f);
		~OrthographicCamera();

		void Position(const Math::Vector3& position) { m_position = position; UpdateMatrices(); }
		void Rotation(float degrees)                 { m_zRotation = degrees;  UpdateMatrices(); }
		void OrthoScale(float scale = 100.0f)        { m_orthoScale = scale;    UpdateMatrices(); }

		const Math::Vector3& Position() const noexcept { return m_position; }
		float Rotation()                const noexcept { return m_zRotation; }
		float OrthoScale()              const noexcept { return m_orthoScale; }

		const Math::Matrix& GetProjectionMatrix() const noexcept { return m_projectionMatrix; }
		const Math::Matrix& GetViewMatrix()       const noexcept { return m_viewMatrix; }
		const Math::Matrix& GetViewProjMatrix()   const noexcept { return m_viewProjMatrix; }

		void OnResizeCallback(int w, int h);

	private:
		void UpdateMatrices();

		Math::Matrix m_projectionMatrix;
		Math::Matrix m_viewMatrix;
		Math::Matrix m_viewProjMatrix;

		Math::Vector3 m_position;
		float m_zRotation = 0.0f;
		float m_orthoScale = 100.0f;
		float m_nearPlane, m_farPlane;
	};
}