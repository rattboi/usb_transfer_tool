#include <string.h>
#include <stdlib.h>
#include <gctypes.h>
#include <fat.h>
#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include <iosuhax_disc_interface.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/fs_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "common/common.h"
#include "ftp.h"
#include "virtualpath.h"
#include "net.h"
#include "background.h"
#include <dirent.h>

///WUP
#define TITLE_TEXT "-----WIIU USB HELPER TRANSFER TOOL-----"
#define TITLE_TEXT2 "By Rattboi (Hikari06,Crediar,Dimok,FIX94,Yardape8000)"

#define HBL_TITLE_ID 0x0005000013374842

#define EXIT_RELAUNCH_ID_ON_LOAD 0x101010

#define MCP_COMMAND_INSTALL_ASYNC 0x81
#define MAX_INSTALL_PATH_LENGTH 0x27F

#define MAX_FOLDERS 1024

static int doInstall = 0;
bool iosuhaxMount = false;
static int installCompleted = 0;
static int installSuccess = 0;
static int installToUsb = 0;
static u32 installError = 0;
static u64 installedTitle = 0;
static u64 baseTitleId = 0;
static int dirNum = 0;
static char installFolder[256] = "";
static char lastFolder[256] = "";
static char errorText1[128] = "";
static char errorText2[128] = "";
static bool folderSelect[MAX_FOLDERS] = {false};
static int update_screen=1;
static bool installFromNetwork=false;

typedef struct {
  s32 socket;
  struct sockaddr_in addr;
  bool initialized;
} BroadcastInfo;

BroadcastInfo CreateBroadcast() {
    BroadcastInfo bcastInfo;
    bcastInfo.initialized = false;

    unsigned short broadcastPort = 14521;
    int broadcastPermission = 1;

    if ((bcastInfo.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        return bcastInfo;

    if (setsockopt(bcastInfo.socket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission,
                   sizeof(broadcastPermission)) < 0)
        return bcastInfo;

    memset(&bcastInfo.addr, 0, sizeof(struct sockaddr_in));
    bcastInfo.addr.sin_family = AF_INET;
    bcastInfo.addr.sin_addr.s_addr = 4294967295;
    bcastInfo.addr.sin_port = htons(broadcastPort);

    bcastInfo.initialized = true;

    return bcastInfo;
}

static void SendBeacon(BroadcastInfo bcast) {
    char *sendString = "HELLO FROM WIIU!";
    sendto(bcast.socket, sendString, (int) strlen(sendString), 0, (struct sockaddr *)&bcast.addr, sizeof(bcast.addr));
}

static int IosInstallCallback(unsigned int errorCode, unsigned int *priv_data)
{
    installError = errorCode;
    installCompleted = 1;
    return 0;
}

static void GetInstallDir(char *dest, int size)
{
    DIR *dirPtr;
    struct dirent *dirEntry;
    dirPtr = opendir("sd:/install/");
    int dirsFound = 0;

    if (dirPtr != NULL)
    {
        dirEntry = readdir(dirPtr);
        if (dirEntry != NULL && dirEntry->d_type == DT_DIR)
        {
            seekdir(dirPtr, dirNum);
            dirEntry = readdir(dirPtr);
            closedir(dirPtr);
            if (dirEntry != NULL && dirEntry->d_type == DT_DIR)
                __os_snprintf(dest, size, "install/%s", dirEntry->d_name);
            else
            {
                dirNum--;
                if (dirNum < 0)
                    dirNum = 0;
            }
            dirsFound = 1;
        }
    }
    if (!dirsFound)
    {
        __os_snprintf(dest, size, "install");
        dirNum = 0;
    }
}

static void setAllFolderSelect(bool state)
{
    DIR *dirPtr;
    struct dirent *dirEntry;

    for (int i = 0; i < MAX_FOLDERS; i++)
        folderSelect[i] = false;

    if (state)
    {
        dirPtr = opendir("sd:/install/");
        if (dirPtr != NULL)
        {
            int i = 0;
            while (1)
            {
                dirEntry = readdir(dirPtr);
                if (dirEntry == NULL || dirEntry->d_type != DT_DIR)
                    break;
                folderSelect[i++] = true;
            }
            closedir(dirPtr);
        }
    }
}

static void RefreshSD() {
    if (!iosuhaxMount) {
        unmount_sd_fat("sd");
        usleep(50000);
        mount_sd_fat("sd");
    } else {
        fatUnmount("sd");
        usleep(50000);
        fatInitDefault();
        VirtualMountDevice("sd:/");
    }

    setAllFolderSelect(false);
    dirNum = 0;
    update_screen = 1;
    usleep(50000);
}

static int getNextSelectedFolder(void) {
    for (int i = 0; i < MAX_FOLDERS; i++) {
        if (folderSelect[i] == true) {
            return i;
        }
    }
    return 0;
}

static bool useFolderSelect() {
    for (int i = 0; i < MAX_FOLDERS; i++) {
        if (folderSelect[i] == true) {
            return true;
        }
    } 
    return false;
}

static void SetupInstallTitle(void) {
    if (useFolderSelect())
        dirNum = getNextSelectedFolder();
    GetInstallDir(installFolder, sizeof(installFolder));
}

static void InstallOrderFromNetwork(char* installPath) {
    strcpy(installFolder, installPath);
    installFromNetwork=true;
}

void CheckForErrors(int installError) {
    const char *errors[7] = {
      "Unknown Error Code",
      "USB Access failed (no USB storage attached?)",
      "Possible missing or bad title.tik file",
      "Possible incorrect console for DLC title.tik file",
      "Possible not enough memory on target device",
      "Possible bad SD card.  Reformat (32k blocks) or replace",
      "Verify WUP files are correct & complete. DLC/E-shop require Sig Patch"
    };

    if (installError != 0) {
        int errorCode = 0;
        switch(installError) {
            case 0xFFFCFFE9:
                errorCode = 1;
                break;
            case 0xFFFBF446:
            case 0xFFFBF43F:
                errorCode = 2;
                break;
            case 0xFFFBF441:
                errorCode = 3;
                break;
            case 0xFFFCFFE4:
                errorCode = 4;
                break;
            case 0xFFFFF825:
                errorCode = 5;
                break;
            default:
                errorCode = 0;
        }
        if ((installError & 0xFFFF0000) == 0xFFFB0000) {
          errorCode = 6;
        }

        __os_snprintf(errorText1, sizeof(errorText1), "Error: install error code 0x%08X", installError);
        __os_snprintf(errorText2, sizeof(errorText2), errors[errorCode]);
    } else {
        installSuccess = 1;
    }
}

void UpdateProgress(char* installFolder, char* progressText, int percent) {
    for (unsigned int i = 0; i < 2; i++) {
        OSScreenClearBufferEx(i, 0);
        DrawBackground(i);

        unsigned int y = (i == 0 ? 9 : 7);
        OSScreenPutFontEx(i, 0, 0, TITLE_TEXT);
        OSScreenPutFontEx(i, 0, 1, TITLE_TEXT2);
        OSScreenPutFontEx(i, 0, y, "Installing title...");
        OSScreenPutFontEx(i, 0, y + 1, installFolder);
        OSScreenPutFontEx(i, 0, y + 2, progressText);

        if (percent == 100) {
            OSScreenPutFontEx(i, 0, y + 3, "Please wait...");
        }

        OSScreenFlipBuffersEx(i); // Flip buffers
    }
}

static void InstallTitle(void) {
    errorText1[0] = 0;
    errorText2[0] = 0;
    installSuccess = 0;
    installedTitle = 0;
    installCompleted = 1;
    installError = 0;

    __os_snprintf(lastFolder, sizeof(lastFolder), installFolder);

    //!---------------------------------------------------
    //! This part of code originates from Crediars MCP patcher assembly code
    //! it is just translated to C
    //!---------------------------------------------------
    unsigned int mcpHandle = MCP_Open();
    if (mcpHandle == 0) {
        __os_snprintf(errorText1, sizeof(errorText1), "Failed to open MCP.");
        return;
    }

    char installPath[256];
    unsigned int *mcpInstallInfo = (unsigned int *)OSAllocFromSystem(sizeof(MCPInstallInfo), 0x40);
    MCPInstallProgress *mcpInstallProgress = (MCPInstallProgress *)OSAllocFromSystem(sizeof(MCPInstallProgress), 0x40);
    char *mcpInstallPath = (char *)OSAllocFromSystem(MAX_INSTALL_PATH_LENGTH, 0x40);
    unsigned int *mcpPathInfoVector = (unsigned int *)OSAllocFromSystem(0x0C, 0x40);

    do {
        if (!mcpInstallInfo || !mcpInstallProgress || !mcpInstallPath || !mcpPathInfoVector) {
            __os_snprintf(errorText1, sizeof(errorText1), "Error: Could not allocate memory.");
            break;
        }

        __os_snprintf(installPath, sizeof(installPath), "/vol/app_sd/%s", installFolder);

        int result = MCP_InstallGetInfo(mcpHandle, installPath, mcpInstallInfo);
        if (result != 0) {
            __os_snprintf(errorText1, sizeof(errorText1), "Error: MCP_InstallGetInfo 0x%08X", MCP_GetLastRawError());
            __os_snprintf(errorText2, sizeof(errorText2), "Confirm complete WUP files are in the folder. Try power down.");
            break;
        }

        u32 titleIdHigh = mcpInstallInfo[0];
        u32 titleIdLow = mcpInstallInfo[1];
        int spoofFiles = 0;
        if ((titleIdHigh == 00050010) && ((titleIdLow == 0x10041000)      // JAP Version.bin
                                       || (titleIdLow == 0x10041100)      // USA Version.bin
                                       || (titleIdLow == 0x10041200)))    // EUR Version.bin
        {
            spoofFiles = 1;
            installToUsb = 0;
        }

        if (spoofFiles || (titleIdHigh == 0x0005000E) // game update
            || (titleIdHigh == 0x00050000)            // game
            || (titleIdHigh == 0x0005000C)            // DLC
            || (titleIdHigh == 0x00050002))           // Demo
        {
            installedTitle = ((u64)titleIdHigh << 32ULL) | titleIdLow;

            result = MCP_InstallSetTargetDevice(mcpHandle, installToUsb);
            if (result != 0) {
                __os_snprintf(errorText1, sizeof(errorText1), "Error: MCP_InstallSetTargetDevice 0x%08X", MCP_GetLastRawError());
                if (installToUsb)
                    __os_snprintf(errorText2, sizeof(errorText2), "Possible USB HDD disconnected or failure");
                break;
            }
            result = MCP_InstallSetTargetUsb(mcpHandle, installToUsb);
            if (result != 0) {
                __os_snprintf(errorText1, sizeof(errorText1), "Error: MCP_InstallSetTargetUsb 0x%08X", MCP_GetLastRawError());
                if (installToUsb)
                    __os_snprintf(errorText2, sizeof(errorText2), "Possible USB HDD disconnected or failure");
                break;
            }

            mcpInstallInfo[2] = (unsigned int)MCP_COMMAND_INSTALL_ASYNC;
            mcpInstallInfo[3] = (unsigned int)mcpPathInfoVector;
            mcpInstallInfo[4] = (unsigned int)1;
            mcpInstallInfo[5] = (unsigned int)0;

            memset(mcpInstallPath, 0, MAX_INSTALL_PATH_LENGTH);
            __os_snprintf(mcpInstallPath, MAX_INSTALL_PATH_LENGTH, installPath);
            memset(mcpPathInfoVector, 0, 0x0C);

            mcpPathInfoVector[0] = (unsigned int)mcpInstallPath;
            mcpPathInfoVector[1] = (unsigned int)MAX_INSTALL_PATH_LENGTH;

            installCompleted = 0;
            result = IOS_IoctlvAsync(mcpHandle, MCP_COMMAND_INSTALL_ASYNC, 1, 0, mcpPathInfoVector, IosInstallCallback, mcpInstallInfo);
            if (result != 0) {
                __os_snprintf(errorText1, sizeof(errorText1), "Error: MCP_InstallTitleAsync 0x%08X", MCP_GetLastRawError());
                break;
            }

            char progressText[256];
            while (!installCompleted) {
                memset(mcpInstallProgress, 0, sizeof(MCPInstallProgress));

                MCP_InstallGetProgress(mcpHandle, mcpInstallProgress);

                if (mcpInstallProgress->inProgress == 1) {
                    u64 totalSize = mcpInstallProgress->sizeTotal;
                    u64 installedSize = mcpInstallProgress->sizeProgress;
                    int percent = ((totalSize != 0) ? (int)((installedSize * 100.0f) / totalSize) : 0);

                    __os_snprintf(progressText, sizeof(progressText), "%08X%08X - %0.1f / %0.1f MB (%i%%)",
                                  titleIdHigh, titleIdLow, installedSize / (1024.0f * 1024.0f),
                                  totalSize / (1024.0f * 1024.0f), percent);

                    UpdateProgress(installFolder, progressText, percent);
                }
                usleep(50000);
            }

            CheckForErrors(installError);
        } else {
            __os_snprintf(errorText1, sizeof(errorText1), "Error: Not a game, game update, DLC, demo or version title");
        }
    } while (0);

    folderSelect[dirNum] = false;
    if (installSuccess && useFolderSelect()) {
        dirNum = getNextSelectedFolder();
        doInstall = 1;
    } else {
        doInstall = 0;
    }

    MCP_Close(mcpHandle);
    if (mcpPathInfoVector)
        OSFreeToSystem(mcpPathInfoVector);
    if (mcpInstallPath)
        OSFreeToSystem(mcpInstallPath);
    if (mcpInstallInfo)
        OSFreeToSystem(mcpInstallInfo);
    if (mcpInstallProgress)
        OSFreeToSystem(mcpInstallProgress);
}

void UpdateLoop(int delay, u64 installedTitle, int installSuccess, int installCompleted) {
    char ip_address[256] = "";
    u32 host_ip = network_gethostip();
    for (unsigned int i = 0; i < 2; i++) {
        char text[160];

        OSScreenClearBufferEx(i, 0);
        DrawBackground(i);

        OSScreenPutFontEx(i, 0, 0, TITLE_TEXT);
        OSScreenPutFontEx(i, 0, 1, TITLE_TEXT2);
        __os_snprintf(ip_address, sizeof(ip_address),
            "YOUR IP: %u.%u.%u.%u:%i %sIOSUHAX SPEED BOOST)",
            (host_ip >> 24) & 0xFF,
            (host_ip >> 16) & 0xFF,
            (host_ip >> 8) & 0xFF,
            (host_ip >> 0) & 0xFF, 21, iosuhaxMount ? "  (" : "(NO ");
        OSScreenPutFontEx(i, 0, 2, ip_address);
        OSScreenPutFontEx(i, 0, 4, lastFolder);
        __os_snprintf(text, sizeof(text), "Install of title %08X-%08X ", (u32)(installedTitle >> 32), (u32)(installedTitle & 0xffffffff));
        if (installSuccess) {
            __os_snprintf(text, sizeof(text), "%s finished successfully.", text);
            OSScreenPutFontEx(i, 0, 3, text);
        } else if (installCompleted) {
            __os_snprintf(text, sizeof(text), "%s failed.", text);
            OSScreenPutFontEx(i, 0, 3, text);
            OSScreenPutFontEx(i, 0, 5, errorText1);
            OSScreenPutFontEx(i, 0, 6, errorText2);
        }

        if (!doInstall) {
            unsigned int y = (i == 0 ? 10 : 8);
            OSScreenPutFontEx(i, 0, y, "Select a title to install (* = Selected)");
            if(strlen(installFolder)<63+8) {
                __os_snprintf(text, sizeof(text), "%c  %s", folderSelect[dirNum] ? '*' : ' ', installFolder+8);
                OSScreenPutFontEx(i, 0, y + 1, text);
            } else {
                __os_snprintf(text, 67, "%c  %s", folderSelect[dirNum] ? '*' : ' ', installFolder+8);
                OSScreenPutFontEx(i, 0, y + 1, text);
                __os_snprintf(text, 160-67, "   %s", installFolder+63+8);
                OSScreenPutFontEx(i, 0, y + 2, text);
            }
        } else {
            OSScreenPutFontEx(i, 0, 8, installFolder);
            __os_snprintf(text, sizeof(text), "Will install in %d", delay / 50);
            OSScreenPutFontEx(i, 0, 9, text);
            OSScreenPutFontEx(i, 0, 10, "Press B-Button to Cancel");
        }
    }

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);
}

unsigned int InitiateWUP(BroadcastInfo bcastInfo, int serverSocket) {
    update_screen = 1;
    int delay = 0;
    VPADData vpad_data;

    u64 currentTitleId = OSGetTitleID();
    int hblChannelLaunch = (currentTitleId == HBL_TITLE_ID);

    // in case we are not in mii maker but in system menu we start the installation
    if (currentTitleId != 0x000500101004A200 && // mii maker eur
        currentTitleId != 0x000500101004A100 && // mii maker usa
        currentTitleId != 0x000500101004A000 && // mii maker jpn
        !hblChannelLaunch) {                    // HBL channel
        InstallTitle();
        return EXIT_RELAUNCH_ID_ON_LOAD;
    }

    baseTitleId = currentTitleId;
    int loopCounter = 0;
    int yPressed = 0;
    while (1) {
        loopCounter++;
        process_ftp_events(serverSocket);

        // print to TV and DRC
        if (update_screen || loopCounter > 150) {
          GetInstallDir(installFolder, sizeof(installFolder));
          SendBeacon(bcastInfo);
          UpdateLoop(delay, installedTitle, installSuccess, installCompleted);
          loopCounter = 0;
        }
        update_screen = 0;

        int vpadError = -1;
        VPADRead(0, &vpad_data, 1, &vpadError);
        u32 pressedBtns = 0;

        if (!vpadError)
            pressedBtns = vpad_data.btns_d | vpad_data.btns_h;

        if (pressedBtns & VPAD_BUTTON_HOME) {
            doInstall = 0;
            break;
        }

        if (!(pressedBtns & VPAD_BUTTON_Y))
            yPressed = 0;

        if (!doInstall) {
            if (!(pressedBtns & (VPAD_BUTTON_UP | VPAD_BUTTON_DOWN)))
                delay = 0;

            if(installFromNetwork) {
                installFromNetwork = false;
                doInstall = 1;
                installToUsb = 1;
                if (hblChannelLaunch) {
                    InstallTitle();
                    update_screen = 1;
                    if (doInstall)
                        delay = 250;
                } else {
                    break;
                }
            }

            if (pressedBtns & (VPAD_BUTTON_A | VPAD_BUTTON_X)) { // install to NAND/USB 
                doInstall = 1;
                installToUsb = (pressedBtns & VPAD_BUTTON_X) ? 1 : 0;
                SetupInstallTitle();
                if (hblChannelLaunch) {
                    InstallTitle();
                    update_screen = 1;
                    if (doInstall)
                        delay = 250;
                } else {
                    break;
                }
            }
            else if (pressedBtns & VPAD_BUTTON_Y) // remount SD
            {
                if (!yPressed) {
                    RefreshSD();
                }
                yPressed = 1;
            }
            else if (pressedBtns & VPAD_BUTTON_UP) // up directory
            {
                if (--delay <= 0)
                {
                    dirNum++;
                    delay = (vpad_data.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
                }
            }
            else if (pressedBtns & VPAD_BUTTON_DOWN) // down directory
            {
                if (--delay <= 0)
                {
                    dirNum--;
                    delay = (vpad_data.btns_d & VPAD_BUTTON_DOWN) ? 6 : 0;
                }
            }
            else if (pressedBtns & (VPAD_BUTTON_LEFT | VPAD_BUTTON_RIGHT)) // unselect/select directory
            {
                folderSelect[dirNum] = (pressedBtns & VPAD_BUTTON_RIGHT) ? true : false;
            }
            else if (pressedBtns & (VPAD_BUTTON_MINUS | VPAD_BUTTON_PLUS)) // unselect/select all directories
            {
                setAllFolderSelect((pressedBtns & VPAD_BUTTON_PLUS) ? true : false);
            }

            if (dirNum >= MAX_FOLDERS || dirNum < 0) // wrap on one end
                dirNum = 0;

            // folder selection button pressed ?
            update_screen |= (pressedBtns & (VPAD_BUTTON_UP | VPAD_BUTTON_DOWN | VPAD_BUTTON_LEFT | VPAD_BUTTON_RIGHT | VPAD_BUTTON_PLUS | VPAD_BUTTON_MINUS | VPAD_BUTTON_Y)) ? 1 : 0;
        } else {
            if (pressedBtns & VPAD_BUTTON_B) { // cancel
                doInstall = 0;
                installSuccess = 0;
                update_screen = 1;
                delay = 0;
            } else if (--delay <= 0) {
                if (hblChannelLaunch) {
                    __os_snprintf(lastFolder, sizeof(lastFolder), installFolder);
                    SetupInstallTitle();
                    InstallTitle();
                    update_screen = 1;
                    if (doInstall)
                        delay = 250;
                }
                else
                    break;
            }
            else
                update_screen = (delay % 50 == 0) ? 1 : 0;
        }
        usleep(20000);
    }

    if (doInstall) {
        return EXIT_RELAUNCH_ON_LOAD;
    }

    setAllFolderSelect(false);
    dirNum = 0;
    installFolder[0] = 0;

    return EXIT_SUCCESS;
}

int InitiateFTP() {
    int serverSocket = create_server(21);
    SetREFRECallBack(&RefreshSD);
    SetINSTCallBack(&InstallOrderFromNetwork);
    return serverSocket;
}

void ShutdownFTP(int serverSocket) {
    cleanup_ftp();
    network_close(serverSocket);
}

int MountSd() {
    int fsaFd = -1;
    int res = IOSUHAX_Open(NULL);
    if (res < 0) { // no iosuhax available, use normal stuff
        mount_sd_fat("sd");
        VirtualMountDevice("sd:/");  
    } else {
        iosuhaxMount = true;
        fatInitDefault();
        fsaFd = IOSUHAX_FSA_Open();
        mount_fs("storage_usb", fsaFd, NULL, "/vol/storage_usb01");
        VirtualMountDevice("sd:/");
        VirtualMountDevice("storage_usb:/");
    }
    return fsaFd;
}

void ShutdownStorage(int fsaFd) {
    if (fsaFd != -1) { // this means we had iosuhax from MountSd
        fatUnmount("sd");
        IOSUHAX_sdio_disc_interface.shutdown();
        IOSUHAX_usb_disc_interface.shutdown();
        unmount_fs("storage_usb");
        IOSUHAX_FSA_Close(fsaFd);
        IOSUHAX_Close();
    } else {
        unmount_sd_fat("sd");
    }
}

unsigned char* screenInitialize() {
    unsigned int screen_buf0_size = 0;
    unsigned int screen_buf1_size = 0;
    // Init screen and screen buffers
    OSScreenInit();
    screen_buf0_size = OSScreenGetBufferSizeEx(0);
    screen_buf1_size = OSScreenGetBufferSizeEx(1);

    unsigned char *screenBuffer = MEM1_alloc(screen_buf0_size + screen_buf1_size, 0x100);

    OSScreenSetBufferEx(0, screenBuffer);
    OSScreenSetBufferEx(1, (screenBuffer + screen_buf0_size));

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    // Clear screens
    OSScreenClearBufferEx(0, 0);
    OSScreenClearBufferEx(1, 0);

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);

    return screenBuffer;
}

/* Entry point */
int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    InitOSFunctionPointers();
    InitSocketFunctionPointers();
    InitFSFunctionPointers();
    InitVPadFunctionPointers();
    InitSysFunctionPointers();

    //!*******************************************************************
    //!                    Initialize heap memory                        *
    //!*******************************************************************

    memoryInitialize();

    //!*******************************************************************
    //!                        Initialize Screen                         *
    //!*******************************************************************

    unsigned char *screenBuffer = screenInitialize();

    //!*******************************************************************
    //!                        Initialize FS                             *
    //!*******************************************************************

    int fsaFd = MountSd();  // fsaFd == -1 -> no iosuhax

    LoadPictures();

    BroadcastInfo bcastInfo = CreateBroadcast(); // never cleaned up?

    int serverSocket = InitiateFTP();

    unsigned int exit_code = InitiateWUP(bcastInfo, serverSocket); // This is a blocking call that is most of the program...

    //!*******************************************************************
    //!                    Exit main application                        *
    //!*******************************************************************

    ShutdownFTP(serverSocket);

    ShutdownStorage(fsaFd);

    UnloadPictures();

    MEM1_free(screenBuffer);

    UnmountVirtualPaths();
    memoryRelease();

    if (exit_code == EXIT_RELAUNCH_ON_LOAD) {
        SYSLaunchMenu();
        return EXIT_RELAUNCH_ON_LOAD;
    }

    if (exit_code == EXIT_RELAUNCH_ID_ON_LOAD) {
        SYSLaunchTitle(baseTitleId);
        return EXIT_RELAUNCH_ON_LOAD;
    }

    return EXIT_SUCCESS;
}
