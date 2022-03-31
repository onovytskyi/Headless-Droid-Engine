#include "Config/Bootstrap.h"

#include "Framework/System/Timer.h"

#include "Debug/Assert.h"

namespace hd
{
    namespace sys
    {
        Timer::Timer()
            : m_DeltaSeconds{}
            , m_GameTimeUs{}
            , m_RealTimeUs{}
            , m_TimeDilation{ 1.0f }
            , m_Paused{}
            , m_GameDeltaUsAccumulator{}
            , m_LastTime{ std::chrono::steady_clock::now() }
        {

        }

        void Timer::Tick()
        {
            auto currentTime{ std::chrono::steady_clock::now() };
            auto realDuration{ std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_LastTime) };
            m_LastTime = currentTime;

            uint64_t realDeltaUs = realDuration.count();

            m_GameDeltaUsAccumulator += IsPaused() ? 0 : realDeltaUs;
            uint64_t gameDeltaUs = (uint64_t)std::floor(m_GameDeltaUsAccumulator * m_TimeDilation);
            m_GameDeltaUsAccumulator -= gameDeltaUs;

            m_DeltaSeconds = gameDeltaUs / 1000000.0f;
            m_RealTimeUs += realDeltaUs;
            m_GameTimeUs += gameDeltaUs;
        }

        float Timer::GetDeltaSeconds() const
        {
            return m_DeltaSeconds;
        }

        uint64_t Timer::GetGameTimeUs() const
        {
            return m_GameTimeUs;
        }

        uint64_t Timer::GetRealTimeUs() const
        {
            return m_RealTimeUs;
        }

        float Timer::GetTimeDilation() const
        {
            return m_TimeDilation;
        }

        void Timer::SetTimeDilation(float dilation)
        {
            m_TimeDilation = dilation;
        }

        bool Timer::IsPaused() const
        {
            return m_Paused > 0;
        }

        void Timer::Pause()
        {
            m_Paused += 1;
        }

        void Timer::Unpause()
        {
            hdAssert(m_Paused > 0, u8"Trying to unpause timer that is not paused.");
            m_Paused -= 1;
        }
    }
}