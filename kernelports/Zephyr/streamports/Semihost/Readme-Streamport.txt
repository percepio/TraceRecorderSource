Tracealyzer Stream Port for Files
Percepio AB
www.percepio.com
-------------------------------------------------

This directory contains a "stream port" for the Tracealyzer recorder library,
i.e., the specific code needed to use a particular interface for streaming a
Tracealyzer RTOS trace. The stream port is defined by a set of macros in
trcStreamPort.h, found in the "include" directory.

This particular stream port is for streaming to a file on the host side when
running a QEMU-target by using the Zephyr semihosting api:s.

To use this stream port, make sure that include/trcStreamPort.h is found
by the compiler (i.e., add this folder to your project's include paths) and
add all included source files to your build. Make sure no other versions of
trcStreamPort.h are included by mistake!