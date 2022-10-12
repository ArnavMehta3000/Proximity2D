#include "Vector3.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace PRX
{
	namespace Math
	{
		const Vector3 Vector3::Zero     = Vector3( 0.0f,  0.0f,  0.0f);
		const Vector3 Vector3::One      = Vector3( 1.0f,  1.0f,  1.0f);
		const Vector3 Vector3::Up       = Vector3( 0.0f,  1.0f,  0.0f);
		const Vector3 Vector3::Down     = Vector3( 0.0f, -1.0f,  0.0f);
		const Vector3 Vector3::Left     = Vector3(-1.0f,  0.0f,  0.0f);
		const Vector3 Vector3::Right    = Vector3( 1.0f,  0.0f,  0.0f);
		const Vector3 Vector3::Forward  = Vector3( 0.0f,  0.0f,  1.0f);
		const Vector3 Vector3::Backward = Vector3( 0.0f,  0.0f, -1.0f);



		bool Vector3::operator==(const Vector3& vec) const noexcept
		{
			DX::XMVECTOR v1 = DX::XMLoadFloat3(this);
			DX::XMVECTOR v2= DX::XMLoadFloat3(&vec);
			return DX::XMVector3Equal(v1, v2);
		}

		bool Vector3::operator!=(const Vector3& vec) const noexcept
		{
			DX::XMVECTOR v1 = DX::XMLoadFloat3(this);
			DX::XMVECTOR v2 = DX::XMLoadFloat3(&vec);
			return DX::XMVector3NotEqual(v1, v2);
		}




		Vector3& Vector3::operator+=(const Vector3& vec) noexcept
		{
			XMVECTOR_LOAD(v1, this);
			XMVECTOR_LOAD(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorAdd(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator-=(const Vector3& vec) noexcept
		{
			XMVECTOR_LOAD(v1, this);
			XMVECTOR_LOAD(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorAdd(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator*=(const Vector3& vec) noexcept
		{
			XMVECTOR_LOAD(v1, this);
			XMVECTOR_LOAD(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorMultiply(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator*=(const F32 number) noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR result = DX::XMVectorScale(v1, number);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator/=(const F32 number) noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR result = DX::XMVectorScale(v1, 1.0f / number);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3 Vector3::operator-() const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR result = DX::XMVectorNegate(v1);
			Vector3 out;
			DX::XMStoreFloat3(&out, result);
			return out;
		}


		F32 Vector3::Length() const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR len = DX::XMVector3Length(v1);
			return DX::XMVectorGetX(len);
		}

		F32 Vector3::LengthSquared() const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR len = DX::XMVector3LengthSq(v1);
			return DX::XMVectorGetX(len);
		}

		F32 Vector3::Dot(const Vector3& vec) const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			XMVECTOR_LOAD(v2, &vec);
			DX::XMVECTOR dot = DX::XMVector3Dot(v1, v2);
			return DX::XMVectorGetX(dot);
		}

		Vector3 Vector3::Cross(const Vector3 vec) const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			XMVECTOR_LOAD(v2, &vec);
			DX::XMVECTOR cross = DX::XMVector3Cross(v1, v2);
			Vector3 result;
			DX::XMStoreFloat3(&result, cross);
			return result;
		}

		void Vector3::Normalize() const noexcept
		{
			XMVECTOR_LOAD(v1, this);
			DX::XMVECTOR norm = DX::XMVector3Normalize(v1);
			//DX::XMStoreFloat3(this, norm);
		}

		void Vector3::Clamp(const Vector3& vecMin, const Vector3& vecMax)
		{
		}

		F32 Vector3::Distance(const Vector3& v1, const Vector3& v2) noexcept
		{
			return F32();
		}

		F32 Vector3::DistanceSquared(const Vector3& v1, const Vector3& v2) noexcept
		{
			return F32();
		}

		Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, F32 t) noexcept
		{
			return Vector3();
		}

		Vector3 Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, F32 step)
		{
			return Vector3();
		}

		Vector3 Vector3::Reflect(const Vector3& inVec, const Vector3& inNormal)
		{
			return Vector3();
		}

		Vector3 Vector3::Transform(const Vector3& vec, const Matrix& mat) noexcept
		{
			return Vector3();
		}

		Vector3 Vector3::TransformNormal(const Vector3& vec, const Matrix& mat)
		{
			return Vector3();
		}
	}
}
