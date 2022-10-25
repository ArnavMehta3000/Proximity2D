#include "Math/Matrix.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace Proximity
{
	namespace Math
	{
		const Matrix Matrix::Identity = Matrix();

		Matrix::Matrix(const Vector3& v1, const Vector3& v2, const Vector3& v3) noexcept
			: XMFLOAT4X4
			(
				v1.x, v1.y, v1.z, 0.0f,
				v2.x, v2.y, v2.z, 0.0f,
				v3.x, v3.y, v3.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			)
		{}

		Matrix& Matrix::operator=(const DX::XMFLOAT3X3& rhs) noexcept
		{
			_11 = rhs._11; _12 = rhs._12; _13 = rhs._13; _14 = 0.f;
			_21 = rhs._21; _22 = rhs._22; _23 = rhs._23; _24 = 0.f;
			_31 = rhs._31; _32 = rhs._32; _33 = rhs._33; _34 = 0.f;
			_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
			return *this;
		}

		Matrix& Matrix::operator=(const DX::XMFLOAT4X3& rhs) noexcept
		{
			_11 = rhs._11; _12 = rhs._12; _13 = rhs._13; _14 = 0.f;
			_21 = rhs._21; _22 = rhs._22; _23 = rhs._23; _24 = 0.f;
			_31 = rhs._31; _32 = rhs._32; _33 = rhs._33; _34 = 0.f;
			_41 = rhs._41; _42 = rhs._42; _43 = rhs._43; _44 = 1.f;
			return *this;
		}

		Matrix& Matrix::operator*=(const Matrix& rhs) noexcept
		{
			XMMATRIX_LOAD(m1, this);
			XMMATRIX_LOAD(m2, &rhs);
			DX::XMMATRIX result = DX::XMMatrixMultiply(m1, m2);
			DX::XMStoreFloat4x4(this, result);
			return *this;
		}

		Matrix& Matrix::operator*=(F32 rhs) noexcept
		{
			auto [v1, v2, v3, v4] = MatrixToVectors(*this);

			auto r1 = DX::XMVectorScale(v1, rhs);
			auto r2 = DX::XMVectorScale(v2, rhs);
			auto r3 = DX::XMVectorScale(v3, rhs);
			auto r4 = DX::XMVectorScale(v4, rhs);

			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_41), r4);
			return *this;
		}

		Matrix& Matrix::operator/=(F32 rhs) noexcept
		{
			assert(rhs != 0.0f);

			auto [v1, v2, v3, v4] = MatrixToVectors(*this);

			F32 value = 1.0f / rhs;

			auto r1 = DX::XMVectorScale(v1, value);
			auto r2 = DX::XMVectorScale(v2, value);
			auto r3 = DX::XMVectorScale(v3, value);
			auto r4 = DX::XMVectorScale(v4, value);

			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_41), r4);
			return *this;
		}

		Matrix& Matrix::operator/=(const Matrix& rhs) noexcept
		{
			auto [x1, x2, x3, x4] = MatrixToVectors(*this);
			auto [y1, y2, y3, y4] = MatrixToVectors(rhs);

			auto r1 = DX::XMVectorDivide(x1, y1);
			auto r2 = DX::XMVectorDivide(x2, y2);
			auto r3 = DX::XMVectorDivide(x3, y3);
			auto r4 = DX::XMVectorDivide(x4, y4);

			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&_41), r4);
			return *this;
		}

		Matrix Matrix::operator-() const noexcept
		{
			auto [v1, v2, v3, v4] = MatrixToVectors(*this);

			auto r1 = DX::XMVectorNegate(v1);
			auto r2 = DX::XMVectorNegate(v2);
			auto r3 = DX::XMVectorNegate(v3);
			auto r4 = DX::XMVectorNegate(v4);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix Matrix::Transpose() const noexcept
		{
			XMMATRIX_LOAD(m, this);
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixTranspose(m));
			return result;
		}

		Matrix Matrix::Invert() const noexcept
		{
			XMMATRIX_LOAD(m, this);
			Matrix result;
			DX::XMVECTOR det;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixInverse(&det, m));
			return result;
		}

		F32 Matrix::Determinant() const noexcept
		{
			XMMATRIX_LOAD(m, this);
			float det = DX::XMVectorGetX(DX::XMMatrixDeterminant(m));
			return det;
		}

		bool Matrix::Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) const noexcept
		{
			DX::XMVECTOR s, r, t;

			if (!DX::XMMatrixDecompose(&s, &r, &t, *this))
				return false;

			DX::XMStoreFloat3(&scale, s);
			DX::XMStoreFloat4(&rotation, r); 
			DX::XMStoreFloat3(&translation, t);
			return true;
		}

		Matrix Matrix::FromQuaternion(const Quaternion& quat)
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixRotationQuaternion(quat));
			return result;
		}

		Matrix Matrix::Translation(const Vector3 pos) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixTranslation(pos.x, pos.y, pos.z));
			return result;
		}

		Matrix Matrix::Translation(F32 x, F32 y, F32 z) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixTranslation(x, y, z));
			return result;
		}

		Matrix Matrix::Scale(const Vector3 scale) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixScaling(scale.x, scale.y, scale.z));
			return result;
		}

		Matrix Matrix::Scale(F32 x, F32 y, F32 z) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixScaling(x, y, z));
			return result;
		}

		Matrix Matrix::RotationX(float rad) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixRotationX(rad));
			return result;
		}

		Matrix Matrix::RotationY(float rad) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixRotationY(rad));
			return result;
		}

		Matrix Matrix::RotationZ(float rad) noexcept
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixRotationZ(rad));
			return result;
		}

		Matrix Matrix::Rotation(const Vector3& rot)
		{
			Matrix result;
			DX::XMStoreFloat4x4(&result, DX::XMMatrixRotationRollPitchYawFromVector(rot));
			return result;
		}


		Matrix::MatrixVectors Matrix::MatrixToVectors(const Matrix& mat) noexcept
		{
			// Reinterpret cast the entire row as a vector
			XMVECTOR_LOAD4(v1, reinterpret_cast<const DX::XMFLOAT4*>(&mat._11));
			XMVECTOR_LOAD4(v2, reinterpret_cast<const DX::XMFLOAT4*>(&mat._21));
			XMVECTOR_LOAD4(v3, reinterpret_cast<const DX::XMFLOAT4*>(&mat._31));
			XMVECTOR_LOAD4(v4, reinterpret_cast<const DX::XMFLOAT4*>(&mat._41));

			return std::make_tuple(v1, v2, v3, v4);
		}


		Matrix operator+(const Matrix& m1, const Matrix& m2) noexcept
		{
			auto [x1, x2, x3, x4] = Matrix::MatrixToVectors(m1);
			auto [y1, y2, y3, y4] = Matrix::MatrixToVectors(m2);

			auto r1 = DX::XMVectorAdd(x1, y1);
			auto r2 = DX::XMVectorAdd(x2, y2);
			auto r3 = DX::XMVectorAdd(x3, y3);
			auto r4 = DX::XMVectorAdd(x4, y4);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix operator-(const Matrix& m1, const Matrix& m2) noexcept
		{
			auto [x1, x2, x3, x4] = Matrix::MatrixToVectors(m1);
			auto [y1, y2, y3, y4] = Matrix::MatrixToVectors(m2);

			auto r1 = DX::XMVectorSubtract(x1, y1);
			auto r2 = DX::XMVectorSubtract(x2, y2);
			auto r3 = DX::XMVectorSubtract(x3, y3);
			auto r4 = DX::XMVectorSubtract(x4, y4);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix operator*(const Matrix& m1, const Matrix& m2) noexcept
		{
			XMMATRIX_LOAD(mat1, &m1);
			XMMATRIX_LOAD(mat2, &m2);
			DX::XMMATRIX mul = DX::XMMatrixMultiply(mat1, mat2);

			Matrix result;
			DX::XMStoreFloat4x4(&result, mul);
			return result;
		}

		Matrix operator*(const Matrix& m, F32 f) noexcept
		{
			auto [v1, v2, v3, v4] = Matrix::MatrixToVectors(m);

			auto r1 = DX::XMVectorScale(v1, f);
			auto r2 = DX::XMVectorScale(v2, f);
			auto r3 = DX::XMVectorScale(v3, f);
			auto r4 = DX::XMVectorScale(v4, f);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix operator/(const Matrix& m, F32 f) noexcept
		{
			auto [v1, v2, v3, v4] = Matrix::MatrixToVectors(m);

			F32 value = 1.0f / f;

			auto r1 = DX::XMVectorScale(v1, value);
			auto r2 = DX::XMVectorScale(v2, value);
			auto r3 = DX::XMVectorScale(v3, value);
			auto r4 = DX::XMVectorScale(v4, value);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix operator/(const Matrix& m1, const Matrix& m2) noexcept
		{
			auto [x1, x2, x3, x4] = Matrix::MatrixToVectors(m1);
			auto [y1, y2, y3, y4] = Matrix::MatrixToVectors(m2);

			auto r1 = DX::XMVectorDivide(x1, y1);
			auto r2 = DX::XMVectorDivide(x2, y2);
			auto r3 = DX::XMVectorDivide(x3, y3);
			auto r4 = DX::XMVectorDivide(x4, y4);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}

		Matrix operator*(F32 f, const Matrix& m) noexcept
		{
			auto [v1, v2, v3, v4] = Matrix::MatrixToVectors(m);


			auto r1 = DX::XMVectorScale(v1, f);
			auto r2 = DX::XMVectorScale(v2, f);
			auto r3 = DX::XMVectorScale(v3, f);
			auto r4 = DX::XMVectorScale(v4, f);

			Matrix result;
			// Reverse reinterpret the entire vector as row
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._11), r1);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._21), r2);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._31), r3);
			DX::XMStoreFloat4(reinterpret_cast<DX::XMFLOAT4*>(&result._41), r4);
			return result;
		}
	}
}