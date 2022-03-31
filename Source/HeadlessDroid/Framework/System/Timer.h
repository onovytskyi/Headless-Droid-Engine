#pragma once

namespace hd
{
	class Timer
	{
	public:
		Timer();

		void Tick();

		float GetDeltaSeconds() const;

		uint64_t GetGameTimeUs() const;
		uint64_t GetRealTimeUs() const;

		float GetTimeDilation() const;
		void SetTimeDilation(float dilation);

		bool IsPaused() const;
		void Pause();
		void Unpause();

	private:
		float m_DeltaSeconds;
		uint64_t m_GameTimeUs;
		uint64_t m_RealTimeUs;
		float m_TimeDilation;
		uint32_t m_Paused;

		uint64_t m_GameDeltaUsAccumulator;

		std::chrono::time_point<std::chrono::steady_clock> m_LastTime;
	};
}
