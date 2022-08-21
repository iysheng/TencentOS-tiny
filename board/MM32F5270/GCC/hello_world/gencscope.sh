#!/bin/sh

fd -e  c -e s -e S -e h . > cscope.files
fd -e  c -e s -e S -e h . ../../ >> cscope.files
fd -e  c -e s -e S -e h . ../../../../arch/arm/arm-v8m/common >> cscope.files
fd -e  c -e s -e S -e h . ../../../../arch/arm/arm-v8m/cortex-m33/gcc >> cscope.files
fd -e  c -e s -e S -e h . ../../../../components/shell >> cscope.files
fd -e  c -e s -e S -e h . ../../../../kernel/core/ >> cscope.files
fd -e  c -e s -e S -e h . ../../../../platform/vendor_bsp/MindMotion/MM32F527/ >> cscope.files

cscope -bqR cscope.files
