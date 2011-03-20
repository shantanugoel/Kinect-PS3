#   KINECT PS3
#   Make File
#
#   Author: Shantanu Goel (http://tech.shantanugoel.com/)
#
#   Copyright (C) 2011 Shantanu Goel
#   This file is part of Kinect-PS3
#   Kinect-PS3 is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   Kinect PS3 is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not , see <http://www.gnu.org/licenses/>.

#The Makefile assumes that this program's source folder is kept in the
#NITE Samples Directory. If the location is different, please
#adjust the paths accordingly
SRC_FILES = main.cpp kps3-input.cpp signal_catch.cpp kbhit.cpp

EXE_NAME = kinect-ps3

ifndef TARGETFS
	TARGETFS=/
endif

include ../NiteSampleMakefile
