#pragma once
#include <afxwin.h>
enum
{
	HUD_WIDTH = 160,
	HUD_HEIGHT = 246,
};

class DoomHead: public CStatic
{
public:
	DECLARE_DYNAMIC( DoomHead );

	DoomHead();

public:
	void SetHeartRate( int hr );

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );

	void NextFrame();

	void DrawHead( HDC hdc );
	void DrawHR( HDC hdc );

private:
	CImage _buffer;
	CImage _sprites;

	int _headIdx;
	int _frameIdx;
	int _step;

	int _heartRate;
};

