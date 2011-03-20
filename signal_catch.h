#ifndef __SIGNAL_CATCH_H__
#define __SIGNAL_CATCH_H__

/****************************************************************************

   KINECT PS3
   Signals intercept module header file

   Author: Shantanu Goel (http://tech.shantanugoel.com/)

****************************************************************************/
/*
   Copyright (C) 2011 Shantanu Goel
   This file is part of Kinect-PS3
   Kinect-PS3 is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   Kinect PS3 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not , see <http://www.gnu.org/licenses/>.
*/

#include <XnPlatform.h>

#if XN_PLATFORM == XN_PLATFORM_WIN32

#define CatchSignals(x)

#else
/*
 Stores pointer to bSignalWasCaught, and starts catching signals
 when a signal is caught, *bSignalWasCaught will be set to true
*/
void CatchSignals(XnBool* bSignalWasCaught);
#endif

#endif
