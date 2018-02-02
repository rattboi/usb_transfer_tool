/****************************************************************************
 * Copyright (C) 2015
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include "../common/common.h"
#include "os_functions.h"

unsigned int coreinit_handle = 0;

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Lib handle functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(int, OSDynLoad_Acquire, const char* rpl, u32 *handle);
EXPORT_DECL(int, OSDynLoad_FindExport, u32 handle, int isdata, const char *symbol, void *address);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Thread functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(int, OSCreateThread, void *thread, s32 (*callback)(s32, void*), s32 argc, void *args, u32 stack, u32 stack_size, s32 priority, u32 attr);
EXPORT_DECL(int, OSResumeThread, void *thread);
EXPORT_DECL(int, OSSuspendThread, void *thread);
EXPORT_DECL(int, OSIsThreadTerminated, void *thread);
EXPORT_DECL(int, OSIsThreadSuspended, void *thread);
EXPORT_DECL(int, OSSetThreadPriority, void * thread, int priority);
EXPORT_DECL(int, OSJoinThread, void * thread, int * ret_val);
EXPORT_DECL(void, OSDetachThread, void * thread);
EXPORT_DECL(void, OSSleepTicks, u64 ticks);
EXPORT_DECL(u64, OSGetTick, void);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Mutex functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(void, OSInitMutex, void* mutex);
EXPORT_DECL(void, OSLockMutex, void* mutex);
EXPORT_DECL(void, OSUnlockMutex, void* mutex);
EXPORT_DECL(int, OSTryLockMutex, void* mutex);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! System functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(u64, OSGetTitleID, void);
EXPORT_DECL(int, OSGetPFID, void);
EXPORT_DECL(void, OSShutdown, void);
EXPORT_DECL(void, __Exit, int);
EXPORT_DECL(void, OSFatal, const char* msg);
EXPORT_DECL(void, OSSetExceptionCallback, u8 exceptionType, exception_callback newCallback);
EXPORT_DECL(void, DCFlushRange, const void *addr, u32 length);
EXPORT_DECL(void, DCInvalidateRange, const void *addr, u32 length);
EXPORT_DECL(void, ICInvalidateRange, const void *addr, u32 length);
EXPORT_DECL(void*, OSEffectiveToPhysical, const void*);
EXPORT_DECL(int, __os_snprintf, char* s, int n, const char * format, ...);
EXPORT_DECL(void*, OSAllocFromSystem, int size, int align);
EXPORT_DECL(void, OSFreeToSystem, void *ptr);

EXPORT_DECL(void, OSScreenInit, void);
EXPORT_DECL(unsigned int, OSScreenGetBufferSizeEx, unsigned int bufferNum);
EXPORT_DECL(int, OSScreenSetBufferEx, unsigned int bufferNum, void * addr);
EXPORT_DECL(int, OSScreenClearBufferEx, unsigned int bufferNum, unsigned int temp);
EXPORT_DECL(int, OSScreenFlipBuffersEx, unsigned int bufferNum);
EXPORT_DECL(int, OSScreenPutFontEx, unsigned int bufferNum, unsigned int posX, unsigned int posY, const char * buffer);
EXPORT_DECL(int, OSScreenEnableEx, unsigned int bufferNum, int enable);
EXPORT_DECL(unsigned int, OSScreenPutPixelEx, unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
EXPORT_DECL(int, IOS_Ioctl,int fd, unsigned int request, void *input_buffer,unsigned int input_buffer_len, void *output_buffer, unsigned int output_buffer_len);
EXPORT_DECL(int, IOS_Open,char *path, unsigned int mode);
EXPORT_DECL(int, IOS_Close,int fd);
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! MCP functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(unsigned int, MCP_Open, void);
EXPORT_DECL(int, MCP_Close, unsigned int handle);
EXPORT_DECL(int, MCP_InstallTitleAbort, unsigned int handle);
EXPORT_DECL(int, MCP_InstallGetInfo, unsigned int handle, const char *path, void * mcp_info);
EXPORT_DECL(int, MCP_InstallTitleAsync, unsigned int handle, const char *path, void * mcp_info);
EXPORT_DECL(int, MCP_InstallGetProgress, unsigned int handle, MCPInstallProgress * buffer);
EXPORT_DECL(int, MCP_InstallSetTargetDevice, unsigned int handle, int device);
EXPORT_DECL(int, MCP_InstallSetTargetUsb, unsigned int handle, int device);
EXPORT_DECL(int, MCP_GetLastRawError, void);
EXPORT_DECL(int, IOS_IoctlvAsync, unsigned int fd, unsigned int command, int cnt_in, int cnt_out, void *ioctlv, void *ipc_callback, void *usrdata);
EXPORT_DECL(int, IOS_IoctlAsync,int fd, unsigned int request, void *input_buffer,unsigned int input_buffer_len, void *output_buffer, unsigned int output_buffer_len, void *cb, void *cbarg);
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Memory functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_VAR(unsigned int *, pMEMAllocFromDefaultHeapEx);
EXPORT_VAR(unsigned int *, pMEMAllocFromDefaultHeap);
EXPORT_VAR(unsigned int *, pMEMFreeToDefaultHeap);

EXPORT_DECL(int, MEMGetBaseHeapHandle, int mem_arena);
EXPORT_DECL(unsigned int, MEMGetAllocatableSizeForFrmHeapEx, int heap, int align);
EXPORT_DECL(void *, MEMAllocFromFrmHeapEx, int heap, unsigned int size, int align);
EXPORT_DECL(void, MEMFreeToFrmHeap, int heap, int mode);
EXPORT_DECL(void *, MEMAllocFromExpHeapEx, int heap, unsigned int size, int align);
EXPORT_DECL(int , MEMCreateExpHeapEx, void* address, unsigned int size, unsigned short flags);
EXPORT_DECL(void *, MEMDestroyExpHeap, int heap);
EXPORT_DECL(void, MEMFreeToExpHeap, int heap, void* ptr);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Loader functions (not real rpl)
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXPORT_DECL(int, LiWaitIopComplete, int unknown_syscall_arg_r3, int * remaining_bytes);
EXPORT_DECL(int, LiWaitIopCompleteWithInterrupts, int unknown_syscall_arg_r3, int * remaining_bytes);

void InitOSFunctionPointers(void)
{
    unsigned int *funcPointer = 0;
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! Lib handle functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    EXPORT_FUNC_WRITE(OSDynLoad_Acquire, (int (*)(const char*, unsigned *))OS_SPECIFICS->addr_OSDynLoad_Acquire);
    EXPORT_FUNC_WRITE(OSDynLoad_FindExport, (int (*)(u32, int, const char *, void *))OS_SPECIFICS->addr_OSDynLoad_FindExport);

    OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);

    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! System functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OS_FIND_EXPORT(coreinit_handle, OSFatal);
    OS_FIND_EXPORT(coreinit_handle, OSGetTitleID);
    OS_FIND_EXPORT(coreinit_handle, OSGetPFID);
    OS_FIND_EXPORT(coreinit_handle, OSSetExceptionCallback);
    OS_FIND_EXPORT(coreinit_handle, DCFlushRange);
    OS_FIND_EXPORT(coreinit_handle, DCInvalidateRange);
    OS_FIND_EXPORT(coreinit_handle, ICInvalidateRange);
    OS_FIND_EXPORT(coreinit_handle, OSEffectiveToPhysical);
    OS_FIND_EXPORT(coreinit_handle, __os_snprintf);
    OS_FIND_EXPORT(coreinit_handle, OSAllocFromSystem);
    OS_FIND_EXPORT(coreinit_handle, OSFreeToSystem);
    OS_FIND_EXPORT(coreinit_handle, OSShutdown);
    OSDynLoad_FindExport(coreinit_handle, 0, "exit", &__Exit);

    OS_FIND_EXPORT(coreinit_handle, OSScreenInit);
    OS_FIND_EXPORT(coreinit_handle, OSScreenGetBufferSizeEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenSetBufferEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenClearBufferEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenFlipBuffersEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenPutFontEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenEnableEx);
    OS_FIND_EXPORT(coreinit_handle, OSScreenPutPixelEx);
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! Thread functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OS_FIND_EXPORT(coreinit_handle, OSCreateThread);
    OS_FIND_EXPORT(coreinit_handle, OSResumeThread);
    OS_FIND_EXPORT(coreinit_handle, OSSuspendThread);
    OS_FIND_EXPORT(coreinit_handle, OSIsThreadTerminated);
    OS_FIND_EXPORT(coreinit_handle, OSIsThreadSuspended);
    OS_FIND_EXPORT(coreinit_handle, OSJoinThread);
    OS_FIND_EXPORT(coreinit_handle, OSSetThreadPriority);
    OS_FIND_EXPORT(coreinit_handle, OSDetachThread);
    OS_FIND_EXPORT(coreinit_handle, OSSleepTicks);
    OS_FIND_EXPORT(coreinit_handle, OSGetTick);
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! Mutex functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OS_FIND_EXPORT(coreinit_handle, OSInitMutex);
    OS_FIND_EXPORT(coreinit_handle, OSLockMutex);
    OS_FIND_EXPORT(coreinit_handle, OSUnlockMutex);
    OS_FIND_EXPORT(coreinit_handle, OSTryLockMutex);

    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! MCP functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OS_FIND_EXPORT(coreinit_handle, MCP_Open);
    OS_FIND_EXPORT(coreinit_handle, MCP_Close);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallTitleAbort);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallGetInfo);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallTitleAsync);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallGetProgress);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallSetTargetDevice);
    OS_FIND_EXPORT(coreinit_handle, MCP_InstallSetTargetUsb);
    OS_FIND_EXPORT(coreinit_handle, MCP_GetLastRawError);
    OS_FIND_EXPORT(coreinit_handle, IOS_IoctlvAsync);

    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! Memory functions
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OSDynLoad_FindExport(coreinit_handle, 1, "MEMAllocFromDefaultHeapEx", &pMEMAllocFromDefaultHeapEx);
    OSDynLoad_FindExport(coreinit_handle, 1, "MEMAllocFromDefaultHeap", &pMEMAllocFromDefaultHeap);
    OSDynLoad_FindExport(coreinit_handle, 1, "MEMFreeToDefaultHeap", &pMEMFreeToDefaultHeap);

    OS_FIND_EXPORT(coreinit_handle, MEMGetBaseHeapHandle);
    OS_FIND_EXPORT(coreinit_handle, MEMGetAllocatableSizeForFrmHeapEx);
    OS_FIND_EXPORT(coreinit_handle, MEMAllocFromFrmHeapEx);
    OS_FIND_EXPORT(coreinit_handle, MEMFreeToFrmHeap);
    OS_FIND_EXPORT(coreinit_handle, MEMAllocFromExpHeapEx);
    OS_FIND_EXPORT(coreinit_handle, MEMCreateExpHeapEx);
    OS_FIND_EXPORT(coreinit_handle, MEMDestroyExpHeap);
    OS_FIND_EXPORT(coreinit_handle, MEMFreeToExpHeap);

    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //! Other function addresses
    //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    OS_FIND_EXPORT(coreinit_handle, IOS_Ioctl);
    OS_FIND_EXPORT(coreinit_handle, IOS_Open);
    OS_FIND_EXPORT(coreinit_handle, IOS_Close);
    OS_FIND_EXPORT(coreinit_handle, IOS_IoctlAsync);
}

