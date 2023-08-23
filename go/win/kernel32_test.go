//go:build kfj && windows

package win

import (
	"testing"
	"unsafe"
)

func TestKernel32TypesSizes(t *testing.T) {
	if got, want := unsafe.Sizeof(SYSTEM_INFO{}), uintptr(48); got != want {
		t.Errorf("expecting size of SYSTEM_INFO to be %v, was %v", want, got)
	}

	if got, want := unsafe.Sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION{}), uintptr(32); got != want {
		t.Errorf("expecting size of SYSTEM_LOGICAL_PROCESSOR_INFORMATION to be %v, was %v", want, got)
	}

	if got, want := unsafe.Sizeof(LOGICAL_PROCESSOR_RELATIONSHIP(0)), uintptr(4); got != want {
		t.Errorf("expecting size of LOGICAL_PROCESSOR_RELATIONSHIP to be %v, was %v", want, got)
	}

	if got, want := unsafe.Sizeof(CACHE_DESCRIPTOR{}), uintptr(12); got != want {
		t.Errorf("expecting size of CACHE_DESCRIPTOR to be %v, was %v", want, got)
	}

	if got, want := unsafe.Sizeof(PROCESSOR_CACHE_TYPE(0)), uintptr(4); got != want {
		t.Errorf("expecting size of PROCESSOR_CACHE_TYPE to be %v, was %v", want, got)
	}
}

func TestGetSystemInfo(t *testing.T) {
	info := GetSystemInfo()

	if got, want := info.ProcessorArchitecture, uint16(9); got != want {
		t.Errorf("expecting ProcessorArchitecture to be %v, was %v", want, got)
	}

	if got, want := info.NumberOfProcessors, uint32(12); got != want {
		t.Errorf("expecting NumberOfProcessors to be %v, was %v", want, got)
	}
}

func TestGetLogicalProcessorInformation(t *testing.T) {
	data := GetLogicalProcessorInformation()

	cores := map[ULONG_PTR]struct{}{}
	for _, v := range data {
		if v.Relationship == RelationProcessorCore {
			cores[v.ProcessorMask] = struct{}{}
		}
	}

	if len(cores) != 6 {
		t.Errorf("expecting 6 logical cores, was %d", len(cores))
	}
}
