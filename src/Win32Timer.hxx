#ifndef WIN32TIMER_HXX
#define WIN32TIMER_HXX

#ifndef _WIN32
#	error	This file should only be used in a Microsoft Windows build.
#endif

#include "Timer.hxx"

class Win32Timer : public Timer
{
	private:
		//	Time frequency
		__int64 m_frequency;

		//	Timer resolution
		float m_resolution;

		//	Multimedia timer variables for start times
		unsigned m_mmTimerStart;

		//	Performance counter timer variables for start times
		__int64 m_pcTimerStart;

		//	Using performance counter timer or multimedia timer?
		bool m_usePerformanceCounter;

		//	How many times the timer has been paused successively
		int m_pauseCount;

		//	When the timer was paused
		double m_pauseTime;

	public:
		/*	Constructor. */
		Win32Timer();

		/*	Reset the timer to zero. */
		void reset();

		/*	Retrieve the time in milliseconds since the timer started. */
		double getTime() const;

		/*	Pause this timer. */
		virtual void pause();

		/*	Unpause this timer. */
		virtual void unpause();
};

#endif
