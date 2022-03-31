#include "Config/Bootstrap.h"

#include "Engine/Scene/FpsCamera.h"

#include "Framework/System/Timer.h"
#include "Framework/System/Utils.h"

namespace hd
{
	FpsCamera::FpsCamera()
		: m_RotationRate{ 0.01f }
		  , m_MaxSpeed{ 100.0f }
		  , m_IsRotating{ false }
		  , m_LastMouseX { -1 }
		  , m_LastMouseY{ -1 }
		  , m_IsBoosted{ false }
		  , m_Velocity{ 0.0f, 0.0f }
		  , m_Position{ 0.0f, 0.0f, 0.0f }
		  , m_Rotation{ 0.0f, 0.0f, 0.0f }
		  , m_Up{ 0.0f, 1.0f, 0.0f }
		  , m_FovY{ 0.0f }
		  , m_FovX{ 0.0f }
		  , m_Aspect{ 0.0f }
		  , m_NearClip{ 0.0f }
		  , m_FarClip{ 0.0f }
		  , m_NearPlaneHeight{ 0.0f }
		  , m_FarPlaneHeight{ 0.0f }
		  , m_View{}
		  , m_Projection{}
		  , m_ViewMatrixDirty{ true }
		  , m_ProjectionMatrixDirty{ true }
	{
		SetFPSCameraLens(1.0f, 1.0f, 0.1f, 1.0f);
	}

	FpsCamera::~FpsCamera()
	{

	}

	void FpsCamera::SetFPSCameraLens(float fovY, float aspect, float near, float far)
	{
		m_FovY = fovY;
		m_Aspect = aspect;
		m_NearClip = near;
		m_FarClip = far;

		float halfTanY = ::tanf(m_FovY * 0.5f);
		m_NearPlaneHeight = m_NearClip * halfTanY * 2.0f;
		m_FarPlaneHeight = m_FarClip * halfTanY * 2.0f;

		m_FovX = 2.0f * ::atanf(halfTanY * m_Aspect);

		m_ProjectionMatrixDirty = true;
	}

	void FpsCamera::OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed)
	{
		if (mouseButtonId == 2)
		{
			if (pressed)
			{
				m_IsRotating = true;
				ShowCursor(false);
			}
			else
			{
				m_IsRotating = false;
				m_LastMouseX = -1;
				m_LastMouseY = -1;
				ShowCursor(true);
			}
		}
	}

	void FpsCamera::OnKeyboardKey(uint8_t keyID, bool pressed)
	{
		// #HACK windows key codes here for now. Ideally they should be converted to platform agnostic enum.
		static const uint8_t SHIFT_ID = 0x10; // VK_SHIFT key code

		if (keyID == 'W')
		{
			if (pressed)
			{
				m_Velocity.x += m_MaxSpeed;
			}
			else
			{
				m_Velocity.x -= m_MaxSpeed;
			}
		}
		else if (keyID == 'S')
		{
			if (pressed)
			{
				m_Velocity.x -= m_MaxSpeed;
			}
			else
			{
				m_Velocity.x += m_MaxSpeed;
			}
		}
		else if (keyID == 'D')
		{
			if (pressed)
			{
				m_Velocity.y += m_MaxSpeed;
			}
			else
			{
				m_Velocity.y -= m_MaxSpeed;
			}
		}
		else if (keyID == 'A')
		{
			if (pressed)
			{
				m_Velocity.y -= m_MaxSpeed;
			}
			else
			{
				m_Velocity.y += m_MaxSpeed;
			}
		}
		else if (keyID == SHIFT_ID)
		{
			if (pressed)
			{
				m_IsBoosted = true;
			}
			else
			{
				m_IsBoosted = false;
			}
		}
	}

	void FpsCamera::UpdateCamera(Timer& gameTimer)
	{
		if (m_IsRotating)
		{
			int32_t mouseX{};
			int32_t mouseY{};
			GetCursorPosition(mouseX, mouseY);

			if (m_LastMouseX != -1 && m_LastMouseY != -1)
			{
				auto deltaX{ mouseX - m_LastMouseX };
				auto deltaY{ mouseY - m_LastMouseY };

				Rotate(deltaX * m_RotationRate, deltaY * m_RotationRate, 0.0f);

				SetCursorPosition(m_LastMouseX, m_LastMouseY);
			}
			else
			{
				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;
			}
		}

		MoveForward(m_Velocity.x * gameTimer.GetDeltaSeconds() * (m_IsBoosted ? 10.0f : 1.0f));
		MoveRight(m_Velocity.y * gameTimer.GetDeltaSeconds() * (m_IsBoosted ? 10.0f : 1.0f));
	}

	void FpsCamera::SetPosition(Vectorf3 const& position)
	{
		m_Position = position;

		m_ViewMatrixDirty = true;
	}

	void FpsCamera::SetRotation(Vectorf3 const& rotation)
	{
		m_Rotation = rotation;

		m_ViewMatrixDirty = true;
	}

	Vectorf3 FpsCamera::GetPosition()
	{
		return m_Position;
	}

	Matrix4x4 FpsCamera::GetViewMatrix()
	{
		if (m_ViewMatrixDirty)
		{
			UpdateViewMatrix();
		}

		return m_View;
	}

	Matrix4x4 FpsCamera::GetProjectionMatrix()
	{
		if (m_ProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		return m_Projection;
	}

	Matrix4x4 FpsCamera::GetViewProjectionMatrix()
	{
		Matrix4x4 view = GetViewMatrix();
		Matrix4x4 projection = GetProjectionMatrix();

		return MatrixMultiply(view, projection);
	}

	void FpsCamera::MoveForward(float deltaOffset)
	{
		m_Position = VectorMultiplyAdd({ deltaOffset, deltaOffset, deltaOffset }, GetForwardVector(), m_Position);
		m_ViewMatrixDirty = true;
	}

	void FpsCamera::MoveRight(float deltaOffset)
	{
		m_Position = VectorMultiplyAdd({ deltaOffset, deltaOffset, deltaOffset }, GetRightVector(), m_Position);
		m_ViewMatrixDirty = true;
	}

	void FpsCamera::Rotate(float yaw, float pitch, float roll)
	{
		m_Rotation.x += yaw;
		m_Rotation.y += pitch;
		m_Rotation.z += roll;

		m_ViewMatrixDirty = true;
	}

	Vectorf3 FpsCamera::GetForwardVector()
	{
		if (m_ViewMatrixDirty)
		{
			UpdateViewMatrix();
		}

		return { m_View._13, m_View._23, m_View._33 };
	}

	Vectorf3 FpsCamera::GetRightVector()
	{
		if (m_ViewMatrixDirty)
		{
			UpdateViewMatrix();
		}

		return { m_View._11, m_View._21, m_View._31 };
	}

	Vectorf3 FpsCamera::GetUpVector()
	{
		if (m_ViewMatrixDirty)
		{
			UpdateViewMatrix();
		}

		return { m_View._12, m_View._22, m_View._32 };
	}

	void FpsCamera::UpdateViewMatrix()
	{
		m_View = MatrixMultiply(MatrixRotationRollPitchYaw(m_Rotation.y, m_Rotation.x, m_Rotation.z), MatrixTranslationFromVector(m_Position));
		m_View = MatrixInverse(m_View);
		m_ViewMatrixDirty = false;
	}

	void FpsCamera::UpdateProjectionMatrix()
	{
		m_Projection = MatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearClip, m_FarClip);
		m_ProjectionMatrixDirty = false;
	}

}
