
// ServoControlDlg.h : header file
//

#pragma once
#include "afxcmn.h"



// CServoControlDlg dialog
class CServoControlDlg : public CDialogEx
{
// Construction
public:
	CServoControlDlg(CWnd* pParent = NULL);	// standard constructor
	static CServoControlDlg * theObjectPtr;
	static LONG CallBackFunction();
	void _update();
	void update();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVOCONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	COpenGLControl_RT_Viewer	m_RT_Viewer;
	ISerialPort*	m_pISerialPort;
	ISerialSink*    m_pISink;

	CSerialReceiveBuffer m_SerialReceiveBuffer;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonPortSetting();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonPidWrite();
	afx_msg void OnBnClickedButtonPidRead();
	float m_fKP;
	float m_fKI;
	float m_fKD;
	// Reference Signal
	float m_fReference;
	// Sampling Time
	float m_fTsa;
	afx_msg void OnThumbposchangingSliderAmplitude(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderAmplitude(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_sldrAmplitude;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
};
