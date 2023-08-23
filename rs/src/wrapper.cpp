#include "jahe/include/wrapper.hpp"

BlobstoreClient::BlobstoreClient() {}

std::unique_ptr<BlobstoreClient> new_blobstore_client()
{
    return std::unique_ptr<BlobstoreClient>(new BlobstoreClient());
}

bool is_user_an_admin()
{
    return IsUserAnAdmin();
}

bool is_supported_os()
{
    bool bIsSupported = false;
    DWORD major = 0;
    DWORD minor = 0;
    LPBYTE pinfoRawData;
    if (IsWindowsServer())
    {
        return false;
    }
    if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
    {
        WKSTA_INFO_100 *pworkstationInfo = (WKSTA_INFO_100 *)pinfoRawData;
        major = pworkstationInfo->wki100_ver_major;
        minor = pworkstationInfo->wki100_ver_minor;
        ::NetApiBufferFree(pinfoRawData);
    }

    if (major >= 10)
    {
        bIsSupported = true;
    }

    return bIsSupported;
}

bool authentic_amd()
{
    char CPUString[0x20];
    int CPUInfo[4] = {-1};
    unsigned nIds;
    char string[] = "AuthenticAMD";

    __cpuid(CPUInfo, 0);
    nIds = CPUInfo[0];
    memset(CPUString, 0, sizeof(CPUString));
    *((int *)CPUString) = CPUInfo[1];
    *((int *)(CPUString + 4)) = CPUInfo[3];
    *((int *)(CPUString + 8)) = CPUInfo[2];

    if (!strcmp(string, CPUString))
        return true;
    else
        return false;
}

int query_drv_service()
{
    SERVICE_STATUS ServiceStatus;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM)
        return -1;

    SC_HANDLE hOpenService = OpenService(hSCM, RM_SER_NAME, SC_MANAGER_ALL_ACCESS);
    if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
    {
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hSCM);
        return -1;
    }
    QueryServiceStatus(hOpenService, &ServiceStatus);
    if (ServiceStatus.dwCurrentState != SERVICE_RUNNING)
    {
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hSCM);
        return -1;
    }

    CloseServiceHandle(hOpenService);
    CloseServiceHandle(hSCM);
    return 0;
}

enum CPU_PackageType
{
    cptFP5 = 0,
    cptAM5 = 0,
    cptFP7 = 1,
    cptFL1 = 1,
    cptFP8 = 1,
    cptAM4 = 2,
    cptFP7r2 = 2,
    cptAM5_B0 = 3,
    cptSP3 = 4,
    cptFP7_B0 = 4,
    cptFP7R2_B0 = 5,
    cptSP3r2 = 7,
    cptUnknown = 0xF
};

bool is_supported_processor()
{
    bool retBool = false;
    int CPUInfo[4] = {-1};
    __cpuid(CPUInfo, 0x80000001);
    unsigned long uCPUID = CPUInfo[0];
    CPU_PackageType pkgType = (CPU_PackageType)(CPUInfo[1] >> 28);
    switch (pkgType)
    {
    case cptFP5:
        // case cptAM5:
        switch (uCPUID)
        {
        case 0x00810F80:
        case 0x00810F81:
        case 0x00860F00:
        case 0x00860F01:
        case 0x00A50F00:
        case 0x00A50F01:
        case 0x00860F81:
        case 0x00A60F00:
        case 0x00A60F01:
        case 0x00A60F10:
        case 0x00A60F11:
        case 0x00A60F12:
            retBool = true;
            break;
        default:
            break;
        }
        break;
    case cptAM4:
    case cptFP7R2_B0:
        switch (uCPUID)
        {
        case 0x00800F00:
        case 0x00800F10:
        case 0x00800F11:
        case 0x00800F12:

        case 0x00810F10:
        case 0x00810F11:

        case 0x00800F82:
        case 0x00800F83:

        case 0x00870F00:
        case 0x00870F10:

        case 0x00810F80:
        case 0x00810F81:

        case 0x00860F00:
        case 0x00860F01:

        case 0x00A20F00:
        case 0x00A20F10:
        case 0x00A20F12:

        case 0x00A50F00:
        case 0x00A50F01:

        // cptFP7r2
        case 0x00A40F00:
        case 0x00A40F40:
        case 0x00A40F41:

        case 0x00A70F00:
        case 0x00A70F40:
        case 0x00A70F41:
        case 0x00A70F42:
        case 0x00A70F80:
            retBool = true;
            break;
        default:
            break;
        }
        break;
    case cptSP3r2:
        switch (uCPUID)
        {
        case 0x00800F10:
        case 0x00800F11:
        case 0x00800F12:

        case 0x00800F82:
        case 0x00800F83:
        case 0x00830F00:
        case 0x00830F10:
            retBool = true;
            break;
        default:
            break;
        }
        break;
    case cptFP7:
    // case cptFL1:
    // case cptFP8:
    // case cptFP7_B0:
    case cptSP3:
        switch (uCPUID)
        {
        case 0x00A40F00:
        case 0x00A40F40:
        case 0x00A40F41:

        case 0x00A60F11:
        case 0x00A60F12:
            retBool = true;
            break;
        case 0x00A00F80:
        case 0x00A00F82:

        case 0x00A70F00:
        case 0x00A70F40:
        case 0x00A70F41:
        case 0x00A70F42:
        case 0x00A70F80:
            retBool = true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return retBool;
}

#define MAX_LENGTH 50

inline void printFunc(LPCTSTR func, BOOL bCore, int i = 0)
{
    if (!bCore)
    {
        _tprintf(_T("%s "), func);
        for (size_t j = 0; j < MAX_LENGTH - _tcslen(func); ++j)
        {
            _tprintf(_T("%c"), '.');
        }
    }
    else
    {
        _tprintf(_T("%s Core : %-2d"), func, i);
        for (size_t j = 0; j < MAX_LENGTH - _tcslen(func) - 9; ++j)
        {
            _tprintf(_T("%c"), '.');
        }
    }
}

TCHAR PrintErr[][63] = {_T("Failure"), _T("Success"), _T("Invalid value"), _T("Method is not implemented by the BIOS"), _T("Cores are already parked. First Enable all the cores"), _T("Unsupported Function")};

void api_call()
{
    bool bRetCode = false;

    IPlatform &rPlatform = GetPlatform();
    bRetCode = rPlatform.Init();
    if (!bRetCode)
    {
        _tprintf(_T("Platform init failed\n"));
        return;
    }
    IDeviceManager &rDeviceManager = rPlatform.GetIDeviceManager();
    ICPUEx *obj = (ICPUEx *)rDeviceManager.GetDevice(dtCPU, 0);
    IBIOSEx *objB = (IBIOSEx *)rDeviceManager.GetDevice(dtBIOS, 0);
    if (obj && objB)
    {
        CACHE_INFO result;
        unsigned long uResult = 0;
        unsigned int uCorePark = 0;
        unsigned int uCoreCount = 0;
        wchar_t package[30] = {0};
        PWCHAR wResult = NULL;
        PTCHAR tResult = NULL;
        WCHAR sDate[50] = {'\0'};
        WCHAR Year[5] = {'\0'};
        WCHAR Month[3] = {'\0'};
        WCHAR Day[3] = {'\0'};
        WCHAR ChipsetID[256] = {'\0'};
        CPUParameters stData;
        int iRet = 0;
        int i, j = 0;
        unsigned short uVDDIO = 0, uMemClock = 0;
        unsigned char uTcl = 0, uTcdrd = 0, uTras = 0, uTrp = 0;

        wResult = (PWCHAR)obj->GetName();
        printFunc(_T("GetName"), FALSE);
        wprintf(L" %s\n", wResult);
        wResult = (PWCHAR)obj->GetDescription();
        printFunc(_T("GetDescription"), FALSE);
        wprintf(L" %s\n", wResult);
        wResult = (PWCHAR)obj->GetVendor();
        printFunc(_T("GetVendor"), FALSE);
        wprintf(L" %s\n", wResult);
        tResult = (PTCHAR)obj->GetClassName();
        printFunc(_T("GetClassName"), FALSE);
        _tprintf(_T(" %s\n"), tResult);

        iRet = obj->GetCoreCount(uCoreCount);
        printFunc(_T("GetCoreCount"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %u\n"), uCoreCount);
        }

        iRet = obj->GetCorePark(uCorePark);
        printFunc(_T("GetCorePark"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %lu Cores parked\n"), uCorePark);
        }

        iRet = obj->GetL1DataCache(result);
        printFunc(_T("GetL1DataCache"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d x %.0fKB\n"), (uCoreCount - uCorePark), result.fSize);
        }

        result = {NULL};
        iRet = obj->GetL1InstructionCache(result);
        printFunc(_T("GetL1InstructionCache"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d x %.0fKB\n"), (uCoreCount - uCorePark), result.fSize);
        }

        result = {NULL};
        iRet = obj->GetL2Cache(result);
        printFunc(_T("GetL2Cache"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d x %.0fKB\n"), (uCoreCount - uCorePark), result.fSize);
        }

        result = {NULL};
        iRet = obj->GetL3Cache(result);
        printFunc(_T("GetL3Cache"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %0.f KB\n"), result.fSize);
        }

        printFunc(_T("GetPackage"), FALSE);
        wprintf(L" %s\n", obj->GetPackage());

        iRet = obj->GetCPUParameters(stData);
        printFunc(_T("GetCPUParameters"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" PPT Current Limit :"));
            LOG_PRINT(stData.fPPTLimit, _T("W"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" PPT Current Value :"));
            LOG_PRINT(stData.fPPTValue, _T("W"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" EDC(VDD) Current Limit :"));
            LOG_PRINT(stData.fEDCLimit_VDD, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" EDC(VDD) Current Value :"));
            LOG_PRINT(stData.fEDCValue_VDD, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" TDC(VDD) Current Limit :"));
            LOG_PRINT(stData.fTDCLimit_VDD, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" TDC(VDD) Current Value :"));
            LOG_PRINT(stData.fTDCValue_VDD, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" EDC(SOC) Current Limit :"));
            LOG_PRINT(stData.fEDCLimit_SOC, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" EDC(SOC) Current Value :"));
            LOG_PRINT(stData.fEDCValue_SOC, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" TDC(SOC) Current Limit :"));
            LOG_PRINT(stData.fTDCLimit_SOC, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" TDC(SOC) Current Value :"));
            LOG_PRINT(stData.fTDCValue_SOC, _T("A"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" cHTC Limit :"));
            LOG_PRINT(stData.fcHTCLimit, _T("Celsius"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" Fabric Clock Frequency :"));
            LOG_PRINT(stData.fFCLKP0Freq, _T("MHz"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" VDDCR(VDD) Power :"));
            LOG_PRINT(stData.fVDDCR_VDD_Power, _T("W"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" VDDCR(SOC) Power :"));
            LOG_PRINT(stData.fVDDCR_SOC_Power, _T("W"));

            printFunc(_T("GetCPUParameters"), FALSE);
            _tprintf(_T(" Fmax(CPU Clock) frequency :"));
            LOG_PRINT(stData.fCCLK_Fmax, _T("MHz"));

            printFunc(_T("GetCurrentOCMode"), FALSE);
            if (stData.eMode.uManual)
            {
                _tprintf(_T(" Manual Mode.\n"));
            }
            else if (stData.eMode.uPBOMode)
            {
                _tprintf(_T(" PBO Mode.\n"));
            }
            else if (stData.eMode.uAutoOverclocking)
            {
                _tprintf(_T(" Auto Overclocking Mode.\n"));
            }
            else if (stData.eMode.uEcoMode)
            {
                _tprintf(_T(" ECO Mode.\n"));
            }
            else
            {
                _tprintf(_T(" Default Mode.\n"));
            }

            for (i = 0; i < stData.stFreqData.uLength; i++)
            {
                if (stData.stFreqData.dFreq[i] != 0)
                {
                    printFunc(_T("GetEffectiveFrequency"), TRUE, j);
                    _tprintf(_T(" %0.1f MHz	C0 Residency...  %0.2f%% \n"), stData.stFreqData.dFreq[i], stData.stFreqData.dState[i]);
                    j++;
                }
            }

            printFunc(_T("GetPeakSpeed"), FALSE);
            _tprintf(_T(" %0.1f MHz\n"), stData.dPeakSpeed);
            printFunc(_T("GetPeakCore(s)Voltage"), FALSE);
            _tprintf(_T(" %0.3f V\n"), stData.dPeakCoreVoltage);
            printFunc(_T("GetAverageCoreVoltage"), FALSE);
            _tprintf(_T(" %0.3f V\n"), stData.dAvgCoreVoltage);
            printFunc(_T("GetVDDCR_SOC"), FALSE);
            _tprintf(_T(" %0.3f V\n"), stData.dSocVoltage);
            ;
            printFunc(_T("GetCurrentTemperature"), FALSE);
            _tprintf(_T(" %0.2f Celsius\n"), stData.dTemperature);
        }

        iRet = objB->GetMemVDDIO(uVDDIO);
        printFunc(_T("GetMemVDDIO"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d mV\n"), uVDDIO);
        }

        iRet = objB->GetCurrentMemClock(uMemClock);
        printFunc(_T("GetCurrentMemClock"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d MHz\n"), uMemClock);
        }

        iRet = objB->GetMemCtrlTcl(uTcl);
        printFunc(_T("GetMemCtrlTcl"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d Memory clock cycles\n"), uTcl);
        }

        iRet = objB->GetMemCtrlTrcdrd(uTcdrd);
        printFunc(_T("GetMemCtrlTrcdrd"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d Memory clock cycles\n"), uTcdrd);
        }

        iRet = objB->GetMemCtrlTras(uTras);
        printFunc(_T("GetMemCtrlTras"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d Memory clock cycles\n"), uTras);
        }

        iRet = objB->GetMemCtrlTrp(uTrp);
        printFunc(_T("GetMemCtrlTrp"), FALSE);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            _tprintf(_T(" %d Memory clock cycles\n"), uTrp);
        }

        printFunc(_T("GetBIOSInfo"), FALSE);
        wcscpy(sDate, objB->GetDate());
        wcsncpy(Year, sDate, 4);
        Year[4] = '\0';
        wcsncpy(Month, sDate + 4, 2);
        Month[2] = '\0';
        wcsncpy(Day, sDate + 6, 2);
        Day[2] = '\0';
        wprintf(L" Version : %s , Vendor : %s , Date : %s/%s/%s\n", objB->GetVersion(), objB->GetVendor(), Year, Month, Day);

        printFunc(_T("GetChipsetName"), FALSE);
        iRet = obj->GetChipsetName(ChipsetID);
        if (iRet)
        {
            _tprintf(_T(" %s\n"), PrintErr[iRet + 1]);
        }
        else
        {
            wprintf(L" %s\n", ChipsetID);
        }
    }
    rPlatform.UnInit();
}
