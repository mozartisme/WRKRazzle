//
// Copyright (c) Microsoft Corporation. All rights reserved. 
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//
//
#ifndef _MCA_H_
#define _MCA_H_

//
// Types of corrected errors that are tracked
//
typedef enum
{
    SingleBitEcc,
    CpuCache,
    CpuTlb,
    CpuBus,
    CpuRegFile
} MCECORRECTEDTYPE, *PMCECORRECTEDTYPE;

typedef struct
{
    LIST_ENTRY List;
    MCECORRECTEDTYPE Type;
    USHORT Counter;
    USHORT Flags;
    LARGE_INTEGER Timestamp;
    
    union
    {
        //
        // For SingleBitEcc type, indicates physical address of page
        // where error occured
        //
        PHYSICAL_ADDRESS SingleBitEccAddress;

        //
        // For Cpu* types, indicates cpu on which the error
        // occured
        //
        ULONG CpuId;
    };
} MCECORRECTEDEVENT, *PMCECORRECTEDEVENT;

typedef enum
{
    CpuStateCheckCache = 0,
    CpuStateCheckTLB = 1,
    CpuStateCheckBus = 2,
    CpuStateCheckRegFile = 3,
    CpuStateCheckMS = 4
};

extern ULONG WmipDisableMCAPopups;

//
// Configurable parameters for managing thresholds for eventlog
// suppression and recovery action for corrected MCE
//

//
// Interval within which multiple identical errors will be reported as
// a single error to the system eventlog. Can be configured under
// HKLM\System\CurrentControlSet\Control\WMI\CoalesceCorrectedErrorInterval
// A value of 0 will cause no coalesce of identical errors
//
extern ULONG WmipCoalesceCorrectedErrorInterval;

//
// Number of single bit ecc errors that can occur in the same page
// before it is attempted to map out the page. Can be configured under : 
// HKLM\System\CurrentControlSet\Control\WMI\SingleBitEccErrorThreshold
// A value of 0 will cause no attempt to map out pages
//
extern ULONG WmipSingleBitEccErrorThreshold;


//
// Maxiumum number of MCE events being tracked at one time. If there is
// more than this limit then the oldest ones are recycled. Can be
// configured under :
// HKLM\System\CurrentControlSet\Control\WMI\MaxCorrectedMCEOutstanding
// A value of 0 will disable tracking of corrected errors
//
extern ULONG WmipMaxCorrectedMCEOutstanding;

//
// List of corrected MCE that are being tracked
//
extern LIST_ENTRY WmipCorrectedMCEHead;
extern ULONG WmipCorrectedMCECount;

//
// Counter of maximum eventlog entries generated by any source. Can be
// configured under:
// HKLM\System\CurrentControlSet\Control\WMI\MaxCorrectedEventlogs
//
extern ULONG WmipCorrectedEventlogCounter;

#if defined(_X86_) || defined(_AMD64_)
NTSTATUS
WmipConstructMCAErrorEvent(
    IN PMCA_EXCEPTION McaException,
    IN ULONG ErrorLogSize,
    IN OUT PWNODE_SINGLE_INSTANCE Wnode,
    IN OUT PMSMCAEvent_Header Header,
    IN OUT PUCHAR *RawPtr,
    IN OUT BOOLEAN *IsFatal
    );
#else
NTSTATUS
WmipConstructMCAErrorEvent(
    IN PERROR_RECORD_HEADER RecordHeader,
    IN ULONG ErrorLogSize,
    IN OUT PWNODE_SINGLE_INSTANCE Wnode,
    IN OUT PMSMCAEvent_Header Header,
    IN OUT PUCHAR *RawPtr,
    IN OUT BOOLEAN *IsFatal
    );
#endif

NTSTATUS WmipFireOffWmiEvent(
    LPGUID Guid,
    ULONG DataSize,
    PVOID DataPtr            
    );

#endif // _MCA_H_

