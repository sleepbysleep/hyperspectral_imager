// CTabHardware.cpp : implementation file
//

#pragma warning(suppress : 4996)

#include "pch.h"
#include "HyperImager.h"
#include "CTabHardware.h"
#include "afxdialogex.h"

#include <Thorlabs.MotionControl.IntegratedStepperMotors.h>

// CTabHardware dialog

IMPLEMENT_DYNAMIC(CTabHardware, CDialogEx)

CTabHardware::CTabHardware(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HW, pParent)
{

}

CTabHardware::~CTabHardware()
{
    if (this->linearStageOpened) {
        // stop polling
        ISC_StopPolling(this->serialNo);
        // close device
        ISC_Close(this->serialNo);
    }
}

void CTabHardware::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_STAGE, linearStageList);
}


BEGIN_MESSAGE_MAP(CTabHardware, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON1, &CTabHardware::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CTabHardware::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CTabHardware::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTabHardware message handlers


BOOL CTabHardware::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    if (TLI_BuildDeviceList() == 0) {
        // get device list size 
        short n = TLI_GetDeviceListSize();
        // get LTS serial numbers
        char serialNos[100];
        TLI_GetDeviceListByTypeExt(serialNos, 100, 45);

        // output list of matching devices
        {
            char* searchContext = nullptr;
            char* p = strtok_s(serialNos, ",", &searchContext);

            while (p != nullptr) {
                TLI_DeviceInfo deviceInfo;
                // get device info from device
                TLI_GetDeviceInfo(p, &deviceInfo);
                // get strings from device info structure
                char desc[65];
                strncpy_s(desc, deviceInfo.description, 64);
                desc[64] = '\0';
                char serialNo[9];
                //this->serialNo = strtol(serialNo, NULL);
                strncpy_s(serialNo, deviceInfo.serialNo, 8);
                serialNo[8] = '\0';
                // output
                wchar_t wcs_p[100], wcs_desc[65], wcs_serialNo[9];
                mbstowcs(wcs_p, p, 100);
                mbstowcs(wcs_desc, desc, 65);
                mbstowcs(wcs_serialNo, serialNo, 9);
                this->linearStageList.AddString(wcs_serialNo);

                p = strtok_s(nullptr, ",", &searchContext);
            }
        }
        this->linearStageList.EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
    } else {
        MessageBox(_T("No linear stage found!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

// Button handler for device opeinging of linear stage
void CTabHardware::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here
    // Scanning the devices of linear stage on the system
    if (this->linearStageList.GetCount() == 0) {
        MessageBox(_T("No linear stage found!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
        return;
    }

    // Get ther serial number of selected linear stage
    CString s;
    this->linearStageList.GetLBText(this->linearStageList.GetCurSel(), s);
    wcstombs(this->serialNo, s.GetBuffer(), 9);

    // open device
    if (ISC_Open(this->serialNo) == 0) {
        this->linearStageList.EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);

        // start the device polling at 200ms intervals
        ISC_StartPolling(this->serialNo, POLLING_MS);
        Sleep(POLLING_MS);

        ISC_LoadSettings(this->serialNo);
        Sleep(1000);

        ISC_RequestPosition(this->serialNo);
        Sleep(POLLING_MS);

        CString s1, s2;

        int pos = ISC_GetPosition(this->serialNo);
        {
            double minPos, maxPos;
            ISC_GetMotorTravelLimits(this->serialNo, &minPos, &maxPos);
            s1.Format(_T("TravelLimit: %f <--- %d ---> %f"), minPos, pos, maxPos);
        }

        {
            int maxPos = ISC_GetStageAxisMaxPos(this->serialNo);
            int minPos = ISC_GetStageAxisMinPos(this->serialNo);
            s2.Format(_T("StageAxisMinMax: %d <--- %d ---> %d"), minPos, pos, maxPos);
        }
        GetDlgItem(IDC_STATIC_STAGE)->SetWindowTextW(s1+_T("\n")+s2);


        // Show massage to confirm selected device has been Opened
        /*
        CString vs;
        WORD messageType;
        WORD messageId;
        DWORD messageData;

        wchar_t wcs_serialNo[9];
        mbstowcs(wcs_serialNo, serialNo, 9);
        vs.Format(_T("Device = '%s', is Opened..."), wcs_serialNo);
        GetDlgItem(IDC_STATIC_STAGE)->SetWindowTextW(vs);

        Sleep(500);
        vs.Format(_T("Homing device = '%s', Please Wait..."), wcs_serialNo);
        GetDlgItem(IDC_STATIC_STAGE)->SetWindowTextW(vs);

        ISC_ClearMessageQueue(this->serialNo);
        ISC_Home(this->serialNo);

        // wait for homing
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        while (messageType != 2 || messageId != 0)
        {
            ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        }

        vs.Format(_T("Device = '%s', is Homed Successfully"), wcs_serialNo);
        GetDlgItem(IDC_STATIC_STAGE)->SetWindowTextW(vs);
        */
        //

        // Setup the parameter of linear stage
        unsigned long home_velocity = ISC_GetHomingVelocity(this->serialNo);
        s.Format(_T("%d"), home_velocity);
        GetDlgItem(IDC_EDIT_HOME_V)->SetWindowTextW(s);

        // TODO: uncomment to set up the homing velocity
        //ISC_SetHomingVelocity(this->serialNo, 43980465);

        // 
        int maxVelocity, currentAcceleration;
        ISC_GetVelParams(this->serialNo, &currentAcceleration, &maxVelocity);
        s.Format(_T("%d"), currentAcceleration);
        GetDlgItem(IDC_EDIT_ACC)->SetWindowTextW(s);
        s.Format(_T("%d"), maxVelocity);
        GetDlgItem(IDC_EDIT_VELOCITY)->SetWindowTextW(s);

        
        GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
        this->linearStageOpened = true;

#if 0
        // Home device
        ISC_ClearMessageQueue(this->serialNo);
        ISC_Home(this->serialNo);
        printf("Device %s homing\r\n", this->serialNo);

        // wait for completion
        WORD messageType;
        WORD messageId;
        DWORD messageData;
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        while (messageType != 2 || messageId != 0) {
            ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        }

        // set velocity if desired
        if (velocity > 0) {
            int currentVelocity, currentAcceleration;
            ISC_GetVelParams(this->serialNo, &currentAcceleration, &currentVelocity);
            ISC_SetVelParams(this->serialNo, currentAcceleration, velocity);
        }

        // move to position (channel 1)
        ISC_ClearMessageQueue(this->serialNo);
        ISC_MoveToPosition(this->serialNo, position);
        printf("Device %s moving\r\n", this->serialNo);

        // wait for completion
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        while (messageType != 2 || messageId != 1)
        {
            ISC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
        }

        // get actual position
        int pos = ISC_GetPosition(this->serialNo);
        printf("Device %s moved to %d\r\n", this->serialNo, pos);

        // stop polling
        ISC_StopPolling(this->serialNo);
        // close device
        ISC_Close(this->serialNo);
#endif
    } else {
        MessageBox(_T("Linear stage fails to open!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
        return;
    }

}

// Button Handler for Homing the linear stage
void CTabHardware::OnBnClickedButton2()
{
    // TODO: Add your control notification handler code here
    if (!this->linearStageOpened) return;
    if (!ISC_CanHome(this->serialNo)) return;

    // Home device
    ISC_ClearMessageQueue(this->serialNo);
    ISC_Home(this->serialNo);
    //printf("Device %s homing\r\n", this->serialNo);

    // wait for completion
    WORD messageType;
    WORD messageId;
    DWORD messageData;
    ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
    while (messageType != 2 || messageId != 0) {
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
    }
}


void CTabHardware::OnBnClickedButton3()
{
    // TODO: Add your control notification handler code here
}

