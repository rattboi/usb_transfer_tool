USB Transfer Tool
=================

Used with Wii U USB Helper to install WUPs.

Build Instructions
==================

1. Install devkitPro/devkitPPC r27. (https://devkitpro.org/wiki/Getting_Started/devkitPPC)

Apparently there are known issues with versions of devkitPPC past r27. Something to do with redefinition of "int". I don't know the details. It's recommended by the community to stick with r27 for now.

2. Install dimok's ogclibs and portlibs. (https://github.com/dimok789/homebrew_launcher/releases/tag/v1.0)

Download libogc.7z and portlibs.7z, and extract them to $DEVKITPRO/libogc and $DEVKITPRO/portlibs, respectively.
If devkitPPC comes with libogc, replace it entirely with dimok's libs.

3. Build libiosuhax for Wii U and install lib into portlibs (https://github.com/dimok789/libiosuhax)

```
git clone https://github.com/dimok789/libiosuhax
cd libiosuhax
make install
```

4. Build libfat for Wii U and place lib in portlibs (https://github.com/aliaspider/libfat)

```
git clone https://github.com/aliaspider/libfat
cd libfat
make install
```

5. Build usb transfer tool (this)
```
git clone https://github.com/rattboi/usb_transfer_tool
cd usb_transfer_tool
make
```

6. Copy usb_transfer_tool.elf to SD card and run from Homebrew Loader
