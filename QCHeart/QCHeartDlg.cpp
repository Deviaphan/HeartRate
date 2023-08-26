#include "pch.h"
#include "framework.h"
#include "QCHeartDlg.h"
#include "HeartRate.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_ALL 1543

#define DEVICE_NAME L"2809_9066A7"

enum
{
	START_DEVICE = WM_USER + 1,
	STOP_DEVICE = WM_USER + 2,
	TIMER_HIDE_HEART = 123,
};

void DrawHR( int heartRate, void * userPtr )
{
	HWND wnd = (HWND)userPtr;
	::SendMessage( wnd, WM_UPDATE_ALL, heartRate, 0 );
}

CQCHeartDlg::CQCHeartDlg( CWnd * pParent /*=nullptr*/ )
	: CDialog( IDD_DOOMHEART_DIALOG, pParent )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CQCHeartDlg::DoDataExchange( CDataExchange * pDX )
{
	CDialog::DoDataExchange( pDX );
	//DDX_Control( pDX, IDC_HUD, _hud );
}

BEGIN_MESSAGE_MAP( CQCHeartDlg, CDialog )
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_UPDATE_ALL, UpdateAll )
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CQCHeartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( m_hIcon, TRUE );			// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	CRect r( 0, 0, HUD_WIDTH, HUD_HEIGHT*2 );

	AdjustWindowRect( r, GetStyle(), FALSE );
	
	SetWindowPos( nullptr, r.left, r.right, r.Width(), r.Height(), SWP_SHOWWINDOW );

	_hud.Create( L"HUD",
		WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
		CRect( 0, 0, 282, 150 ),
		this );

	_hud.Init();

	HeartRate_Initialize( DEVICE_NAME, DrawHR, this->GetSafeHwnd() );

	return TRUE;
}

void CQCHeartDlg::OnPaint()
{
	if( IsIconic() )
	{
		CPaintDC dc( this ); // device context for painting

		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0 );

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect( &rect );
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon( x, y, m_hIcon );
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CQCHeartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQCHeartDlg::OnOK()
{
	;
}


void CQCHeartDlg::OnCancel()
{
	HeartRate_StopMonitoring();

	CDialog::OnCancel();
}

LRESULT CQCHeartDlg::UpdateAll( WPARAM wParam, LPARAM lParam )
{
	_hud.SetHeartRate( (int)wParam );
	SetTimer( TIMER_HIDE_HEART, 250, nullptr );

	return 0;
}

void CQCHeartDlg::OnContextMenu( CWnd * /*pWnd*/, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu( MF_STRING, START_DEVICE, L"* Start HR Monitor" );
	menu.AppendMenu( MF_STRING, STOP_DEVICE, L"  Stop HR monitor" );

	//ClientToScreen( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}


BOOL CQCHeartDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if( HIWORD( wParam ) == 0 )
	{
		int id = LPARAM( wParam );

		if( id == START_DEVICE )
		{
			HeartRate_StopMonitoring();

			_hud.Reset();
			HeartRate_StartMonitoring();

			return TRUE;
		}
		else if( id == STOP_DEVICE )
		{
			_hud.Reset();

			HeartRate_StopMonitoring();

			DrawHR( 0, this->GetSafeHwnd() );

			return TRUE;
		}
	}

	return CDialog::OnCommand( wParam, lParam );
}


void CQCHeartDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch( nIDEvent )
	{
		case TIMER_HIDE_HEART:
		{
			KillTimer( TIMER_HIDE_HEART );

			_hud._showHeart = false;
			_hud.Invalidate( FALSE );
			break;
		}

		default:
			CDialog::OnTimer( nIDEvent );
	}
}
