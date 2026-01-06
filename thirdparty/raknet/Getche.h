/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#if   (defined(_WIN32))
#include <conio.h> /* getche() */

#else
#if (!defined(__DREAMCAST__) && !defined(XENON))
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#else
#define NO_ONLINE // @TODO: nothing uses this, wtf is this?
#endif
char getche();
#endif 
