/****************************************************************************

   KINECT PS3
   Signals intercept module

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

#include "signal_catch.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


XnBool * g_pValueToSet = NULL;

void SignalHandler(int nSignal)
{
	printf("Caught signal: %d\n", nSignal);
	if (NULL != g_pValueToSet) {
		*g_pValueToSet = true;
	}
}

void CatchSignals(XnBool* bSignalWasCaught)
{
	g_pValueToSet = bSignalWasCaught;

	struct sigaction act;

	memset( &act, 0, sizeof( act ) );
	act.sa_handler = &SignalHandler;
	act.sa_flags = 0;
	sigaction( SIGINT, &act, NULL );
	sigaction( SIGTERM, &act, NULL );
	sigaction( SIGKILL, &act, NULL );
}
