#pragma once

#include <windows.h>
#include <Shlobj.h>
#include <iostream>
#include <VersionHelpers.h>
#include <intrin.h>

#include "IPlatform.h"

#include "ICPUEx.h"

#include "IDeviceManager.h"
#include "IBIOSEx.h"

#include <tchar.h>

#include <process.h>

#include <shlobj.h>

#include <Lm.h>

#include <memory>

class BlobstoreClient
{
public:
  BlobstoreClient();
};

std::unique_ptr<BlobstoreClient> new_blobstore_client();

bool is_user_an_admin();

#define RM_SER_NAME _T("AMDRyzenMasterDriverV22")
#define AOD_DRIVER_NAME L"AMDRyzenMasterDriverV22"
#define DRIVER_FILE_PATH_64 L"bin\\AMDRyzenMasterDriver.sys"

bool is_supported_os();
bool authentic_amd();
int query_drv_service();
bool is_supported_processor();

#define LOG_PRINT(dRet, ch) (dRet == -1) ? _tprintf(_T(" NA\n")) : _tprintf(_T(" %0.1f %s\n"), dRet, ch);

#define LOG_PROCESS_ERROR(__CONDITION__) \
  do                                     \
  {                                      \
    if (!(__CONDITION__))                \
    {                                    \
      goto Exit0;                        \
    }                                    \
  } while (false)

void api_call();