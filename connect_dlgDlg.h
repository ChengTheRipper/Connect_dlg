
// connect_dlgDlg.h: 头文件
//

#pragma once

#include "opencv_include.h"
#include "CPublic.h"
// CconnectdlgDlg 对话框
class CconnectdlgDlg : public CDialogEx
{
public:
	

// 构造
public:
	CconnectdlgDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONNECT_DLG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenPic();
	virtual void OnCancel();
	afx_msg void OnBnClickedOk();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	

	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton1();
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
private:
	CSliderCtrl m_slider_ROI1_width;
public:
	CSliderCtrl m_slider_ROI1_height;
	afx_msg void OnNMCustomdrawSbRoi1H(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonRoiUp();
	afx_msg void OnBnClickedButtonRoiDown();
	afx_msg void OnBnClickedButtonRoiLeft();
	afx_msg void OnBnClickedButtonRoiRight();
};
