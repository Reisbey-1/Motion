
// MotionControlDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
//#include "C:\sw\DVR\implement\Label.h"



// CMotionControlDlg dialog
class CMotionControlDlg : public CDialogEx
{
	friend class CTimer_1;
	friend class CTimer_2;

	CTimer_1* p_mCTimer_1;
	CTimer_2* p_mCTimer_2;

	enum {ACTION_START= 0x0011, ACTION_ACCEL, ACTION_DECEL, ACTION_STOP, ACT_READ_RESONATOR, ACT_WRITE_RESONATOR, ACT_READ_ACCELERATOR };

	enum { bSTART = 0, bACCEL, bDECEL, bSTOP, bWRITE_RESONATOR};
	
	void _interprate_incoming_telegram();
	void _action(int act, double data = 0.0);
protected:


	ISerialPort*	m_pISerialPort;
	IFabric *		m_pIFabric;
	ISerialSink*    m_pISink;

	DeTel m_Detel;

	BYTE m_czData[LENGTH_CONROLBITS  + MAXIMUM_DATA_BYTE];

// Construction
public:
	CMotionControlDlg(CWnd* pParent = NULL);	// standard constructor
	~CMotionControlDlg();

	static LONG CallBackFunction();
	static CMotionControlDlg* theObjectPtr;


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTIONCONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	void update();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetRs232();
	afx_msg void OnBnClickedButtonConnectRs23();
	CLabel m_LbL_1;
	CStatic m_Picture;
	CCustomEdit m_ecMonitor;	
	float m_fAccel;
	float m_fV1;

	afx_msg void OnBnClickedButtonTrajectory();
	afx_msg void OnBnClickedButtonEcho();
	afx_msg void OnBnClickedRadioAscii();
	afx_msg void OnBnClickedRadioRtu();
	afx_msg void OnBnClickedRadioDetel();
	afx_msg void OnBnClickedButtonTimer1();

	afx_msg void OnBnClickedButton_ToMtnController();

	float m_fAccumulator;
	float m_fAccelerator_acceleration;
	float m_fAccelerator_current_frequency;
	float m_fAccelerator_final_frequency;
	float m_fAccelerator_lower_bound;
	float m_fAccelerator_upper_bound;
	afx_msg void OnBnClickedButton_ReadAc();
	CSliderCtrl m_sldrAccu;
	afx_msg void OnReleasedcapture_SliderAccu(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonOnOff();
	afx_msg void OnBnClickedButtonTimer2();
	afx_msg void OnClose();
	afx_msg void OnEnChangeEditAccu();
	
};
