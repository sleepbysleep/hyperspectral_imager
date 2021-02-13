// CTabHardware.cpp : implementation file
//

#include "pch.h"
#include "HyperImager.h"
#include "CTabHardware.h"
#include "afxdialogex.h"


// CTabHardware dialog

IMPLEMENT_DYNAMIC(CTabHardware, CDialogEx)

CTabHardware::CTabHardware(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HW, pParent)
{

}

CTabHardware::~CTabHardware()
{
}

void CTabHardware::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabHardware, CDialogEx)
END_MESSAGE_MAP()


// CTabHardware message handlers
