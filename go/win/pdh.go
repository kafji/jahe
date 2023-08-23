package win

import (
	"golang.org/x/sys/windows"
)

var (
	dllPDH *windows.LazyDLL = windows.NewLazySystemDLL("pdh.dll")

	procOpenQuery *windows.LazyProc = dllPDH.NewProc("PdhOpenQuery")
)

// https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhopenqueryw
func PdhOpenQuery(
	szDataSource uintptr,
	dwUserData uintptr,
	phQuery HANDLE,
) {
	procOpenQuery.Call(
		szDataSource, dwUserData, uintptr(phQuery),
	)
}
