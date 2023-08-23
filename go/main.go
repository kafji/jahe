package main

import (
	"fmt"

	"kafji.net/jahe/win"
)

func main() {
	sysInfo := win.GetSystemInfo()
	fmt.Println(sysInfo)

	lpInfo := win.GetLogicalProcessorInformation()
	fmt.Println(lpInfo)

	logicalCores := map[win.ULONG_PTR]struct{}{}
	for _, lp := range lpInfo {
		if lp.Relationship == win.RelationProcessorCore {
			logicalCores[lp.ProcessorMask] = struct{}{}
		}
	}

	fmt.Printf("physical cores: %d\nlogical cores: %d\n", int(sysInfo.NumberOfProcessors)-len(logicalCores), len(logicalCores))
}
