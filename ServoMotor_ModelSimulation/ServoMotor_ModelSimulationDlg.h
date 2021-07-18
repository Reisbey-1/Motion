
// ServoMotor_ModelSimulationDlg.h : header file
//

#pragma once


// CServoMotor_ModelSimulationDlg dialog
class CServoMotor_ModelSimulationDlg : public CDialogEx
{
// Construction
public:
	CServoMotor_ModelSimulationDlg(CWnd* pParent = NULL);	// standard constructor
	static CServoMotor_ModelSimulationDlg * theObjectPtr;
	static LONG CallBack_SerialPort();
	static LONG CallBack_System(char *pBuffer, uint16_t index, uint16_t sizeBuffer);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVOMOTOR_MODELSIMULATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	static int geciciSayac;

// Implementation
protected:
	HICON m_hIcon;
	CSystem3 *m_pMotorControlSystem;
	static char ** m_ppBuffer;
//	static int     m_nIndex;
//	static int     m_nSize;
	static class CRingBuffer<float> *m_pRingBuffer;

	ISerialPort*	m_pISerialPort;
	ISerialSink*    m_pISink;
	void update();
	void CalledFrom_System(char *pBuffer, uint16_t index, uint16_t size);

	CSerialReceiveBuffer m_SerialReceiveBuffer;
	void _UpdateData();
	void _InitDialog();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	void Draw_MotorPosition(CDC *pDC, float fPostAngle, float fRadius);
	
	afx_msg void OnPaint();


	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnClose();
	//  Kp
	float m_fKP;
	// Ki
	float m_fKI;
	// Kd
	float m_fKD;
	
	float m_dt;					// sampling time
	float m_dt_Asyncron;		// sampling time asyncron

	float m_fJ;					// Inertia
	float m_fb;					// Viscocity
	float m_fKt;				// Torque constant
	float m_fKe;				// Electro motive force constant
	
	float m_fR;					// Coil Resistance constant
	float m_fL;					// coil inductance 
	float m_fN;					// Motor reduction

	float m_fLoadTorgue;		// Load Torque

	// initials
	float m_fTi;
	float m_fVi;
	float m_fYi;
	
	float m_fRefSignal;			// Reference Signal ?r
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonPortSetting();
	afx_msg void OnBnClickedButtonAsenkron();

	afx_msg void OnBnClickedRadioEnableAsenkron();
	afx_msg void OnBnClickedRadioEnableSenkron();
	afx_msg void OnBnClickedButtonResetSystem();
	
	afx_msg void OnBnClickedButtonResetSystemAsenkron();
	
};
