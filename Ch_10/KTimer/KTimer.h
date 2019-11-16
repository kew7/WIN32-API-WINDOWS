#ifndef KTIMER_H
#define KTIMER_H

#pragma warning(disable : 4035)

typedef enum { Msec, Usec, Ns_100, Ns_10, Nsec } TimeUnit;
static int  et[10];

inline unsigned __int64 GetCycleCount(void) {
	_asm    rdtsc
}

class KTimer {
    __int64  t_start;
    __int64  t_stop;
    int  overhead;
	// коэффициенты-делители
	double nCyclePer1nanoSec;
	double nCyclePer10nanoSec;
	double nCyclePer100nanoSec;
 	double nCyclePer1microSec;
	double nCyclePer1milliSec;
	double divizor; // текущий делитель
public:
    KTimer(void) {
		// Калибровка overhead
		overhead = 0;
		for (int i = 0; i < 10; ++i) {
			t_start = GetCycleCount();
			et[i] = GetTick();
		}
		overhead = et[0];
		for (int i = 1; i < 10; ++i)
			if (et[i] < overhead)  overhead = et[i];

		// Калибровка делителей
		timeBeginPeriod(1);
		Sleep(20);

		Start();
		Sleep(1000); 
		t_stop = GetCycleCount() - t_start - overhead;

		nCyclePer1nanoSec   = t_stop / 1000000000.;
		nCyclePer10nanoSec  = t_stop / 100000000.;
		nCyclePer100nanoSec = t_stop / 10000000.;
		nCyclePer1microSec  = t_stop / 1000000.;
		nCyclePer1milliSec  = t_stop / 1000.;
		timeEndPeriod(1);
		divizor = nCyclePer1milliSec;  // делитель по умолчанию
    }

	void SetUnit(TimeUnit unit) {
		switch (unit) {
		case Msec: divizor = nCyclePer1milliSec; break;
		case Usec: divizor = nCyclePer1microSec; break;
		case Nsec: divizor = nCyclePer1nanoSec;  break;
		case Ns_10: divizor = nCyclePer10nanoSec;  break;
		case Ns_100: divizor = nCyclePer100nanoSec;  break;
		}
	}
    
    inline void Start(void) { t_start = GetCycleCount(); }

    inline int GetTick(void) {
		return (int)(GetCycleCount() - t_start - overhead);
    }

    inline double GetTime(void) {
		t_stop = GetCycleCount() - t_start - overhead;
		return t_stop / divizor;
    }

    inline int GetTickPerUsec(void) { return (int)nCyclePer1microSec; }

	inline void uDelay(int uSec) {
		int tElapsed = 0;
		SetUnit(Usec);
		Start();
		while ( tElapsed < uSec)
			tElapsed = (int)GetTime();
	}

	int GetOverhead() {
		return overhead;
	}
};

#endif  /* KTIMER_H */