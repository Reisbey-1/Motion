
// MotionControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"

#include "MotionControlDlg.h"

#include "MotionControl.h"

#include "Timer_1.h"
#include "Timer_2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMotionControlDlg dialog
CMotionControlDlg* CMotionControlDlg::theObjectPtr = NULL;

LONG CMotionControlDlg::CallBackFunction()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CMotionControlDlg* pSelf = theObjectPtr;
	pSelf->update();
	return 1;
}

void CMotionControlDlg::update()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// pTheModuleState = AfxGetStaticModuleState();
//	AfxSetModuleState(pTheModuleState);

	// return; 
	BYTE bt;
	CListCtrl* pLisRamBild = (CListCtrl*)GetDlgItem(IDC_LIST_RECEIVE);
	int i = 1;
	short l;
	unsigned char * p = NULL;
	m_pISink->get_ReceiveBuffer(&p);
	m_pISink->get_lengthReceivedChars(&l);
	Beep(1000, 100);

	//CString csMessage(p);
	CString csMessage, csDumy, csAdd;
	
	//csMessage = L"mustafa dulger";
	 CNumberUtility::BYTEStreamToChar(p, l, csMessage);
	
	SHORT  ps = CSmartConnector::Get_SmartConnector()->GetProtcolSwitch();
		
//	csMessage.TrimLeft();
//	csMessage.TrimRight();
	CStringW csDigit;
	int LineIndex = 1;
	int bIndex = 0;
	;

	int ln = csMessage.GetLength();
	wchar_t * pWc;// [500];
	pWc = ((wchar_t*) csMessage.GetString());
	if (ps == CSmartConnector::PS_MODBUS_ASCII)
	{

		do {
			csAdd = pWc[i];
			csDigit = csDigit + csAdd;
			if (!(i % 2))
			{
				if (csDigit.CompareNoCase(L"fd") == 0)
				{
					//csDumy.Append(csDigit + L"\r\n");
					csDumy = csDumy + csDigit + L"\r\n";

					LineIndex = 1;
					bIndex = 0;
					CNumberUtility::HexToByte(csDigit, bt);
					m_czData[bIndex++] = bt;
					m_Detel.XON = bt;
				}
				else if (csDigit.CompareNoCase(L"fe") == 0)
				{
					//csDumy.Append(csDigit );
					csDumy = csDumy + csDigit;

					CNumberUtility::HexToByte(csDigit, bt);
					m_czData[bIndex++] = bt;
					m_Detel.XOF = bt;

					_interprate_incoming_telegram();
				}
				else
				{
					//csDigit.Append(csAdd);
					CNumberUtility::HexToByte(csDigit, bt);

					if (bIndex < LENGTH_CONROLBITS  + MAXIMUM_DATA_BYTE - 1)
					{

						if (bIndex == 1) {
							m_Detel.cm1 = bt;  // m_nTelegram_CMD0 = bt; 
						}
						if (bIndex == 2) {
							m_Detel.cm2 = bt; // m_nTelegram_CMD1 = bt;
						}
						if (bIndex == 3) {
							m_Detel.ad0 = bt; // m_nTelegram_AD0 = bt;
						}
						if (bIndex == 4) {
							m_Detel.ad1 = bt; // m_nTelegram_AD1 = bt;
						}
						if (bIndex == 5) {
							m_Detel.ad2 = bt;
						}
						if (bIndex == 6) {
							m_Detel.ad3 = bt;
						}
						if (bIndex == 7) {
							m_Detel.nil = bt;
						}
						if (bIndex == 8) {
							m_Detel.rs0 = bt;
						}
						if (bIndex == 9) {
							m_Detel.rs1 = bt;
						}
						if (bIndex == 10) {
							m_Detel.rs2 = bt;
						}
						if (bIndex == 11) {
							m_Detel.cnt = bt;
						}
						if (bIndex > 11) {
							m_czData[bIndex] = bt;
							m_Detel.db[bIndex - (LENGTH_CONROLBITS - 1) ] = bt;
						}



						bIndex++;
					}

					//csDumy.Append(csDigit +L" ");
					csDumy = csDumy + csDigit + L" ";

				}
				csDigit.Empty();
			}
			if (!(LineIndex % 32))
			{
				//csDumy.Append(L"\r\n");
				csDumy = csDumy + L"\r\n";
			}

			// 
			if (i == 24) {
				LineIndex = 0;
				//csDumy.Append(L"\r\n");
				csDumy = csDumy + L"\r\n";
			}
			LineIndex++;
			i++;
		} while (csMessage.GetLength() > i);
		//AfxMessageBox(csDumy);

		m_ecMonitor.SetWindowTextW(csDumy);
		if (p_mCTimer_1) {
			p_mCTimer_1->m_bDeTelArrived = true;
			p_mCTimer_1->update();
		}

		if (p_mCTimer_2) {
			p_mCTimer_2->m_bDeTelArrived = true;
			p_mCTimer_2->update();
		}
	}

	if (ps == CSmartConnector::PS_DETEL)
	{
		CNumberUtility::BYTEStreamToString(p, l, csMessage);
	//	AfxMessageBox(csMessage);

	}

	/**/
}

void CMotionControlDlg::_interprate_incoming_telegram()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	/// 
	/*
	float m_fAccelerator_acceleration;
	float m_fAccelerator_current_frequency;
	float m_fAccelerator_final_frequency;
	float m_fAccelerator_lower_bound;
	float m_fAccelerator_upper_bound;

	double m_dAcceleration;				// acceleration
	double m_dCurrentFrequency;			// current frequency (velocity)
	double m_dUpperBound;				// upper bound (maximum) 10KHz.
	double m_dLowerBound;				// lower bound (minimum) 0 Hz. as starting value
	double m_dFinalFrequency;			// final frequency (velocity)

	*/
	union convert
	{
		uint8_t		_db[4];
		uint32_t	_wd;
	};
	convert myData;
	
	//sequence is important !
	if ( (m_Detel.cm1 == SP_ACTN) & (m_Detel.rs0 == ACT_READ_ACCELERATOR) )
	{
		// acceleration
		for (int i = 0; i < 4; i++)				
			myData._db[i] = m_Detel.db[i];
		m_fAccelerator_acceleration = CNumberUtility::unpack754_32(myData._wd);

		// current_frequency
		for (int i = 0; i < 4; i++)
			myData._db[i] = m_Detel.db[i+4];
		m_fAccelerator_current_frequency = CNumberUtility::unpack754_32(myData._wd);

		// upper_bound
		for (int i = 0; i < 4; i++)
			myData._db[i] = m_Detel.db[i + 8];
		m_fAccelerator_upper_bound = CNumberUtility::unpack754_32(myData._wd);
	
		// lower_bound
		for (int i = 0; i < 4; i++)
			myData._db[i] = m_Detel.db[i + 12];
		m_fAccelerator_lower_bound = CNumberUtility::unpack754_32(myData._wd);
		
		// final_frequency
		for (int i = 0; i < 4; i++)
			myData._db[i] = m_Detel.db[i + 16];
		m_fAccelerator_final_frequency = CNumberUtility::unpack754_32(myData._wd);
				
		CDialog::OnInitDialog();
	}
}

CMotionControlDlg::CMotionControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MOTIONCONTROL_DIALOG, pParent)
	, m_fAccel(3.5)
	, m_fV1(0.0)

	, m_fAccumulator(10)
	, m_fAccelerator_acceleration(0)
	, m_fAccelerator_current_frequency(0)
	, m_fAccelerator_final_frequency(0)
	, m_fAccelerator_lower_bound(0)
	, m_fAccelerator_upper_bound(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pISerialPort = NULL;
	m_pIFabric = NULL;
	m_pISink = NULL;
	theObjectPtr = this;
	p_mCTimer_1 = NULL; 
	p_mCTimer_2 = NULL;
}

CMotionControlDlg::~CMotionControlDlg()
{
	CSmartConnector::Kill_SmartConnector();
	CSmartTelFabConnector::Kill_SmartTelFabConnector();
	RS232Settings::GetSettings()->WriteIntoRegistry(L"SOFTWARE", L"AyfaLtd\\MotionControl");
}

void CMotionControlDlg::DoDataExchange(CDataExchange* pDX)
{
	//pTheModuleState = AfxGetStaticModuleState();

	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LB1, m_LbL_1);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_Picture);
	DDX_Text(pDX, IDC_EDIT_ACCEL, m_fAccel);
	DDX_Text(pDX, IDC_EDIT_VELOCITY1, m_fV1);
	DDX_Control(pDX, IDC_EDIT_MONITOR, m_ecMonitor);

	DDX_Text(pDX, IDC_EDIT_ACCU, m_fAccumulator);
	DDX_Text(pDX, IDC_EDIT_AC_ACCELERATION, m_fAccelerator_acceleration);
	DDX_Text(pDX, IDC_EDIT_AC_CURRENT_FREQUENCY, m_fAccelerator_current_frequency);
	DDX_Text(pDX, IDC_EDIT_AC_FINAL_FREQUENCY, m_fAccelerator_final_frequency);
	DDX_Text(pDX, IDC_EDIT_AC_LOWER_BOUND, m_fAccelerator_lower_bound);
	DDX_Text(pDX, IDC_EDIT_AC_UPPER_BOUND, m_fAccelerator_upper_bound);
	DDX_Control(pDX, IDC_SLIDER_ACCU, m_sldrAccu);
}

BEGIN_MESSAGE_MAP(CMotionControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SET_RS232, &CMotionControlDlg::OnBnClickedButtonSetRs232)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_RS23, &CMotionControlDlg::OnBnClickedButtonConnectRs23)	
	ON_BN_CLICKED(IDC_BUTTON_TRAJECTORY, &CMotionControlDlg::OnBnClickedButtonTrajectory)
	ON_BN_CLICKED(IDC_BUTTON_ECHO, &CMotionControlDlg::OnBnClickedButtonEcho)
	ON_BN_CLICKED(IDC_RADIO_ASCII, &CMotionControlDlg::OnBnClickedRadioAscii)
	ON_BN_CLICKED(IDC_RADIO_RTU, &CMotionControlDlg::OnBnClickedRadioRtu)
	ON_BN_CLICKED(IDC_RADIO_DETEL, &CMotionControlDlg::OnBnClickedRadioDetel)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_1, &CMotionControlDlg::OnBnClickedButtonTimer1)
	ON_BN_CLICKED(IDC_BUTTON_TO_MTN_CONTROLLER, &CMotionControlDlg::OnBnClickedButton_ToMtnController)
	ON_BN_CLICKED(IDC_BUTTON_READ_AC, &CMotionControlDlg::OnBnClickedButton_ReadAc)
	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_ACCU, &CMotionControlDlg::OnReleasedcapture_SliderAccu)
	ON_BN_CLICKED(IDC_BUTTON_ON_OFF, &CMotionControlDlg::OnBnClickedButtonOnOff)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_2, &CMotionControlDlg::OnBnClickedButtonTimer2)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT_ACCU, &CMotionControlDlg::OnEnChangeEditAccu)

END_MESSAGE_MAP()

// CMotionControlDlg message handlers

BOOL CMotionControlDlg::OnInitDialog()
{
//		AFX_MANAGE_STATE(pTheModuleState);
	CDialogEx::OnInitDialog();
	
	/*  test for memory leaks
	BYTE b[10] = { 1,2,3,4,5,6,7,8,9,0 };
	CString csByte;

	CNumberUtility::BYTEStreamToChar(b, 10, csByte);

	CString csHex = L"A";

	CNumberUtility::BYTEStreamToString(b, 4, csByte);

	CNumberUtility::decimalToOctal(130);
	
	WORD w;
	CNumberUtility::DecToWord(L"267", w);

	BYTE b2 = CNumberUtility::GetBYTE(csHex);
	*/

	CListCtrl* pLisRamBild = (CListCtrl*)GetDlgItem(IDC_LIST_RECEIVE);

	pLisRamBild->InsertColumn(0, L"line", LVCFMT_LEFT, 50);
	pLisRamBild->InsertColumn(1, L"received:", LVCFMT_LEFT, 100);
	pLisRamBild->InsertColumn(2, L"continue:", LVCFMT_LEFT, 100);

	
/**/
	pLisRamBild->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);

	//m_csSymbolFile = pApp->GetProfileString(L"Monitor", L"SymbolFile", L"");



	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//////////////////////////////////////////////////////////

// get dvr resource 
#ifdef DEBUG
	HMODULE hMod = LoadLibrary(L"dvrd.dll");
#else 
	HMODULE hMod = LoadLibrary(L"dvr.dll");
#endif
	GET_DVR_RESOURCE(hDVRmdl)
		if (hDVRmdl)
		{
			// Store the current resource handle
			HINSTANCE hClientResources = AfxGetResourceHandle();
			AfxSetResourceHandle(hDVRmdl);

			// connect buttons 
			CButton* pButtonConnect = (CButton*)GetDlgItem(IDC_BUTTON_CONNECT_RS23);
			CButton* pButtonPortSetting = (CButton*)GetDlgItem(IDC_BUTTON_SET_RS232);
			pButtonConnect->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_UNCONNECT)));
			pButtonPortSetting->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_SETTING)));

			// Restore the client application resource handle
			AfxSetResourceHandle(hClientResources);
		}
	
	// get setting
	// get connector with protocol
	// set callback function
	// get 
	RS232Settings::GetSettings()->ReadFromRegistry(L"SOFTWARE", L"AyfaLtd\\MotionControl");
	SHORT protocolSwitch = RS232Settings::GetSettings()->m_nProtocol;
	CSmartConnector * pSmartConnector =  CSmartConnector::Get_SmartConnector(&CMotionControlDlg::CallBackFunction, protocolSwitch);
	CSmartConnector::Get_SmartConnector()->SetProtcolSwitch(( CSmartConnector::protocolSwitch) protocolSwitch);
	m_pISerialPort = pSmartConnector->m_pISerialPort;

	CSmartConnector::Get_SmartConnector()->GetSink((IUnknown**)&m_pISink);// { *ppSink = m_pISink; }

	m_pIFabric = CSmartTelFabConnector::Get_SmartTelFabConnector()->m_pIFabric;

	//m_pIFabric->Release();
	// Add "About..." menu item to system menu.

	// update RS232 Settings
	CButton * pBtn;
	if (RS232Settings::GetSettings()->m_nProtocol == CSmartConnector::PS_DETEL) {
		pBtn = (CButton*)GetDlgItem(IDC_RADIO_DETEL);
		pBtn->SetCheck(1);
	} else 	if (RS232Settings::GetSettings()->m_nProtocol == CSmartConnector::PS_MODBUS_ASCII) {
		pBtn = (CButton*)GetDlgItem(IDC_RADIO_ASCII);
		pBtn->SetCheck(1);
	}
	
	else if (RS232Settings::GetSettings()->m_nProtocol == CSmartConnector::PS_MODBUS_RTU) {
		pBtn = (CButton*)GetDlgItem(IDC_RADIO_RTU);
		pBtn->SetCheck(1);
	}

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_LbL_1.SetFontSize(15);
	m_sldrAccu.SetRange(0, 10);
	m_sldrAccu.SetSelection(2, 98);
	
	RECT r;
	//GetClientRect(&r);
	GetWindowRect(&r);
	
	//	ScreenToClient(&r);

	int shiftX = r.left -100 ;
	int shiftY = 100;
									 
	r.left = r.left - shiftX;
	r.right = r.right - shiftX;
	r.top = r.top + shiftY;
	r.bottom = r.bottom + shiftY;
	//ClientToScreen(&r);
	MoveWindow(&r);
	//////////////////////////////////////////////////////////
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMotionControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMotionControlDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMotionControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMotionControlDlg::OnBnClickedButtonSetRs232()
{
	CRunDlg r(this);
	if (IDOK == r.Run(CRunDlg::RS232_SETTINGS))
	{
//??		m_nPortSel = RS232Settings::GetSettings()->m_nComPort;
//??		m_nBaudRate = RS232Settings::GetSettings()->m_nBaudRate;
//??		m_nParitySel = RS232Settings::GetSettings()->m_nParityIndex;
//??		m_nDataBitSel = RS232Settings::GetSettings()->m_nDataBitIndex;
//??		m_nStopBitSel = RS232Settings::GetSettings()->m_nStopBitIndex;
//??		m_dwCommEvents = RS232Settings::GetSettings()->m_dwCommEvents;
	}
}

void CMotionControlDlg::OnBnClickedButtonConnectRs23()
{
	static bool bPortOpen = false;
	CString csCaption;
	CString csError;
	CString csPortString;
	CString csBaud = gsBaud[RS232Settings::GetSettings()->m_nBaudRate];
	CString csPrty = gsParity[RS232Settings::GetSettings()->m_nParityIndex];
	CString	csData = gsDataBit[RS232Settings::GetSettings()->m_nDataBitIndex];
	CString csStop = gsStopBit[RS232Settings::GetSettings()->m_nStopBitIndex];
	DWORD   dwCommEvents = RS232Settings::GetSettings()->m_dwCommEvents;
	int		nPortSel = RS232Settings::GetSettings()->m_nComPort;

	csPortString.Format(_T("CPP_Parameters::OnBnClickedButtonConnect() - \n <port=%i> <baud=%s> <parity=%s> <databits=%s> <stopbits=%s>"), nPortSel, csBaud, csPrty, csData, csStop);
	//EAG_LOG_DEBUG(csPortString)

		///AfxMessageBox(csPortString);

	CButton* pButtonConnect = (CButton*)GetDlgItem(IDC_BUTTON_CONNECT_RS23);

	BSTR bstr = csPortString.AllocSysString();

	if (m_pISerialPort == NULL)
	{
		//EAG_LOG_ERROR("port object is not available !") 
		//		csError.LoadString(IDS_ERR_NOPORTOBJECT);
		//		EAG_LOG_ERROR(csError) 					

		return;
	}

	if (bPortOpen == false)
	{
		// if already open return 
		if (m_pISerialPort->CanDo() == S_OK) return;

		m_pISerialPort->put_ComEvents(dwCommEvents);

		if (S_OK == m_pISerialPort->OpenPort(&bstr, nPortSel))
		{
			////m_pISerialPort->StartMonitoring();
			CSmartConnector::Get_SmartConnector()->SetProtcolSwitch((CSmartConnector::protocolSwitch) RS232Settings::GetSettings()->m_nProtocol);
			CString csInfo = L"CModBusMasterDlg::OnBnClickedButtonConnect - port opened";

				// get dvr resource 
				GET_DVR_RESOURCE(hDVRmdl)
				if (hDVRmdl)
				{
					// Store the current resource handle
					HINSTANCE hClientResources = AfxGetResourceHandle();
					AfxSetResourceHandle(hDVRmdl);
					// connect button 
					pButtonConnect->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_CONNECT)));
					//pButtonConnect->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_UNCONNECT)));

					// Restore the client application resource handle
					AfxSetResourceHandle(hClientResources);
				}
				else
				{
					//EAG_LOG_ERROR(L"CModBusMasterDlg::OnBnClickedButtonConnect - can not load button picture ")
				}
			bPortOpen = true;
			if (S_OK == m_pISerialPort->StartMonitoring())
			{
				// AfxMessageBox(L"Start monitoring");
				; 
			
			}
			//Beep(1000, 500);
				//EAG_LOG_CRITICAL(L"CModBusMasterDlg::OnBnClickedButtonConnect() - monitoring started")
		}
		else
		{
			// error
			//		csError.LoadString(IDS_ERR_CANNOT_OPEN_COMPORT);
			AfxMessageBox(L"can not open the port !!", MB_ICONSTOP | MB_OK);
				return;
		}
	} // 

	else
	{
		if (m_pISerialPort->ClosePort() == S_OK)
		{
			// get dvr resource 
			GET_DVR_RESOURCE(hDVRmdl)
				if (hDVRmdl)
				{
					// Store the current resource handle
					HINSTANCE hClientResources = AfxGetResourceHandle();
					AfxSetResourceHandle(hDVRmdl);
					// connect button 
					pButtonConnect->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_UNCONNECT)));

					// Restore the client application resource handle
					AfxSetResourceHandle(hClientResources);
				}
			bPortOpen = false;

		}

	}
}

void CMotionControlDlg::OnBnClickedButtonTrajectory()
{
#ifdef  _DEBUG
	ShellExecuteA(
		NULL,				// A handle to the parent window used for displaying a UI or error messages
		"open",				// open
		"T1d.exe",			// executable
		"",					// parameter
		"C:/sw/bin/",		// working directory
		SW_SHOW);
#else
	ShellExecuteA(NULL, "open",
		"T1d.exe",
		"",
		"C:/sw/bin", SW_SHOW);
#endif //  _DEBUG
}

void CMotionControlDlg::OnBnClickedButtonEcho()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr;	
	BOOL bError;
	IMTelegram* pTelEcho = NULL;


	if (m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pISerialPort->CanDo()) {
		return;
	}

	try
	{
		if (m_pIFabric)	{
			hr = m_pIFabric->Echo((IUnknown**)&pTelEcho);
		} else 	{
			return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pISink->ClearError();
			hr = m_pISerialPort->SendDeTelTelegram(pTelEcho);
			if (hr != S_OK) {
				// release Telegram Echo over connector !!!!
				// com object interface created in a dll causes
				// ASSER error when tried to release out of dll
				// work-around: release over connector which is also created in a dll!!
				// this works
				CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
				//pTelEcho->Release();
				return;
			}
			// Error !
			m_pISink->get_error(&bError);
			// if (!bError)
			{

			}
			//pTelEcho->Release();
			CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
		}
	}
	catch (...)
	{

	}
/**/	
}

void CMotionControlDlg::OnBnClickedRadioAscii()
{
	RS232Settings::GetSettings()->m_nProtocol = CSmartConnector::PS_MODBUS_ASCII;
	CSmartConnector::Get_SmartConnector()->SetProtcolSwitch(CSmartConnector::PS_MODBUS_ASCII);
}

void CMotionControlDlg::OnBnClickedRadioRtu()
{
	RS232Settings::GetSettings()->m_nProtocol = CSmartConnector::PS_MODBUS_RTU;
	CSmartConnector::Get_SmartConnector()->SetProtcolSwitch(CSmartConnector::PS_MODBUS_RTU);
}

void CMotionControlDlg::OnBnClickedRadioDetel()
{
	RS232Settings::GetSettings()->m_nProtocol = CSmartConnector::PS_DETEL;
	CSmartConnector::Get_SmartConnector()->SetProtcolSwitch(CSmartConnector::PS_DETEL);
}

void CMotionControlDlg::OnBnClickedButton_ToMtnController()
{
	// TODO: Add your control notification handler code here
	CDialog::UpdateData();
	CDialog::OnInitDialog();

	if (m_fAccumulator == 0)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ACCELERATE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DECELARETE))->SetCheck(1);
		_action(bDECEL);

	}
	else if (m_fAccumulator < m_fV1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ACCELERATE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DECELARETE))->SetCheck(1);
		_action(bDECEL);
	}

	else if (m_fAccumulator >  m_fV1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ACCELERATE))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_DECELARETE))->SetCheck(0);
		_action(bACCEL);
	}

	m_fV1 = m_fAccumulator;
	CDialog::OnInitDialog();
}

void CMotionControlDlg::_action(int act, double data)
{
	
	HRESULT hr;
	BOOL bError;
	IMTelegram* pTelAction = NULL;
	// CDialog::OnInitDialog();

	uint32_t	*pData;
	uint32_t	dataBuffer[2]; // = { CNumberUtility::pack754_32(m_fAccel), CNumberUtility::pack754_32(m_fV1) };
	pData = dataBuffer;
	if (m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pISerialPort->CanDo()) {
		return;
	}
	


	try
	{
		if (m_pIFabric)  // enum {ACTION_START = 0x0011, ACTION_ACCEL, ACTION_DECEL, ACTION_STOP };
		{
			/// put telegram specific action code
			//////////////////////////
			if (act == bSTART) {
				dataBuffer[0] = CNumberUtility::pack754_32(m_fAccel);
				dataBuffer[1] = CNumberUtility::pack754_32(m_fAccumulator);
				hr = m_pIFabric->Action(ACTION_START, (BYTE**)&pData, 8, (IUnknown**)&pTelAction);			// start   
			}
			else if (act == bACCEL) {
				dataBuffer[0] = CNumberUtility::pack754_32(m_fAccel);
				dataBuffer[1] = CNumberUtility::pack754_32(m_fAccumulator);
				hr = m_pIFabric->Action(ACTION_ACCEL, (BYTE**)&pData, 8, (IUnknown**)&pTelAction);			// accel   
			}
			else if (act == bDECEL) {
				dataBuffer[0] = CNumberUtility::pack754_32(m_fAccel);
				dataBuffer[1] = CNumberUtility::pack754_32(m_fAccumulator);
				hr = m_pIFabric->Action(ACTION_DECEL, (BYTE**)&pData, 8, (IUnknown**)&pTelAction);			// decel   0x010
			}
			else if (act == bSTOP) {
				dataBuffer[0] = CNumberUtility::pack754_32(m_fAccel);
				dataBuffer[1] = CNumberUtility::pack754_32(0.0); m_fAccumulator = 0;
				hr = m_pIFabric->Action(ACTION_STOP, (BYTE**)&pData, 8, (IUnknown**)&pTelAction);			// stop   0x016

			}
			else if (act == bWRITE_RESONATOR) {
				dataBuffer[0] = CNumberUtility::pack754_32(data);
				hr = m_pIFabric->Action(ACT_WRITE_RESONATOR, (BYTE**)&pData, 4, (IUnknown**)&pTelAction);			// stop   0x016

			}
			else return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pISink->ClearError();
			hr = m_pISerialPort->SendDeTelTelegram(pTelAction);
			if (hr != S_OK) {
				// release over connector
				CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
				return;
			}
			// Error !
			m_pISink->get_error(&bError);
			// if (!bError)
			{

			}
			// release over connector
			CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
		}
	}
	catch (...)
	{

	}

}

void CMotionControlDlg::OnBnClickedButton_ReadAc()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;
	BOOL bError;
	IMTelegram* pTelAction = NULL;
	BYTE data[1] = { 0 };

	if (m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pISerialPort->CanDo()) {
		return;
	}

	try
	{
		if (m_pIFabric) {
			hr = m_pIFabric->Action(ACT_READ_ACCELERATOR, (BYTE**) &data, 0, (IUnknown**)&pTelAction);
		}
		else {
			return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pISink->ClearError();
			hr = m_pISerialPort->SendDeTelTelegram(pTelAction);
			if (hr != S_OK) {
				// release over connector
				CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
				return;
			}
			// Error !
			m_pISink->get_error(&bError);
			// if (!bError)
			{

			}
			// release over connector
			CSmartTelFabConnector::Get_SmartTelFabConnector()->releaseLastTelegram();
		}
	}
	catch (...)
	{

	}
}

void CMotionControlDlg::OnReleasedcapture_SliderAccu(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	float range = m_fAccelerator_upper_bound;
	if (range == 0.0)
		range = 1000.0;
	m_fAccumulator= ((m_sldrAccu.GetPos() * 1.0) / 100.0) * range;
	*pResult = 0;

	CDialog::OnInitDialog();
	OnBnClickedButton_ToMtnController();
}

void CMotionControlDlg::OnBnClickedButtonOnOff()
{
	if ( ((CButton*)GetDlgItem(IDC_RADIO_START))->GetCheck() == 1)
	{
		CDialog::UpdateData();
	 if (m_fAccumulator == 0.0)
		AfxMessageBox(L"No starting frequency, please set accumulator", MB_OK | MB_ICONEXCLAMATION);
	 else  
			_action(bSTART);
	}

	else if (((CButton*)GetDlgItem(IDC_RADIO_STOP))->GetCheck() == 1)
	{
		_action(bSTOP);
	}
}

void CMotionControlDlg::OnBnClickedButtonTimer2()
{
	CNonModalDlg<CTimer_2> rs_dlg;
	rs_dlg.m_pMCDlg = this;
	p_mCTimer_2 = &rs_dlg;
	rs_dlg.DoModal();
}

void CMotionControlDlg::OnBnClickedButtonTimer1()
{
	CNonModalDlg<CTimer_1> t1_dlg;
	
	t1_dlg.m_pMCDlg = this;
	p_mCTimer_1 = &t1_dlg;
	t1_dlg.DoModal();
}

void CMotionControlDlg::OnClose()
{
	if (p_mCTimer_1)
		p_mCTimer_1->SendMessage(WM_CLOSE);
	if (p_mCTimer_2)
		p_mCTimer_2->SendMessage(WM_CLOSE);

	CDialogEx::OnClose();
}

void CMotionControlDlg::OnEnChangeEditAccu()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}



