#include "pch.h"
#include "DoomHead.h"
#include "resource.h"


void LoadPng( CImage & img, UINT id )
{
	CPngImage pngImage;
	pngImage.Load( id );
	img.Attach( (HBITMAP)pngImage.Detach() );
}


IMPLEMENT_DYNAMIC( DoomHead, CStatic )

BEGIN_MESSAGE_MAP( DoomHead, CStatic )
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

DoomHead::DoomHead()
	: _headIdx( 0 )
	, _frameIdx( 0 )
	, _step( 1 )
	, _heartRate( 0 )
{
	_buffer.Create( HUD_WIDTH, HUD_HEIGHT, 24, BI_RGB );

	LoadPng( _sprites, IDB_SPRITES );
}

void DoomHead::OnPaint()
{
	CPaintDC pdc( this );
	HDC hdc = _buffer.GetDC();
	//CDC * dc = CDC::FromHandle( hdc );

	_sprites.BitBlt( hdc, 0, 0, HUD_WIDTH, HUD_HEIGHT, 315, 0, SRCCOPY );

	DrawHead( hdc );
	DrawHR( hdc );


	_buffer.ReleaseDC();
	_buffer.BitBlt( pdc.GetSafeHdc(), 0, 0, SRCCOPY );
}

BOOL DoomHead::OnEraseBkgnd( CDC * pDC )
{
	return FALSE;
}

void DoomHead::NextFrame()
{
	_frameIdx += _step;
	if( _frameIdx != 1 )
		_step = -_step;
}

void DoomHead::SetHeartRate( int hr )
{
	_heartRate = hr;

	_headIdx = (_heartRate / 10) - 7;
	if( _headIdx < 0 )
		_headIdx = 0;
	if( _headIdx > 9 )
		_headIdx = 9;

	Invalidate( FALSE );

	NextFrame();
}

void DoomHead::DrawHead( HDC hdc )
{
	enum { HEAD_W = 105, HEAD_H = 123 };

	const int fromX = HEAD_W * _frameIdx;
	const int fromY = HEAD_H * _headIdx;

	_sprites.BitBlt( hdc, 27, 19, HEAD_W, HEAD_H, fromX, fromY, SRCCOPY );
}

void DoomHead::DrawHR( HDC hdc )
{
	enum { D_W = 65, D_H = 56, DX = 315, DY = 246, CY = 180 };

	const int d1 = _heartRate % 10;
	const int d2 = (_heartRate / 10) % 10;
	const int d3 = (_heartRate / 100) % 10;

	int cx = (d3 == 0) ? 160 - 15 : (160 - 3 );

	if( d3 )
	{
		int c = cx - D_W - D_W - D_W/2;
		_sprites.BitBlt( hdc, c, CY, D_W, D_H, DX, DY + D_H *d3, SRCCOPY );
	}

	cx -= d3 ? 60 : D_W;
	_sprites.BitBlt( hdc, cx, CY, D_W, D_H, DX, DY + D_H * d1, SRCCOPY );

	cx -= d3 ? 55 : D_W;
	_sprites.BitBlt( hdc, cx, CY, D_W, D_H, DX, DY + D_H *d2, SRCCOPY );


}

