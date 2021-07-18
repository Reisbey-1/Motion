
// ServoMotor_ModelSimulationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServoMotor_ModelSimulation.h"
#include "ServoMotor_ModelSimulationDlg.h"
#include "afxdialogex.h"
#include "WindowsPainter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 int CServoMotor_ModelSimulationDlg::geciciSayac = 0;


CServoMotor_ModelSimulationDlg *CServoMotor_ModelSimulationDlg::theObjectPtr = 0;
char ** CServoMotor_ModelSimulationDlg::m_ppBuffer = NULL;
//int     CServoMotor_ModelSimulationDlg::m_nIndex = 0; 
//int     CServoMotor_ModelSimulationDlg::m_nSize = 0;

CRingBuffer<float> * CServoMotor_ModelSimulationDlg::m_pRingBuffer = 0;
////////////////////////////////////////////////////////////////////////
// callback : serial port                                            ///
////////////////////////////////////////////////////////////////////////
LONG CServoMotor_ModelSimulationDlg::CallBack_SerialPort()
{
	CServoMotor_ModelSimulationDlg* pSelf = theObjectPtr;
	pSelf->update();
	return 1;


}
void CServoMotor_ModelSimulationDlg::update()
{
	CEdit * pEdt = NULL;
	unsigned char parameters[MAX_SZ];
//	unsigned char prtcl[CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 7];
	unsigned char *  prtcl = NULL; // [CAsyncronServoComProtocol::MAXIMUM_SIZE_PARAMETER + 7];

	int nEntire_Prtcl_Size = 0;
	CAsyncronServoComProtocol  dumyProtocol;
	int i = 0, j = 0;
	wchar_t buffer[8];
	CString cs;

	unsigned char	*pData = NULL,  command = 0, cDumy = 0;
	WORD  nIndex = 0;
	int nResultSerialFeed = 0;
	uint16_t cSize = 0;
	SHORT lengt = 0;;
/**/
	// for convertion purpose

	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;

	} convert;
	

	if (m_pISink)
	{
		m_pISink->get_LastReceived((CHAR*)&cDumy);
			nResultSerialFeed = m_SerialReceiveBuffer.Feed(cDumy);
		m_SerialReceiveBuffer.GetDataBuffer((unsigned char **)&pData, cSize, nIndex);

		if (nResultSerialFeed == 0)		{
			// do nothing 
			lengt = lengt;
	
		}

		else if (nResultSerialFeed == 1) 		
		{
			//command = m_SerialReceiveBuffer.GetInternalBufferAt(4);  
			command = m_SerialReceiveBuffer.GetInternalBufferAt(COMMAND_BYTE);  

			switch (command)
			{

			case CAsyncronServoComProtocol::SERVO_MOVE_START:
				OnBnClickedButtonStart();
				break;
	
			case CAsyncronServoComProtocol::SERVO_MOVE_STOP:
				OnBnClickedButtonStop();
				break;

			case CAsyncronServoComProtocol::SERVO_PID_READ:
			//	CDialog::UpdateData();
			//	Invalidate();
				// kp, ki, kd constants
				// prapere new protocol SERVO_PID_READ with parameter buffer
				// send it to master
				// 
				i= 0;
				pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KP);   pEdt->GetWindowTextW(buffer, 10);  m_fKP = _wtof(buffer);
				pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KI);   pEdt->GetWindowTextW(buffer, 10);  m_fKI = _wtof(buffer);
				pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KD);   pEdt->GetWindowTextW(buffer, 10);  m_fKD = _wtof(buffer);
				pEdt = (CEdit *)GetDlgItem(IDC_EDIT_DT);   pEdt->GetWindowTextW(buffer, 10);  m_dt = _wtof(buffer);

				convert.fFloat = m_fKP; parameters[i++] = convert.ch[0];  parameters[i++] = convert.ch[1];  parameters[i++] = convert.ch[2]; parameters[i++] = convert.ch[3];
				convert.fFloat = m_fKI; parameters[i++] = convert.ch[0];  parameters[i++] = convert.ch[1];  parameters[i++] = convert.ch[2]; parameters[i++] = convert.ch[3];
				convert.fFloat = m_fKD; parameters[i++] = convert.ch[0];  parameters[i++] = convert.ch[1];  parameters[i++] = convert.ch[2]; parameters[i++] = convert.ch[3];
				convert.fFloat = m_dt;  parameters[i++] = convert.ch[0];  parameters[i++] = convert.ch[1];  parameters[i++] = convert.ch[2]; parameters[i++] = convert.ch[3];

				if (true == CFabric_AsyncronServoComProtocol::GetProtocol(dumyProtocol, CAsyncronServoComProtocol::SERVO_PID_SEND, parameters, 16,0))
				{
					dumyProtocol.GetEntireProtocolBuffer(&prtcl, nEntire_Prtcl_Size);
					if (m_pISerialPort)
					{
						if (m_pISerialPort->CanDo() == S_OK)
						{
							m_pISerialPort->WriteBuffer(prtcl, nEntire_Prtcl_Size);
						}
					}
				
				}
				return ;
				break;

			case CAsyncronServoComProtocol::SERVO_PID_WRITE:
				
				// update PID from parameters
				/*
				for (j = 0; j < 4; j++)
					convert.ch[j] = m_SerialReceiveBuffer.GetInternalBufferAt(7 + i++);
				m_fKP = convert.fFloat;
				for (j = 0; j < 4; j++)
					convert.ch[j] = m_SerialReceiveBuffer.GetInternalBufferAt(7 + i++);
				m_fKI = convert.fFloat;
				for (j = 0; j < 4; j++)
					convert.ch[j] = m_SerialReceiveBuffer.GetInternalBufferAt(7 + i++);
				m_fKD = convert.fFloat;
*/
				m_fKP = m_SerialReceiveBuffer.GetParameterAsfloat(0);
				m_fKI = m_SerialReceiveBuffer.GetParameterAsfloat(1);
				m_fKD = m_SerialReceiveBuffer.GetParameterAsfloat(2);
				m_dt  = m_SerialReceiveBuffer.GetParameterAsfloat(3);

				cs.Format(L"%f", m_fKP);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KP);  	pEdt->SetWindowTextW(cs);
				cs.Format(L"%f", m_fKI);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KI);  	pEdt->SetWindowTextW(cs);
				cs.Format(L"%f", m_fKD);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KD);  	pEdt->SetWindowTextW(cs);
				cs.Format(L"%f", m_dt);   pEdt = (CEdit*)GetDlgItem(IDC_EDIT_DT);  	pEdt->SetWindowTextW(cs);

				break;

			case CAsyncronServoComProtocol::SERVO_REFERENCE:
				m_fRefSignal = m_SerialReceiveBuffer.GetParameterAsfloat(0);
				cs.Format(L"%f", m_fRefSignal);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_REFERENCE);  	pEdt->SetWindowTextW(cs);
				if (m_pMotorControlSystem)
					m_pMotorControlSystem->SetReference(m_fRefSignal);

				break;
			default:
				return ;
				break;
			}
			
		}

		else if (nResultSerialFeed == -1) 		{
			// error 
			lengt = lengt;
		}
	}



}

////////////////////////////////////////////////////////////////////////
// callback : control system                                         ///
////////////////////////////////////////////////////////////////////////


/*
#define CHAR_SIZE_OF_FLOAT  4
#define CHAR_SIZE_OF_INT	4
#define MAX_SZ              1401														// maximum allovable parmater size 
#define MAX_OVERALL_SIZE       CHAR_SIZE_OF_FLOAT* CHAR_SIZE_OF_FLOAT* (MAX_SZ) + 8		// maximum overall size 
*/

void CServoMotor_ModelSimulationDlg::CalledFrom_System(char  *pBuffer, uint16_t index, uint16_t sizeBuffer)
{

	//if (geciciSayac)
	//	return;
	//geciciSayac++;
 //see document ; C:\sw\MotionControl\ServoMotor_ModelSimulation\doc\Model.doc
/*
	char L = 'A';
	char H = '1';
	char V = 0;
	CNumberUtility::get_From_Hex(V, L, H);  /// yanlis ?????????????????
	CNumberUtility::put_In_Hex(V, L, H);

	float   :  4 Byte
	int		:  4 Byte
*/	
	/*
	static int nSayici = 0;
	if (nSayici > 2){
		nSayici = 0;
		return;
	}
*/
	// for convertion purpose
	union {
		unsigned char ch[4];
		int  nInteger;
		float fFloat;

	} convert; 

	convert.ch[0] = pBuffer[0];
	convert.ch[1] = pBuffer[1];
	convert.ch[2] = pBuffer[2];
	convert.ch[3] = pBuffer[3];

//	if (convert.fFloat > 1.0)
//		AfxMessageBox(L"Hopla _MotorSimulation");

	

	int slidingIndex = 0;
	int ov_size = CHAR_SIZE_OF_FLOAT * (sizeBuffer / CHAR_SIZE_OF_FLOAT) + 8;
	
	if (MAX_OVERALL_SIZE < ov_size)
		return; // !! error case increase MAX_ASCI_SIZE

	unsigned char cAsci2[MAX_OVERALL_SIZE], *pAscii2 = cAsci2, *pAscii_Shift2 = (cAsci2 + sizeof(unsigned char));   // +3     //':'=0x3A , CR=0x0A,  LF=0x0D,  
	pAscii2[slidingIndex++] = ':';   
/*
	convert.nInteger = ov_size;
	CNumberUtility::CharBuffer2HexBuffer(convert.ch, &cAsci2[slidingIndex], 4, 8); 
	slidingIndex = slidingIndex + 8;    // overall
	
	if (FALSE == CNumberUtility::CharBuffer2HexBuffer((unsigned char*)(pBuffer), &cAsci2[slidingIndex], size , size * 2))  
		return;
	slidingIndex = slidingIndex + size * 2;  // buffer
	
	convert.nInteger = index;
	CNumberUtility::CharBuffer2HexBuffer(convert.ch, &cAsci2[slidingIndex], 4, 8); 
	slidingIndex = slidingIndex + 8;
	
	cAsci2[slidingIndex++] = 0x0D;
	cAsci2[slidingIndex++] = 0x0A;
	cAsci2[slidingIndex] = '\0';

	#ifdef DEBUG
	/// log into file 
	char * filename = "c:\\temp\\motordan.txt";
 	CLogUtility::Log(pAscii2, ov_size, 20, filename);
	#endif // DEBUG
*/

	/////////////////////////////////////////////////////////////////////////////////

	CAsyncronServoComProtocol servoCommunication(CAsyncronServoComProtocol::SERVO_MOVE_DATA,0,0);
	CFabric_AsyncronServoComProtocol::GetProtocol(servoCommunication, CAsyncronServoComProtocol::SERVO_MOVE_DATA, (unsigned char*) pBuffer, sizeBuffer, index);
//	servoCommunication.InternalBuild((unsigned char*)pBuffer, index, sizeBuffer);


	unsigned char* pControlBuffer = servoCommunication.GetControlBuffer();
	unsigned char Size_ControlBuffer = servoCommunication.GetControlBufferSize();
	unsigned char *pCheckSum = servoCommunication.GetChecSumBuffer();

	//unsigned char prtcl[MAX_OVERALL_SIZE];
	unsigned char* prtcl = NULL;
	int length = 0;
	servoCommunication.GetEntireProtocolBuffer(&prtcl, length);

	if (m_pISerialPort)
	{
		if (m_pISerialPort->CanDo() == S_OK)
		{ 
			m_pISerialPort->WriteBuffer((BYTE*)prtcl, length);

	/*		
			m_pISerialPort->WriteBuffer((BYTE*)pControlBuffer,  (int)Size_ControlBuffer);
			Sleep(1);
			m_pISerialPort->WriteBuffer((BYTE*)pBuffer, (int)sizeBuffer);
			Sleep(1);
			m_pISerialPort->WriteBuffer((BYTE*)pCheckSum, 1);
			
			*/
		}
	}
	CStatic * pPaintWindow = (CStatic *)GetDlgItem(IDC_STATI_MOTOR_GRAPH);
	pPaintWindow->SetFocus();
	pPaintWindow->Invalidate(NULL);
	pPaintWindow->RedrawWindow();
	//Update();
	//OnPaint();
	// nSayici ++;
	 Invalidate(NULL);
	return;

}
LONG CServoMotor_ModelSimulationDlg::CallBack_System(char *pBuffer, uint16_t index, uint16_t size)
{
	CServoMotor_ModelSimulationDlg* pSelf = theObjectPtr;
	pSelf->CalledFrom_System(pBuffer, index, size);
	return 1;
}

CServoMotor_ModelSimulationDlg::CServoMotor_ModelSimulationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVOMOTOR_MODELSIMULATION_DIALOG, pParent)
	, m_fKP(1)							// 
	, m_fKI(0)
	, m_fKD(0)
	, m_dt(0.01)
	// The motor chosen to make the simulation is the Maxon DC motor 2140.937, with the following characteristics :
	// Mechanical time constant = 30 ms Jm/b = 30E-3 => b = 2.21E-6/30E-3 = 7.36E-5 Nm/ (rad./s) 
	// Electrical time constant = L/R = 5.02E-3/41.5 = 0.12 ms, as we can confirm, it can be neglected comparing
	, m_fJ(2.21E-6)							// motor inertia   kg.m²
	, m_fb(7.36E-5)							// 7.36E-5 Nm / (rad./s)
	, m_fKe(55.2E-3)						// Ke = 55.2E-3 V/rad/sec		; Speed constant = 173 rpm/V ; 
	, m_fKt(55.2E-3)						// Kt = 55.2E-3 N.m/A		    ; Speed constant = 173 rpm/V ; 
	, m_fR(41.5)							// coil resistance (ohm)
	, m_fL(0.0)								// coil inductance H  (neglecible small)
	, m_fN(1)
	, m_fTi(0)
	, m_fVi(0)
	, m_fYi(0.0)
	, m_fRefSignal(1.0)
	, m_dt_Asyncron(0)
	, m_fLoadTorgue(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMotorControlSystem = NULL;
	theObjectPtr = this;
}
void CServoMotor_ModelSimulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KP, m_fKP);
	DDX_Text(pDX, IDC_EDIT_KI, m_fKI);
	DDX_Text(pDX, IDC_EDIT_KD, m_fKD);
	DDX_Text(pDX, IDC_EDIT_DT, m_dt);
	DDX_Text(pDX, IDC_EDIT_J, m_fJ);
	DDX_Text(pDX, IDC_EDIT_B, m_fb);
	DDX_Text(pDX, IDC_EDIT_Ke, m_fKe);
	DDX_Text(pDX, IDC_EDIT_Kt, m_fKt);
	DDX_Text(pDX, IDC_EDIT_REDUCTION, m_fN);
	DDX_Text(pDX, IDC_EDIT_R, m_fR);

	DDX_Text(pDX, IDC_EDIT_TI, m_fTi);
	//  DDX_Text(pDX, IDC_EDIT_Yi, m_fVi);
	DDX_Text(pDX, IDC_EDIT_Vi, m_fVi);
	DDX_Text(pDX, IDC_EDIT_Yi, m_fYi);
	DDX_Text(pDX, IDC_EDIT_REFERENCE, m_fRefSignal);
	DDX_Text(pDX, IDC_EDIT_DT_ASENKRON, m_dt_Asyncron);
	DDX_Text(pDX, IDC_EDIT_LOAD_TORQUE, m_fLoadTorgue);
}

BEGIN_MESSAGE_MAP(CServoMotor_ModelSimulationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_PORT_SETTING, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonPortSetting)
	ON_BN_CLICKED(IDC_BUTTON_ASENKRON, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonAsenkron)

	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO_ENABLE_ASENKRON, &CServoMotor_ModelSimulationDlg::OnBnClickedRadioEnableAsenkron)
	ON_BN_CLICKED(IDC_RADIO_ENABLE_SENKRON, &CServoMotor_ModelSimulationDlg::OnBnClickedRadioEnableSenkron)
	ON_BN_CLICKED(IDC_BUTTON_RESET_SYSTEM, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonResetSystem)
	ON_BN_CLICKED(IDC_BUTTON_RESET_SYSTEM_ASENKRON, &CServoMotor_ModelSimulationDlg::OnBnClickedButtonResetSystemAsenkron)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////
// CServoMotor_ModelSimulationDlg message handlers                   ///
////////////////////////////////////////////////////////////////////////

BOOL CServoMotor_ModelSimulationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//////////////////////////////////////////////////////
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

	RS232Settings::GetSettings()->ReadFromRegistry(L"SOFTWARE", L"AyfaLtd\\ServoMotorModelSimulation");

	// update RS232 Settings
	SHORT protocolSwitch = RS232Settings::GetSettings()->m_nProtocol;
	m_pISerialPort = CSmartConnector::Get_SmartConnector(&CServoMotor_ModelSimulationDlg::CallBack_SerialPort, protocolSwitch)->m_pISerialPort;
	CSmartConnector::Get_SmartConnector()->GetSink((IUnknown**)&m_pISink);// { *ppSink = m_pISink; }


	// create system with 
	// deafault parameters
	if (!m_pMotorControlSystem)
	{ 
		//m_pMotorControlSystem = new CSystem2(m_fKP, m_fKI, m_fKD, m_dt, m_fM, m_fb, m_fKe, m_fYi, m_fVi, m_fTi);
		m_pMotorControlSystem = new CSystem3(m_fKP, m_fKI, m_fKD, m_dt, m_fR, m_fJ, m_fKe, m_fb, m_fYi, m_fVi, m_fLoadTorgue, m_fTi);
		m_pMotorControlSystem->SetReference(m_fRefSignal);
		m_pMotorControlSystem->AdviseCallBack((LONG*)&CServoMotor_ModelSimulationDlg::CallBack_System);
	}
	
	//////////////////////////////////////////////////////
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CServoMotor_ModelSimulationDlg::Draw_MotorPosition(CDC *pDC, float fPostAngle,float fRadius)
{
	int X =  cos(fPostAngle)*fRadius * 50 + 50;
	int Y = sin(fPostAngle)*fRadius * 50 + 50;

	CWindowsPainter::GetPainter()->MarkNode(pDC, X, Y, COLORREF(RGB(255, 0, 0)));

}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CServoMotor_ModelSimulationDlg::OnPaint()
{
	static float fdumy = 0.0;
	CStatic * pPaintWindow = (CStatic *)GetDlgItem(IDC_STATI_MOTOR_GRAPH);
	CPaintDC dc(pPaintWindow); // device context for painting
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
		// 
		if (m_pMotorControlSystem)
		{ 
			//m_pMotorControlSystem->GetOutput();
			//Draw_MotorPosition(&dc, m_pMotorControlSystem->GetOutput(), 1.0);
			Draw_MotorPosition(&dc, fdumy, 1.0);
			fdumy += 0.1;
		}
		CDialogEx::OnPaint();
	}
}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServoMotor_ModelSimulationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServoMotor_ModelSimulationDlg::_InitDialog()
{
	
	CString cs;
	CEdit * pEdt = NULL;
	cs.Format(L"%f", m_fKP);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KP);  	pEdt->SetWindowTextW(cs);
	cs.Format(L"%f", m_fKI);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KI);  	pEdt->SetWindowTextW(cs);
	cs.Format(L"%f", m_fKD);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KD);  	pEdt->SetWindowTextW(cs);
	cs.Format(L"%f", m_dt);   pEdt = (CEdit*)GetDlgItem(IDC_EDIT_DT);  	pEdt->SetWindowTextW(cs); 
	cs.Format(L"%f", m_fRefSignal);   pEdt = (CEdit*)GetDlgItem(IDC_EDIT_REFERENCE);  	pEdt->SetWindowTextW(cs);
	
	cs.Format(L"%f", m_fR / m_fKe);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_R_K);  	pEdt->SetWindowTextW(cs);
	cs.Format(L"%f", m_fR * m_fJ);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_RJ);  	pEdt->SetWindowTextW(cs);
	cs.Format(L"%f", m_fKe * m_fKe + m_fR * m_fb);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_KKRB);  	pEdt->SetWindowTextW(cs);
	
	cs.Format(L"%f", m_fLoadTorgue);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_LOAD_TORQUE);  	pEdt->SetWindowTextW(cs);

	
}
void CServoMotor_ModelSimulationDlg::_UpdateData()
{

	CEdit * pEdt = NULL;
	wchar_t buffer[20];
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KP);   pEdt->GetWindowTextW(buffer, 10);  m_fKP = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KI);   pEdt->GetWindowTextW(buffer, 10);  m_fKI = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_KD);   pEdt->GetWindowTextW(buffer, 10);  m_fKD = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_DT);   pEdt->GetWindowTextW(buffer, 10);  m_dt = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_REFERENCE);   pEdt->GetWindowTextW(buffer, 10);  m_fRefSignal = _wtof(buffer);
	
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_J);   pEdt->GetWindowTextW(buffer, 10);  m_fJ = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_B);   pEdt->GetWindowTextW(buffer, 10);  m_fb = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_Kt);   pEdt->GetWindowTextW(buffer, 10);  m_fKt = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_Ke);   pEdt->GetWindowTextW(buffer, 10);  m_fKe = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_R);   pEdt->GetWindowTextW(buffer, 10);  m_fR = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_REDUCTION);   pEdt->GetWindowTextW(buffer, 10);  m_fN = _wtof(buffer);
	pEdt = (CEdit *)GetDlgItem(IDC_EDIT_LOAD_TORQUE);   pEdt->GetWindowTextW(buffer, 10);  m_fLoadTorgue = _wtof(buffer);

}

void CServoMotor_ModelSimulationDlg::OnBnClickedButtonStart()
{
	geciciSayac = 0;
	_UpdateData();
	CPlant * pPlant = 0;
	CPidController * pPIDController = 0;

	_InitDialog();
	if (m_pMotorControlSystem == NULL)
	{
		// no pid, 
		// step 0.1
		// simple oscilation   void AdviseCallBack(LONG(*f))
		//m_pMotorControlSystem = new CSystem2(m_fKP, m_fKI, m_fKD, m_dt, m_fJ, m_fC, m_fK, m_fYi, m_fVi, m_fTi);
		m_pMotorControlSystem = new CSystem3(m_fKP, m_fKI, m_fKD, m_dt, m_fR,  m_fJ, m_fKe, m_fb, m_fYi, m_fVi, m_fLoadTorgue, m_fTi);
		m_pMotorControlSystem->SetReference(m_fRefSignal);
		m_pMotorControlSystem->AdviseCallBack((LONG*) &CServoMotor_ModelSimulationDlg::CallBack_System);
	
	}
	else
	{
		// stop system 
		m_pMotorControlSystem->SetRunModi(CSystem2::S_STOP);
		
		// set entire system
		m_pMotorControlSystem->ReSetEntireSystem(m_fKP, m_fKI, m_fKD, m_dt, m_fR, m_fJ, m_fKe, m_fb, m_fYi, m_fVi, m_fLoadTorgue, m_fTi);
		m_pMotorControlSystem->SetReference(m_fRefSignal);
		/*
		m_pMotorControlSystem->SetStep(m_dt);
		pPlant = m_pMotorControlSystem->GetPlant();
		if (pPlant) {
			pPlant->SetPlant(m_fM, m_fC, m_fK, m_dt);
			pPlant->SetInitials(m_fYi, m_fVi, m_fTi);
		}

		pPIDController = m_pMotorControlSystem->GetPidController();
		if (pPIDController)
			pPIDController->SetPidController(m_fKP, m_fKI, m_fKD, m_dt);
	*/
	
	}
	// restart system
	m_pMotorControlSystem->m_n_Ring_Index = 0;
	m_pMotorControlSystem->Resume();
	m_pMotorControlSystem->SetRunModi(CSystem2::S_RUN);

}
void CServoMotor_ModelSimulationDlg::OnBnClickedButtonAsenkron()
{
	CEdit * pEdtR = NULL, * pEdt_dt = NULL;
	 
	//float bufferY[CBase::TP_MAXIMUM_SIZE]; 
	float* bufferY = NULL;
	int iSize = CBase::TP_MAXIMUM_SIZE;
	_UpdateData();
	_InitDialog();

//	CSystem3 * pSystemAsenkron = NULL;  IDC_EDIT_DT_ASENKRON
	wchar_t buffer[20];
	//float t_Asyncron = 0.0;				// asynron samplingtime
	pEdtR = (CEdit *)GetDlgItem(IDC_EDIT_TIME_RANGE);   pEdtR->GetWindowTextW(buffer, 10);  m_dt_Asyncron = _wtof(buffer) / CBase::TP_MAXIMUM_SIZE;

	CString cs;
	cs.Format(L"%f", m_dt_Asyncron);  pEdt_dt = (CEdit*)GetDlgItem(IDC_EDIT_DT_ASENKRON);  	pEdt_dt->SetWindowTextW(cs);


	if (m_dt_Asyncron == 0.0)
	{ 
		pEdtR->SetWindowTextW(L"val");
		pEdtR->SetSel(0, 3);
		pEdtR->SetFocus();
		return;
	}

	if (m_pMotorControlSystem )
	{
		
		 m_pMotorControlSystem->Pause();			// stop the senkron-system clock

		// set entire system
		//m_pMotorControlSystem->SetEntireSystem(m_fKP, m_fKI, m_fKD, m_dt, m_fJ, m_fC, m_fK, m_fYi, m_fVi, m_fTi);
	
		// asyncrone system with new sampling time
		m_pMotorControlSystem->ReSetEntireSystem(m_fKP, m_fKI, m_fKD, m_dt_Asyncron, m_fR, m_fJ, m_fKe, m_fb, m_fYi, m_fVi, m_fLoadTorgue, m_fTi);
		m_pMotorControlSystem->SetReference(m_fRefSignal);
		m_pMotorControlSystem->RunAsenkron();
		m_pMotorControlSystem->Get_Y_Buffer(&bufferY, iSize);

		// reset senkron system
	//	m_pMotorControlSystem->SetEntireSystem(m_fKP, m_fKI, m_fKD, t_Asyncron, m_fR, m_fJ, m_fKe, m_fb, m_fYi, m_fVi, m_fTi);


		CAsyncronServoComProtocol servoCommunication(CAsyncronServoComProtocol::SERVO_MOVE_DATA, 0, 0);
		CFabric_AsyncronServoComProtocol::GetProtocol(servoCommunication
			, CAsyncronServoComProtocol::SERVO_MOVE_DATA
			, (unsigned char*)bufferY
			, 4 * CBase::TP_MAXIMUM_SIZE
			, 0);


		//unsigned char  prtcl[MAX_OVERALL_SIZE];
		unsigned char  *prtcl= NULL;
		int length = 0;
		servoCommunication.GetEntireProtocolBuffer(&prtcl, length);
	
		if (m_pISerialPort)
		{
			if (m_pISerialPort->CanDo() == S_OK)
			{
				m_pISerialPort->WriteBuffer((BYTE*)prtcl, length);

				/*
				m_pISerialPort->WriteBuffer((BYTE*)pControlBuffer,  (int)Size_ControlBuffer);
				Sleep(1);
				m_pISerialPort->WriteBuffer((BYTE*)pBuffer, (int)sizeBuffer);
				Sleep(1);
				m_pISerialPort->WriteBuffer((BYTE*)pCheckSum, 1);

				*/
			}
		}

	}
	


}
void CServoMotor_ModelSimulationDlg::OnBnClickedButtonResetSystem()
{
	CButton * pCheckSekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_SENKRON);
	CButton * pCheckASekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_ASENKRON);
	CString cs;
	CEdit * pEdt = NULL;

	if (pCheckSekron->GetCheck() == 1)
	{
		
		_UpdateData();
		cs.Format(L"%f", m_dt);  pEdt = (CEdit*)GetDlgItem(IDC_EDIT_DT_SYSTEM);  	pEdt->SetWindowTextW(cs);
		_InitDialog();
	}
	else if (pCheckASekron->GetCheck() == 1)
	{
		
	}

	else
	{
		AfxMessageBox(L"Senkron, ASyncron ?? ", MB_ICONQUESTION);

	}
}
void CServoMotor_ModelSimulationDlg::OnBnClickedButtonResetSystemAsenkron()
{
	CEdit * pEdtR = NULL, *pEdt_dt_System = NULL, *pEdt_dt_ASyncron = NULL;
	CString cs;
	//CEdit * pEdt = NULL;

	//float bufferY[CBase::TP_MAXIMUM_SIZE]; 
	float* bufferY = NULL;
	int iSize = CBase::TP_MAXIMUM_SIZE;
	_UpdateData();
	_InitDialog();

	//	CSystem3 * pSystemAsenkron = NULL;  IDC_EDIT_DT_ASENKRON
	wchar_t buffer[20];
	//float t_Asyncron = 0.0;				// asynron samplingtime
	pEdtR = (CEdit *)GetDlgItem(IDC_EDIT_TIME_RANGE);   pEdtR->GetWindowTextW(buffer, 10);  m_dt_Asyncron = _wtof(buffer) / CBase::TP_MAXIMUM_SIZE;

	//CString cs;
	cs.Format(L"%f", m_dt_Asyncron);  pEdt_dt_ASyncron = (CEdit*)GetDlgItem(IDC_EDIT_DT_ASENKRON);  	pEdt_dt_ASyncron->SetWindowTextW(cs);


	if (m_dt_Asyncron == 0.0)
	{
		pEdtR->SetWindowTextW(L"val");
		pEdtR->SetSel(0, 3);
		pEdtR->SetFocus();
		return;
	}
	cs.Format(L"%f", m_dt_Asyncron);  pEdt_dt_System = (CEdit*)GetDlgItem(IDC_EDIT_DT_SYSTEM);  	pEdt_dt_System->SetWindowTextW(cs);

	
}



void CServoMotor_ModelSimulationDlg::OnBnClickedButtonStop()
{
	if (m_pMotorControlSystem) {
		m_pMotorControlSystem->SetRunModi(CSystem2::S_STOP);
	}
}
void CServoMotor_ModelSimulationDlg::OnClose()
{
	if (m_pMotorControlSystem)
	{
		m_pMotorControlSystem->KillClockThread();
		delete m_pMotorControlSystem;
		m_pMotorControlSystem = NULL;

	}
	RS232Settings::GetSettings()->WriteIntoRegistry(L"SOFTWARE", L"AyfaLtd\\ServoMotorModelSimulation");
	RS232Settings::GetSettings()->KillSettings();
	CSmartConnector::Get_SmartConnector()->Kill_SmartConnector();
	CWindowsPainter::KillPainter();
	CDialogEx::OnClose();
}
void CServoMotor_ModelSimulationDlg::OnBnClickedButtonConnect()
{
	static bool bPortOpen = false;
	//EAG_LOG_DEBUG(L"CPageConnections::OnButtonConnect - start")
	CString csCaption;
	CString csError;
	CString csPortString;
	//int nPort = 6;
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
			CString csInfo = L"CModBusMasterDlg::OnBnClickedButtonConnect - port opened";
			//csInfo.LoadString(L"IDS_STRING_INFO_COMPORT_OPENED");
			//EAG_LOG_DEBUG(csInfo)

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
void CServoMotor_ModelSimulationDlg::OnBnClickedButtonPortSetting()
{
	CRunDlg r(this);
	if (IDOK == r.Run(CRunDlg::RS232_SETTINGS))
	{
	}
}

void CServoMotor_ModelSimulationDlg::OnBnClickedRadioEnableAsenkron()
{
		CButton * pCheckASekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_ASENKRON);
	//CButton * pCheckSekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_SENKRON);


	CButton * pStart = (CButton *)GetDlgItem(IDC_BUTTON_START);
	CButton * pStop = (CButton *)GetDlgItem(IDC_BUTTON_STOP);

	CButton * pASyn = (CButton *)GetDlgItem(IDC_BUTTON_ASENKRON);

	if (pCheckASekron->GetCheck() == 1)   
	{
		(CButton *)GetDlgItem(IDC_BUTTON_START)->EnableWindow(0);
		(CButton *)GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(0);
		(CButton *)GetDlgItem(IDC_BUTTON_RESET_SYSTEM)->EnableWindow(0);
		(CEdit *)GetDlgItem(IDC_EDIT_DT)->EnableWindow(0);

		(CButton *)GetDlgItem(IDC_BUTTON_ASENKRON)->EnableWindow(1);
		(CButton *)GetDlgItem(IDC_BUTTON_RESET_SYSTEM_ASENKRON)->EnableWindow(1);
		(CEdit *)GetDlgItem(IDC_EDIT_TIME_RANGE)->EnableWindow(1);

	}

}
void CServoMotor_ModelSimulationDlg::OnBnClickedRadioEnableSenkron()
{
//	CButton * pCheckASekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_ASENKRON);

	
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DT);

	CButton * pCheckSekron = (CButton *)GetDlgItem(IDC_RADIO_ENABLE_SENKRON);


	CButton * pStart = (CButton *)GetDlgItem(IDC_BUTTON_START);
	CButton * pStop = (CButton *)GetDlgItem(IDC_BUTTON_STOP);

	CButton * pASyn = (CButton *)GetDlgItem(IDC_BUTTON_ASENKRON);

	if (pCheckSekron->GetCheck() == 1)
	{
		(CButton *)GetDlgItem(IDC_BUTTON_START)->EnableWindow(1);
		(CButton *)GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(1);  
		(CButton *)GetDlgItem(IDC_BUTTON_RESET_SYSTEM)->EnableWindow(1);
		(CEdit *)GetDlgItem(IDC_EDIT_DT)->EnableWindow(1);
	
		(CButton *)GetDlgItem(IDC_BUTTON_ASENKRON)->EnableWindow(0);
		(CButton *)GetDlgItem(IDC_BUTTON_RESET_SYSTEM_ASENKRON)->EnableWindow(0);
		(CEdit *)GetDlgItem(IDC_EDIT_TIME_RANGE)->EnableWindow(0);

		


	}

}




