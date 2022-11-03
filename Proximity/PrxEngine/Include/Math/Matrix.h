#pragma once
#include "MathDefines.h"
#include <DirectXMath.h>
#include <tuple>
namespace DX = DirectX;

namespace Proximity
{
	namespace Math
	{
		struct Vector3;
		struct Quaternion;


		struct Matrix : public DX::XMFLOAT4X4
		{
			typedef std::tuple<DX::XMVECTOR, DX::XMVECTOR, DX::XMVECTOR, DX::XMVECTOR> MatrixVectors;

			// Creates identity matrix
			Matrix() noexcept
				: XMFLOAT4X4
				(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				)
			{}
			Matrix(const XMFLOAT4X4& M) noexcept { memcpy(this, &M, sizeof(XMFLOAT4X4)); }
			Matrix(const Vector3& v1, const Vector3& v2, const Vector3& v3) noexcept;
			Matrix(const DX::XMMATRIX& M) noexcept { DX::XMStoreFloat4x4(this, M); }
			
			Matrix(const Matrix&) = default;
			Matrix& operator =(const Matrix&) = default;

			Matrix(Matrix&&) = default;
			Matrix& operator=(Matrix&&) = default;

			Matrix& operator=  (const DX::XMFLOAT3X3& rhs) noexcept;
			Matrix& operator=  (const DX::XMFLOAT4X3& rhs) noexcept;
			Matrix& operator+= (const Matrix& rhs) noexcept;  // NOTE: Not implemented
			Matrix& operator-= (const Matrix& rhs) noexcept;  // NOTE: Not implemented
			Matrix& operator*= (const Matrix& rhs) noexcept;
			Matrix& operator*= (F32 rhs) noexcept;
			Matrix& operator/= (F32 rhs) noexcept;
			Matrix& operator/= (const Matrix& rhs) noexcept;  // Per element division

			Matrix operator+ () const noexcept { return *this; }
			Matrix operator- () const noexcept;

			operator DX::XMMATRIX() const noexcept { return DX::XMLoadFloat4x4(this); }

			
			Matrix Transpose() const noexcept;
			Matrix Invert() const noexcept;
			float Determinant() const noexcept;
			bool Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) const noexcept;
			


			static Matrix FromQuaternion(const Quaternion& quat);
			
			static Matrix Translation(const Vector3 pos) noexcept;
			static Matrix Translation(F32 x, F32 y, F32 z) noexcept;
			
			static Matrix Scale(const Vector3 scale) noexcept;
			static Matrix Scale(F32 x, F32 y, F32 z) noexcept;

			static Matrix RotationX(float rad) noexcept;
			static Matrix RotationY(float rad) noexcept;
			static Matrix RotationZ(float rad) noexcept;

			static Matrix Rotation(const Vector3& rot);


			static const Matrix Identity;

			friend Matrix operator+ (const Matrix& m1, const Matrix& m2) noexcept;
			friend Matrix operator- (const Matrix& m1, const Matrix& m2) noexcept;
			friend Matrix operator* (const Matrix& m1, const Matrix& m2) noexcept;
			friend Matrix operator* (const Matrix& m, F32 f) noexcept;
			friend Matrix operator/ (const Matrix& m, F32 f) noexcept;
			friend Matrix operator/ (const Matrix& m1, const Matrix& m2) noexcept;
			friend Matrix operator* (F32 f, const Matrix& m) noexcept;
			

			static MatrixVectors MatrixToVectors(const Matrix& mat) noexcept;
		};

		
	}
}