/****************************************************************************

   KINECT PS3
   Key Detection Module

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
#include "kbhit.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

int _kbhit()
{
	int ch;
	int oldf;
	struct termios old_termios;
	struct termios new_termios;

	tcgetattr( STDIN_FILENO, &old_termios );
	new_termios = old_termios;
	new_termios.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &new_termios );
	oldf = fcntl( STDIN_FILENO, F_GETFL, 0 );
	fcntl( STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK );

	ch = getchar();

	tcsetattr( STDIN_FILENO, TCSANOW, &old_termios );
	fcntl( STDIN_FILENO, F_SETFL, oldf );

	if( ch != EOF ) {
		ungetc( ch, stdin );
		return 1;
	}

	return 0;
}
