#pragma once
#include "MathDefines.h"
#include <DirectXMath.h>
namespace DX = DirectX;


// Inspired from unity's vector math library

namespace Proximity
{
	namespace Math
	{
		struct Matrix;
		struct Quaternion;

		struct Vector3 : public DX::XMFLOAT3
		{
			Vector3() noexcept
				: DX::XMFLOAT3(0.0f, 0.0f, 0.f) {}
			Vector3(F32 _x, F32 _y, F32 _z)
				: DX::XMFLOAT3(_x, _y, _z) {}
			Vector3(F32 num) noexcept
				: DX::XMFLOAT3(num, num, num) {}
			Vector3(DX::FXMVECTOR vec) noexcept { DX::XMStoreFloat3(this, vec); }
			Vector3(const XMFLOAT3& vec) noexcept
			{
				this->x = vec.x;
				this->y = vec.y;
				this->z = vec.z;
			}

			Vector3(const Vector3&) = default;
			Vector3& operator=(const Vector3&) = default;

			Vector3(Vector3&&) = default;
			Vector3& operator=(Vector3&&) = default;

			operator DX::XMVECTOR() const noexcept { return DX::XMLoadFloat3(this); }

			// Operators
			bool operator == (const Vector3& vec) const noexcept;
			bool operator != (const Vector3& vec) const noexcept;

			Vector3& operator = (const DX::XMVECTORF32& vec) noexcept
			{
				x = vec.f[0];
				y = vec.f[1];
				z = vec.f[2];
				return *this;
			}

			Vector3& operator += (const Vector3& vec) noexcept;
			Vector3& operator -= (const Vector3& vec) noexcept;
			Vector3& operator *= (const Vector3& vec) noexcept;
			Vector3& operator *= (const F32 number) noexcept;
			Vector3& operator /= (const F32 number) noexcept;

			Vector3 operator + () const noexcept { return *this; }
			Vector3 operator - () const noexcept;

			

			
			F32 Length() const noexcept;
			F32 LengthSquared() const noexcept;

			F32 Dot(const Vector3& vec) const noexcept;
			Vector3 Cross(const Vector3 vec) const noexcept;

			// Clamp this vector in range
			void Clamp(const Vector3& vecMin, const Vector3& vecMax);



			static Vector3 Normalize(const Vector3& v)noexcept;
			static F32 Distance(const Vector3& v1, const Vector3& v2) noexcept;
			static F32 DistanceSquared(const Vector3& v1, const Vector3& v2) noexcept;

			static Vector3 Lerp(const Vector3& v1, const Vector3& v2, F32 t) noexcept;
			static Vector3 SmoothStep(const Vector3& v1, const Vector3& v2, F32 step);
			static Vector3 Reflect(const Vector3& inVec, const Vector3& inNormal);

			static Vector3 Transform(const Vector3& vec, const Matrix& mat) noexcept;
			static Vector3 Transform(const Vector3& vec, const Quaternion& quat) noexcept;
			static Vector3 TransformNormal(const Vector3& vec, const Matrix& mat);

			static const Vector3 Zero;
			static const Vector3 One;
			static const Vector3 Up;
			static const Vector3 Down;
			static const Vector3 Left;
			static const Vector3 Right;
			static const Vector3 Forward;
			static const Vector3 Backward;


			// NOTE: Other possible functions
			// - Min / Max
			// - Refraction
			// - Transform with quaternions
		};

		Vector3 operator+ (const Vector3& v1, const Vector3& v2) noexcept;
		Vector3 operator- (const Vector3& v1, const Vector3& v2) noexcept;
		Vector3 operator* (const Vector3& v1, const Vector3& v2) noexcept;
		Vector3 operator* (const Vector3& v1, const F32& f)      noexcept;
		Vector3 operator/ (const Vector3& v1, const Vector3& v2) noexcept;
		Vector3 operator/ (const Vector3& v1, const F32& f)      noexcept;
		Vector3 operator* (const F32& f,      const Vector3& v1) noexcept;

		typedef Vector3 Vec3;
		typedef Vector3 vec3;
	}
}