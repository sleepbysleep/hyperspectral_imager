/*
 Copyright 2011 Hoyoung Lee.

 This file is part of ENVIewer.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, please visit www.gnu.org.
*/

#include "pch.h"
#include "Graph.h"
#include "MemDC.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraph

CGraph::CGraph()
{
	m_BackColor = RGB(0,0,0);
	m_AxisColor = RGB(255,255,255);
	m_TextColor = RGB(255,255,255);
	m_GridColor = RGB(128,128,128);
	m_TickFontSize = 12;

	m_XAxisMin = 0;
	m_XAxisMax = 512;
	m_XAxisDelta = 64;
	m_YAxisMin = -10;
	m_YAxisMax = 10;
	m_YAxisDelta = 2;

	m_AutoScale = false;

	memset(&m_Object, 0, sizeof(m_Object));
	m_ObjectCount = 0;

	m_Object.previous = NULL;
	m_Object.next = NULL;
	m_Object.count = 0;

	m_NumXAxisLabels = 0;
	m_XAxisLabels = NULL;
}

CGraph::~CGraph()
{
	removeAll();
}

void CGraph::removeAll()
{
	struct graph_object *iterator, *next;

	for (iterator = m_Object.next; iterator != NULL; iterator = next) {
		delete [] iterator->data;
		next = iterator->next;
		delete iterator;
	}
	m_Object.next = NULL;
	m_ObjectCount = 0;
}

//template<class Type>
void CGraph::addData(COLORREF color, double *data, int count)
{
	struct graph_object *next, *obj;

	obj = new struct graph_object;

	next = m_Object.next;
	m_Object.next = obj;
	if (next) next->previous = obj;
	obj->next = next;
	obj->previous = &m_Object;

	obj->data = new double [count];
	obj->color = color;
	obj->count = count;

	for (int i = 0; i < count; i++)
		obj->data[i] = data[i];

	m_ObjectCount++;
}

//template<class Type>
void CGraph::replaceData(COLORREF color, double *data, int count)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color) {
			if (iterator->count != count) {
				delete [] iterator->data;
				iterator->data = new double [count];
			}
			for (int i = 0; i < count; i++)
				iterator->data[i] = data[i];
			break;
		}
	}
}

void CGraph::removeData(COLORREF color)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color) {
			iterator->previous->next = iterator->next;
			if (iterator->next) iterator->next->previous = iterator->previous;
			delete [] iterator->data;
			delete iterator;
			m_ObjectCount--;
			break;
		}
	}
}

bool CGraph::getValue(double *val, COLORREF color, int index)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color) {
			if (index < iterator->count) {
				*val = iterator->data[index];
				return true;
			}
		}
	}
	return false;
}

int CGraph::getData(double *data, int count, COLORREF color)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color) {
			for (int i = 0; i < min(count, iterator->count); i++) {
				data[i] = iterator->data[i];
			}
			return min(count, iterator->count);
		}
	}
	return 0;
}

int CGraph::getDataLength(COLORREF color)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color)
			return iterator->count;
	}
	return 0;
}

bool CGraph::isDataExist(COLORREF color)
{
	struct graph_object *iterator;

	for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
		if (color == iterator->color)
			return true;
	}
	return false;
}

void CGraph::readObjectExtrema(double *minval, double *maxval, struct graph_object *object)
{
	*minval = *maxval = object->data[0];
	for (int i = 1; i < object->count; i++) {
		if (object->data[i] < *minval) *minval = object->data[i];
		if (object->data[i] > *maxval) *maxval = object->data[i];
	}
}
/*
void CGraph::setXTickLabel(TCHAR **label, int n)
{
	int i;

	for (i = 0; i < m_NumXAxisLabels; i++) delete [] m_XAxisLabels[i];
	delete [] m_XAxisLabels;


	m_XAxisLabels = new TCHAR *[n];
	for (i = 0; i < n; i++) {
		m_XAxisLabels[i] = _tcsdup(label[i]);
	}
	m_NumXAxisLabels = n;
}
*/
void CGraph::setXTickLabel(char **label, int n)
{
	int i;

	for (i = 0; i < m_NumXAxisLabels; i++) delete [] m_XAxisLabels[i];
	delete [] m_XAxisLabels;

	m_XAxisLabels = new TCHAR *[n];
	for (i = 0; i < n; i++) {
		CA2T temp(label[i]);
		m_XAxisLabels[i] = _tcsdup(temp);
	}
	m_NumXAxisLabels = n;
}

void CGraph::drawGraph(CDC* pDC)
{
	CRect rect;
	CWnd *pWnd;

	pWnd = pDC->GetWindow();
	pWnd->GetClientRect(&rect);
	CodeProject::CMemDC memDC(pDC, &rect);

	CFont tickFont, yFont, xFont, *pOldFont = NULL;

	tickFont.CreateFont(m_TickFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, TEXT("Arial"));

	yFont.CreateFont(m_TickFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, TEXT("Arial"));

	xFont.CreateFont(m_TickFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, TEXT("Arial"));

	CPen axisPen(PS_SOLID, 1, m_AxisColor);
	CPen gridPen(PS_SOLID, 1, m_GridColor);
	CPen whitePen(PS_SOLID, 1, RGB(255, 255, 255));

	// Draw background with CBrush
	CBrush backBrush(m_BackColor);
	CBrush *pOldBackBrush = memDC.SelectObject(&backBrush);

	memDC.Rectangle(0, 0, rect.Width(), rect.Height());
	memDC.SelectObject(pOldBackBrush);

	// Get proper Font size
	TEXTMETRIC tm;
	int tickCharWidth, tickCharHeight;

	pOldFont = (CFont*)memDC.SelectObject(&tickFont);

	memDC.GetTextMetrics(&tm);
	tickCharWidth = tm.tmAveCharWidth; // * (int)log10((float)m_YCoordMax);
	tickCharHeight = tm.tmHeight;
//  memDC.SelectObject(pOldFont);

	//determine axis specifications
	int xApexPoint, yApexPoint, xAxisLength, yAxisLength;

#define XMARGIN (5)
#define YMARGIN (5)
#define XGAP (5)
#define YGAP (5)

	xApexPoint = XMARGIN + XGAP + ((int)log10((float)m_YAxisMax)*tickCharWidth) + XGAP + 2; //labelHeight added for y-axis label height
	yApexPoint = rect.Height() - YMARGIN - YGAP - tickCharHeight - YGAP - 2;

	if (m_XAxisLabels) {
		xAxisLength = rect.Width() - XMARGIN - (_tcslen(m_XAxisLabels[m_NumXAxisLabels-1])*tickCharWidth) - xApexPoint;
	} else {
		xAxisLength = rect.Width() - XMARGIN - ((int)log10((float)m_XAxisMax)*tickCharWidth) - xApexPoint;
	}
	yAxisLength = yApexPoint - YMARGIN - tickCharHeight;

	CPen *pOldPen = memDC.SelectObject(&axisPen);

	//draw y axis
	memDC.MoveTo(xApexPoint, yApexPoint);
	memDC.LineTo(xApexPoint, yApexPoint - yAxisLength);

	//draw x axis
	//memDC.MoveTo(xApexPoint, yApexPoint);
	//memDC.LineTo(xApexPoint + xAxisLength, yApexPoint);
	//memDC.SelectObject(pOldPen);

	double yMin, yMax;

	if (m_AutoScale) {
		double minval, maxval;
		struct graph_object *iterator;

		yMin = m_YAxisMax;
		yMax = m_YAxisMin;
		for (iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
			readObjectExtrema(&minval, &maxval, iterator);
			if (minval < yMin) yMin = minval;
			if (maxval > yMax) yMax = maxval;
		}
		if (yMin > yMax) { minval = yMin; yMin = yMax; yMax = minval; }

	} else {
		yMin = m_YAxisMin;
		yMax = m_YAxisMax;
	}

	//draw y tick and grid
	int oldMode = memDC.SetBkMode(TRANSPARENT);

	COLORREF oldColor = memDC.SetTextColor(m_TextColor);
	memDC.SelectObject(&yFont);

	CString tickLabel;
	int yTick, topYTick;

	int numTicks = (int)((double)(m_YAxisMax - m_YAxisMin) / (double)m_YAxisDelta);
	//int numTicks = (int)((double)(yMax - yMin) / (double)m_YAxisDelta);
	double deltaTick = (double)(yMax - yMin) / (double)numTicks;
	double scaleTick = (double)yAxisLength / (double)numTicks;

	for (int i = 0; i <= numTicks; i++) {
		yTick = (int)((double)yApexPoint - (double)i * scaleTick);

		if (1/*graphHasGridLines*/) {
			//draw grid lines
			//pOldPen = memDC.SelectObject(&gridPen);
			memDC.SelectObject(&gridPen);
			memDC.MoveTo(xApexPoint, yTick);
			memDC.LineTo(xApexPoint + xAxisLength, yTick);
			//memDC.SelectObject(pOldPen);
		}

		//draw tick mark
		//pOldPen = memDC.SelectObject(&whitePen);
		memDC.SelectObject(&whitePen);
		memDC.MoveTo(xApexPoint - 2, yTick);
		memDC.LineTo(xApexPoint + 3, yTick);
		//memDC.SelectObject(pOldPen);

		//draw tick label
		tickLabel.Format(_T("%d"), (int)((double)yMin + (double)i * deltaTick));
		memDC.TextOut(xApexPoint - 3 - XGAP - tickLabel.GetLength() * tickCharWidth, yTick - tickCharHeight/2, tickLabel);
		//memDC.SelectObject(pOldFont);
		topYTick = yTick;
	}

	// draw x axis and grid
	int xTick, rightXTick;

	numTicks = (m_XAxisMax - m_XAxisMin) / m_XAxisDelta;
	deltaTick = (double)(m_XAxisMax - m_XAxisMin) / (double)numTicks;
	scaleTick = (double)xAxisLength / (double)numTicks;
	
	memDC.SelectObject(&xFont);
	for (int i = 0; i <= numTicks; i++) {
		xTick = (int)((double)xApexPoint + (double)i * scaleTick);

		//draw tick mark
		memDC.SelectObject(&whitePen);
		memDC.MoveTo(xTick, yApexPoint - 2);
		memDC.LineTo(xTick, yApexPoint + 3);
		//memDC.SelectObject(pOldPen);

		//draw tick label
		if (m_XAxisLabels) {
			tickLabel.Format(_T("%s"), m_XAxisLabels[(int)(i * deltaTick)]);
			memDC.TextOut(xTick - (tickLabel.GetLength() * tickCharWidth)/2, yApexPoint + 2 + YGAP, tickLabel);
		} else {
			tickLabel.Format(_T("%d"), (int)((double)m_XAxisMin + (double)i * deltaTick));
			//memDC.SetBkMode(TRANSPARENT);
			//memDC.SetTextColor(m_TextColor);
			memDC.TextOut(xTick - (tickLabel.GetLength() * tickCharWidth)/2, yApexPoint + 2 + YGAP, tickLabel);
			//memDC.SelectObject(pOldFont);
		}
		rightXTick = xTick;
	}

	int xValue, yValue;
	double yAxisScale, xAxisScale;
  
	yAxisScale = (double)(yApexPoint - topYTick) / (double)(yMax - yMin); //(double)(m_YAxisMax - m_YAxisMin);
	xAxisScale = (double)(rightXTick - xApexPoint) / (double)(m_XAxisMax - m_XAxisMin);

	if (xAxisScale >= 1.0) {
		for (struct graph_object *iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
			CPen linePen(PS_SOLID, 1, iterator->color);
			memDC.SelectObject(&linePen);
			for (int i = 0; i < min(m_XAxisMax - m_XAxisMin + 1, iterator->count); i++) {
				yValue = iterator->data[i] < m_YAxisMax ? iterator->data[i] : m_YAxisMax;
				yValue = yValue < m_YAxisMin ? m_YAxisMin : yValue;
				xTick = (int)((double)xApexPoint + (double)i * xAxisScale);
				yTick = (int)((double)yApexPoint - ((double)yValue - (double)yMin /*(double)m_YAxisMin*/) * yAxisScale);
				//draw ellipse...
				if (i == 0) memDC.MoveTo(xTick, yTick);
				else memDC.LineTo(xTick, yTick);
			}
		}
	} else {
		for (struct graph_object *iterator = m_Object.next; iterator != NULL; iterator = iterator->next) {
			CPen linePen(PS_SOLID, 1, iterator->color);
			memDC.SelectObject(&linePen);
			for (xTick = xApexPoint; xTick < rightXTick; xTick++) {
				int i = (int)((double)(xTick - xApexPoint) / xAxisScale);
				if (i >= (m_XAxisMax - m_XAxisMin) || i >= iterator->count) break;
				yValue = iterator->data[i] < m_YAxisMax ? iterator->data[i] : m_YAxisMax;
				yValue = yValue < m_YAxisMin ? m_YAxisMin : yValue;

				yTick = yApexPoint - (int)((double)(yValue - yMin /* m_YAxisMin */) * yAxisScale);
#if 1
				if (xTick == xApexPoint) memDC.MoveTo(xTick, yTick);
				else memDC.LineTo(xTick, yTick);
#else
				memDC.SetPixel(CPoint(xTick, yTick), iterator->color);
#endif
			}
		}
	}
//error_return:
	memDC.SelectObject(pOldFont);
	memDC.SetBkMode(oldMode);
	memDC.SelectObject(pOldPen);
	memDC.SetTextColor(oldColor);
	//delete memDC;
}

BEGIN_MESSAGE_MAP(CGraph, CStatic)
	//{{AFX_MSG_MAP(CGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraph message handlers
