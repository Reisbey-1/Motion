#include "stdafx.h"
#include "WindowsPainter.h"


CWindowsPainter* CWindowsPainter::m_pWindowsPainter = NULL;

CWindowsPainter* CWindowsPainter::GetPainter()
{
	if (m_pWindowsPainter == NULL)
		m_pWindowsPainter = new CWindowsPainter;
	return m_pWindowsPainter;

}
void CWindowsPainter::KillPainter()
{
	if (m_pWindowsPainter)
	{
		delete m_pWindowsPainter;
		m_pWindowsPainter = NULL;
	}
}

void CWindowsPainter::DrawBackColor(CDC *pDC)
{
	COLORREF	bkcolor = RGB(250, 0, 170);			// back color
	RECT rc;
	pDC->GetClipBox(&rc);
	//rc.left -= m_pointOrigin.x;
	//rc.top   -= m_pointOrigin.y;
	CBrush backGroundBrush;
	backGroundBrush.CreateSolidBrush(bkcolor);
	CBrush* pOriginalBrush = (CBrush*)pDC->SelectObject(&backGroundBrush);
	pDC->FillRect(&rc, &backGroundBrush);
	backGroundBrush.Detach();
	backGroundBrush.DeleteObject();
	pDC->SelectObject(*pOriginalBrush);

}
void CWindowsPainter::MarkNode(CDC *pDC, int X , int Y , COLORREF color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBrush nodeBrush;
	CPen nodePen;
	COLORREF  nodeColor_HL = RGB(255, 0, 0);
	//COLORREF  nodeColor = RGB(0, 0, 255);

	//RECT rc = { X,Y,X+25,Y+25 };
	RECT NodeRc{ X,Y,X + 5,Y + 5 };
	nodePen.CreatePen(PS_SOLID, 1, nodeColor_HL);
	nodeBrush.CreateSolidBrush(color);

	CPen * pOldPen = pDC->SelectObject(&nodePen);
	CBrush* pbOldBrs = pDC->SelectObject(&nodeBrush);

	/*
	NodeRc.top = rc.top + (rc.bottom - rc.top) / 8;
	NodeRc.bottom = rc.bottom - (rc.bottom - rc.top) / 8;
	NodeRc.left = rc.left + (rc.right - rc.left) / 2;
	NodeRc.right = rc.right - (rc.right - rc.left) / 2;
	*/

	CRgn rgn;
	rgn.CreateEllipticRgnIndirect(&NodeRc);
	pDC->FillRgn(&rgn, &nodeBrush);
	pDC->Ellipse(&NodeRc);

	// select old objects
	pDC->SelectObject(pOldPen)->DeleteObject();
	pDC->SelectObject(pbOldBrs)->DeleteObject();

	// Free resource
	nodeBrush.Detach();
	nodeBrush.DeleteObject();

	nodePen.Detach();
	nodePen.DeleteObject();

	rgn.Detach();
	rgn.DeleteObject();

}

CWindowsPainter::CWindowsPainter()
{
}


CWindowsPainter::~CWindowsPainter()
{
}
