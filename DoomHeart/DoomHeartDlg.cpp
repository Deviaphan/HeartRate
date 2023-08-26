#include "pch.h"
#include "framework.h"
#include "DoomHeartDlg.h"
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
};

void DrawHR( int heartRate, void * userPtr )
{
	HWND wnd = (HWND)userPtr;
	::SendMessage( wnd, WM_UPDATE_ALL, heartRate, 0 );
}

CDoomHeartDlg::CDoomHeartDlg( CWnd * pParent /*=nullptr*/ )
	: CDialog( IDD_DOOMHEART_DIALOG, pParent )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CDoomHeartDlg::DoDataExchange( CDataExchange * pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_HUD, _hud );
}

BEGIN_MESSAGE_MAP( CDoomHeartDlg, CDialog )
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_UPDATE_ALL, UpdateAll )
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


BOOL CDoomHeartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( m_hIcon, TRUE );			// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	CRect r( 0, 0, HUD_WIDTH, HUD_HEIGHT );

	AdjustWindowRect( r, GetStyle(), FALSE );
	SetWindowPos( nullptr, r.left, r.right, r.Width(), r.Height(), SWP_SHOWWINDOW );

	HeartRate_Initialize( DEVICE_NAME, DrawHR, this->GetSafeHwnd() );

	return TRUE;
}

void CDoomHeartDlg::OnPaint()
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

HCURSOR CDoomHeartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDoomHeartDlg::OnOK()
{
	;
}


void CDoomHeartDlg::OnCancel()
{
	HeartRate_StopMonitoring();

	CDialog::OnCancel();
}

LRESULT CDoomHeartDlg::UpdateAll( WPARAM wParam, LPARAM lParam )
{
	_hud.SetHeartRate( (int)wParam );

	return 0;
}

void CDoomHeartDlg::OnContextMenu( CWnd * /*pWnd*/, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu( MF_STRING, START_DEVICE, L"* Start HR Monitor" );
	menu.AppendMenu( MF_STRING, STOP_DEVICE, L"  Stop HR monitor" );

	//ClientToScreen( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}


BOOL CDoomHeartDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if( HIWORD( wParam ) == 0 )
	{
		int id = LPARAM( wParam );

		if( id == START_DEVICE )
		{
			HeartRate_StartMonitoring();

			return TRUE;
		}
		else if( id == STOP_DEVICE )
		{
			HeartRate_StopMonitoring();

			DrawHR( 0, this->GetSafeHwnd() );

			return TRUE;
		}
	}

	return CDialog::OnCommand( wParam, lParam );
}
