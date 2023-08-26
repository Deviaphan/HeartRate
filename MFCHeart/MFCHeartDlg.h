#pragma once
#include "HeartRateInfo.h"

class CMFCHeartDlg : public CDialog
{
public:
	CMFCHeartDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCHEART_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual void OnOK() {}
	virtual void OnCancel() {}

	DECLARE_MESSAGE_MAP()
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT UpdateAll( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedFinish();
	afx_msg void OnPaint();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnContextMenu( CWnd* /*pWnd*/, CPoint /*point*/ );
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

private:
	int _hrValue;

	HeartRateInfo _heartRateInfo;
	CButton _btnFinish;

public:
	CString deviceName;
};
