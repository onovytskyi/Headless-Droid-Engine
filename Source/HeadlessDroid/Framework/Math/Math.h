#pragma once

#include "Framework/Math/Win64/MathWin64.h"

namespace hd
{
	Matrix4x4 MatrixIdentity();
	Matrix4x4 MatrixMultiply(Matrix4x4 const& first, Matrix4x4 const& second);
	Vectorf3 VectorMultiplyAdd(Vectorf3 const& v1, Vectorf3 const& v2, Vectorf3 const& v3);
	Matrix4x4 MatrixRotationRollPitchYaw(float pitch, float yaw, float roll);
	Matrix4x4 MatrixTranslationFromVector(Vectorf3 const& translation);
	Matrix4x4 MatrixInverse(Matrix4x4 const& matrix);
	Matrix4x4 MatrixTranspose(Matrix4x4 const& matrix);
	Matrix4x4 MatrixInverseTranspose(Matrix4x4 const& matrix);
	Matrix4x4 MatrixPerspectiveFovLH(float fov, float aspect, float nearZ, float farZ);
	float ConvertToRadians(float degrees);
}
