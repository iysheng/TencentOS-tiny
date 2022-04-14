#!/bin/sh

TOPDIR=/home/red/Projects/TencentOS-tiny
BOARDDIR=$TOPDIR/board/GD32F310G_START
KERNELDIR=$TOPDIR/kernel
OSALDIR=$TOPDIR/osal
ARCHDIR=$TOPDIR/arch/arm/arm-v7m/common
ARCHDIR1=$TOPDIR/arch/arm/arm-v7m/cortex-m4
VENORDIR=$TOPDIR/platform/vendor_bsp/gd/GD32F3x0_Firmware_Library


fd -e c -e h -e s -e S . $BOARDDIR > cscope.files
fd -e c -e h -e s -e S . $KERNELDIR >> cscope.files
fd -e c -e h -e s -e S . $OSALDIR >> cscope.files
fd -e c -e h -e s -e S . $ARCHDIR >> cscope.files
fd -e c -e h -e s -e S . $ARCHDIR1 >> cscope.files
fd -e c -e h -e s -e S . $VENORDIR >> cscope.files
fd -e c -e h -e s -e S . $STARTFILE >> cscope.files

cscope -bqR
