
// ServoControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServoControl.h"
#include "ServoControlDlg.h"
#include "afxdialogex.h"


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
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServoControlDlg dialog

CServoControlDlg * CServoControlDlg::theObjectPtr = 0;
LONG CServoControlDlg::CallBackFunction()
{
	CServoControlDlg* pSelf = theObjectPtr;
	pSelf->update();
	return 1;
}
void CServoControlDlg::_update()
{
	unsigned char *pBufferAscii = NULL, *pBufferAscii_Shifted = NULL; /* without ':' */
	unsigned char pBfChar[4 * CBase::TP_MAXIMUM_SIZE], *pChar = pBfChar;
	unsigned char pNew[100] , *pBNew = pNew;
	memset(pNew, '\0', 100);
	
	// for convertion purpose
	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;

	} convert;

	pBufferAscii_Shifted = pBufferAscii + sizeof(unsigned char);
	
	SHORT Short = 0;
	int nSize = 0;

	if (m_pISink)
	{

		m_pISink->get_ReceiveBuffer(&pBufferAscii);
		pBufferAscii_Shifted = pBufferAscii + sizeof(unsigned char);
		m_pISink->get_lengthReceivedChars(&Short);
		#ifdef DEBUG
		char * filename = "c:\\temp\\gelen.txt";
		/// log into file 
		///
		CLogUtility::Log(pBufferAscii, nSize, 20, filename);
		#endif
		CNumberUtility::HexBuffer2CharBuffer(&pBufferAscii_Shifted, &pChar,  4 * CBase::TP_MAXIMUM_SIZE, Short);

		memcpy(pNew, pBufferAscii, sizeof(char) * Short);
		

		memset(pNew, '\0', 100);
		CNumberUtility::HexBuffer2CharBuffer( &pBufferAscii_Shifted, &pBNew,  (Short-4)/2, Short);   // extract only Control buffer

		// overall size
		int iCarCounter = 0;	
		convert.ch[0] = pNew[iCarCounter++];
		convert.ch[1] = pNew[iCarCounter++];
		convert.ch[2] = pNew[iCarCounter++];
		convert.ch[3] = pNew[iCarCounter++];
		int ov_size = convert.nInteger;
		
		// DATA
		// data buffer  size in char
		int size = (ov_size - 20) / 2;
		float data[10];
		int ix = 0;
		
		for (ix = 0; ix < size/4; ix++)  // loop on float 
		{
			convert.ch[0]= pNew[iCarCounter++];
			convert.ch[1] = pNew[iCarCounter++];
			convert.ch[2] = pNew[iCarCounter++];
			convert.ch[3] = pNew[iCarCounter++];
			data[ix] = convert.fFloat;
		}

		// INDEX
		convert.ch[0] = pNew[iCarCounter++];
		convert.ch[1] = pNew[iCarCounter++];
		convert.ch[2] = pNew[iCarCounter++];
		convert.ch[3] = pNew[iCarCounter];
		int index = convert.nInteger;

		m_RT_Viewer.Update_Channel_List_Local(&data[0], 1, index, 0);


		nSize = 1;
	}

}

void CServoControlDlg::update() 
{
	CEdit*pEdit = NULL;
	//wchar_t buffer[8];
	CString cs;
	unsigned char	*pData = NULL,  command = 0;
	static WORD nIndexExpected = 0;
	static float fPrevious = 0.0, fDumy = 0.0, fDifference = 0.0;
	WORD  nIndex = 0, nDifferenceIndex =0 ;
	uint16_t cSize = 0;
	//*pBufferAscii_Shifted = NULL; /* without ':' */
//	unsigned char pBfChar[4 * CBase::TP_MAXIMUM_SIZE], *pChar = pBfChar;
//	static unsigned char pNew[100];
//	unsigned char *pBNew = pNew;
	char cDumy;

	int nResultSerialFeed = 0;

	//memset(pNew, '\0', 100);
	SHORT lengt = 0;;

	// for convertion purpose
	

	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;

	} convert;
/**/

	if (m_pISink)
	{
		m_pISink->get_LastReceived(&cDumy);
	//	pBufferAscii_Shifted = pBufferAscii + sizeof(unsigned char);
	//	m_pISink->get_lengthReceivedChars(&lengt);

		nResultSerialFeed = m_SerialReceiveBuffer.Feed(cDumy);
		m_SerialReceiveBuffer.GetDataBuffer((unsigned char **) &pData, cSize, nIndex);

		if (nResultSerialFeed == 0)
		{
			// do nothing 
			lengt = lengt;
		}

		else if (nResultSerialFeed == 1)
		{
			// buffer ok
			//lengt = lengt;
			//SERVO_PID_SEND 
			command = m_SerialReceiveBuffer.GetInternalBufferAt(COMMAND_BYTE);


			switch (command)
			{
			
			case CAsyncronServoComProtocol::SERVO_MOVE_DATA :
				if (nIndex == 0)
					nIndexExpected = 0;
				/**/
				convert.ch[0] = *pData;
				convert.ch[1] = *(pData +1);
				convert.ch[2] = *(pData +2);
				convert.ch[3] = *(pData + 3);

				//if (convert.fFloat > 1.0)
				//	AfxMessageBox(L"Hopla");
			
				// investigate if the index comforms withe expected index
				nDifferenceIndex = nIndex - nIndexExpected;
				if ((nDifferenceIndex))
				{
					fDifference = (convert.fFloat - fPrevious) / nDifferenceIndex;
					for (WORD it = 0; it < nDifferenceIndex; it++) 
					{
						fDumy = fPrevious + fDifference * it;
						// m_RT_Viewer.Update_Channel_List_Local(&fDumy, 1, ((nIndexExpected + it) % CBase::TP_MAXIMUM_SIZE), 0);
						m_RT_Viewer.Update_Channel_List_Local(&fDumy, 1, ((nIndexExpected++) % CBase::TP_MAXIMUM_SIZE), 0);
					}
				}
				else
				{
					//m_RT_Viewer.Update_Channel_List_Local(&convert.fFloat, 1, nIndex, 0);
					m_RT_Viewer.Update_Channel_List_Local((float*)pData, cSize/4, nIndex, 0);
					nIndexExpected++;
				}
				nIndexExpected = nIndexExpected % CBase::TP_MAXIMUM_SIZE;
				fPrevious = convert.fFloat;
				break;

			case CAsyncronServoComProtocol::SERVO_PID_SEND:
				m_fKP = m_SerialReceiveBuffer.GetParameterAsfloat(0);
				m_fKI = m_SerialReceiveBuffer.GetParameterAsfloat(1);
				m_fKD = m_SerialReceiveBuffer.GetParameterAsfloat(2);
				m_fTsa = m_SerialReceiveBuffer.GetParameterAsfloat(3);

				cs.Format(L"%f", m_fKP);  pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KP);  	pEdit->SetWindowTextW(cs);
				cs.Format(L"%f", m_fKI);  pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KI);  	pEdit->SetWindowTextW(cs);
				cs.Format(L"%f", m_fKD);  pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KD);  	pEdit->SetWindowTextW(cs);
				cs.Format(L"%f", m_fTsa);  pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TSA);  	pEdit->SetWindowTextW(cs);
			}
		}  

		else if (nResultSerialFeed == -1)
		{
			// error 
			lengt = lengt;
		}
	}

}

CServoControlDlg::CServoControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVOCONTROL_DIALOG, pParent)
	, m_fKP(1)
	, m_fKI(0)
	, m_fKD(0)
	, m_fReference(1.0)
	, m_fTsa(0.001)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pISerialPort = NULL;
	m_pISink = NULL;
	theObjectPtr = this;
}

void CServoControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KP, m_fKP);
	DDX_Text(pDX, IDC_EDIT_KI, m_fKI);
	DDX_Text(pDX, IDC_EDIT_KD, m_fKD);
	DDX_Control(pDX, IDC_SLIDER_AMPLITUDE, m_sldrAmplitude);
	DDX_Text(pDX, IDC_EDIT_REFERENCE, m_fReference);
	DDX_Text(pDX, IDC_EDIT_TSA, m_fTsa);
}

BEGIN_MESSAGE_MAP(CServoControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CServoControlDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_PORT_SETTING, &CServoControlDlg::OnBnClickedButtonPortSetting)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PID_WRITE, &CServoControlDlg::OnBnClickedButtonPidWrite)
	ON_BN_CLICKED(IDC_BUTTON_PID_READ, &CServoControlDlg::OnBnClickedButtonPidRead)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_AMPLITUDE, &CServoControlDlg::OnThumbposchangingSliderAmplitude)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AMPLITUDE, &CServoControlDlg::OnReleasedcaptureSliderAmplitude)
	ON_BN_CLICKED(IDC_BUTTON_START, &CServoControlDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CServoControlDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()

// CServoControlDlg message handlers

BOOL CServoControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	CRect rect;
	GetDlgItem(IDC_STATIC_RT_VIEWER)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_RT_Viewer.Create(rect, this);
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
		CButton* pButtonConnect = (CButton*)GetDlgItem(IDC_BUTTON_CONNECT);
		CButton* pButtonPortSetting = (CButton*)GetDlgItem(IDC_BUTTON_PORT_SETTING);
		pButtonConnect->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_UNCONNECT)));
		pButtonPortSetting->SetBitmap(LoadBitmap(hDVRmdl, MAKEINTRESOURCE(IDB_SETTING)));

		// Restore the client application resource handle
		AfxSetResourceHandle(hClientResources);
	}

		// update RS232 Settings
		RS232Settings::GetSettings()->ReadFromRegistry(L"SOFTWARE", L"AyfaLtd\\ServoControl");
		SHORT protocolSwitch = RS232Settings::GetSettings()->m_nProtocol;
		// protocolSwitch = CSmartConnector::PS_MODBUS_ASCII;
		protocolSwitch = CSmartConnector::PS_KEINE;

		m_pISerialPort = CSmartConnector::Get_SmartConnector(&CServoControlDlg::CallBackFunction, protocolSwitch)->m_pISerialPort;
		CSmartConnector::Get_SmartConnector()->GetSink((IUnknown**)&m_pISink);// { *ppSink = m_pISink; }

		m_sldrAmplitude.SetRange(0, 10);
		m_sldrAmplitude.SetSelection(2, 98);
		m_sldrAmplitude.SetPos(10);
		

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServoControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServoControlDlg::OnPaint()
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
HCURSOR CServoControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServoControlDlg::OnBnClickedButtonConnect()
{
	static bool bPortOpen = false;
	//	EAG_LOG_DEBUG(L"CPageConnections::OnButtonConnect - start")
	CString csCaption;
	CString csError;
	CString csPortString;

	/*
	CString csBaud = gsBaud[m_nBaudRate];
	CString csPrty = gsParity[m_nParitySel];
	CString	csData = gsDataBit[m_nDataBitSel];
	CString csStop = gsStopBit[m_nStopBitSel];
	*/
	CString csBaud = gsBaud[RS232Settings::GetSettings()->m_nBaudRate];
	CString csPrty = gsParity[RS232Settings::GetSettings()->m_nParityIndex];
	CString	csData = gsDataBit[RS232Settings::GetSettings()->m_nDataBitIndex];
	CString csStop = gsStopBit[RS232Settings::GetSettings()->m_nStopBitIndex];
	DWORD   dwCommEvents = RS232Settings::GetSettings()->m_dwCommEvents;
	int		nPortSel = RS232Settings::GetSettings()->m_nComPort;


	csPortString.Format(_T("CPP_Parameters::OnBnClickedButtonConnect() - \n <port=%i> <baud=%s> <parity=%s> <databits=%s> <stopbits=%s>"), nPortSel, csBaud, csPrty, csData, csStop);
	//EAG_LOG_DEBUG(csPortString)

	///AfxMessageBox(csPortString);

	CButton* pButtonConnect = (CButton*)GetDlgItem(IDC_BUTTON_CONNECT);

	BSTR bstr = csPortString.AllocSysString();

	if (m_pISerialPort == NULL)
	{
		//EAG_LOG_ERROR("port object is not available !") 
		//		csError.LoadString(IDS_ERR_NOPORTOBJECT);
		//		EAG_LOG_ERROR(csError) 					
		SysFreeString(bstr);
		return;
	}

	if (bPortOpen == false)
	{
		// if already open return 
		if (m_pISerialPort->CanDo() == S_OK)
		{
			SysFreeString(bstr);

			return;
		}

		m_pISerialPort->put_ComEvents(dwCommEvents);

		if (S_OK == m_pISerialPort->OpenPort(&bstr, nPortSel))
		{
			////m_pISerialPort->StartMonitoring();
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
				;
			//EAG_LOG_CRITICAL(L"CModBusMasterDlg::OnBnClickedButtonConnect() - monitoring started")
		}
		else
		{
			// error
			//		csError.LoadString(IDS_ERR_CANNOT_OPEN_COMPORT);
			//EAG_LOG_ERROR(L"can not open the port !!")
			SysFreeString(bstr);
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

	SysFreeString(bstr);
}


void CServoControlDlg::OnBnClickedButtonPortSetting()
{
	CRunDlg r(this);
	if (IDOK == r.Run(CRunDlg::RS232_SETTINGS))
	{

	}
/*
	//CDdlgRS232 dlg(this);
	CAboutDlg dlg;
	dlg.DoModal();

*/
}


void CServoControlDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// kill  Settings
	RS232Settings::GetSettings()->WriteIntoRegistry(L"SOFTWARE", L"AyfaLtd\\ServoControl");
	RS232Settings::GetSettings()->KillSettings();
	CSmartConnector::Get_SmartConnector()->Kill_SmartConnector();
}


void CServoControlDlg::OnBnClickedButtonPidWrite()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int i = 0, j = 0;
	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;
	} convert;

	unsigned char parameters[MAX_SZ];
	unsigned char *prtcl = NULL; // [4 * CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 8];
	int nEntire_Prtcl_Size = 0;
	CAsyncronServoComProtocol  pidWrite;

	// build up PID parameters
	convert.fFloat = m_fKP;
	for (j = 0; j < 4; j++)
		parameters[i++] = convert.ch[j];
	convert.fFloat = m_fKI;
	for (j = 0; j < 4; j++)
		parameters[i++] = convert.ch[j];
	convert.fFloat = m_fKD;
	for (j = 0; j < 4; j++)
		parameters[i++] = convert.ch[j];

	convert.fFloat = m_fTsa;
	for (j = 0; j < 4; j++)
		parameters[i++] = convert.ch[j];

	// 		static bool GetProtocol(CAsyncronServoComProtocol& scp, unsigned char command, unsigned char prParameters[], uint16_t sizeParameter , uint16_t index);

	if (true == CFabric_AsyncronServoComProtocol::GetProtocol(pidWrite, CAsyncronServoComProtocol::SERVO_PID_WRITE, parameters, 4*4, 0))
	{
		pidWrite.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
			}
		}
	}
}


void CServoControlDlg::OnBnClickedButtonPidRead()
{
	unsigned char parameters[MAX_SZ];
	unsigned char *prtcl = NULL; // [4 * CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 8];
	int nEntire_Prtcl_Size = 0;

	CAsyncronServoComProtocol  pidRead;
	//static bool GetProtocol(CAsyncronServoComProtocol& scp, unsigned char command, unsigned char prParameters[], uint16_t sizeParameter, uint16_t index);
	if (true == CFabric_AsyncronServoComProtocol::GetProtocol(pidRead, CAsyncronServoComProtocol::SERVO_PID_READ, parameters,16,0))
	{
		pidRead.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
			}
		}
	}
	
}


void CServoControlDlg::OnThumbposchangingSliderAmplitude(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CServoControlDlg::OnReleasedcaptureSliderAmplitude(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CEdit * pEdit = NULL;
	CString cs;
	//float range = m_fAccelerator_upper_bound;
	//if (range == 0.0)
	//	range = 1000.0;
	m_fReference = ((m_sldrAmplitude.GetPos() * 1.0) / 10.0) ;
	*pResult = 0;

	cs.Format(L"%f", m_fReference);  pEdit = (CEdit*)GetDlgItem(IDC_EDIT_REFERENCE);  	pEdit->SetWindowTextW(cs);
//	OnInitDialog();

	// TODO: Add your control notification handler code here
//	UpdateData();
	int i = 0, j = 0;
	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;

	} convert;

	unsigned char parameters[MAX_SZ];
	unsigned char *prtcl = NULL; // [4 * CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 8];
	int nEntire_Prtcl_Size = 0;

	CAsyncronServoComProtocol  protocolReference;

	// build up PID parameters
	convert.fFloat = m_fReference;
	for (j = 0; j < 4; j++)
		parameters[i++] = convert.ch[j];



	if (true == CFabric_AsyncronServoComProtocol::GetProtocol(protocolReference, CAsyncronServoComProtocol::SERVO_REFERENCE, parameters,4,0))
	{
		protocolReference.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
			}
		}
	}



	*pResult = 0;
}


void CServoControlDlg::OnBnClickedButtonStart()
{
	//UpdateData();

	unsigned char parameters[MAX_SZ];
	unsigned char *prtcl = NULL; // [4 * CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 8];
	int nEntire_Prtcl_Size = 0;
	CAsyncronServoComProtocol  start;

	if (true == CFabric_AsyncronServoComProtocol::GetProtocol(start, CAsyncronServoComProtocol::SERVO_MOVE_START, parameters,0,0))
	{
		start.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
			}
		}
	}
}


void CServoControlDlg::OnBnClickedButtonStop()
{
	//UpdateData();
	unsigned char parameters[MAX_SZ];
	unsigned char *prtcl = NULL; // [4 * CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 8];
	int nEntire_Prtcl_Size = 0;

	CAsyncronServoComProtocol  stop;

	if (true == CFabric_AsyncronServoComProtocol::GetProtocol(stop, CAsyncronServoComProtocol::SERVO_MOVE_STOP, parameters,0,0))
	{
		stop.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
			}
		}
	}
}
