// Timer_2.cpp : implementation file
//

#include "stdafx.h"
#include "MotionControl.h"
#include "Timer_2.h"
#include "afxdialogex.h"
double thePreScaler_t2[8] = { 0,	1,	8,	32, 64,		128,	256,	1024 };


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
#define TOV0	0
#define OCF0	1
#define TOV1	2
#define OCF1B	3
#define OCF1A	4
#define ICF1	5
#define TOV2	6
#define OCF2	7

#define	TCCR2	0x45
#define FOC2	7 
#define WGM20	6
#define COM21	5
#define COM20	4
#define WGM21	3
#define CS22	2
#define CS21	1
#define CS20	0

#define OCR2    0x43  // _SFR_IO8(0x23)
// CTimer_2 dialog

IMPLEMENT_DYNAMIC(CTimer_2, CDialogEx)

CTimer_2::CTimer_2(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TIMER_2, pParent)
	, m_dF_r(100)
	, m_TIMSK(0)
	, m_TCCR2(0)
	, m_clsel(7)
	, m_clsel_H(7)
	, m_pMCDlg(NULL)
	, m_COM_21_20 (0)
	, m_bDeTelArrived(false)

{

}

CTimer_2::~CTimer_2()
{
}

void CTimer_2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_R_PRE_SCALER, m_cbPreScaler);
	DDX_Control(pDX, IDC_COMBO_R_MODE, m_cbMode);
	DDX_Control(pDX, IDC_COMBO_R_TOP, m_cbTop);
	DDX_Control(pDX, IDC_COMBO_R_CPU_FREQUENCY, m_cbCpuFrequency);
	DDX_Text(pDX, IDC_EDIT_RESONATOR_FREQUENCY, m_dF_r);
	DDX_Control(pDX, IDC_COMBO_R_COMPARE_OUTPUT_MOD, m_cbCompareOutputMode);
	DDX_Control(pDX, IDC_EDIT_TCCR2_HEX, m_hexTCCR2);
	DDX_Control(pDX, IDC_EDIT_TIFR2_HEX, m_hexTIFR);
	DDX_Control(pDX, IDC_EDIT_TIMSK2_HEX, m_hexTIMSK);
	DDX_Control(pDX, IDC_EDIT_OCR2_HEX, m_hexOCR2);
}


BEGIN_MESSAGE_MAP(CTimer_2, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_R_MODE, &CTimer_2::OnCbnSelchange_ComboRMode)
	ON_BN_CLICKED(IDC_CHECK_EN_TOV_INT, &CTimer_2::OnBnClickedCheckEnTovInt)
	ON_BN_CLICKED(IDC_CHECK_EN_OC_INT, &CTimer_2::OnBnClickedCheckEnOcInt)
	ON_BN_CLICKED(IDC_CHECK_R_ENABLE_TIMER, &CTimer_2::OnBnClickedCheckREnableTimer)
	ON_BN_CLICKED(IDC_RADIO_R_NONE, &CTimer_2::OnBnClickedRadioRNone)
	ON_BN_CLICKED(IDC_RADIO_FAST, &CTimer_2::OnBnClickedRadioFast)
	ON_BN_CLICKED(IDC_RADIO_PC, &CTimer_2::OnBnClickedRadioPc)
	ON_CBN_SELCHANGE(IDC_COMBO_R_COMPARE_OUTPUT_MOD, &CTimer_2::OnCbnSelchangeComboRCompareOutputMod)
	ON_BN_CLICKED(IDC_BUTTON_R_UPDATE, &CTimer_2::OnBnClickedButtonRUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_R_PRE_SCALER, &CTimer_2::OnCbnSelchangeComboRPreScaler)
	ON_EN_CHANGE(IDC_EDIT_RESONATOR_FREQUENCY, &CTimer_2::OnEnChangeEditResonatorFrequency)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_TCCR2, &CTimer_2::OnBnClickedButton_WriteTccr2)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_OCR2, &CTimer_2::OnBnClickedButton_WriteOcr2)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_TIMSK, &CTimer_2::OnBnClickedButton_WriteTimsk)
	ON_BN_CLICKED(IDC_BUTTON_READ_TCCR2, &CTimer_2::OnBnClickedButton_ReadTccr2)
	ON_BN_CLICKED(IDC_BUTTON_READ_OCR2, &CTimer_2::OnBnClickedButton_ReadOcr2)
	ON_BN_CLICKED(IDC_BUTTON_READ_TIMSK, &CTimer_2::OnBnClickedButton_ReadTimsk)
	ON_BN_CLICKED(IDC_BUTTON_READ_Fr, &CTimer_2::OnBnClickedButtonReadFr)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_Fr, &CTimer_2::OnBnClickedButtonWriteFr)
END_MESSAGE_MAP()


// CTimer_2 message handlers


BOOL CTimer_2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// cpu frequency
	m_cbCpuFrequency.AddString(L"1 MHz.");
	m_cbCpuFrequency.AddString(L"2 MHz.");
	m_cbCpuFrequency.AddString(L"4 MHz.");
	m_cbCpuFrequency.AddString(L"8 MHz.");
	m_cbCpuFrequency.AddString(L"16 MHz.");
	m_cbCpuFrequency.AddString(L"32 MHz.");
	m_cbCpuFrequency.SetCurSel(3);

	// atmega16 datasheet page.125
	m_cbPreScaler.AddString(L"none");
	m_cbPreScaler.AddString(L"1");
	m_cbPreScaler.AddString(L"8");
	m_cbPreScaler.AddString(L"32");
	m_cbPreScaler.AddString(L"64");
	m_cbPreScaler.AddString(L"128");
	m_cbPreScaler.AddString(L"256");
	m_cbPreScaler.AddString(L"1024");

	m_cbPreScaler.SetCurSel(m_clsel);

	CButton *pCOM21 = (CButton*)GetDlgItem(IDC_CHECK_COM21);
	CButton *pCOM20 = (CButton*)GetDlgItem(IDC_CHECK_COM20);
	pCOM21->SetCheck(0);
	pCOM20->SetCheck(1);
	// table 50
	m_cbMode.AddString(L"0) Normal");
	m_cbMode.AddString(L"1) PWM, Phase correct");
	m_cbMode.AddString(L"2) CTC");
	m_cbMode.AddString(L"3) Fast PWM");
	m_cbMode.SetCurSel(2);
	OnCbnSelchange_ComboRMode();
	

	m_cbTop.AddString(L"0xFF");
	m_cbTop.AddString(L"OCR2");
	m_cbTop.SetCurSel(1);


	// enable timer initialy
	CButton *pEnableTimer = (CButton*)GetDlgItem(IDC_CHECK_R_ENABLE_TIMER);
	pEnableTimer->SetCheck(1);
	OnBnClickedCheckREnableTimer();



	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
void CTimer_2::OnCbnSelchange_ComboRMode()
{
	CButton *pCheck = NULL;
	CButton * pCheck1 = (CButton*)GetDlgItem(IDC_RADIO_R_NONE);
	CButton * pCheck2 = (CButton*)GetDlgItem(IDC_RADIO_FAST);
	CButton * pCheck3 = (CButton*)GetDlgItem(IDC_RADIO_PC);
	CButton * pWGM20 = (CButton*)GetDlgItem(IDC_CHECK_WGM20);
	CButton * pWGM21 = (CButton*)GetDlgItem(IDC_CHECK_WGM21);

	pWGM20->SetCheck(0);
	pWGM21->SetCheck(0);


	uint8_t sel = m_cbMode.GetCurSel();
	if (sel == 2)
		m_cbTop.SetCurSel(1);
	else m_cbTop.SetCurSel(0);

	if ((sel == 2) || (sel == 0))
	{
		//pCheck = (CButton*)GetDlgItem(IDC_RADIO_R_NONE);
		pCheck1->SetCheck(1);
		pCheck2->SetCheck(0);
		pCheck3->SetCheck(0);
		OnBnClickedRadioRNone();
		if (sel == 2)
			pWGM21->SetCheck(1);

	}
	else if (sel == 1)
	{
//		pCheck = (CButton*)GetDlgItem(IDC_RADIO_FAST);
//		pCheck->SetCheck(1);
		pCheck1->SetCheck(0);
		pCheck2->SetCheck(1);
		pCheck3->SetCheck(0);
		OnBnClickedRadioFast();
		pWGM20->SetCheck(1);

	}
	else if  (sel == 3)
	{
		//pCheck = (CButton*)GetDlgItem(IDC_RADIO_PC);
		//pCheck->SetCheck(1);
		pCheck1->SetCheck(0);
		pCheck2->SetCheck(0);
		pCheck3->SetCheck(1);
		OnBnClickedRadioPc();
		pWGM20->SetCheck(1);
		pWGM21->SetCheck(1);

	}

	// TCCR2 : FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20 
	m_TCCR2 &= ~((1 << WGM21) | (1 << WGM20));
	m_TCCR2 |= ((sel >> 1) << WGM21);
	m_TCCR2 |= ((sel & 0x01) << WGM20);
	//OnCbnSelchangeComboRCompareOutputMod();

	OnBnClickedButtonRUpdate();
	CDialog::OnInitDialog();
}
void CTimer_2::OnBnClickedCheckEnTovInt()
{
	// interrupts
	// TIMSK : OCIE2 TOIE2 TICIE1 OCIE1A OCIE1B TOIE1 – TOIE0 
	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_EN_TOV_INT);
	if (pCheck->GetCheck())
		m_TIMSK |= (1 << TOIE2);  //
	else
		m_TIMSK &= ~(1 << TOIE2);

	OnBnClickedButtonRUpdate();
}
void CTimer_2::OnBnClickedCheckEnOcInt()
{
	// interrupts
	// TIMSK : OCIE2 TOIE2 TICIE1 OCIE1A OCIE1B TOIE1 – TOIE0 
	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_EN_OC_INT);
	if (pCheck->GetCheck())
		m_TIMSK |= (1 << OCIE2);  //
	else
		m_TIMSK &= ~(1 << OCIE2);
	OnBnClickedButtonRUpdate();

}
void CTimer_2::OnBnClickedCheckREnableTimer()
{
	// TCCR2 : FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20 
	// enable timer

	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_R_ENABLE_TIMER);
	if (pCheck->GetCheck()) {
		m_cbPreScaler.SetCurSel(m_clsel_H);
//		m_TCCR2 = m_TCCR2 & (0xF8);
//		m_TCCR2 |= (m_clsel & 0x07);	// clock selection:	0000 0111:	----  -xxx
	}
	else
	{
//		m_TCCR2 = m_TCCR2 & (0xF8);
		m_cbPreScaler.SetCurSel(0);
	}
	CDialog::OnInitDialog();
	OnCbnSelchangeComboRPreScaler();
}

void CTimer_2::OnBnClickedRadioRNone()
{
	// TCCR2 : FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20 

	CButton *pCheck = (CButton*)GetDlgItem(IDC_RADIO_R_NONE);
	CButton *pCOM21 = (CButton*)GetDlgItem(IDC_CHECK_COM21);
	CButton *pCOM20 = (CButton*)GetDlgItem(IDC_CHECK_COM20);
	//uint8_t m_COM_21_20 = 0;
	m_COM_21_20 = 0;


	if (pCheck->GetCheck()) {
		m_cbCompareOutputMode.ResetContent();
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC2 disconnected.");
		m_cbCompareOutputMode.AddString(L"Toggle OC2 on compare match.");
		m_cbCompareOutputMode.AddString(L"Clear OC2 on compare match");
		m_cbCompareOutputMode.AddString(L"Set OC2 on compare match");
	
		if (pCOM20->GetCheck())
			m_COM_21_20 = 1;
		if (pCOM21->GetCheck())
			m_COM_21_20 += 2;
		m_TCCR2 = ((m_TCCR2) & (0b11000000)) | (m_COM_21_20 << COM20);
		m_cbCompareOutputMode.SetCurSel(m_COM_21_20);
	}	
}
void CTimer_2::OnBnClickedRadioFast()
{
	CButton *pCheck = (CButton*)GetDlgItem(IDC_RADIO_FAST);
	CButton *pCOM21 = (CButton*)GetDlgItem(IDC_CHECK_COM21);
	CButton *pCOM20 = (CButton*)GetDlgItem(IDC_CHECK_COM20);
	uint8_t COM_21_20 = 0;
	if (pCheck->GetCheck()) {
		m_cbCompareOutputMode.ResetContent();
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC2 disconnected.");
		m_cbCompareOutputMode.AddString(L"reserved.");
		m_cbCompareOutputMode.AddString(L"Clear OC2 on compare match, set OC2 at BOTTOM, (non-inverting mode)");
		m_cbCompareOutputMode.AddString(L"Set OC2 on compare match, clear OC2 at BOTTOM, (inverting mode)");

		if (pCOM20->GetCheck())
			COM_21_20 = 1;
		if (pCOM21->GetCheck())
			COM_21_20 += 2;
		m_TCCR2 = ((m_TCCR2) & (0b11000000)) | (COM_21_20 << COM20);
		m_cbCompareOutputMode.SetCurSel(COM_21_20);
	}
}
void CTimer_2::OnBnClickedRadioPc()
{
	CButton *pCheck = (CButton*)GetDlgItem(IDC_RADIO_PC);
	CButton *pCOM21 = (CButton*)GetDlgItem(IDC_CHECK_COM21);
	CButton *pCOM20 = (CButton*)GetDlgItem(IDC_CHECK_COM20);
	uint8_t COM_21_20 = 0;
	if (pCheck->GetCheck()) {
		m_cbCompareOutputMode.ResetContent();
		m_cbCompareOutputMode.AddString(L"Normal port operation, OC2 disconnected.");
		m_cbCompareOutputMode.AddString(L"reserved.");
		m_cbCompareOutputMode.AddString(L"Clear OC2 on compare match when up-counting. Set OC2 on compare match when downcounting.");
		m_cbCompareOutputMode.AddString(L"Set OC2 on compare match when up-counting. Clear OC2 on compare match when downcounting.");

		if (pCOM20->GetCheck())
			COM_21_20 = 1;
		if (pCOM21->GetCheck())
			COM_21_20 += 2;
		m_TCCR2 = ((m_TCCR2) & (0b11000000)) | (COM_21_20 << COM20);
		m_cbCompareOutputMode.SetCurSel(COM_21_20);
	}
}
void CTimer_2::OnCbnSelchangeComboRCompareOutputMod()
{
	uint8_t sel = m_cbCompareOutputMode.GetCurSel();
	CButton *pCOM20 = (CButton*)GetDlgItem(IDC_CHECK_COM20);
	CButton *pCOM21 = (CButton*)GetDlgItem(IDC_CHECK_COM21);

	if ((sel == 1) || (sel == 3))
		pCOM20->SetCheck(1);
	else 
		pCOM20->SetCheck(0);

	if ((sel==2) || (sel==3))
		pCOM21->SetCheck(1);
	else 
		pCOM21->SetCheck(0);

	m_TCCR2 = ((m_TCCR2) & (0b11001111)) | (sel << COM20);

	OnBnClickedButtonRUpdate();

}
void CTimer_2::OnBnClickedButtonRUpdate()
{
	uint16_t regCompare, TmrFrequency;
//	uint32_t CPUfrequency;
	m_hexTCCR2.SetValue(m_TCCR2, true);
	m_hexTIMSK.SetValue(m_TIMSK, true);
	
//	m_hexTIFR

	// setup
	m_OCR2 = 0;
	int sel = m_cbCpuFrequency.GetCurSel();
	int N = m_cbPreScaler.GetCurSel();
	if (N)
	{
		TmrFrequency = lCpuFrequencies[sel] / thePreScaler_t2[ m_cbPreScaler.GetCurSel()];

		//TmrFrequency = (CPUfrequency / N );
	
		regCompare = ( TmrFrequency / m_dF_r) - 1;
		if (regCompare > 0xFF)
		{
			AfxMessageBox(L"invalid frequency: \nincresae prescaler and/or frequncy");
			return; //  4;		// timer frequency is high
		}


		m_OCR2 = regCompare;
	}

	m_hexOCR2.SetValue(m_OCR2, true);
	CDialog::OnInitDialog();

}
void CTimer_2::OnCbnSelchangeComboRPreScaler()
{
	// TCCR2 : FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20 
	CButton *pCS20 = (CButton*)GetDlgItem(IDC_CHECK_CS20);
	CButton *pCS21 = (CButton*)GetDlgItem(IDC_CHECK_CS21);
	CButton *pCS22 = (CButton*)GetDlgItem(IDC_CHECK_CS22);


	m_clsel = m_cbPreScaler.GetCurSel();

	// updates CS20, CS21 and CS22 checkbox contols
	pCS20->SetCheck(0);
	pCS21->SetCheck(0);
	pCS22->SetCheck(0);
	if ((m_clsel == 1) || (m_clsel == 3) || (m_clsel == 5) || (m_clsel == 7))
		pCS20->SetCheck(1);
	if ((m_clsel == 2) || (m_clsel == 3) || (m_clsel == 6) || (m_clsel == 7))
		pCS21->SetCheck(1);
	if (m_clsel> 3) 
		pCS22->SetCheck(1);


	// keep last non-zero as history
	m_TCCR2 &= 0b11111000;
	m_TCCR2 |=  0b00000111 & m_clsel;

	if (m_clsel)
		m_clsel_H = m_clsel;
	OnBnClickedButtonRUpdate();
	
}
void CTimer_2::OnEnChangeEditResonatorFrequency()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_RESONATOR_FREQUENCY);
	if (pEdit->GetWindowTextLengthW() == 0)
		return;
	CDialog::UpdateData();
}

void CTimer_2::_WriteRam(WORD ramAdress, BYTE* buffer, BYTE L, BYTE OR)
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
void CTimer_2::_ReadRam(WORD ramAdress, BYTE L)
{
	HRESULT hr;
	BOOL bError;
	IMTelegram* pRead = NULL;
	//WORD ramAdress = OCR1A;
	//BYTE L = 2;
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
	catch (...) {
	}
}
void CTimer_2::update()
{
	// CMotionControlDlg * pDlg = m_pMCDlg;
	BYTE L;
	BYTE H;
	WORD adr, val;

	CEdit *pEdit;
	wchar_t buffer[8];

	union convert
	{
		uint8_t		_db[4];
		uint32_t	_wd;
	};
	convert myData;
	if (m_pMCDlg->m_Detel.cm1 == SP_RRAM)
	{
		// adress 
		adr = ((WORD)m_pMCDlg->m_Detel.ad1 << 8) | m_pMCDlg->m_Detel.ad0;

		// Get Word
		L = m_pMCDlg->m_Detel.db[0]; // [LENGTH_CONROLBITS - 1];		// MAXIMUM_DATA_BYTE];
		H = m_pMCDlg->m_Detel.db[1]; // m_czData[LENGTH_CONROLBITS];			// MAXIMUM_DATA_BYTE];

		val = ((WORD)H << 8) | L;

		if (adr == TCCR2) {
			m_TCCR2 = L;
			/*
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ICR1);
			_itow_s(val, buffer, 10);
			pEdit->SetWindowText(buffer);
			*/
			m_hexTCCR2.SetValue(val, true);
		}

		else if (adr == OCR2) {
			m_OCR2 = L;
			/*
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_OCR1A);
			_itow_s(val, buffer, 10);
			pEdit->SetWindowText(buffer);*/

			m_hexOCR2.SetValue(val, true);
			//		CDialog::OnInitDialog(); 
		}
		else if (adr == TIMSK) {
			m_TIMSK = L;
			/*
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_OCR1B);
			_itow_s(val, buffer, 10);
			pEdit->SetWindowText(buffer);*/

			m_hexTIMSK.SetValue(val, true);
			//		CDialog::OnInitDialog();
		}

	}
	if ((m_pMCDlg->m_Detel.cm1 == SP_ACTN) && ( (m_pMCDlg->m_Detel.rs0 == CMotionControlDlg::ACT_READ_RESONATOR) ||  (m_pMCDlg->m_Detel.rs0 == CMotionControlDlg::ACT_WRITE_RESONATOR) ))
	{
		// acceleration
		for (int i = 0; i < 4; i++)
			myData._db[i] = m_pMCDlg->m_Detel.db[i];
		m_dF_r = CNumberUtility::unpack754_32(myData._wd);
	//	CDialog::OnInitDialog();
		CEdit* pF_r = (CEdit*)GetDlgItem(IDC_EDIT_RESONATOR_FREQUENCY);
		CString csVal;
		csVal.Format(L"%f", m_dF_r);
		pF_r->SetWindowTextW(csVal);

	}

}

// write /read AVR
void CTimer_2::OnBnClickedButton_WriteTccr2()
{
	BYTE buffer[1], L = 1;
	WORD ramAdress = TCCR2;
	buffer[0] = BYTE(m_TCCR2);
	_WriteRam(ramAdress, buffer, L);
}
void CTimer_2::OnBnClickedButton_WriteOcr2()
{
	BYTE buffer[1], L = 1;
	WORD ramAdress = OCR2;
	buffer[0] = BYTE(m_OCR2);
	_WriteRam(ramAdress, buffer, L);
}
void CTimer_2::OnBnClickedButton_WriteTimsk()
{
	BYTE OR = 1;
	BYTE buffer[1], L = 1;
	WORD ramAdress = TIMSK;
	buffer[0] = BYTE(m_TIMSK);
	_WriteRam(ramAdress, buffer, L, OR);
}


void CTimer_2::OnBnClickedButton_ReadTccr2()
{
	// clear arrived switch. 
	// will be set by main dialog when DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = TCCR2;
	_ReadRam(ramAdress, 1);
}
void CTimer_2::OnBnClickedButton_ReadOcr2()
{
	// clear arrived switch. 
	// will be set by main dialog when DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = OCR2;
	_ReadRam(ramAdress, 1);
}
void CTimer_2::OnBnClickedButton_ReadTimsk()
{
	// clear arrived switch. 
	// will be set by main dialog when DeTel arrives
	m_bDeTelArrived = false;

	WORD ramAdress = TIMSK;
	_ReadRam(ramAdress, 1);
}


void CTimer_2::OnBnClickedButtonReadFr()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;
	BOOL bError;
	IMTelegram* pTelAction = NULL;
	BYTE data[1] = { 0 };



	if (m_pMCDlg->m_pISerialPort == NULL) {
		return;
	}
	if (S_FALSE == m_pMCDlg->m_pISerialPort->CanDo()) {
		return;
	}

	try
	{
		if (m_pMCDlg->m_pIFabric) {
			hr = m_pMCDlg->m_pIFabric->Action(CMotionControlDlg::ACT_READ_RESONATOR, (BYTE**)&data, 0, (IUnknown**)&pTelAction);
		}
		else {
			return;
		}

		if (hr == S_OK)
		{
			// clear sink;
			m_pMCDlg->m_pISink->ClearError();
			hr = m_pMCDlg->m_pISerialPort->SendDeTelTelegram(pTelAction);
			if (hr != S_OK) {
				pTelAction->Release();
				return;
			}
			// Error !
			m_pMCDlg->m_pISink->get_error(&bError);
			// if (!bError)
			{

			}
			pTelAction->Release();
		}
	}
	catch (...)
	{

	}
}


void CTimer_2::OnBnClickedButtonWriteFr()
{
	m_pMCDlg->_action(CMotionControlDlg::bWRITE_RESONATOR, m_dF_r);
}
