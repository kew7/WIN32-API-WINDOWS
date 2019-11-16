#ifndef QTIMER_H
#define QTIMER_H

#pragma warning(disable : 4035)

typedef enum { Msec, Usec, Ns_100, Ns_10, Nsec } TimeUnit;

class QTimer {

	LARGE_INTEGER freq;
	LARGE_INTEGER count_beg;
	LARGE_INTEGER count_end;

	LONGLONG ll_c_beg;
	LONGLONG ll_c_end;
	long d_tic;

	double dt;

	DWORD lowPart;
	LONG highPart;   
	
	// коэффициенты-делители
	double nCyclePer1nanoSec;
	double nCyclePer10nanoSec;
	double nCyclePer100nanoSec;
 	double nCyclePer1microSec;
	double nCyclePer1milliSec;
	double divizor; // текущий делитель
public:
    QTimer(void) {

		// Калибровка
		timeBeginPeriod(1);
		Sleep(20);    // !!!!!!!!!!!!!!!!!!!

		BOOL success = QueryPerformanceFrequency(&freq);
		if (!success)
			MessageBox(NULL, "Счетчик QueryPerformance не поддерживается", 0, MB_OK | MB_ICONSTOP);
		lowPart = freq.LowPart;
		highPart = freq.HighPart;

		// Калибровка делителей
		nCyclePer1nanoSec   = freq.LowPart / 1000000000.;
		nCyclePer10nanoSec  = freq.LowPart / 100000000.;
		nCyclePer100nanoSec = freq.LowPart / 10000000.;
		nCyclePer1microSec  = freq.LowPart / 1000000.;
		nCyclePer1milliSec  = freq.LowPart / 1000.;
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
    
    inline void Start(void) { QueryPerformanceCounter(&count_beg); }

    inline long GetTick(void) {

		QueryPerformanceCounter(&count_end);

		ll_c_beg = count_beg.QuadPart;
		ll_c_end = count_end.QuadPart;
		d_tic = ll_c_end - ll_c_beg;
		return d_tic;
    }

    inline double GetTime(void) {
		QueryPerformanceCounter(&count_end);
		ll_c_beg = count_beg.QuadPart;
		ll_c_end = count_end.QuadPart;
		d_tic = ll_c_end - ll_c_beg;

		return d_tic / divizor;
    }

    inline int GetTickPerUsec(void) { return (int)nCyclePer1microSec; }

	inline void uDelay(int uSec) {
		int tElapsed = 0;
		SetUnit(Usec);
		Start();
		while ( tElapsed < uSec)
			tElapsed = (int)GetTime();
	}
	
};

#endif  /* QTIMER_H */