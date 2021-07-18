#pragma once
class CWindowsPainter
{
public:


	static CWindowsPainter* GetPainter();
	static void KillPainter();
	static CWindowsPainter*  m_pWindowsPainter;
	
	void DrawBackColor(CDC *pDC);
	void MarkNode(CDC *pDC, int X = 0, int Y = 0, COLORREF color = RGB(125, 125, 125));
protected:
	CWindowsPainter();
	~CWindowsPainter();

	CBrush		m_BackGroundBrush;	//(m_bkcolor);
	CPen		m_GridPen;
//	COLORREF	m_bkcolor;			// back color
	COLORREF	m_gridcolor;		// back color
};

