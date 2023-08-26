#pragma once
#include <afxwin.h>
#include <vector>
enum
{
	HUD_WIDTH = 282,
	HUD_HEIGHT = 75,
};

class QCHead: public CStatic
{
public:
	DECLARE_DYNAMIC( QCHead );

	QCHead();

public:
	void Init();
	void SetHeartRate( int hr );
	void Reset();

protected:
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );

	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );


	void DrawHR( HDC dc );
	void DrawDiagram( HDC dc );
	void UpdateDiagram();


private:
	CImage _buffer;

	CImage _heart;
	CImage _blend;

	std::vector<COLORREF> _redTemplate;
	int _redWidth;
	int _redHeight;

	inline COLORREF GetRed( const int& x, const int& y )const
	{
		return _redTemplate[y * _redWidth + x];
	}

	CImage _diagram;
	int _data[250];
	int _vline[250];
	DWORD _lastTime;

	int _heartRate;

	CFont _hrFont;
	CFont _lineFont;

public:
	bool _showHeart;
	virtual void PreSubclassWindow();
};

