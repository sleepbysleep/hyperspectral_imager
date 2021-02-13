#pragma once


// CTabHardware dialog

class CTabHardware : public CDialogEx
{
	DECLARE_DYNAMIC(CTabHardware)

public:
	CTabHardware(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTabHardware();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
