#include "Vector3.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MathUtils.h"
#include <stdexcept>

namespace Proximity
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
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorAdd(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator-=(const Vector3& vec) noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorAdd(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator*=(const Vector3& vec) noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vec);
			DX::XMVECTOR result = DX::XMVectorMultiply(v1, v2);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator*=(const F32 number) noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			DX::XMVECTOR result = DX::XMVectorScale(v1, number);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3& Vector3::operator/=(const F32 number) noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			DX::XMVECTOR result = DX::XMVectorScale(v1, 1.0f / number);
			DX::XMStoreFloat3(this, result);
			return *this;
		}
		Vector3 Vector3::operator-() const noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			DX::XMVECTOR result = DX::XMVectorNegate(v1);
			Vector3 out;
			DX::XMStoreFloat3(&out, result);
			return out;
		}

		F32 Vector3::Length() const noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			DX::XMVECTOR len = DX::XMVector3Length(v1);
			return DX::XMVectorGetX(len);
		}

		F32 Vector3::LengthSquared() const noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			DX::XMVECTOR len = DX::XMVector3LengthSq(v1);
			return DX::XMVectorGetX(len);
		}

		F32 Vector3::Dot(const Vector3& vec) const noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vec);
			DX::XMVECTOR dot = DX::XMVector3Dot(v1, v2);
			return DX::XMVectorGetX(dot);
		}

		Vector3 Vector3::Cross(const Vector3 vec) const noexcept
		{
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vec);
			DX::XMVECTOR cross = DX::XMVector3Cross(v1, v2);
			Vector3 result;
			DX::XMStoreFloat3(&result, cross);
			return result;
		}
		void Vector3::Clamp(const Vector3& vecMin, const Vector3& vecMax)
		{
			XMVECTOR_LOAD3(v1, this);
			XMVECTOR_LOAD3(v2, &vecMin);
			XMVECTOR_LOAD3(v3, &vecMax);
			DX::XMVECTOR clamped = DX::XMVectorClamp(v1, v2, v3);
			DX::XMStoreFloat3(this, clamped);
		}

		Vector3 Vector3::Normalize(const Vector3& v) noexcept
		{
			XMVECTOR_LOAD3(vec, &v);
			DX::XMVECTOR norm = DX::XMVector3Normalize(vec);
			Vector3 result;
			DX::XMStoreFloat3(&result, norm);
			return result;
		}

		F32 Vector3::Distance(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR sub = DX::XMVectorSubtract(vec1, vec2);
			DX::XMVECTOR len = DX::XMVector3Length(sub);
			return DX::XMVectorGetX(len);
		}

		F32 Vector3::DistanceSquared(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR sub = DX::XMVectorSubtract(vec1, vec2);
			DX::XMVECTOR len = DX::XMVector3LengthSq(sub);
			return DX::XMVectorGetX(len);
		}

		Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, F32 t) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR lerp = DX::XMVectorLerp(vec1, vec2, t);
			Vector3 result;
			DX::XMStoreFloat3(&result, lerp);
			return result;
		}

		Vector3 Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, F32 step)
		{
			step = Utils::Clamp01(step);
			step = step * step * (3.0f - 2.0f * step);  // Smoothing equation
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR lerp = DX::XMVectorLerp(vec1, vec2, step);
			Vector3 result;
			DX::XMStoreFloat3(&result, lerp);
			return result;
		}

		Vector3 Vector3::Reflect(const Vector3& inVec, const Vector3& inNormal)
		{
			XMVECTOR_LOAD3(i, &inVec);
			XMVECTOR_LOAD3(n, &inNormal);
			DX::XMVECTOR ref = DX::XMVector3Reflect(i, n);
			Vector3 result;
			DX::XMStoreFloat3(&result, ref);
			return result;
		}

		Vector3 Vector3::Transform(const Vector3& vec, const Matrix& mat) noexcept
		{
			XMVECTOR_LOAD3(v, &vec);
			XMMATRIX_LOAD(m, &mat);
			DX::XMVECTOR transform = DX::XMVector3TransformCoord(v, m);
			Vector3 result;
			DX::XMStoreFloat3(&result, transform);
			return result;
		}

		Vector3 Vector3::Transform(const Vector3& vec, const Quaternion& quat) noexcept
		{
			XMVECTOR_LOAD3(v, &vec);
			XMVECTOR_LOAD4(q, &quat);
			DX::XMVECTOR rot = DX::XMVector3Rotate(v, q);
			Vector3 result;
			DX::XMStoreFloat3(&result, rot);
			return result;
		}

		Vector3 Vector3::TransformNormal(const Vector3& vec, const Matrix& mat)
		{
			XMVECTOR_LOAD3(v, &vec);
			XMMATRIX_LOAD(m, &mat);
			DX::XMVECTOR transform = DX::XMVector3TransformNormal(v, m);
			Vector3 result;
			DX::XMStoreFloat3(&result, transform);
			return result;
		}



		Vector3 operator+(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR result = DX::XMVectorAdd(vec1, vec2);
			Vector3 vecResult; 
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator-(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR result = DX::XMVectorSubtract(vec1, vec2);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator*(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR result = DX::XMVectorMultiply(vec1, vec2);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator*(const Vector3& v1, const F32& f) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			DX::XMVECTOR result = DX::XMVectorScale(vec1, f);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator/(const Vector3& v1, const Vector3& v2) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			XMVECTOR_LOAD3(vec2, &v2);
			DX::XMVECTOR result = DX::XMVectorDivide(vec1, vec2);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator/(const Vector3& v1, const F32& f) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			DX::XMVECTOR result = DX::XMVectorScale(vec1, 1.0f / f);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}

		Vector3 operator*(const F32& f, const Vector3& v1) noexcept
		{
			XMVECTOR_LOAD3(vec1, &v1);
			DX::XMVECTOR result = DX::XMVectorScale(vec1, f);
			Vector3 vecResult;
			DX::XMStoreFloat3(&vecResult, result);
			return vecResult;
		}
}
}
