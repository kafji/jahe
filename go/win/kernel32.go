package win

import (
	"fmt"
	"strings"
	"unsafe"

	"golang.org/x/sys/windows"
)

var (
	dllKernel32 *windows.LazyDLL = windows.NewLazySystemDLL("kernel32.dll")

	procGetSystemInfo                  *windows.LazyProc = dllKernel32.NewProc("GetSystemInfo")
	procGetLogicalProcessorInformation                   = dllKernel32.NewProc("GetLogicalProcessorInformation")
)

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo
func GetSystemInfo() SYSTEM_INFO {
	systemInfo := SYSTEM_INFO{}
	procGetSystemInfo.Call(uintptr(unsafe.Pointer(&systemInfo)))
	return systemInfo
}

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
type SYSTEM_INFO struct {
	ProcessorArchitecture     WORD
	_reserved                 WORD // Reserved.
	PageSize                  DWORD
	MinimumApplicationAddress uintptr
	MaximumApplicationAddress uintptr
	ActiveProcessorMask       DWORD_PTR
	NumberOfProcessors        DWORD
	_processorType            DWORD // Obsolete.
	AllocationGranularity     DWORD
	ProcessorLevel            WORD
	ProcessorRevision         WORD
}

func (i SYSTEM_INFO) String() string {
	b := new(strings.Builder)

	b.WriteRune('{')

	b.WriteString("ProcessorArchitecture: ")
	{
		var name, meaning string
		value := i.ProcessorArchitecture
		switch value {
		case PROCESSOR_ARCHITECTURE_AMD64:
			name, meaning = "PROCESSOR_ARCHITECTURE_AMD64", "x64 (AMD or Intel)"
		case PROCESSOR_ARCHITECTURE_ARM:
			name, meaning = "PROCESSOR_ARCHITECTURE_ARM", "ARM"
		case PROCESSOR_ARCHITECTURE_ARM64:
			name, meaning = "PROCESSOR_ARCHITECTURE_ARM64", "ARM64"
		case PROCESSOR_ARCHITECTURE_IA64:
			name, meaning = "PROCESSOR_ARCHITECTURE_IA64", "Intel Itanium-based"
		case PROCESSOR_ARCHITECTURE_INTEL:
			name, meaning = "PROCESSOR_ARCHITECTURE_INTEL", "x86"
		case PROCESSOR_ARCHITECTURE_UNKNOWN:
			name, meaning = "PROCESSOR_ARCHITECTURE_UNKNOWN", "Unknown architecture."
		default:
			err := fmt.Sprintf("unexpected value for ProcessorArchitecture, was %v", value)
			panic(err)
		}
		fmt.Fprintf(b, "%s(%s(%d))", meaning, name, value)
	}

	b.WriteString(", ")
	b.WriteString("PageSize: ")
	fmt.Fprint(b, i.PageSize)

	b.WriteString(", ")
	b.WriteString("MinimumApplicationAddress: ")
	fmt.Fprint(b, i.MinimumApplicationAddress)

	b.WriteString(", ")
	b.WriteString("MaximumApplicationAddress: ")
	fmt.Fprint(b, i.MaximumApplicationAddress)

	b.WriteString(", ")
	b.WriteString("ActiveProcessorMask: ")
	fmt.Fprint(b, i.ActiveProcessorMask)

	b.WriteString(", ")
	b.WriteString("NumberOfProcessors: ")
	fmt.Fprint(b, i.NumberOfProcessors)

	b.WriteString(", ")
	b.WriteString("AllocationGranularity: ")
	fmt.Fprint(b, i.AllocationGranularity)

	b.WriteString(", ")
	b.WriteString("ProcessorLevel: ")
	fmt.Fprint(b, i.ProcessorLevel)

	b.WriteString(", ")
	b.WriteString("ProcessorRevision: ")
	fmt.Fprint(b, i.ProcessorRevision)

	b.WriteRune('}')

	return b.String()
}

const (
	PROCESSOR_ARCHITECTURE_AMD64   WORD = 9
	PROCESSOR_ARCHITECTURE_ARM     WORD = 5
	PROCESSOR_ARCHITECTURE_ARM64   WORD = 12
	PROCESSOR_ARCHITECTURE_IA64    WORD = 6
	PROCESSOR_ARCHITECTURE_INTEL   WORD = 0
	PROCESSOR_ARCHITECTURE_UNKNOWN WORD = 0xffff
)

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
func GetLogicalProcessorInformation() []SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
	data := ([]SYSTEM_LOGICAL_PROCESSOR_INFORMATION)(nil)
	returnLength := DWORD(0)

	for {
		var buffer unsafe.Pointer
		if data != nil {
			buffer = unsafe.Pointer(&data[0])
		}
		r1, _, err := procGetLogicalProcessorInformation.Call(
			uintptr(buffer),
			uintptr(unsafe.Pointer(&returnLength)),
		)
		if r1 == 1 {
			return data
		} else {
			switch err {
			case windows.ERROR_INSUFFICIENT_BUFFER:
				size := unsafe.Sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION{})
				length := int(returnLength) / int(size)
				data = append(data, make([]SYSTEM_LOGICAL_PROCESSOR_INFORMATION, length-len(data))...)
			default:
				err := fmt.Errorf("error: %v", err)
				panic(err)
			}
		}
	}
}

// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-system_logical_processor_information
type SYSTEM_LOGICAL_PROCESSOR_INFORMATION struct {
	ProcessorMask ULONG_PTR
	Relationship  LOGICAL_PROCESSOR_RELATIONSHIP
	union0        [16]byte
}

func (i SYSTEM_LOGICAL_PROCESSOR_INFORMATION) ProcessorCoreFlags() BYTE {
	return BYTE(i.union0[0])
}

func (i SYSTEM_LOGICAL_PROCESSOR_INFORMATION) NumaNodeNodeNumber() DWORD {
	return DWORD(i.union0[0])<<8 | DWORD(i.union0[1])
}

func (i SYSTEM_LOGICAL_PROCESSOR_INFORMATION) Cache() CACHE_DESCRIPTOR {
	level := i.union0[0]
	associativity := i.union0[1]
	lineSize := WORD(i.union0[2])<<8 | WORD(i.union0[3])
	size := DWORD(i.union0[4])<<24 | DWORD(i.union0[5])<<16 | DWORD(i.union0[6])<<8 | DWORD(i.union0[7])
	type_ := PROCESSOR_CACHE_TYPE(i.union0[8])
	return CACHE_DESCRIPTOR{Level: level, Associativity: associativity, LineSize: lineSize, Size: size, Type: type_}
}

func (i SYSTEM_LOGICAL_PROCESSOR_INFORMATION) String() string {
	b := new(strings.Builder)

	b.WriteRune('{')

	b.WriteString("ProcessorMask: ")
	fmt.Fprint(b, i.ProcessorMask)

	b.WriteString(", ")
	b.WriteString("Relationship: ")
	fmt.Fprint(b, i.Relationship)

	b.WriteString(", ")
	switch i.Relationship {
	case RelationProcessorCore:
		b.WriteString("ProcessorCoreFlags: ")
		fmt.Fprint(b, i.ProcessorCoreFlags())
	case RelationNumaNode:
		b.WriteString("NumaNodeNodeNumber: ")
		fmt.Fprint(b, i.NumaNodeNodeNumber())
	case RelationCache:
		b.WriteString("Cache: ")
		fmt.Fprint(b, i.Cache())
	}

	b.WriteRune('}')

	return b.String()
}

// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ne-winnt-logical_processor_relationship
type LOGICAL_PROCESSOR_RELATIONSHIP uint32

const (
	RelationProcessorCore LOGICAL_PROCESSOR_RELATIONSHIP = iota
	RelationNumaNode
	RelationCache
	RelationProcessorPackage
	RelationGroup
	RelationProcessorDie
	RelationNumaNodeEx
	RelationProcessorModule
	RelationAll = 0xffff
)

func (r LOGICAL_PROCESSOR_RELATIONSHIP) String() string {
	var name string
	switch r {
	case RelationProcessorCore:
		name = "RelationProcessorCore"
	case RelationNumaNode:
		name = "RelationNumaNode"
	case RelationCache:
		name = "RelationCache"
	case RelationProcessorPackage:
		name = "RelationProcessorPackage"
	case RelationGroup:
		name = "RelationGroup"
	case RelationProcessorDie:
		name = "RelationProcessorDie"
	case RelationNumaNodeEx:
		name = "RelationNumaNodeEx"
	case RelationProcessorModule:
		name = "RelationProcessorModule"
	case RelationAll:
		name = "RelationAll"
	default:
		err := fmt.Sprintf("unexpected value, was %d", r)
		panic(err)
	}
	return fmt.Sprintf("%s(%d)", name, r)
}

// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-cache_descriptor
type CACHE_DESCRIPTOR struct {
	Level         BYTE
	Associativity BYTE
	LineSize      WORD
	Size          DWORD
	Type          PROCESSOR_CACHE_TYPE
}

func (d CACHE_DESCRIPTOR) String() string {
	b := new(strings.Builder)

	b.WriteRune('{')

	b.WriteString("Level: ")
	fmt.Fprint(b, d.Level)

	b.WriteString(", ")
	b.WriteString("Associativity: ")
	fmt.Fprint(b, d.Associativity)

	b.WriteString(", ")
	b.WriteString("LineSize: ")
	fmt.Fprint(b, d.LineSize)

	b.WriteString(", ")
	b.WriteString("Size: ")
	fmt.Fprint(b, d.Size)

	b.WriteString(", ")
	b.WriteString("Type: ")
	fmt.Fprint(b, d.Type)

	b.WriteRune('}')

	return b.String()
}

// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ne-winnt-processor_cache_type
type PROCESSOR_CACHE_TYPE uint32

const (
	CacheUnified PROCESSOR_CACHE_TYPE = iota
	CacheInstruction
	CacheData
	CacheTrace
)

func (t PROCESSOR_CACHE_TYPE) String() string {
	var name string
	switch t {
	case CacheUnified:
		name = "CacheUnified"
	case CacheInstruction:
		name = "CacheInstruction"
	case CacheData:
		name = "CacheData"
	case CacheTrace:
		name = "CacheTrace"
	default:
		err := fmt.Sprintf("unexpected value, was %d", t)
		panic(err)
	}
	return fmt.Sprintf("%s(%d)", name, t)
}
