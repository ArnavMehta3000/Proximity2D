#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix.h"

namespace Proximity
{
	namespace Math
	{
		const Quaternion Quaternion::Identity = Quaternion();

		Quaternion::Quaternion(const Vector3& v, F32 w) noexcept
			: XMFLOAT4(v.x, v.y, v.z, w)
		{}

		Quaternion& Proximity::Math::Quaternion::operator+=(const Quaternion& q) noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			XMVECTOR_LOAD4(q2, &q);
			DX::XMStoreFloat4(this, DX::XMVectorAdd(q1, q2));
			return *this;
		}

		Quaternion& Proximity::Math::Quaternion::operator-=(const Quaternion& q) noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			XMVECTOR_LOAD4(q2, &q);
			DX::XMStoreFloat4(this, DX::XMVectorSubtract(q1, q2));
			return *this;
		}

		Quaternion& Quaternion::operator*=(const Quaternion& q) noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			XMVECTOR_LOAD4(q2, &q);
			DX::XMStoreFloat4(this, DX::XMQuaternionMultiply(q1, q2));
			return *this;
		}

		Quaternion& Quaternion::operator*=(F32 f) noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			DX::XMStoreFloat4(this, DX::XMVectorScale(q1,f ));
			return *this;
		}

		Quaternion& Proximity::Math::Quaternion::operator/=(const Quaternion& q) noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			XMVECTOR_LOAD4(q2, &q);
			
			// Inverse of quaternion is same as 1/q
			q2 = DX::XMQuaternionInverse(q2);
			DX::XMStoreFloat4(this, DX::XMQuaternionMultiply(q1, q2));
			return *this;
		}

		Quaternion Quaternion::operator-() const noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMVectorNegate(q1));
			return result;
		}

		F32 Quaternion::Length() const noexcept
		{
			XMVECTOR_LOAD4(q, this);
			return DX::XMVectorGetX(DX::XMQuaternionLength(q));
		}

		F32 Quaternion::LengthSquared() const noexcept
		{
			XMVECTOR_LOAD4(q, this);
			return DX::XMVectorGetX(DX::XMQuaternionLengthSq(q));
		}

		F32 Quaternion::Dot(const Quaternion& q) const noexcept
		{
			XMVECTOR_LOAD4(q1, this);
			XMVECTOR_LOAD4(q2, &q);
			return DX::XMVectorGetX(DX::XMQuaternionDot(q1, q2));
		}

		Quaternion Quaternion::Normalize(const Quaternion& q) noexcept
		{
			XMVECTOR_LOAD4(q1, &q);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionNormalize(q1));
			return result;
		}

		Quaternion Quaternion::FromAxisAngle(const Vector3& axis, F32 angle) noexcept
		{
			XMVECTOR_LOAD3(a, &axis);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionRotationAxis(a, angle));
			return result;
		}

		Quaternion Quaternion::FromVector(const Vector3 rot) noexcept
		{
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionRotationRollPitchYawFromVector(rot));
			return result;
		}

		Quaternion Quaternion::FromVector(F32 x, F32 y, F32 z) noexcept
		{
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionRotationRollPitchYaw(x, y, z));
			return result;
		}

		Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, F32 t) noexcept
		{
			XMVECTOR_LOAD4(v1, &q1);
			XMVECTOR_LOAD4(v2, &q2);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionSlerp(v1, v2, t));
			return result;
		}


		Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q1);
			XMVECTOR_LOAD4(q2, &Q2);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMVectorAdd(q1, q2));
			return result;
		}

		Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q1);
			XMVECTOR_LOAD4(q2, &Q2);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMVectorSubtract(q1, q2));
			return result;
		}
		Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q1);
			XMVECTOR_LOAD4(q2, &Q2);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionMultiply(q1, q2));
			return result;
		}
		Quaternion operator* (const Quaternion& Q, F32 f) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMVectorScale(q1, f));
			return result;
		}

		Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q1);
			XMVECTOR_LOAD4(q2, &Q2);

			// Inverse of quaternion is same as 1/q
			q2 = DX::XMQuaternionInverse(q2);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMQuaternionMultiply(q1, q2));
			return result;
		}

		Quaternion operator* (F32 f, const Quaternion& Q) noexcept
		{
			XMVECTOR_LOAD4(q1, &Q);
			Quaternion result;
			DX::XMStoreFloat4(&result, DX::XMVectorScale(q1, f));
			return result;
		}
	}
}