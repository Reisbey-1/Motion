
// ServoMotor_ModelSimulation.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CServoMotor_ModelSimulationApp:
// See ServoMotor_ModelSimulation.cpp for the implementation of this class
//

class CServoMotor_ModelSimulationApp : public CWinApp
{
public:
	CServoMotor_ModelSimulationApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CServoMotor_ModelSimulationApp theApp;