#include "Config/Bootstrap.h"

#include "Framework/Math/Math.h"

using namespace DirectX;

namespace hd
{
	Matrix4x4 MatrixIdentity()
	{
		XMMATRIX resultXM = XMMatrixIdentity();

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixMultiply(Matrix4x4 const& first, Matrix4x4 const& second)
	{
		XMMATRIX firstXM = XMLoadFloat4x4(&first);
		XMMATRIX secondXM = XMLoadFloat4x4(&second);
		XMMATRIX resultXM = XMMatrixMultiply(firstXM, secondXM);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Vectorf3 VectorMultiplyAdd(Vectorf3 const& v1, Vectorf3 const& v2, Vectorf3 const& v3)
	{
		XMVECTOR v1XM = XMLoadFloat3(&v1);
		XMVECTOR v2XM = XMLoadFloat3(&v2);
		XMVECTOR v3XM = XMLoadFloat3(&v3);  
		XMVECTOR resultXM = XMVectorMultiplyAdd(v1XM, v2XM, v3XM);

		XMFLOAT3 result;
		XMStoreFloat3(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixRotationRollPitchYaw(float pitch, float yaw, float roll)
	{
		XMMATRIX resultXM = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixTranslationFromVector(Vectorf3 const& translation)
	{
		XMVECTOR translationXM = XMLoadFloat3(&translation);
		XMMATRIX resultXM = XMMatrixTranslationFromVector(translationXM);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixInverse(Matrix4x4 const& matrix)
	{
		XMMATRIX matrixXM = XMLoadFloat4x4(&matrix);
		XMMATRIX resultXM = XMMatrixInverse(nullptr, matrixXM);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixTranspose(Matrix4x4 const& matrix)
	{
		XMMATRIX matrixXM = XMLoadFloat4x4(&matrix);
		XMMATRIX resultXM = XMMatrixTranspose(matrixXM);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixInverseTranspose(Matrix4x4 const& matrix)
	{
		XMMATRIX matrixXM = XMLoadFloat4x4(&matrix);
		XMMATRIX resultXM = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, matrixXM));

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	Matrix4x4 MatrixPerspectiveFovLH(float fov, float aspect, float nearZ, float farZ)
	{
		XMMATRIX resultXM = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, resultXM);

		return result;
	}

	float ConvertToRadians(float degrees)
	{
		return XMConvertToRadians(degrees);
	}
}
