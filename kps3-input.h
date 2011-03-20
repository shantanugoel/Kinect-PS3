#ifndef __KINECT_PS3_INPUT_H
#define __KINECT_PS3_INPUT_H

/****************************************************************************

   KINECT PS3
   Input Module Header File

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

typedef enum
{
  DIR_FWD = 0,
  DIR_BACK,
  DIR_NONE
} direction_t;

int setup_uinput_device(void);
void move_cursor(int x, int y, int z);
void press_button(int x);
void rls_button(int x);

#endif /* __KINECT_PS3_INPUT_H */
