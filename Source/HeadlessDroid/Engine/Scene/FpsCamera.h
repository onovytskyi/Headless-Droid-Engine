#pragma once

#include "Framework/Math/Math.h"

namespace hd
{
	class Timer;

    class FpsCamera
    {
    public:
        FpsCamera();
        ~FpsCamera();

        void SetFPSCameraLens(float fovY, float aspect, float near, float far);

        void OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed);
        void OnKeyboardKey(uint8_t keyID, bool pressed);
        void UpdateCamera(Timer& gameTimer);

        void SetPosition(Vectorf3 const& position);
        void SetRotation(Vectorf3 const& rotation);

        Vectorf3 GetPosition();

        Matrix4x4 GetViewMatrix();
        Matrix4x4 GetProjectionMatrix();
        Matrix4x4 GetViewProjectionMatrix();

    private:
        void MoveForward(float deltaOffset);
        void MoveRight(float deltaOffset);
        void Rotate(float yaw, float pitch, float roll);

        Vectorf3 GetForwardVector();
        Vectorf3 GetRightVector();
        Vectorf3 GetUpVector();

        void UpdateViewMatrix();
        void UpdateProjectionMatrix();

        float m_RotationRate;
        float m_MaxSpeed;

        bool m_IsRotating;
        int32_t m_LastMouseX;
        int32_t m_LastMouseY;

        bool m_IsBoosted;
        Vectorf2 m_Velocity;

        Vectorf3 m_Position;
        Vectorf3 m_Rotation;
        Vectorf3 m_Up;

        float m_FovY;
        float m_FovX;
        float m_Aspect;
        float m_NearClip;
        float m_FarClip;

        float m_NearPlaneHeight;
        float m_FarPlaneHeight;

        Matrix4x4 m_View;
        Matrix4x4 m_Projection;

        bool m_ViewMatrixDirty;
        bool m_ProjectionMatrixDirty;
    };
}