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

#if !defined(AFX_GRAPH_H__5F764DF2_10AA_47D5_9707_E79B37FFFE47__INCLUDED_)
#define AFX_GRAPH_H__5F764DF2_10AA_47D5_9707_E79B37FFFE47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Graph.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraph window

class CGraph : public CStatic
{
// Construction
public:
	CGraph();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraph)
	//}}AFX_VIRTUAL

// Implementation
public:
//template<class Type>
	void addData(COLORREF color, double *data, int count);
//template<class Type>
	void replaceData(COLORREF color, double *data, int count);
	void removeData(COLORREF color);
	void removeAll();
	bool isDataExist(COLORREF color);
	int getYTickMax() { return m_YAxisMax; }
	int getDataCount() { return m_ObjectCount; }
	int getData(double *data, int count, COLORREF color);
	bool getValue(double *val, COLORREF color, int index);
	int getDataLength(COLORREF color);
	int getXAxisMin() { return m_XAxisMin; }
	int getXAxisMax() { return m_XAxisMax; }
	int getYAxisMin() { return m_YAxisMin; }
	int getYAxisMax() { return m_YAxisMax; }
	void setTickFontSize(int size) { m_TickFontSize = size; }
	void setTextColor(COLORREF color) { m_TextColor = color; }
	void setGridColor(COLORREF color) { m_GridColor = color; }
	void setAxisColor(COLORREF color) { m_AxisColor = color; }
	void setBackColor(COLORREF color) { m_BackColor = color; }
	void setYTickLimit(int minTick, int maxTick, int stepTick) { m_YAxisMin = minTick; m_YAxisMax = maxTick; m_YAxisDelta = stepTick; }
	void setXTickLimit(int minTick, int maxTick, int stepTick) { m_XAxisMin = minTick; m_XAxisMax = maxTick; m_XAxisDelta = stepTick; }
	void setAutoScale(bool enable) { m_AutoScale = enable; }
	//void setXTickLabel(TCHAR **label, int n);
	void setXTickLabel(char **label, int n);
	void drawGraph(CDC* pDC);

	virtual ~CGraph();

	// Generated message map functions
protected:
	struct graph_object {
		struct graph_object *previous;
		struct graph_object *next;
		int count;
		double *data;
		COLORREF color;
		//string *
	};
	struct graph_object m_Object;
	int m_ObjectCount;

	COLORREF m_TextColor;
	COLORREF m_BackColor;
	COLORREF m_AxisColor;
	COLORREF m_GridColor;
	bool m_AutoScale;
	int m_TickFontSize;
	int m_XAxisMin, m_XAxisMax, m_XAxisDelta;
	int m_YAxisMin, m_YAxisMax, m_YAxisDelta;

	void readObjectExtrema(double *minval, double *maxval, struct graph_object *object);

	int m_NumXAxisLabels;
	TCHAR **m_XAxisLabels;

	//{{AFX_MSG(CGraph)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPH_H__5F764DF2_10AA_47D5_9707_E79B37FFFE47__INCLUDED_)
