#pragma once

#include "Engine/Framework/Math/Math.h"

namespace hd
{
    namespace sys
    {
        class Timer;
    }

    namespace scene
    {
        class FpsCamera
        {
        public:
            FpsCamera();
            ~FpsCamera();

            void SetFPSCameraLens(float fovY, float aspect, float near, float far);

            void OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed);
            void OnKeyboardKey(uint8_t keyID, bool pressed);
            void UpdateCamera(sys::Timer& gameTimer);

            void SetPosition(math::Vectorf3 const& position);
            void SetRotation(math::Vectorf3 const& rotation);

            math::Vectorf3 GetPosition();

            math::Matrix4x4 GetViewMatrix();
            math::Matrix4x4 GetProjectionMatrix();
            math::Matrix4x4 GetViewProjectionMatrix();

        private:
            void MoveForward(float deltaOffset);
            void MoveRight(float deltaOffset);
            void Rotate(float yaw, float pitch, float roll);

            math::Vectorf3 GetForwardVector();
            math::Vectorf3 GetRightVector();
            math::Vectorf3 GetUpVector();

            void UpdateViewMatrix();
            void UpdateProjectionMatrix();

            float m_RotationRate;
            float m_MaxSpeed;

            bool m_IsRotating;
            int32_t m_LastMouseX;
            int32_t m_LastMouseY;

            bool m_IsBoosted;
            math::Vectorf2 m_Velocity;

            math::Vectorf3 m_Position;
            math::Vectorf3 m_Rotation;
            math::Vectorf3 m_Up;

            float m_FovY;
            float m_FovX;
            float m_Aspect;
            float m_NearClip;
            float m_FarClip;

            float m_NearPlaneHeight;
            float m_FarPlaneHeight;

            math::Matrix4x4 m_View;
            math::Matrix4x4 m_Projection;

            bool m_ViewMatrixDirty;
            bool m_ProjectionMatrixDirty;
        };
    }
}