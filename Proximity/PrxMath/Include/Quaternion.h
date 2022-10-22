#pragma once
#include "MathDefines.h"
#include <DirectXMath.h>
namespace DX = DirectX;

namespace Proximity
{
	namespace Math
	{
		struct Matrix;
		struct Vector3;

		// Reference from https://docs.unity3d.com/ScriptReference/Quaternion.html
		struct Quaternion : public DX::XMFLOAT4
		{
			Quaternion() noexcept
				: DX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
			{}
			Quaternion(F32 x, F32 y, F32 z, F32 w) noexcept
				: DX::XMFLOAT4(x, y, z, w)
			{}
			Quaternion(const Vector3& v, F32 w) noexcept;
			Quaternion(const DX::XMFLOAT4& q) noexcept { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; }
			
			Quaternion(const Quaternion&) = default;
			Quaternion& operator=(const Quaternion&) = default;

			Quaternion(Quaternion&&) = default;
			Quaternion& operator=(Quaternion&&) = default;

			operator DX::XMVECTOR() const noexcept { return DX::XMLoadFloat4(this); }

			Quaternion& operator=  (const DX::XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
			Quaternion& operator+= (const Quaternion& q) noexcept;
			Quaternion& operator-= (const Quaternion& q) noexcept;
			Quaternion& operator*= (const Quaternion& q) noexcept;
			Quaternion& operator*= (F32 f) noexcept;
			Quaternion& operator/= (const Quaternion& q) noexcept;

			Quaternion operator+ () const  noexcept { return *this; }
			Quaternion operator- () const noexcept;

			F32 Length() const noexcept;
			F32 LengthSquared() const noexcept;
			F32 Dot(const Quaternion& q) const noexcept;

			static Quaternion Normalize(const Quaternion& q) noexcept;
			static Quaternion FromAxisAngle(const Vector3& axis, F32 angle) noexcept;
			static Quaternion FromVector(const Vector3 rot) noexcept;
			static Quaternion FromVector(F32 x, F32 y, F32 z) noexcept;
			static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, F32 t) noexcept;

			static const Quaternion Identity;
		};

		Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2) noexcept;
		Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2) noexcept;
		Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2) noexcept;
		Quaternion operator* (const Quaternion& Q, F32 f) noexcept;
		Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2) noexcept;
		Quaternion operator* (F32 f, const Quaternion& Q) noexcept;

	}
}