#pragma once
#include <afxwin.h>

class HeartRateInfo: public CStatic
{
public:
	DECLARE_DYNAMIC( HeartRateInfo );

public:
	HeartRateInfo();

private:
	HeartRateInfo( HeartRateInfo & ) = delete;
	HeartRateInfo & operator=( const HeartRateInfo & ) = delete;

public:
	void SetHeartRate( int hr );
	void ShowHeart( bool show );

	CSize GetControlSize() const;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );

private:
	CImage _buffer;
	CImage _digits;

	int _heartrate;
	int _hrMax;
	int _hrAvg;
	int _hrMin;
	bool _showHeart;

	static const int digitBigW = 72;
	static const int digitBigH = 128;
	static const int digitBigX = 0;
	static const int digitBigY = 0;
};

