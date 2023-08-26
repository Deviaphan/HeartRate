#pragma once

#include "QCHead.h"

class CQCHeartDlg: public CDialog
{
public:
	CQCHeartDlg( CWnd * pParent = nullptr );	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DOOMHEART_DIALOG };
#endif

protected:
	virtual void DoDataExchange( CDataExchange * pDX );	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT UpdateAll( WPARAM wParam, LPARAM lParam );

protected:
	HICON m_hIcon;
	QCHead _hud;
public:
	afx_msg void OnContextMenu( CWnd * /*pWnd*/, CPoint /*point*/ );
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer( UINT_PTR nIDEvent );
};
