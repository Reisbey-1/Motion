#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MotionControlDlg.h"
// CTimer_1 dialog

class CTimer_1 : public CDialogEx
{
	DECLARE_DYNAMIC(CTimer_1)

public:
	CTimer_1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimer_1();

	enum { TM0 = 0, TM1, TM2, TM3, TM4, TM5, TM6, TM7, TM8, TM9, TM10, TM11, TM12, TM13, TM14, TM15	};

// Dialog Data
#ifdef AFX_DESIGN_TIME
	//enum { IDD = IDD_DIALOG_TIMER_1 };
#endif
	enum { IDD = IDD_DIALOG_TIMER_1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//int _Setup_PWM(uint8_t pwm_mod, uint8_t clsel, uint32_t TargetFrq, uint8_t percent_duty);
	int _Setup_PWM(uint8_t pwm_mod, uint8_t clsel, double TargetFrq, uint8_t percent_duty);


	int _Setup_CTC(uint8_t pwm_mod, uint8_t clsel, uint32_t TargetFrq);
	int _Setup_FIX(uint8_t fix_mod, uint8_t clsel, uint32_t TargetFrq, uint8_t percent_duty);

	bool CTimer_1::_BuildFromFile(CString csFileName);
	bool CTimer_1::_StoreIntoFile(CString csFileName);

	////////////////////////
	uint8_t		m_TCCR1A;
	uint8_t		m_TCCR1B;
	uint32_t	m_ICR1 ;
	uint32_t	m_OCR1A;
	uint32_t	m_OCR1B;
	uint8_t		m_TIMSK;
	uint8_t		m_TIFR;

	
/*
	CButton * A0; // = (CButton *)GetDlgItem(IDC_CHECK_COM1A0);
	CButton * B0; // = (CButton *)GetDlgItem(IDC_CHECK_COM1B0);

	CButton * A1; // = (CButton *)GetDlgItem(IDC_CHECK_COM1A1);
	CButton * B1; // = (CButton *)GetDlgItem(IDC_CHECK_COM1B1);
	
	CButton * EINT_A; // enable intrupt A
	CButton * EINT_B; // enable interrup B
*/
	
	////////////////////////
	void _ReadRam(WORD ramAdress);
	void _WriteRam(WORD ramAdress, BYTE* buffer, BYTE L, BYTE OR = 0);

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	CComboBox m_cbPreScaler;
	CComboBox m_cbMode;
	CComboBox m_cbTop;
	CComboBox m_cbCpuFrequency;
	uint8_t	m_mode;

	float	m_PercentDuty;
	double	m_nCpuFrequency;
	double	m_nTargetFrequency;
	double	m_nTimerFrequency;
	
	afx_msg void OnSelchangeComboMode();

	afx_msg void OnBnClickedButtonSetup();
	SHORT s;

	UINT m_nTop;
	//DWORD m_nTargetFrequency;


	CLabel m_LbError;
	CComboBox m_cbCompareOutputMode;
	CMotionControlDlg * m_pMCDlg;

	afx_msg void OnBnClickedButtonOcr1aW();
	afx_msg void OnBnClickedButtonOcr1aR();
	afx_msg void OnBnClickedButtonOcr1bW();
	afx_msg void OnBnClickedButtonOcr1bR();
	afx_msg void OnBnClickedButtonIcr1W();
	afx_msg void OnBnClickedButtonIcr1R();
	afx_msg void OnBnClickedButtonTcr1bR();
	afx_msg void OnBnClickedButtonTccr1bW();
	afx_msg void OnCbnSelchangeComboPreCompareOutputMod();

	
	BOOL bA0;
	BOOL bA1;
	BOOL bB0;
	BOOL bB1;
	BOOL bIA;
	BOOL bIB;
	BOOL bET;
	BOOL bEToV;

	DHexEdit m_hedOCR1A;
	DHexEdit m_hedICR1;
	DHexEdit m_hedOCR1B;
	DHexEdit m_hedTimerFrequency;
	DHexEdit m_hedTIMSK;
	DHexEdit m_hedTCCR1A;
	DHexEdit m_hedTCCR1B;
	DHexEdit m_hedTIFR;
	CSliderCtrl m_sldr_ICR1;
	CSliderCtrl m_sldr_OCR1A;
	CSliderCtrl m_sldr_OCR1B;

	bool m_bDeTelArrived;
	void update();
	afx_msg void OnBnClickedCheckInterruptA();
	afx_msg void OnBnClickedCheckInterruptB();
	afx_msg void OnBnClickedCheckEnableTimer();
	afx_msg void OnBnClickedOk();
	
	afx_msg void OnBnClicked_ButtonTifrR();
	afx_msg void OnBnClicked_ButtonTifrW();
	afx_msg void OnEnChangeEditTop();
	afx_msg void OnBnClickedRadioChannelBoth();
	afx_msg void OnBnClickedRadioChannelB();
	afx_msg void OnBnClickedRadioChannelA();
	afx_msg void OnReleasedcaptureSlider_Icr1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSlider_Ocr1a(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSlider_Ocr1b(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnSelchange_ComboCpuFrequency();
};
