// Timer_1.cpp : implementation file
//

#include "stdafx.h"
#include "MotionControl.h"
#include "Timer_1.h"
#include "afxdialogex.h"

/*Mega16*/
#define ICR1    0x46  // _SFR_IO16(0x26)   // add 
#define OCR1A	0x4A  // _SFR_IO16(0x2A)
#define OCR1B	0x48  //_SFR_IO16(0x28)

#define TCCR1A	0x4F	// _SFR_IO8(0x2F)
#define WGM10 0
#define WGM11 1
#define FOC1B 2
#define FOC1A 3
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B	0x4E	// _SFR_IO8(0x2E)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TIMSK  0x59 //_SFR_IO8(0x39)
#define TOIE0 0
#define OCIE0 1
#define TOIE1 2
#define OCIE1B 3
#define OCIE1A 4
#define TICIE1 5
#define TOIE2 6
#define OCIE2 7

#define TIFR	0x58  // _SFR_IO8(0x38)
#define TOV0 0
#define OCF0 1
#define TOV1 2
#define OCF1B 3
#define OCF1A 4
#define ICF1 5
#define TOV2 6
#define OCF2 7


CString csErrorMsg[6] = {L"", L"timer mode is not supported", L"invalid clock selection", L"external clock", L"counter overload - increase pre scaler", L"top value is 0, decrease pre scaler" };
long lCpuFrequencies[6] = { 1000000L,2000000L,4000000L,8000000L,16000000L, 32000000L };
// CTimer_1 dialog

IMPLEMENT_DYNAMIC(CTimer_1, CDialogEx)

CTimer_1::CTimer_1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TIMER_1, pParent)

	, m_nCpuFrequency(8000000)
	, m_OCR1A(0)
	, m_OCR1B(0)
	, m_ICR1(0)
	, m_TIFR(0)
	, m_TIMSK(0)	
	, m_mode(14)
	, m_PercentDuty(10)
	, m_nTop(0)
	, m_nTargetFrequency(200)
	, m_nTimerFrequency(0)
	, m_pMCDlg(NULL)

	, bA0(FALSE)
	, bA1(FALSE)
	, bB0(FALSE)
	, bB1(FALSE)
	, bIA(FALSE)
	, bIB(FALSE)
	, bET(FALSE)
	, m_bDeTelArrived(false)
	
	, bEToV(FALSE)
{

}

CTimer_1::~CTimer_1()
{
}
void CTimer_1::update()
{
	CMotionControlDlg * pDlg = m_pMCDlg;
	CButton *pButton = 0;
	BYTE L;
	BYTE H;
	WORD adr, val;

	CEdit *pEdit;
	wchar_t buffer[8];
	uint8_t  _t8Val = 0;

	if (m_pMCDlg->m_Detel.cm1 == SP_RRAM)
	{
		// adress 
		adr = ((WORD)m_pMCDlg->m_Detel.ad1 << 8) | m_pMCDlg->m_Detel.ad0;

		// Get Word
		L = m_pMCDlg->m_Detel.db[0]; // [LENGTH_CONROLBITS - 1];		// MAXIMUM_DATA_BYTE];
		H = m_pMCDlg->m_Detel.db[1]; // m_czData[LENGTH_CONROLBITS];			// MAXIMUM_DATA_BYTE];

		val = ((WORD)H << 8) | L;

		if (adr == ICR1) {
			m_ICR1 = val;

			pEdit = (CEdit*) GetDlgItem(IDC_EDIT_ICR1);
			_itow_s(val, buffer, 10);
			pEdit->SetWindowText(buffer);

			m_hedICR1.SetValue(val, true);
			// CDialog::OnInitDialog();  // from call back causes error
		}

		else if (adr == OCR1A) {
			m_OCR1A = val;
			
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_OCR1A);
			_itow_s(val, buffer,10);
			pEdit->SetWindowText(buffer);

			m_hedOCR1A.SetValue(val, true);
	//		CDialog::OnInitDialog(); 
		}
		else if (adr == OCR1B) {
			m_OCR1B = val;

			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_OCR1B);
			_itow_s(val, buffer, 10);
			pEdit->SetWindowText(buffer);

			m_hedOCR1B.SetValue(val, true);
	//		CDialog::OnInitDialog();
		}

		else if (adr == TIFR) {
			m_TIFR = val & (0x0004);  // filter onlye TOV1 bit for timer-1
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TIFR);
			_itow_s(m_TIFR, buffer, 10);
			pEdit->SetWindowText(buffer);
			m_hedTIFR.SetValue(m_TIFR, true);
			
			////////////////////////////
			m_TIMSK = val >> 8 & (0x00FF);   
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TIMSK);
			_itow_s(m_TIMSK, buffer, 10);
			
			pEdit->SetWindowText(buffer);
			m_hedTIMSK.SetValue(m_TIMSK, true);
			
			// indicates each control bit in TIMSK
			_t8Val = m_TIMSK;
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_0);
			pButton->SetCheck((_t8Val >> TOIE0) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_1);
			pButton->SetCheck((_t8Val >> OCIE0) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_2);
			pButton->SetCheck((_t8Val >> TOIE1) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_3);
			pButton->SetCheck((_t8Val >> OCIE1B) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_4);
			pButton->SetCheck((_t8Val >> OCIE1A) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_5);
			pButton->SetCheck((_t8Val >> TICIE1) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_6);
			pButton->SetCheck((_t8Val >> TOIE2) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TIMSK_7);
			pButton->SetCheck((_t8Val >> OCIE2) & (0x01));





		}

		else if (adr == TCCR1B) {
			m_TCCR1B = val & (0x00FF);
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TCCR1B);
			_itow_s(m_TCCR1B, buffer, 10);
			pEdit->SetWindowText(buffer);
			m_hedTCCR1B.SetValue(m_TCCR1B, true);
			
			// indicates each control bit in TCCR1B
			_t8Val = m_TCCR1B;
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_0);
			pButton->SetCheck((_t8Val >> CS10) & (0x01));
		
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_1);
			pButton->SetCheck((_t8Val >> CS11) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_2);
			pButton->SetCheck((_t8Val >> CS12) & (0x01));
	
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_3);
			pButton->SetCheck((_t8Val >> WGM12) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_4);
			pButton->SetCheck((_t8Val >> WGM13) & (0x01));
		
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_6);
			pButton->SetCheck((_t8Val >> ICES1) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1B_7);
			pButton->SetCheck((_t8Val >> ICNC1) & (0x01));


			////////////////////////////
			m_TCCR1A = val >> 8 & (0x00FF);
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TCCR1A);
			_itow_s(m_TCCR1A, buffer, 10);
			pEdit->SetWindowText(buffer);
			m_hedTCCR1A.SetValue(m_TCCR1A, true);

			// indicates each control bit in TCCR1A
			_t8Val = m_TCCR1A;
			pButton = (CButton*) GetDlgItem(IDC_CHECK_TCR1A_0);
			pButton->SetCheck( (_t8Val >> WGM10) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_1);
			pButton->SetCheck((_t8Val >> WGM11) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_2);
			pButton->SetCheck((_t8Val >> FOC1B) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_3);
			pButton->SetCheck((_t8Val >> FOC1A) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_4);
			pButton->SetCheck((_t8Val >> COM1B0) & (0x01));
			
			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_5);
			pButton->SetCheck((_t8Val >> COM1B1) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_6);
			pButton->SetCheck((_t8Val >> COM1A0) & (0x01));

			pButton = (CButton*)GetDlgItem(IDC_CHECK_TCR1A_7);
			pButton->SetCheck((_t8Val >> COM1A1) & (0x01));

		}

	}

}

void CTimer_1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_CBIndex(pDX, IDC_COMBO_PRE_SCALER, m_cPreScaler);
	DDX_Control(pDX, IDC_COMBO_PRE_SCALER, m_cbPreScaler);
	DDX_Control(pDX, IDC_COMBO_MODE, m_cbMode);
	DDX_Control(pDX, IDC_COMBO_TOP, m_cbTop);
	//  DDX_Text(pDX, IDC_EDIT_TOP, m_nTop);
	DDX_Text(pDX, IDC_EDIT_DUTY_PERCENT, m_PercentDuty);
	DDX_Text(pDX, IDC_EDIT_TOP, m_nTop);

	DDX_Text(pDX, IDC_EDIT_TIMER_FRQENCY, m_nTimerFrequency);
	//	DDV_MinMaxUInt(pDX, m_nTimerFrequency, 0, 65535);

	DDX_Text(pDX, IDC_EDIT_TARGET_FREQUENCY, m_nTargetFrequency);
	DDV_MinMaxUInt(pDX, m_nTargetFrequency, 0, 65535);

	DDX_Text(pDX, IDC_EDIT_TCCR1A, m_TCCR1A);
	DDV_MinMaxUInt(pDX, m_TCCR1A, 0, 255);

	DDX_Text(pDX, IDC_EDIT_TCCR1B, m_TCCR1B);
	DDV_MinMaxUInt(pDX, m_TCCR1B, 0, 255);

	DDX_Text(pDX, IDC_EDIT_ICR1, m_ICR1);
	DDV_MinMaxUInt(pDX, m_ICR1, 0, 65535);

	DDX_Text(pDX, IDC_EDIT_OCR1A, m_OCR1A);
	DDV_MinMaxUInt(pDX, m_OCR1A, 0, 65535);

	DDX_Text(pDX, IDC_EDIT_OCR1B, m_OCR1B);
	DDV_MinMaxUInt(pDX, m_OCR1B, 0, 65535);

	DDX_Control(pDX, IDC_STATIC_ERROR, m_LbError);
	DDX_Control(pDX, IDC_COMBO_PRE_COMPARE_OUTPUT_MOD, m_cbCompareOutputMode);
	DDX_Text(pDX, IDC_EDIT_TIMSK, m_TIMSK);
	DDV_MinMaxUInt(pDX, m_TIMSK, 0, 255);

	DDX_Check(pDX, IDC_CHECK_COM1A0, bA0);
	DDX_Check(pDX, IDC_CHECK_COM1A1, bA1);
	DDX_Check(pDX, IDC_CHECK_COM1B0, bB0);
	DDX_Check(pDX, IDC_CHECK_COM1B1, bB1);
	DDX_Check(pDX, IDC_CHECK_INTERRUPT_A, bIA);
	DDX_Check(pDX, IDC_CHECK_INTERRUPT_B, bIB);
	DDX_Check(pDX, IDC_CHECK_ENABLE_TIMER, bET);
	DDX_Control(pDX, IDC_EDIT_OCR1A_HEX, m_hedOCR1A);
	DDX_Control(pDX, IDC_EDIT_ICR1_HEX, m_hedICR1);
	DDX_Control(pDX, IDC_EDIT_OCR1B_HEX, m_hedOCR1B);
	DDX_Control(pDX, IDC_EDIT_TIMER_FRQENCY_HEX, m_hedTimerFrequency);
	DDX_Control(pDX, IDC_EDIT_TIMSK_HEX, m_hedTIMSK);
	DDX_Control(pDX, IDC_EDIT_TCCR1A_HEX, m_hedTCCR1A);
	DDX_Control(pDX, IDC_EDIT_TCCR1B_HEX, m_hedTCCR1B);
	DDX_Text(pDX, IDC_EDIT_TIFR, m_TIFR);
	DDX_Control(pDX, IDC_EDIT_TIFR_HEX, m_hedTIFR);
	DDX_Control(pDX, IDC_SLIDER_ICR1, m_sldr_ICR1);
	DDX_Control(pDX, IDC_SLIDER_OCR1A, m_sldr_OCR1A);
	DDX_Control(pDX, IDC_SLIDER_OCR1B, m_sldr_OCR1B);
	DDX_Check(pDX, IDC_CHECK_ENABLE_TOV, bEToV);
	DDX_Control(pDX, IDC_COMBO_CPU_FREQUENCY, m_cbCpuFrequency);
}

BEGIN_MESSAGE_MAP(CTimer_1, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, &CTimer_1::OnSelchangeComboMode)
	ON_BN_CLICKED(IDC_BUTTON_SETUP, &CTimer_1::OnBnClickedButtonSetup)
	ON_BN_CLICKED(IDC_BUTTON_OCR1A_W, &CTimer_1::OnBnClickedButtonOcr1aW)
	ON_BN_CLICKED(IDC_BUTTON_OCR1A_R, &CTimer_1::OnBnClickedButtonOcr1aR)
	ON_BN_CLICKED(IDC_BUTTON_OCR1B_W, &CTimer_1::OnBnClickedButtonOcr1bW)
	ON_BN_CLICKED(IDC_BUTTON_OCR1B_R, &CTimer_1::OnBnClickedButtonOcr1bR)
	ON_BN_CLICKED(IDC_BUTTON_ICR1_W, &CTimer_1::OnBnClickedButtonIcr1W)
	ON_BN_CLICKED(IDC_BUTTON_ICR1_R, &CTimer_1::OnBnClickedButtonIcr1R)
	ON_BN_CLICKED(IDC_BUTTON_TCR1B_R, &CTimer_1::OnBnClickedButtonTcr1bR)
	ON_BN_CLICKED(IDC_BUTTON_TCCR1B_W, &CTimer_1::OnBnClickedButtonTccr1bW)
	
	ON_CBN_SELCHANGE(IDC_COMBO_PRE_COMPARE_OUTPUT_MOD, &CTimer_1::OnCbnSelchangeComboPreCompareOutputMod)
	ON_BN_CLICKED(IDC_CHECK_INTERRUPT_A, &CTimer_1::OnBnClickedCheckInterruptA)
	ON_BN_CLICKED(IDC_CHECK_INTERRUPT_B, &CTimer_1::OnBnClickedCheckInterruptB)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_TIMER, &CTimer_1::OnBnClickedCheckEnableTimer)
	ON_BN_CLICKED(IDOK, &CTimer_1::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_TIFR_R, &CTimer_1::OnBnClicked_ButtonTifrR)
	ON_BN_CLICKED(IDC_BUTTON_TIFR_W, &CTimer_1::OnBnClicked_ButtonTifrW)
	ON_EN_CHANGE(IDC_EDIT_TOP, &CTimer_1::OnEnChangeEditTop)
	ON_BN_CLICKED(IDC_RADIO_CHANNEL_BOTH, &CTimer_1::OnBnClickedRadioChannelBoth)
	ON_BN_CLICKED(IDC_RADIO_CHANNEL_B, &CTimer_1::OnBnClickedRadioChannelB)
	ON_BN_CLICKED(IDC_RADIO_CHANNEL_A, &CTimer_1::OnBnClickedRadioChannelA)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_ICR1, &CTimer_1::OnReleasedcaptureSlider_Icr1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OCR1A, &CTimer_1::OnReleasedcaptureSlider_Ocr1a)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OCR1B, &CTimer_1::OnReleasedcaptureSlider_Ocr1b)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CTimer_1::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTimer_1::OnBnClickedButtonOpen)
	ON_CBN_SELCHANGE(IDC_COMBO_CPU_FREQUENCY, &CTimer_1::OnSelchange_ComboCpuFrequency)
	
END_MESSAGE_MAP()


BOOL CTimer_1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cbPreScaler.AddString(L"0");
	m_cbPreScaler.AddString(L"1");
	m_cbPreScaler.AddString(L"8");
	m_cbPreScaler.AddString(L"64");
	m_cbPreScaler.AddString(L"256");
	m_cbPreScaler.AddString(L"1024");
	m_cbPreScaler.AddString(L"External");
	m_cbPreScaler.SetCurSel(3);

	m_cbMode.AddString(L"0) Normal");
	m_cbMode.AddString(L"1) PWM, Phase correct, 8-bit");
	m_cbMode.AddString(L"2) PWM, Phase correct, 9-bit");
	m_cbMode.AddString(L"3) PWM, Phase correct, 10-bit");
	m_cbMode.AddString(L"4) CTC");
	m_cbMode.AddString(L"5) Fast-PWM,8-bit");
	m_cbMode.AddString(L"6) Fast-PWM,9-bit");
	m_cbMode.AddString(L"7) Fast-PWM,10-bit");
	m_cbMode.AddString(L"8) PWM, Phase and frequency correct");
	m_cbMode.AddString(L"9) PWM, Phase and frequency correct");
	m_cbMode.AddString(L"10) PWM, Phase correct");
	m_cbMode.AddString(L"11) PWM, Phase correct");
	m_cbMode.AddString(L"12) CTC");
	m_cbMode.AddString(L"13) Reserved");
	m_cbMode.AddString(L"14) Fast-PWM");
	m_cbMode.AddString(L"15) Fast-PWM");
	m_cbMode.SetCurSel(m_mode);


	m_cbTop.AddString(L"Fixed : ");
	m_cbTop.AddString(L"OCR1A");
	m_cbTop.AddString(L"ICR1");
	m_cbTop.AddString(L"--");
	
	OnSelchangeComboMode();

	m_cbCpuFrequency.AddString(L"1 MHz."); 
	m_cbCpuFrequency.AddString(L"2 MHz.");
	m_cbCpuFrequency.AddString(L"4 MHz.");
	m_cbCpuFrequency.AddString(L"8 MHz.");
	m_cbCpuFrequency.AddString(L"16 MHz.");
	m_cbCpuFrequency.AddString(L"32 MHz.");
	m_cbCpuFrequency.SetCurSel(3);

	const COLORREF rgbRed = 0x000000FF;
	const COLORREF rgbGreen = 0x0000FF00;
	const COLORREF rgbBlue = 0x00FF0000;
	const COLORREF rgbBlack = 0x00000000;
	const COLORREF rgbWhite = 0x00FFFFFF;

	m_LbError.SetBkColor(rgbRed);
	m_LbError.SetFontSize(12);

	m_sldr_ICR1.SetRange(0, 100);

	RECT r, rT;
	m_pMCDlg->GetWindowRect(&r);
	GetWindowRect(&rT);
//	ScreenToClient(&r);

	int shiftX = (r.right - r.left); // +10;
	//int shiftY = 40;
	rT.left = rT.left + shiftX ;
	rT.right = rT.right + shiftX ;
	rT.top = r.top;
	//r.top = r.top + shiftY;
	//r.bottom = r.bottom + shiftY;
//	ClientToScreen(&rT);	

	MoveWindow(&rT);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
void CTimer_1::OnSelchangeComboMode()
{
	int nSelection = m_cbMode.GetCurSel();
	CEdit * pTargetFrqEdit = (CEdit*) GetDlgItem(IDC_EDIT_TARGET_FREQUENCY);
	CEdit * pTargetDutyPercent = (CEdit*)GetDlgItem(IDC_EDIT_DUTY_PERCENT);

	pTargetFrqEdit->EnableWindow();
	pTargetDutyPercent->EnableWindow();
	m_mode = nSelection;
	
	switch (nSelection)
	{
	case TM0:
		m_cbTop.SetCurSel(0);
		m_nTop = 0xFFFF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM1:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x00FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM2:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x01FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM3:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x03FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM4:   // CTC Togle mode
		m_cbTop.SetCurSel(1);
		m_nTop = m_OCR1A;
		pTargetDutyPercent->EnableWindow(0);
		break;

	case TM5:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x00FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM6:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x01FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM7:
		m_cbTop.SetCurSel(0);
		m_nTop = 0x03FF;
		pTargetFrqEdit->EnableWindow(0);
		break;

	case TM8:
		m_cbTop.SetCurSel(2);
		m_nTop = m_ICR1;
		break;

	case TM9:
		m_cbTop.SetCurSel(1);
		m_nTop = m_OCR1A;
		break;

	case TM10:
		m_cbTop.SetCurSel(2);
		m_nTop = m_ICR1;
		break;

	case TM11:
		m_cbTop.SetCurSel(1);
		m_nTop = m_OCR1A;
		break;	
	
	case TM12:
		m_cbTop.SetCurSel(2);
		m_nTop = m_ICR1;
		break;

	case TM13:
		AfxMessageBox(L"Reserved");
		break;

	case TM14:
		m_cbTop.SetCurSel(2);
		m_nTop = m_ICR1;
		break;

	case TM15:
		m_cbTop.SetCurSel(1);
		m_nTop = m_OCR1A;
		break;

	default:
		break;



	}
	int nSlc = m_cbCompareOutputMode.GetCurSel();
	if (nSlc==-1)
		nSlc = 2;

	m_cbCompareOutputMode.ResetContent();


	/// CTC (Table 44)
	if ((nSelection == TM12) || (nSelection == TM4))
	{
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC1A/OC1B disconnected");
		m_cbCompareOutputMode.AddString(L"Toggle OC1A/OC1B on compare match");
		m_cbCompareOutputMode.AddString(L"Clear OC1A/OC1B on compare match (Set	output to low level)");
		m_cbCompareOutputMode.AddString(L"Set OC1A/OC1B on compare match (Set output to high level)");
	}



	/// FAST PWM (Table 45)
	if ((nSelection ==  TM14) || (nSelection == TM15) ||
		(nSelection == TM5) || (nSelection == TM6) || (nSelection == TM7) )
	{
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC1A/OC1B disconnected");
		if (nSelection == TM15)
			m_cbCompareOutputMode.AddString(L"Toggle OC1A on Compare Match, OC1B disconnected");
		else
			m_cbCompareOutputMode.AddString(L"Normal port operation, OC1A / OC1B disconnected.");

		m_cbCompareOutputMode.AddString(L"Clear OC1A/OC1B on compare match, set	OC1A / OC1B at BOTTOM, 	(non - inverting mode)");
		m_cbCompareOutputMode.AddString(L"Set OC1A/OC1B on compare match, clear	OC1A / OC1B at BOTTOM, 	(inverting mode)");
		m_cbCompareOutputMode.SetCurSel(nSlc);
	}

	/// Phase Correct,  Phase and Frequency Correct  (Table 46)
	if ((nSelection == TM1) || (nSelection == TM2) || (nSelection == TM3) ||
		(nSelection == TM8) || (nSelection == TM9) || (nSelection == TM10) || (nSelection == TM11) )
	{
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC1A/OC1B disconnected");
		if (nSelection == TM9)
			m_cbCompareOutputMode.AddString(L"Toggle OC1A on Compare Match, OC1B disconnected");
		else
			m_cbCompareOutputMode.AddString(L"Normal port operation, OC1A / OC1B disconnected.");
		m_cbCompareOutputMode.AddString(L"Clear OC1A/OC1B on compare match when up - counting. Set when downcounting.");
		m_cbCompareOutputMode.AddString(L"Set OC1A/OC1B on compare match when up - counting. Clear when downcounting.");
		m_cbCompareOutputMode.SetCurSel(nSlc);
	}
	
	OnCbnSelchangeComboPreCompareOutputMod();
	CDialog::OnInitDialog();



	// TODO: Add your control notification handler code here
}
int CTimer_1::_Setup_PWM(uint8_t pwm_mod, uint8_t clsel,  double TargetFrq, uint8_t percent_duty)
{
	//TCCR1A :  COM1A1	COM1A0	COM1B1	COM1B0		FOC1A	FOC1B	WGM11	WGM10
	//TCCR1B :	ICNC1	ICES1	–		WGM13		WGM12	CS12	CS11	CS10
	m_TCCR1A	= 0;
	m_TCCR1B	= 0;
	m_OCR1A		= 0;
	m_OCR1B		= 0;
	m_ICR1		= 0;
	m_TIMSK = 0;
	m_TIFR = 0;

	int scale = 1;

	if (//   !(pwm_mod == 5)				// Fast PWM, 8-bit						TOP = 0x00FF
		//&& !(pwm_mod == 6)				// Fast PWM, 9-bit						TOP = 0x01FF
		//&& !(pwm_mod == 7)				// Fast PWM,10-bit						TOP = 0x03FF
		   !(pwm_mod == TM8)				// PWM, Phase and Frequency Correct		TOP = ICR1		( OCR1A as compare register)    (up-down count)
		&& !(pwm_mod == TM9)				// PWM, Phase and Frequency Correct		TOP = OCR1A										(up-down count)
		&& !(pwm_mod == TM10)				// PWM, Phase Correct					TOP = ICR1		( OCR1A as compare register)	(up-down count)
		&& !(pwm_mod == TM11)				// PWM, Phase Correct					TOP = OCR1A										(up-down count)
		&& !(pwm_mod == TM14)				// PWM, Fast							TOP = ICR1		( OCR1A as compare register)
		&& !(pwm_mod == TM15)				// PWM, Fast							TOP = OCR1A
		)
		return 1;

	switch (clsel)
	{
	case 0:
		return 2;
		break;
	case 1:
		scale = 1;
		break;
	case 2:
		scale = 8;
		break;
	case 3:
		scale = 64;
		break;
	case 4:
		scale = 256;
		break;
	case 5:
		scale = 1024;
		break;

	default:
		return 3;
	}

	// timer frequency
	m_nTimerFrequency = (((m_nCpuFrequency / scale)) );

	// fast pwm's
	uint32_t frDumy = 0;
	if ((pwm_mod == TM14) || (pwm_mod == TM15))
	{
		frDumy = (uint32_t)(((m_nTimerFrequency / TargetFrq) ) - 1);
	}
	// phase and/or frequency correct
	else if ((pwm_mod == TM8) || (pwm_mod == TM9) || (pwm_mod == TM10) || (pwm_mod == TM11))
	{
		frDumy = (uint32_t)(((m_nTimerFrequency / TargetFrq) / 2) - 1);
	}
	// is valid ?
	if (frDumy > 0xFFFF)
		return 4;				// compare register overloads counter overload - increase scale
	if (frDumy < 1)
		return 5;				// top value 0 - decrease pre-scaler
	m_nTop = (uint16_t)frDumy;
	// 

	// set up hardware for channel A
	//m_mode = pwm_mod;
	if ((pwm_mod == TM8) || (pwm_mod == TM10) || (pwm_mod == TM14)) {
		m_ICR1 = m_nTop;							// top 
		m_OCR1B = (m_nTop*percent_duty) / 100;		// as compare register for  B channel
		m_OCR1A = (m_nTop*percent_duty) / 100;		// as compare register for  A channel

	}
	else if ((pwm_mod == TM9) || (pwm_mod == TM11) || (pwm_mod == TM15)) {
		m_OCR1A = m_nTop;
		m_OCR1B = (m_nTop*percent_duty) / 100;		// phase
	}


	// WGM1 : wave generation mode :
	// see Table 47. Waveform
	// Generation Mode Bit Description(1) page:112
	m_TCCR1A |= (pwm_mod & 0x03);									//0000 0011         ----  --xx
	m_TCCR1B |= (pwm_mod & 0x0C) << 1;								//0001 1100			---x  x---

	
																	
																	// Compare Output Mode
	// see Table 45. and 46.
	// Clear OC1A/OC1B on compare match when
	// up-counting. Set OC1A/OC1B on compare
	// match when down counting.
	if (bA0)
		m_TCCR1A |= (1 << COM1A0);
	else  m_TCCR1A &= ~(1 << COM1A0);

	if (bB0)
		m_TCCR1A |= (1 << COM1B0);
	else  m_TCCR1A &= ~(1 << COM1B0);

	if (bA1)
		m_TCCR1A |= (1 << COM1A1);
	else  m_TCCR1A &= ~(1 << COM1A1);

	if (bB1)
		m_TCCR1A |= (1 << COM1B1);
	else  m_TCCR1A &= ~(1 << COM1B1);

	// interrupts
	if (bIA)
		m_TIMSK |= (1 << OCIE1A);  //
	else  m_TIMSK &= ~(1 << OCIE1A);

	if (bIB)
		m_TIMSK |= (1 << OCIE1B);  // 
	else  m_TIMSK &= ~(1 << OCIE1B);
	
	if (bEToV)
		m_TIMSK |= (1 << TOIE1);  // 
	else  m_TIMSK &= ~(1 << TOIE1);
	// enable timer
	if(bET)
		m_TCCR1B |= (clsel & 0x07);		// clock selection			0000 0111			----  -xxx
	else  m_TCCR1B &= ~(clsel & 0x07);

	return 0;


}
int CTimer_1::_Setup_CTC(uint8_t ctc_mod, uint8_t clsel, uint32_t TargetFrq)
{
	//TCCR1A :  COM1A1	COM1A0	COM1B1	COM1B0		FOC1A	FOC1B	WGM11	WGM10
	//TCCR1B :	ICNC1	ICES1	–		WGM13		WGM12	CS12	CS11	CS10
	m_TCCR1A = 0;
	m_TCCR1B = 0;
	m_OCR1A = 0;
	m_OCR1B = 0;
	m_ICR1 = 0;

	m_TIMSK = 0;
	m_TIFR = 0;

	int scale = 1;

	if (!(ctc_mod == 12) && !(ctc_mod == 4))
		return 1;

	switch (clsel)
	{
	case 0:
		return 2;
		break;
	case 1:
		scale = 1;
break;
	case 2:
		scale = 8;
		break;
	case 3:
		scale = 64;
		break;
	case 4:
		scale = 256;
		break;
	case 5:
		scale = 1024;
		break;

	default:
		return 3;
	}

	// timer frequency
	m_nTimerFrequency = (((m_nCpuFrequency / scale)));

	uint32_t frDumy = 0;
	frDumy  = (uint32_t)(((m_nTimerFrequency / TargetFrq)) - 1);
	// is valid ?
	if (frDumy > 0xFFFF)
		return 4;				// compare register overloads counter overload - increase scale
	if (frDumy < 1)
		return 5;				// top value 0 - decrease pre-scaler
	m_nTop = frDumy;

	// set up hardware for channel A
	m_mode = ctc_mod;
	if (ctc_mod == TM4)
		m_OCR1A = m_nTop;		// compare register
	else
		m_ICR1 = m_nTop;

	// WGM1 : wave generation mode : 
	// see Table 47. Waveform 
	// Generation Mode Bit Description(1) page:112
	m_TCCR1A |= (ctc_mod & 0x03);									//0000 0011         ----  --xx
	m_TCCR1B |= (ctc_mod & 0x0C) << 1;								//0000 1100			---x  x---
																	//TCCR1A |=  (1 << COM1B1 ) | (1 << FOC1B ) ;	//connect pin       x---  ----
	// Compare Output Mode, non-PWM
	// see Table 44. Waveform 
	// Toggle OC1A/OC1B on compare match
	//m_TCCR1A |= (1 << COM1A0) | (1 << COM1B0);

	if (bA0)
		m_TCCR1A |= (1 << COM1A0);
	else  m_TCCR1A &= ~(1 << COM1A0);

	if (bB0)
		m_TCCR1A |= (1 << COM1B0);
	else  m_TCCR1A &= ~(1 << COM1B0);

	if (bA1)
		m_TCCR1A |= (1 << COM1A1);
	else  m_TCCR1A &= ~(1 << COM1A1);

	if (bB1)
		m_TCCR1A |= (1 << COM1B1);
	else  m_TCCR1A &= ~(1 << COM1B1);


	// interrupts
	if (bIA)
		m_TIMSK |= (1 << OCIE1A);  //
	else  m_TIMSK &= ~(1 << OCIE1A);

	if (bIB)
		m_TIMSK |= (1 << OCIE1B);  // 
	else  m_TIMSK &= ~(1 << OCIE1B);

	if (bEToV)
		m_TIMSK |= (1 << TOIE1);  // 
	else  m_TIMSK &= ~(1 << TOIE1);

	/////////////////////////////////////////
	// enable timer
	if (bET)
		m_TCCR1B |= (clsel & 0x07);		// clock selection			0000 0111			----  -xxx
	else  m_TCCR1B &= ~(clsel & 0x07);

	return 0;

}
int CTimer_1::_Setup_FIX(uint8_t fix_mod, uint8_t clsel, uint32_t TargetFrq, uint8_t percent_duty)
{
	//TCCR1A :  COM1A1	COM1A0	COM1B1	COM1B0		FOC1A	FOC1B	WGM11	WGM10
	//TCCR1B :	ICNC1	ICES1	–		WGM13		WGM12	CS12	CS11	CS10
	m_TCCR1A = 0;
	m_TCCR1B = 0;
	m_OCR1A = 0;
	m_OCR1B = 0;
	m_ICR1 = 0;
	m_TIMSK = 0;
	m_TIFR = 0;

	int scale = 1;

	if (!(fix_mod == TM1)						// Phase Correct, 8-bit					TOP = 0x00FF    (up-down count)
		&& !(fix_mod == TM2)					// Phase Correct, 9-bit					TOP = 0x01FF	(up-down count)
		&& !(fix_mod == TM3)				    // Phase Correct,10-bit					TOP = 0x03FF	(up-down count)
		&& !(fix_mod == TM5)					// Fast PWM, 9-bit						TOP = 0x00FF
		&& !(fix_mod == TM6)				    // Fast PWM,10-bit						TOP = 0x01FF
		&& !(fix_mod == TM7)					// Fast PWM, 9-bit						TOP = 0x03FF
		)
		return 1;

	switch (clsel)
	{
	case 0:
		return 2;
		break;
	case 1:
		scale = 1;
		break;
	case 2:
		scale = 8;
		break;
	case 3:
		scale = 64;
		break;
	case 4:
		scale = 256;
		break;
	case 5:
		scale = 1024;
		break;

	default:
		return 3;
	}

	// timer frequency
	m_nTimerFrequency = (((m_nCpuFrequency / scale)));

	// set top fix values
	if ((fix_mod == TM1) || (fix_mod == TM5))  {
		m_nTop = 0x00FF;
	} 
	else if ((fix_mod == TM2) || (fix_mod == TM6)) {
		m_nTop = 0x01FF;
	}

	else if ((fix_mod == TM3) || (fix_mod == TM7)) {
		m_nTop = 0x03FF;
	}

	
	// fast pwm's
	if ((fix_mod == TM5) || (fix_mod == TM6) || (fix_mod == TM7))
	{
		// m_nTop = (uint16_t)(((m_nTimerFrequency / TargetFrq)) - 1);
		 // top is fixed. Calculate target freqency !!!!
		m_nTargetFrequency = m_nTimerFrequency / (m_nTop + 1);
	}
	// phase and/or frequency correct
	else if ((fix_mod == TM1) || (fix_mod == TM2) || (fix_mod == TM3) )
	{
		//m_nTop = (uint16_t)(((m_nTimerFrequency / TargetFrq) / 2) - 1);
		// top is fixed. Calculate target freqency !!!!
		m_nTargetFrequency = m_nTimerFrequency / (2 * m_nTop + 1);

	}
	// is m_nTop valid ? yes no need to check
	///////////////////////////////////////////////////////////////////
	m_OCR1B = (m_nTop*percent_duty) / 100;		// as compare register for B channel
	m_OCR1A = (m_nTop*percent_duty) / 100;		// as compare register for A channel

	/*
	// set up hardware for channel A
	if ((fix_mod == TM1) || (fix_mod == TM2) || (fix_mod == TM3)) {
		//m_ICR1 = m_nTop;							// top 
		m_OCR1A = (m_nTop*percent_duty) / 100;		// as compare register for A channel

	}
	else if ((fix_mod == TM5) || (fix_mod == TM6) || (fix_mod == TM7)) {
		m_OCR1B = (m_nTop*percent_duty) / 100;		// phase
	}
*/
	m_TCCR1A |= (fix_mod & 0x03);									//0000 0011         ----  --xx
	m_TCCR1B |= (fix_mod & 0x0C) << 1;								//0000 1100			---x  x---

	// WGM1 : wave generation mode :
	// see Table 47. Waveform
	// Generation Mode Bit Description(1) page:112

																	// Compare Output Mode
																	// see Table 45. and 46.
																	// Clear OC1A/OC1B on compare match when
																	// up-counting. Set OC1A/OC1B on compare
																	// match when down counting.

	if (bA0)
		m_TCCR1A |= (1 << COM1A0);
	else  m_TCCR1A &= ~(1 << COM1A0);

	if (bB0)
		m_TCCR1A |= (1 << COM1B0);
	else  m_TCCR1A &= ~(1 << COM1B0);

	if (bA1)
		m_TCCR1A |= (1 << COM1A1);
	else  m_TCCR1A &= ~(1 << COM1A1);

	if (bB1)
		m_TCCR1A |= (1 << COM1B1);
	else  m_TCCR1A &= ~(1 << COM1B1);

	// interrupts
	if (bIA)
		m_TIMSK |= (1 << OCIE1A);  //
	else  m_TIMSK &= ~(1 << OCIE1A);

	if (bIB)
		m_TIMSK |= (1 << OCIE1B);  // 
	else  m_TIMSK &= ~(1 << OCIE1B);

	if (bEToV)
		m_TIMSK |= (1 << TOIE1);  // 
	else  m_TIMSK &= ~(1 << TOIE1);

	// enable timer
	if (bET)
		m_TCCR1B |= (clsel & 0x07);		// clock selection			0000 0111			----  -xxx
	else  m_TCCR1B &= ~(clsel & 0x07);


	return 0;



}
void CTimer_1::OnBnClickedButtonSetup()
{
	CDialog::UpdateData();
	int error;
	int nSelectionMode = m_cbMode.GetCurSel();
	uint8_t nClock = m_cbPreScaler.GetCurSel();
	
	// NORMAL
	if (nSelectionMode == TM0) 	{
		AfxMessageBox(csErrorMsg[1]); // not yet supported
		m_LbError.SetText(L"ERROR !!");
		m_LbError.Invalidate();

		return;
	}


	// CTC
	else if ((nSelectionMode == TM4) || (nSelectionMode == TM12)) 	{
		error = _Setup_CTC(m_mode, nClock, m_nTargetFrequency);
	}

	// PWM
	else if (	(nSelectionMode == TM8) || (nSelectionMode == TM9)		||				
				(nSelectionMode == TM10) || (nSelectionMode == TM11)	||
				(nSelectionMode == TM14) || (nSelectionMode == TM15)       	)  	{
		error = _Setup_PWM(m_mode, nClock, m_nTargetFrequency,  m_PercentDuty);
	}

	// FIX
	else if ((nSelectionMode == TM1) || (nSelectionMode == TM2) || (nSelectionMode == TM3) ||					
			(nSelectionMode == TM5) || (nSelectionMode == TM6) || (nSelectionMode == TM7) )  {
		error = _Setup_FIX(m_mode, nClock, m_nTargetFrequency, m_PercentDuty);
	}

	// not supported case
	else  {
		AfxMessageBox(csErrorMsg[1]); // not yet supported
		m_LbError.SetText(L"ERROR !!");
		m_LbError.Invalidate();

		return;
	}

	// give error message if exist
	if (error) {
		AfxMessageBox(csErrorMsg[error]); // not yet supported
		m_LbError.SetText(L"ERROR !!");
		m_LbError.Invalidate();

		return;
	}
	m_LbError.SetText(L"OK");
	
	m_hedTimerFrequency.SetValue(m_nTimerFrequency, true);

	m_sldr_ICR1.SetPos(((m_ICR1  * 1.0) / (0xFFFF)) *(100));
	m_sldr_OCR1A.SetPos(((m_OCR1A  * 1.0) / (0xFFFF)) *(100));
	m_sldr_OCR1B.SetPos(((m_OCR1B  * 1.0) / (0xFFFF)) *(100));
	
	CDialog::OnInitDialog();
	return ;

}
void CTimer_1::OnBnClickedButtonOcr1aW()
{
	// WORD ramAdress = OCR1A;
	BYTE buffer[2], L = 2;
	WORD ramAdress = OCR1A;
	buffer[0] = BYTE(m_OCR1A);
	buffer[1] = BYTE(m_OCR1A >> 8);
	_WriteRam(ramAdress, buffer , L);
	m_hedOCR1A.SetValue(m_OCR1A, true);

}
void CTimer_1::OnBnClickedButtonOcr1aR()
{	
	// clear arrived switch. will be set by main dialog whan DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = OCR1A;
	_ReadRam(ramAdress);

	/*
	for (int versuch = 0; versuch < 3; versuch++)
	{
		Sleep(300 + 100 * versuch); // sleep a while if it cracks
		// check if DeTel arrived
		if (m_bDeTelArrived)
		{
			WORD wd;

			BYTE L = m_pMCDlg->m_czData[LENGTH_CONROLBITS - 1]; //  MAXIMUM_DATA_BYTE];
			BYTE H = m_pMCDlg->m_czData[LENGTH_CONROLBITS]; // MAXIMUM_DATA_BYTE];
			wd = ((WORD)H << 8) | L;
			m_OCR1A = wd;
			m_hedOCR1A.SetValue(wd, true);
			CDialog::OnInitDialog();
			break;
		}
		AfxMessageBox(L"Error");
	}*/
}
void CTimer_1::_ReadRam(WORD ramAdress)
{
	HRESULT hr;
	BOOL bError;
	IMTelegram* pRead = NULL;
	//WORD ramAdress = OCR1A;
	BYTE L = 2;
	/*
	BYTE bBuffer[2], L = 2;
	bBuffer[0] = BYTE(m_OCR1A);
	bBuffer[1] = BYTE(m_OCR1A >> 8);
	*/
	if (!m_pMCDlg)
		return;
	if (m_pMCDlg->m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pMCDlg->m_pISerialPort->CanDo()) {
		return;
	}

	try
	{
		if (m_pMCDlg->m_pIFabric) {
			hr = m_pMCDlg->m_pIFabric->ReadRAM(ramAdress, L, (IUnknown**)&pRead);
		}
		else {
			return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pMCDlg->m_pISink->ClearError();
			hr = m_pMCDlg->m_pISerialPort->SendDeTelTelegram(pRead);
			if (hr != S_OK) {
				pRead->Release();
				return;
			}
			// Error !
			m_pMCDlg->m_pISink->get_error(&bError);
			if (bError) {
				//	m_pISink->get_Message(&bstrMsg);
				//	AfxMessageBox( CStringUtility::bstrToCString(bstrMsg));
			}
			
			pRead->Release();
		}
	}
	catch (...) 	{
	}
}
void CTimer_1::_WriteRam(WORD ramAdress, BYTE* buffer, BYTE L, BYTE OR )
{
	HRESULT hr;
	BOOL bError;
	IMTelegram* pWrite = NULL;
	// WORD ramAdress = OCR1A;
//	BYTE bBuffer[2], L = 2;
//	bBuffer[0] = BYTE(m_OCR1A);
//	bBuffer[1] = BYTE(m_OCR1A >> 8);

	if (!m_pMCDlg)
		return;
	if (m_pMCDlg->m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pMCDlg->m_pISerialPort->CanDo()) {
		return;
	}

	try
	{
		if (m_pMCDlg->m_pIFabric) {
			hr = m_pMCDlg->m_pIFabric->WriteRAM(ramAdress, buffer, L, (IUnknown**)&pWrite);
		}
		else {
			return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pMCDlg->m_pISink->ClearError();
			// means write with | 
			pWrite->put_rs0(OR);
			hr = m_pMCDlg->m_pISerialPort->SendDeTelTelegram(pWrite);
			if (hr != S_OK) {
				pWrite->Release();
				return;
			}
			// Error !
			m_pMCDlg->m_pISink->get_error(&bError);
			{
				//	m_pISink->get_Message(&bstrMsg);
				//	AfxMessageBox( CStringUtility::bstrToCString(bstrMsg));
			}
			pWrite->Release();
		}
	}
	catch (...)
	{
	}
}
bool CTimer_1::_BuildFromFile(CString csFileName)
{
	HXMLITEM hNextNext;
	HXMLITEM hNext;
	HXMLITEM hRoot;
	CXmlStruct  TreeStruct;

	CString csDumy;
	int nDumy;

	/**/

	if (TreeStruct.ReadDataFile(csFileName))
	{
		return false;
	}
	
	hRoot = TreeStruct.GetRootItem();

	/////////////////////////////

	
	hNext = TreeStruct.GetChildItem(hRoot, CString("DESIGN"));

	// mode
	hNextNext = TreeStruct.GetChildItem(hNext, CString("TIMER-MODE"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	m_mode = _wtoi(csDumy.GetBuffer(0));
	m_cbMode.SetCurSel(m_mode);

	// pre scaler
	hNextNext = TreeStruct.GetChildItem(hNext, CString("PRE-SCALER"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	nDumy= _wtoi(csDumy.GetBuffer(0));
	m_cbPreScaler.SetCurSel(nDumy);

	// target frequency
	hNextNext = TreeStruct.GetChildItem(hNext, CString("TARGET-FREQUENCY"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	m_nTargetFrequency = _wtof(csDumy.GetBuffer(0));

	// Duty Percent
	hNextNext = TreeStruct.GetChildItem(hNext, CString("DUTY-PERCENT"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	m_PercentDuty = _wtoi(csDumy.GetBuffer(0));

	// enable timer
	hNextNext = TreeStruct.GetChildItem(hNext, CString("ENABLE-TIMER"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bET = _wtoi(csDumy.GetBuffer(0));

	// enable interrupt A
	hNextNext = TreeStruct.GetChildItem(hNext, CString("ENABLE-INTERRUPT-A"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bIA = _wtoi(csDumy.GetBuffer(0));

	// enable interrupt B
	hNextNext = TreeStruct.GetChildItem(hNext, CString("ENABLE-INTERRUPT-B"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bIB = _wtoi(csDumy.GetBuffer(0));


	// compare output mode
	hNext = TreeStruct.GetChildItem(hRoot, CString("COMPARE_OUTPUT_MOD"));

	// COM1A0
	hNextNext = TreeStruct.GetChildItem(hNext, CString("COM1A0"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bA0 = _wtoi(csDumy.GetBuffer(0));

	// COM1A1
	hNextNext = TreeStruct.GetChildItem(hNext, CString("COM1A1"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bA1 = _wtoi(csDumy.GetBuffer(0));

	// COM1B0
	hNextNext = TreeStruct.GetChildItem(hNext, CString("COM1B0"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bB0 = _wtoi(csDumy.GetBuffer(0));
	
	// COM1B1
	hNextNext = TreeStruct.GetChildItem(hNext, CString("COM1B1"));
	csDumy = TreeStruct.GetElementValue(hNextNext);
	bB1 = _wtoi(csDumy.GetBuffer(0));

	return true;
}
bool CTimer_1::_StoreIntoFile(CString csFileName)
{
	HXMLITEM hNewItem;
	HXMLITEM hNext;
	HXMLITEM hRoot;
	CXmlStruct  TreeStruct;
	CString csDumy, csNFactor;

	hRoot = TreeStruct.InsertItem(CString("TIMER_1_CONFIGURATION"), XML_TI_ROOT);

	//	Anlegen Design
	hNewItem = TreeStruct.InsertItem(CString("DESIGN"), hRoot);

	//	Anlegen mode
	hNext = TreeStruct.InsertItem(CString("TIMER-MODE"), hNewItem);
	csDumy.Format(L"%d", m_mode);
	TreeStruct.SetElementValue(hNext, csDumy);

	// pre scaler
	hNext = TreeStruct.InsertItem(CString("PRE-SCALER"), hNewItem);
	csDumy.Format(L"%d", m_cbPreScaler.GetCurSel());
	TreeStruct.SetElementValue(hNext, csDumy);
	TreeStruct.WriteDataFile(csFileName);

	// Target Frequency
	hNext = TreeStruct.InsertItem(CString("TARGET-FREQUENCY"), hNewItem);
	csDumy.Format(L"%f", m_nTargetFrequency);
	TreeStruct.SetElementValue(hNext, csDumy);
	TreeStruct.WriteDataFile(csFileName);

	// Duty Percent
	hNext = TreeStruct.InsertItem(CString("DUTY-PERCENT"), hNewItem);
	csDumy.Format(L"%f", m_PercentDuty);
	TreeStruct.SetElementValue(hNext, csDumy);
	TreeStruct.WriteDataFile(csFileName);

	// enable timer
	hNext = TreeStruct.InsertItem(CString("ENABLE-TIMER"), hNewItem);
	csDumy.Format(L"%d", bET);
	TreeStruct.SetElementValue(hNext, csDumy);

	// enable interrupt A 
	hNext = TreeStruct.InsertItem(CString("ENABLE-INTERRUPT-A"), hNewItem);
	csDumy.Format(L"%d", bIA);
	TreeStruct.SetElementValue(hNext, csDumy);

	// enable interrupt B 
	hNext = TreeStruct.InsertItem(CString("ENABLE-INTERRUPT-B"), hNewItem);
	TreeStruct.SetElementValue(hNext, csDumy);
	csDumy.Format(L"%d", bIA);

	//	Anlegen Ouptput Mode
	hNewItem = TreeStruct.InsertItem(CString("COMPARE_OUTPUT_MOD"), hRoot);
	
	// COM1A0
	hNext = TreeStruct.InsertItem(CString("COM1A0"), hNewItem);
	csDumy.Format(L"%d", bA0);
	TreeStruct.SetElementValue(hNext, csDumy);

	// COM1A1
	hNext = TreeStruct.InsertItem(CString("COM1A1"), hNewItem);
	csDumy.Format(L"%d", bA1);
	TreeStruct.SetElementValue(hNext, csDumy);

	// COM1B0
	hNext = TreeStruct.InsertItem(CString("COM1B0"), hNewItem);
	csDumy.Format(L"%d", bB0);
	TreeStruct.SetElementValue(hNext, csDumy);

	// COM1B1
	hNext = TreeStruct.InsertItem(CString("COM1B1"), hNewItem);
	csDumy.Format(L"%d", bB1);
	TreeStruct.SetElementValue(hNext, csDumy);

	TreeStruct.WriteDataFile(csFileName);

	return true;
}
void CTimer_1::OnBnClickedButtonOcr1bW()
{
	// WORD ramAdress = OCR1B;
	BYTE buffer[2], L = 2;
	WORD ramAdress = OCR1B;
	buffer[0] = BYTE(m_OCR1B);
	buffer[1] = BYTE(m_OCR1B >> 8);
	_WriteRam(ramAdress, buffer, L);
	m_hedOCR1B.SetValue(m_OCR1B, true);

}
void CTimer_1::OnBnClickedButtonOcr1bR()
{
	// clear arrived switch. will be set by main dialog whan DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = OCR1B;
	_ReadRam(ramAdress);
	
	/*
	for (int versuch = 0; versuch < 3; versuch++)
	{
		Sleep(300 + 300* versuch); // sleep a while if it cracks
		// check if DeTel arrived
		if (m_bDeTelArrived)
		{
			WORD wd;
			BYTE L = m_pMCDlg->m_czData[LENGTH_CONROLBITS - 1] ;	// MAXIMUM_DATA_BYTE];
			BYTE H = m_pMCDlg->m_czData[LENGTH_CONROLBITS ] ;		// MAXIMUM_DATA_BYTE];
			wd = ((WORD)H << 8) | L;
			m_OCR1B = wd;
			m_hedOCR1B.SetValue(wd, true);
			CDialog::OnInitDialog();
			break;
		}
		AfxMessageBox(L"Error");

	}*/
}
void CTimer_1::OnBnClickedButtonIcr1W()
{
	// WORD ramAdress = ICR1;
	BYTE buffer[2], L = 2;
	WORD ramAdress = ICR1;
	buffer[0] = BYTE(m_ICR1);
	buffer[1] = BYTE(m_ICR1 >> 8);
	_WriteRam(ramAdress, buffer, L);
	
	m_hedICR1.SetValue(m_ICR1, true);


}
void CTimer_1::OnBnClickedButtonIcr1R()
{
	// clear arrived switch. will be set by main dialog whan DeTel arrives
	m_bDeTelArrived = false;   

	WORD ramAdress = ICR1;
	_ReadRam(ramAdress);
	/*
	for (int versuch = 0; versuch < 3; versuch++)
	{
		Sleep(100 + 300 * versuch); // sleep a while if it cracks
		// check if DeTel arrived
		if (m_bDeTelArrived)
		{
			WORD wd;
			BYTE L = m_pMCDlg->m_czData[LENGTH_CONROLBITS - 1];		// MAXIMUM_DATA_BYTE];
			BYTE H = m_pMCDlg->m_czData[LENGTH_CONROLBITS];			// MAXIMUM_DATA_BYTE];
			wd = ((WORD)H << 8) | L;
			m_ICR1 = wd;
			m_hedICR1.SetValue(wd, true);
			CDialog::OnInitDialog();
			break;
		}
		AfxMessageBox(L"Error");
	}*/
}

void CTimer_1::OnBnClickedButtonTcr1bR()
{
	WORD ramAdress = TCCR1B; 
	_ReadRam(ramAdress);
}

void CTimer_1::OnBnClickedButtonTccr1bW()
{
	// WORD ramAdress = ICR1;
	BYTE buffer[2], L = 2;
	WORD ramAdress = TCCR1B;
	buffer[0] = BYTE(m_TCCR1B);
	buffer[1] = BYTE(m_TCCR1A);
	_WriteRam(ramAdress, buffer, L);

	m_hedTCCR1B.SetValue(m_TCCR1B, true);
	m_hedTCCR1A.SetValue(m_TCCR1A, true);
}

void CTimer_1::OnCbnSelchangeComboPreCompareOutputMod()
{
/*
	CButton * A0 = (CButton *)GetDlgItem(IDC_CHECK_COM1A0);
	CButton * B0 = (CButton *)GetDlgItem(IDC_CHECK_COM1B0);

	CButton * A1 = (CButton *)GetDlgItem(IDC_CHECK_COM1A1);
	CButton * B1 = (CButton *)GetDlgItem(IDC_CHECK_COM1B1);
*/
	int nSel = m_cbCompareOutputMode.GetCurSel();
	int nSelChn_A = ((CButton*) GetDlgItem(IDC_RADIO_CHANNEL_A))->GetCheck();
	int nSelChn_B = ((CButton*)GetDlgItem(IDC_RADIO_CHANNEL_B))->GetCheck();
//	int nSelChn_Both = ((CButton*)GetDlgItem(IDC_RADIO_CHANNEL_BOTH))->GetCheck();
/*
	if (nSelChn_Both)
	{ 
		if (nSel == 0)
		{
			bA0 = FALSE;
			bA1 = FALSE;
			bB0 = FALSE;
			bB1 = FALSE;
		}
		else if (nSel == 1)
		{
			bA0 = TRUE;
			bA1 = FALSE;
			bB0 = TRUE;
			bB1 = FALSE;

		}
		else if (nSel == 2)
		{
			bA0 = FALSE;
			bA1 = TRUE;
			bB0 = FALSE;
			bB1 = TRUE;

		}
		else if (nSel == 3)
		{
			bA0 = TRUE;
			bA1 = TRUE;
			bB0 = TRUE;
			bB1 = TRUE;

		}
	}
*/
	if (nSelChn_A)
	{
		if (nSel == 0)
		{
			bA0 = FALSE;
			bA1 = FALSE;
	
//			bB0 = FALSE;
//			bB1 = FALSE;
		}
		else if (nSel == 1)
		{
			bA0 = TRUE;
			bA1 = FALSE;
			
//			bB0 = FALSE;
//			bB1 = FALSE;

		}
		else if (nSel == 2)
		{
			bA0 = FALSE;
			bA1 = TRUE;
			
//			bB0 = FALSE;
//			bB1 = FALSE;

		}
		else if (nSel == 3)
		{
			bA0 = TRUE;
			bA1 = TRUE;

//			bB0 = FALSE;
//			bB1 = FALSE;

		}

	}


	else if (nSelChn_B)
	{
		if (nSel == 0)
		{
//			bA0 = FALSE;
//			bA1 = FALSE;
			
			bB0 = FALSE;
			bB1 = FALSE;
		}
		else if (nSel == 1)
		{
//			bA0 = FALSE;
//			bA1 = FALSE;
			
			bB0 = TRUE;
			bB1 = FALSE;

		}
		else if (nSel == 2)
		{
//			bA0 = FALSE;
//			bA1 = FALSE;

			bB0 = FALSE;
			bB1 = TRUE;

		}
		else if (nSel == 3)
		{
//			bA0 = FALSE;
//			bA1 = FALSE;

			bB0 = TRUE;
			bB1 = TRUE;

		}

	}

	CDialog::OnInitDialog();
	// TODO: Add your control notification handler code here
}

void CTimer_1::OnBnClickedCheckInterruptA()
{
	CButton* pEIntA = (CButton*)GetDlgItem(IDC_CHECK_INTERRUPT_A);
	if (pEIntA->GetCheck())
		bIA = true;
	else 
		bIA = false;

	CDialog::OnInitDialog();// TODO: Add your control notification handler code here
}

void CTimer_1::OnBnClickedCheckInterruptB()
{
	CButton* pEIntB = (CButton*)GetDlgItem(IDC_CHECK_INTERRUPT_B);
	if (pEIntB->GetCheck())
		bIB = true;
	else
		bIB = false;
}

void CTimer_1::OnBnClickedCheckEnableTimer()
{
	CButton* pET = (CButton*)GetDlgItem(IDC_CHECK_ENABLE_TIMER);
	if (pET->GetCheck())
		bET = true;
	else
		bET = false;

	CDialog::OnInitDialog();// TODO: Add your control notification handler code here
}

void CTimer_1::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void CTimer_1::OnBnClicked_ButtonTifrR()
{
	// clear arrived switch. 
	// will be set by main dialog whan DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = TIFR;
	_ReadRam(ramAdress);
}

void CTimer_1::OnBnClicked_ButtonTifrW()
{
	// WORD ramAdress = ICR1;
	BYTE buffer[2], L = 2;
	WORD ramAdress = TIFR;
	buffer[0] = BYTE(m_TIFR);
	buffer[1] = BYTE(m_TIMSK);
	_WriteRam(ramAdress, buffer, L, 1);   // write with or   

	m_hedTIFR.SetValue(m_TIFR, true);
	m_hedTIMSK.SetValue(m_TIMSK, true);
}

void CTimer_1::OnEnChangeEditTop()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CTimer_1::OnBnClickedRadioChannelBoth()
{
	OnCbnSelchangeComboPreCompareOutputMod();
}

void CTimer_1::OnBnClickedRadioChannelB()
{
	int nSel = (1 << bB1) | bB0;
	m_cbCompareOutputMode.SetCurSel(nSel);

//	OnCbnSelchangeComboPreCompareOutputMod();
}

void CTimer_1::OnBnClickedRadioChannelA()
{
	
//	OnCbnSelchangeComboPreCompareOutputMod();
	int nSel = (1 << bA1) | bA0;
	m_cbCompareOutputMode.SetCurSel(nSel);
	
}

void CTimer_1::OnReleasedcaptureSlider_Icr1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_ICR1 = ((m_sldr_ICR1.GetPos() * 1.0)  / 100.0 ) * 0xFFFF ;
	*pResult = 0;

	CDialog::OnInitDialog();
}

void CTimer_1::OnReleasedcaptureSlider_Ocr1a(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_OCR1A = ((m_sldr_OCR1A.GetPos() * 1.0) / 100.0) * 0xFFFF;
	*pResult = 0;
	CDialog::OnInitDialog();
}

void CTimer_1::OnReleasedcaptureSlider_Ocr1b(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_OCR1B = ((m_sldr_OCR1B.GetPos() * 1.0) / 100.0) * 0xFFFF;
	*pResult = 0;
	CDialog::OnInitDialog();
}


void CTimer_1::OnBnClickedButtonSave()
{

	UpdateData();

	TCHAR szFilters[] = _T("timer (*.tmr)|*.tmr|All Files (*.*)|*.*||");
	CFileDialog dlgFile(FALSE, _T("timer"), _T("*.tmr"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
	dlgFile.m_ofn.lpstrTitle = L"save timer";
	CString fileName;

	const int c_cMaxFiles = 100;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlgFile.GetOFN().nMaxFile = c_cMaxFiles;

	if (dlgFile.DoModal() == IDCANCEL)
	{ 
		fileName.ReleaseBuffer();
		return;
	}

	_StoreIntoFile(fileName);
	fileName.ReleaseBuffer();
}


void CTimer_1::OnBnClickedButtonOpen()
{
	TCHAR szFilters[] = _T("timer (*.tmr)|*.tmr|All Files (*.*)|*.*||");
	CFileDialog dlgFile(TRUE, _T("timer configuration"), _T("*.tmr"), OFN_CREATEPROMPT | OFN_HIDEREADONLY, szFilters);
	CString fileName;
	const int c_cMaxFiles = 100;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlgFile.GetOFN().nMaxFile = c_cMaxFiles;

	if (dlgFile.DoModal() == IDCANCEL)
	{
		//AfxMessageBox(L"ho ho");
		fileName.ReleaseBuffer();
		return;
	}

	if (_BuildFromFile(fileName) == false)
	{
		AfxMessageBox(L"error in reading !!");
	}

	fileName.ReleaseBuffer();
	
	//update 
	OnSelchangeComboMode();
	((CButton*)GetDlgItem(IDC_RADIO_CHANNEL_A))->SetCheck(1);
	
	OnCbnSelchangeComboPreCompareOutputMod();
	
	CDialog::OnInitDialog();

}


void CTimer_1::OnSelchange_ComboCpuFrequency()
{
	int nSel = m_cbCpuFrequency.GetCurSel();
	m_nCpuFrequency =   lCpuFrequencies[nSel];
	// CDialog::OnInit
}

