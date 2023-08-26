#include "pch.h"
#include "HeartRateInfo.h"
#include "resource.h"

IMPLEMENT_DYNAMIC( HeartRateInfo, CStatic )

BEGIN_MESSAGE_MAP( HeartRateInfo, CStatic )
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

HeartRateInfo::HeartRateInfo()
	: _heartrate( 0 )
	, _hrMax( 0 )
	, _hrAvg( 0 )
	, _hrMin( 0 )
	, _showHeart(false)
{
	const CSize sz = GetControlSize();

	_digits.LoadFromResource( AfxGetResourceHandle(), IDB_DIGITS );
	_buffer.Create( sz.cx, sz.cy, 24, BI_RGB );
}

CSize HeartRateInfo::GetControlSize() const
{
	CSize sz;

	sz.cx = 328;
	sz.cy = 144;

	return sz;
}


void HeartRateInfo::SetHeartRate( int hr )
{
	_heartrate = hr;
	_showHeart = true;

	Invalidate( FALSE );
}

void HeartRateInfo::ShowHeart( bool show )
{
	_showHeart = show;
	
	Invalidate( FALSE );
}

void HeartRateInfo::OnPaint()
{
	CPaintDC pdc( this );

	COLORREF bgColor = GetSysColor( COLOR_3DFACE );

	HDC hdc = _buffer.GetDC();
	CDC * dc = CDC::FromHandle( hdc );

	const CSize sz = GetControlSize();

	dc->FillSolidRect( 0, 0, sz.cx, sz.cy, bgColor );

	CRect big( 0, 0, digitBigW, digitBigH );
	big.OffsetRect( 8, 8 );

	int digitColor = 0;

	if( _heartrate < 120 )
	{
	}
	else if( _heartrate < 140 )
	{
		digitColor = digitBigH;
	}
	else
	{
		digitColor = digitBigH * 2;
	}

	int div = 100;

	for( int ni = 0; ni < 3; ++ni )
	{
		int digit = (_heartrate / div) % 10;
		if( ni > 0 || digit != 0 )
		{
			_digits.TransparentBlt( hdc, big, CRect( digit * digitBigW, digitColor, (digit+1) * digitBigW, digitColor+digitBigH), RGB( 255, 0, 255 ) );
		}

		div /= 10;
		big.OffsetRect( digitBigW + 16, 0 );
	}

	if( _showHeart )
	{
		_digits.TransparentBlt( hdc, 8, 8, 32, 32, 848, 352, 32,32, RGB( 255, 0, 255 ) );
	}

	_buffer.ReleaseDC();
	_buffer.BitBlt( pdc.GetSafeHdc(), 0, 0, SRCCOPY );
}


BOOL HeartRateInfo::OnEraseBkgnd( CDC * pDC )
{
	return FALSE;
}
