
// MFCHeartDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCHeart.h"
#include "MFCHeartDlg.h"
#include "afxdialogex.h"
#include "HeartRate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_ALL (WM_USER+1543)

#define DEVICE_NAME L"2809_9066A7"


enum
{
	START_DEVICE = WM_USER + 1,
	STOP_DEVICE = WM_USER + 2,
	TIMER_HIDE_HEART = 123,
};

CMFCHeartDlg::CMFCHeartDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCHEART_DIALOG, pParent)
	, _hrValue( 0 )
	, deviceName( DEVICE_NAME )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCHeartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_HR_INFO, _heartRateInfo );
	DDX_Control( pDX, IDC_FINISH, _btnFinish );
}

BEGIN_MESSAGE_MAP(CMFCHeartDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE( WM_UPDATE_ALL, UpdateAll )
	ON_BN_CLICKED( IDC_FINISH, OnBnClickedFinish )
	ON_WM_TIMER()
END_MESSAGE_MAP()

void DrawHR( int heartRate, void * userPtr )
{
	HWND wnd = (HWND)userPtr;
	::SendMessage( wnd, WM_UPDATE_ALL, heartRate, 0 );
}

LRESULT CMFCHeartDlg::UpdateAll( WPARAM wParam, LPARAM lParam )
{
	_hrValue = (int)wParam;

	_heartRateInfo.SetHeartRate( _hrValue );

	SetTimer( TIMER_HIDE_HEART, 250, nullptr );

	return 0;
}

BOOL CMFCHeartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CSize hrinfoSize = _heartRateInfo.GetControlSize();

	CRect wndRect;
	GetWindowRect( wndRect );
	ClientToScreen( wndRect );
	
	CRect clientRect( 0, 0, hrinfoSize.cx, hrinfoSize.cy );

	CRect btnRect;
	_btnFinish.GetWindowRect( btnRect );
	int h = btnRect.Height();
	clientRect.bottom += h;

	btnRect.top = hrinfoSize.cy;
	btnRect.bottom = btnRect.top + h;
	btnRect.left = hrinfoSize.cx - btnRect.Width();
	btnRect.right = hrinfoSize.cx;
	_btnFinish.MoveWindow( btnRect );

	AdjustWindowRect( clientRect, WS_CAPTION | WS_BORDER, FALSE );

	clientRect.OffsetRect( wndRect.left, wndRect.top );

	SetWindowPos( nullptr, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_SHOWWINDOW );

	HeartRate_Initialize( DEVICE_NAME, DrawHR, this->GetSafeHwnd() );

	return TRUE;
}

void CMFCHeartDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CMFCHeartDlg::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu( MF_STRING, START_DEVICE, L"* Start HR Monitor" );
	menu.AppendMenu( MF_STRING, STOP_DEVICE, L"  Stop HR monitor" );

	//ClientToScreen( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}


BOOL CMFCHeartDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if( HIWORD( wParam ) == 0 )
	{
		int id = LPARAM( wParam );

		if( id == START_DEVICE )
		{
			HeartRate_StopMonitoring();

			DrawHR( 0, this->GetSafeHwnd() );

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

HCURSOR CMFCHeartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCHeartDlg::OnBnClickedFinish()
{
	HeartRate_StopMonitoring();

	CDialog::OnCancel();
}

void CMFCHeartDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch( nIDEvent )
	{
		case TIMER_HIDE_HEART:
		{
			KillTimer( TIMER_HIDE_HEART );

			_heartRateInfo.ShowHeart( false );

			break;
		}

		default:
			CDialog::OnTimer( nIDEvent );
	}
}
