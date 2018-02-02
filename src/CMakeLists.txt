cmake_minimum_required(VERSION 3.9)
project(src)

set(CMAKE_CXX_STANDARD 11)
set(DEVKITPRO $ENV{HOME}/devkitPro/)
set(DEVKITPPC ${DEVKITPRO}/devkitPPC/)
set(PORTLIBS_DIR ${DEVKITPRO}/portlibs/ppc)
set(LIBOGC_DIR ${DEVKITPRO}/libogc)


set(HEADER_FILES
        ${DEVKITPPC}/powerpc-eabi/include
        ${PORTLIBS_DIR}/include
        ${LIBOGC_DIR}/include
)
include_directories(
        ${HEADER_FILES}
)

add_executable(
        src
        common/common.h
        common/fs_defs.h
        common/os_defs.h
        common/types.h
        dynamic_libs/ax_functions.c
        dynamic_libs/ax_functions.h
        dynamic_libs/fs_functions.c
        dynamic_libs/fs_functions.h
        dynamic_libs/gx2_functions.c
        dynamic_libs/gx2_functions.h
        dynamic_libs/gx2_types.h
        dynamic_libs/os_functions.c
        dynamic_libs/os_functions.h
        dynamic_libs/padscore_functions.c
        dynamic_libs/padscore_functions.h
        dynamic_libs/socket_functions.c
        dynamic_libs/socket_functions.h
        dynamic_libs/sys_functions.c
        dynamic_libs/sys_functions.h
        dynamic_libs/vpad_functions.c
        dynamic_libs/vpad_functions.h
        fs/fs_utils.c
        fs/fs_utils.h
        fs/sd_fat_devoptab.c
        fs/sd_fat_devoptab.h
        system/memory.c
        system/memory.h
        utils/logger.c
        utils/logger.h
        utils/utils.h
        background.c
        background.h
        entry.c
        ftp.c
        ftp.h
        main.c
        main.h
        net.c
        net.h
        os_functions.h
        virtualpath.c
        virtualpath.h
        vrt.c
        vrt.h
        ${HEADER_FILES}
)
