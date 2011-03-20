/****************************************************************************

   KINECT PS3
   Input Module

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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <unistd.h>
#include "kps3-input.h"

/*
 * Confirm that this is where your distribution creates the uinput device node
 * The default one in the file works with most recent Ubuntu versions.
 * Generally, /dev/input/uinput is the other location that is commonly used
 */
#define UINPUT_DEVICE_NODE "/dev/uinput"

static int fd = -1;
struct uinput_user_dev uinp;
struct input_event event;

int setup_uinput_device()
{
  int i=0;
  fd = open(UINPUT_DEVICE_NODE, O_WRONLY | O_NDELAY);
  if (!fd)
  {
    printf("Unable to open Uinput device\n");
    return -1;
  }
  memset(&uinp,0,sizeof(uinp));
  strncpy(uinp.name, "Kinect Mouse", UINPUT_MAX_NAME_SIZE);
  uinp.id.version = 4;
  uinp.id.bustype = BUS_USB;
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_EVBIT, EV_REL);
  /*
     Make sure that the keys you want to send have also been properly
     configured in the ps3 controller software configuration xml
   */
  ioctl(fd, UI_SET_RELBIT, REL_X);
  ioctl(fd, UI_SET_RELBIT, REL_Y);
  ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE); 
  ioctl(fd, UI_SET_RELBIT, REL_Z);
  ioctl(fd, UI_SET_KEYBIT, KEY_W);
  ioctl(fd, UI_SET_KEYBIT, KEY_A);
  ioctl(fd, UI_SET_KEYBIT, KEY_S);
  ioctl(fd, UI_SET_KEYBIT, KEY_D);
  ioctl(fd, UI_SET_KEYBIT, KEY_X);
  write(fd, &uinp, sizeof(uinp));
  if (ioctl(fd, UI_DEV_CREATE))
  {
    printf("Unable to create UINPUT device.");
    return -1;
  }
  return 1;
}

void move_cursor(int x, int y, int z )
{
  printf("Moving by %d %d %d\n", x, y, z);
  memset(&event, 0, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_X;
  event.value = x;
  write(fd, &event, sizeof(event));
  event.type = EV_REL;
  event.code = REL_Y;
  event.value = y;
  write(fd, &event, sizeof(event));
  event.type = EV_REL;
  event.code = REL_Z;
  event.value = z;
  write(fd, &event, sizeof(event));
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(fd, &event, sizeof(event));
}

void press_button(int x)
{
  printf("Pressing button %d\n", x);
  memset(&event, 0, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = x;
  event.value = 1;
  write(fd, &event, sizeof(event));
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(fd, &event, sizeof(event));
}

void rls_button(int x)
{
  printf("Releasing button %d\n", x);
  memset(&event, 0, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = x;
  event.value = 0;
  write(fd, &event, sizeof(event));
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(fd, &event, sizeof(event));
}
