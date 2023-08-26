#include "pch.h"
#include "QCHead.h"
#include "resource.h"
#include <string>

void LoadPng( CImage & img, UINT id )
{
	CPngImage pngImage;
	pngImage.Load( id );
	img.Attach( (HBITMAP)pngImage.Detach() );
}


IMPLEMENT_DYNAMIC( QCHead, CStatic )

BEGIN_MESSAGE_MAP( QCHead, CStatic )
	//ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

QCHead::QCHead()
	: _heartRate( 0 )
	, _showHeart( false )
	, _lastTime( 0 )
{
	_buffer.Create( HUD_WIDTH, HUD_HEIGHT*2, 24, BI_RGB );

	LoadPng( _blend, IDB_RED );
	LoadPng( _heart, IDB_HEART );

	_redWidth = _blend.GetWidth();
	_redHeight = _blend.GetHeight() / 2;

	_diagram.Create( _redWidth, _redHeight*2, 24, BI_RGB );

	_redTemplate.reserve( _redWidth * _redHeight );
	for( int y = 0; y < _redHeight; ++y )
	{
		for( int x = 0; x < _redWidth; ++x )
		{
			const COLORREF color = _blend.GetPixel( x, y );
			_redTemplate.push_back( color );
		}
	}
}

void QCHead::Reset()
{
	for( int& i : _data )
	{
		i =  -1;
	}

	for( int& i : _vline )
		i = 0;

	_lastTime = 0;
}

void QCHead::Init()
{
	CFont* font = CFont::FromHandle( (HFONT)::GetStockObject( DEFAULT_GUI_FONT ) );

	LOGFONT logFont;
	font->GetLogFont( &logFont );

	logFont.lfHeight = -32;
	logFont.lfWeight = 800;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	_hrFont.CreateFontIndirect( &logFont );

	logFont.lfHeight = -8;
	logFont.lfWeight = 400;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	_lineFont.CreateFontIndirect( &logFont );
}

//void QCHead::OnPaint()
//{
//	CPaintDC pdc( this );
//	HDC hdc = _buffer.GetDC();
//	CDC * dc = CDC::FromHandle( hdc );
//	dc->FillSolidRect( 0, 0, 32, HUD_HEIGHT*2, RGB( 0, 0, 0 ) );
//
//	DrawDiagram( dc );
//
//	DrawHR( dc);
//
//	dc->ReleaseOutputDC();
//
//	_buffer.ReleaseDC();
//	_buffer.BitBlt( pdc.GetSafeHdc(), 0, 0, SRCCOPY );
//}

void QCHead::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	HDC hdc = _buffer.GetDC();

	static CRect rect( 0, 0, 32, HUD_HEIGHT * 2 );

	const CBrush brush( RGB( 0, 0, 0 ) );
	::FillRect( hdc, rect, brush );

	DrawDiagram( hdc );

	DrawHR( hdc);

	_buffer.ReleaseDC();
	_buffer.BitBlt( lpDrawItemStruct->hDC, 0, 0, SRCCOPY );
}

BOOL QCHead::OnEraseBkgnd( CDC * pDC )
{
	return TRUE;
}

void QCHead::SetHeartRate( int hr )
{
	_heartRate = hr;
	_showHeart = true;
	UpdateDiagram();

	Invalidate( FALSE );
}

void QCHead::UpdateDiagram()
{
	for( int x = 249; x > 0; --x )
	{
		_data[x] = _data[x - 1];
		_vline[x] = _vline[x - 1];
	}

	constexpr float scale = 75.0 / 120.0;

	int startY = (int)((_heartRate - 60) * scale + 0.5f);

	if( startY > 74 )
		startY = 74;

	_data[0] = startY;

	const DWORD curTime = GetTickCount();
	if( (curTime - _lastTime) > 60000 )
	{
		_lastTime = curTime;
		_vline[0] = 1;
	}
	else
	{
		_vline[0] = 0;
	}

	_blend.BitBlt( _diagram.GetDC(), 0, HUD_HEIGHT, HUD_WIDTH, HUD_HEIGHT, 0, HUD_HEIGHT, SRCCOPY );
	_diagram.ReleaseDC();

	unsigned char * linePtr = (unsigned char*)_diagram.GetBits();
	const int pitch = _diagram.GetPitch();

#pragma omp parallel for
	for( int y = 0; y < HUD_HEIGHT; ++y )
	{
		unsigned char* lineColor = linePtr + pitch * (74 - y);
		unsigned char* lineAlpha = linePtr + pitch * (74 - y + HUD_HEIGHT);

		for( int x = 0; x < 250; ++x )
		{
			if( y < _data[x] )
			{
				//_diagram.SetPixel( x, 74 - y, GetRed( x, 74 - y ) );
				const COLORREF rgb = GetRed( x, 74 - _data[x] );
				lineColor[0] = GetBValue( rgb );
				lineColor[1] = GetGValue( rgb );
				lineColor[2] = GetRValue( rgb );
			}
			else if( y == _data[x] )
			{
				//_diagram.SetPixel( x, 74 - y, RGB( 255, 0, 0 ) );
				lineColor[0] = 0;
				lineColor[1] = 0;
				lineColor[2] = 255;
			}
			else
			{
				if( (_vline[x] > 0) && (y & 1) )
				{
					//_diagram.SetPixel( x, 74 - y, RGB( 100, 255, 150 ) );

					lineColor[0] = 150;
					lineColor[1] = 255;
					lineColor[2] = 100;
				}
				else
				{
					//_diagram.SetPixel( x, 74 - y, RGB( 0, 0, 0 ) );
					lineColor[0] = 0;
					lineColor[1] = 0;
					lineColor[2] = 0;

					//_diagram.SetPixel( x, 74 - y + HUD_HEIGHT, RGB( 0, 0, 0 ) );
					lineAlpha[0] = 0;
					lineAlpha[1] = 0;
					lineAlpha[2] = 0;
				}
			}

			lineColor += 3;
			lineAlpha += 3;
		}
	}
}

void QCHead::DrawHR( HDC hdc )
{
	::SetTextColor( hdc, RGB( 255, 255, 255 ) );
	::SetBkMode( hdc, TRANSPARENT );

	{
		static CRect rH( 0, 5, 50, 35 );
		static CRect rA( 0, 5 + HUD_HEIGHT, 50, 35 + HUD_HEIGHT );

		const auto oldFont = ::SelectObject( hdc, _hrFont );
		const std::wstring text = std::to_wstring( _heartRate );
		::DrawText( hdc, text.c_str(), (int)text.size(), rH, DT_VCENTER | DT_LEFT | DT_NOCLIP | DT_SINGLELINE );
		::DrawText( hdc, text.c_str(), (int)text.size(), rA, DT_VCENTER | DT_LEFT | DT_NOCLIP | DT_SINGLELINE );
		::SelectObject( hdc, oldFont );
	}
	if( _showHeart )
	{
		_heart.BitBlt( hdc, 0, 40, 32, 32, 0, 0, SRCCOPY );
		_heart.BitBlt( hdc, 0, 40 + HUD_HEIGHT, 32, 32, 0, 32, SRCCOPY );
	}

	constexpr int offY[2] = {0, HUD_HEIGHT};
	const CPen pen( PS_DOT, 1, RGB( 192, 192, 200 ) );

	for( int oy = 0; oy < 2; ++oy )
	{
		const int OY = offY[oy];

		auto oldPen = ::SelectObject( hdc, pen );
		::MoveToEx( hdc, 32, 0 + OY, nullptr );
		::LineTo( hdc, 270, 0 + OY );

		::MoveToEx( hdc, 32, 25 + OY, nullptr );
		::LineTo( hdc, 270, 25 + OY );

		::MoveToEx( hdc, 32, 50 + OY, nullptr );
		::LineTo( hdc, 270, 50 + OY );

		::MoveToEx( hdc, 32, 74 + OY, nullptr );
		::LineTo( hdc, 270, 74 + OY );

		::SelectObject( hdc, oldPen );

		{
			auto oldFont = ::SelectObject( hdc, _lineFont );

			DrawText( hdc, L"180", 3, CRect( 262, 0 + OY, 262 + 20, 0 + 8 + OY ), DT_VCENTER | DT_RIGHT | DT_NOCLIP | DT_SINGLELINE );
			DrawText( hdc, L"140", 3, CRect( 262, 25 - 4 + OY, 262 + 20, 25 + 4 + OY ), DT_VCENTER | DT_RIGHT | DT_NOCLIP | DT_SINGLELINE );
			DrawText( hdc, L"100", 3, CRect( 262, 50 - 4 + OY, 262 + 20, 50 + 4 + OY ), DT_VCENTER | DT_RIGHT | DT_NOCLIP | DT_SINGLELINE );
			DrawText( hdc, L"60", 2, CRect( 262, 75 - 8 + OY, 262 + 20, 75 + OY ), DT_VCENTER | DT_RIGHT | DT_NOCLIP | DT_SINGLELINE );

			::SelectObject( hdc, oldFont );
		}
	}
}

void QCHead::DrawDiagram( HDC hdc )
{
	_diagram.BitBlt( hdc, 32, 0, SRCCOPY );
}


void QCHead::PreSubclassWindow()
{
	//BOOL result = ModifyStyle( 0, SS_OWNERDRAW );

	CStatic::PreSubclassWindow();
}
