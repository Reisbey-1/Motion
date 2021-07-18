#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MotionControlDlg.h"

// CTimer_2 dialog

class CTimer_2 : public CDialogEx
{
	DECLARE_DYNAMIC(CTimer_2)

public:
	CTimer_2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimer_2();
	void update();

// Dialog Data
	enum { IDD = IDD_TIMER_2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void _WriteRam(WORD ramAdress, BYTE* buffer, BYTE L, BYTE OR = 0);
	void _ReadRam(WORD ramAdress, BYTE L = 2);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbPreScaler;
	CComboBox m_cbMode;
	CComboBox m_cbTop;
	CComboBox m_cbCpuFrequency;
	
	uint8_t m_TIMSK; 
	uint8_t m_TCCR2;
	uint8_t	m_OCR2;
	uint8_t m_clsel;
	uint8_t m_clsel_H;
	uint8_t m_COM_21_20 ;

	bool m_bDeTelArrived;


	CMotionControlDlg * m_pMCDlg;


	// final frequency
	double m_dF_r;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchange_ComboRMode();
	afx_msg void OnBnClickedCheckEnTovInt();
	afx_msg void OnBnClickedCheckEnOcInt();
	afx_msg void OnBnClickedCheckREnableTimer();
	afx_msg void OnBnClickedRadioRNone();
	CComboBox m_cbCompareOutputMode;
	afx_msg void OnBnClickedRadioFast();
	afx_msg void OnBnClickedRadioPc();
	afx_msg void OnCbnSelchangeComboRCompareOutputMod();
	afx_msg void OnBnClickedButtonRUpdate();
	DHexEdit m_hexTCCR2;
	DHexEdit m_hexTIFR;
	DHexEdit m_hexTIMSK;
	DHexEdit m_hexOCR2;
	afx_msg void OnCbnSelchangeComboRPreScaler();
	afx_msg void OnEnChangeEditResonatorFrequency();
	afx_msg void OnBnClickedButton_WriteTccr2();
	afx_msg void OnBnClickedButton_WriteOcr2();
	afx_msg void OnBnClickedButton_WriteTimsk();
	afx_msg void OnBnClickedButton_ReadTccr2();
	afx_msg void OnBnClickedButton_ReadOcr2();
	afx_msg void OnBnClickedButton_ReadTimsk();
	afx_msg void OnBnClickedButtonReadFr();
	afx_msg void OnBnClickedButtonWriteFr();
};
