/****************************************************************************

   KINECT PS3
   Main Module

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
#include <unistd.h>

#include <XnOpenNI.h>
#include <XnVSessionManager.h>
#include "XnVMultiProcessFlowClient.h"
#include <XnVWaveDetector.h>
#include <XnVSelectableSlider2D.h>
#include <XnVFlowRouter.h>

#include "kps3-input.h"
#include <linux/input.h>
#include "kbhit.h"
#include "signal_catch.h"

#define SAMPLE_XML_FILE "../../Data/Sample-Tracking.xml"
#define KINECT_PS3_VERSION "0.0.1"

XnBool g_bQuit = false;

XnUInt32 g_TP_XDim = 10;
XnUInt32 g_TP_YDim = 10;
XnUInt32 g_TP_XPrev = 0;
XnUInt32 g_TP_YPrev = 0;

int g_ScrollingOn = 1;
int g_numFwdSelect = 0;
int g_numBkwdSelect = 0;

int mouseX = 0;
int mouseY = 0;
int mouseZ = 0;
int mousePrevX = 0;
int mousePrevY = 0;
int mousePrevZ = 0;
int depth_averaging_mode = 1;
int num_samples_for_depth_avg = 10;
int cur_num_samples = 0;
int depth_avg = 0;
int movement_for_depth_change_action = 150;
int dir_button = 0;
int dir_button_prev = 0;

typedef enum
{
  PROFILE_UNKNOWN = 0,
  PROFILE_MENU,
  PROFILE_GAME
} profile_t;

profile_t g_curProfile = PROFILE_UNKNOWN;

#define PROFILE_DEFAULT PROFILE_MENU

XnVSessionManager*     g_pSessionManager = NULL;
XnVSelectableSlider2D* g_pTrackPad = NULL;
XnVWaveDetector*       g_pWaveDetector = NULL;
XnVFlowRouter*         g_pFlowRouter = NULL;

void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem,
                                          XnVDirection eDir, void* cxt);

void XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus,
                                      const XnPoint3D& ptFocusPoint,
                                      XnFloat fProgress, void* UserCxt)
{
	printf("Session progress (%6.2f,%6.2f,%6.2f) - %6.2f [%s]\n", ptFocusPoint.X,
          ptFocusPoint.Y, ptFocusPoint.Z, fProgress,  strFocus);
}

void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt)
{
	printf("Session started. Please wave (%6.2f,%6.2f,%6.2f)...\n",
         ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z);
}

void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
	printf("Session ended. Please perform focus gesture to start session\n");
}

void XN_CALLBACK_TYPE OnPointUpdate(const XnVHandPointContext* pContext,
                                    void* cxt)
{
	printf("%d: (%f,%f,%f) [%f]\n", pContext->nID, pContext->ptPosition.X,
         pContext->ptPosition.Y, pContext->ptPosition.Z, pContext->fTime);
  mouseX = pContext->ptPosition.X - mousePrevX;
  mouseY = -(pContext->ptPosition.Y) + mousePrevY;
  mouseZ = pContext->ptPosition.Z - mousePrevZ;
  printf("%d %d %d %d %d %d\n", mouseX, mouseY, mouseZ, mousePrevX, mousePrevY,
         mousePrevZ);
  mousePrevX = pContext->ptPosition.X;
  mousePrevY = pContext->ptPosition.Y;
  mousePrevZ = pContext->ptPosition.Z;
  /*
    This works for using mouse "Minority Report" style on PC but somehow doesn't
    work too well on PS3. Maybe PS3 requires absolute positions instead of
    relative.
  */
  
  //move_cursor(mouseX, mouseY, mouseZ);

  move_cursor((int)(pContext->ptPosition.X/4),(int) -(pContext->ptPosition.Y/4),
              (int) (pContext->ptPosition.Z/4));
}

void XN_CALLBACK_TYPE TrackPad_ItemHover(XnInt32 nXItem, XnInt32 nYItem,
                                         void* cxt)
{
  printf("Hover %d %d\n", nXItem, nYItem);
  g_numFwdSelect = 0;
  g_numBkwdSelect = 0;
  int key = 0;
  if(g_ScrollingOn == 1)
  {
    if(nXItem > g_TP_XPrev)
    {
      printf("Right\n");
      key = KEY_D;
    }
    else if(nXItem < g_TP_XPrev)
    {
      printf("Left\n");
      key = KEY_A;
    }
    g_TP_XPrev = nXItem;
    if(nYItem > g_TP_YPrev)
    {
      printf("UP\n");
      key = KEY_W;
    }
    else if(nYItem < g_TP_YPrev)
    {
      printf("Down\n");
      key = KEY_S;
    }
    if(0 != key)
    {
      press_button(key);
      /*
         Using a 100ms delay between key press and key release, otherwise
         many times the key press wasn't picked up correctly and only key
         release event was being sent to the PS3.
       */
      usleep(100*1000);
      rls_button(key);
    }
    g_TP_XPrev = nXItem;
    g_TP_YPrev = nYItem;
  }
}

int SwitchProfile(profile_t profile)
{
  if(profile != g_curProfile)
  {
    switch(profile)
    {
      case PROFILE_MENU:
        {
          printf("Activating menu profile\n");
          g_pFlowRouter->SetActive(g_pTrackPad);
          break;
        }
      case PROFILE_GAME:
        {
          printf("Activating game profile\n");
          g_pFlowRouter->SetActive(g_pWaveDetector);
          break;
        }
    }
    g_curProfile = profile;
  }
}

void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem,
                                          XnVDirection eDir, void* cxt)
{
	printf("Select: %d,%d (%s)\n", nXItem, nYItem, XnVDirectionAsString(eDir));
  if(DIRECTION_BACKWARD == eDir)
  {
    g_numFwdSelect = 0;
    /*
      1 backward selection toggles scrolling with hand.
      3 successive backward selections switch the profile
    */
    if(g_numBkwdSelect < 2)
    {
      g_numBkwdSelect++;
      if(g_ScrollingOn == 0)
      {
        g_ScrollingOn = 1;
      }
      else
      {
        g_ScrollingOn = 0;
      }
      g_TP_XPrev = nXItem;
      g_TP_YPrev = nYItem;
      printf("Toggled Scrolling to %d\n", g_ScrollingOn);
    }
    else
    {
      g_numBkwdSelect = 0;
      printf("Switching Profiles\n");
      if(g_curProfile == PROFILE_MENU)
      {
        SwitchProfile(PROFILE_GAME);
      }
      else
      {
        SwitchProfile(PROFILE_MENU);
      }
    }
  }
  else if(DIRECTION_FORWARD == eDir)
  {
    g_numBkwdSelect = 0;
    /*
      2 successive forward selections will activate/select the current item.
    */
    if(g_numFwdSelect == 0)
    {
      g_numFwdSelect++;
      printf("Waiting for 2nd select\n");
    }
    else if(g_numFwdSelect == 1)
    {
      g_numFwdSelect = 0;
      printf("Selecting Item\n");
      press_button(KEY_X);
      usleep(100*1000);
      rls_button(KEY_X);
    }

  }
}

void cleanup()
{
  if(NULL != g_pSessionManager)
  {
    delete g_pSessionManager;
  }
  if(NULL != g_pFlowRouter)
  {
    delete g_pFlowRouter;
  }
  if(NULL != g_pTrackPad)
  {
    delete g_pTrackPad;
  }
  if(NULL != g_pWaveDetector)
  {
    delete g_pWaveDetector;
  }
}

int main(int argc, char** argv)
{
  xn::Context context;

  if(argc > 1)
  {
    if(strcmp(argv[1], "-h"))
    {
      printf("Invalid Args\n");
    }
    else
    {
      printf("Kinect PS3 by Shantanu Goel (http://tech.shantanugoel.com/) - \
              Version %s\n", KINECT_PS3_VERSION);
      exit(0);
    }
  }

  XnStatus rc = context.InitFromXmlFile(SAMPLE_XML_FILE);
  if (rc != XN_STATUS_OK)
  {
    printf("Couldn't initialize: %s\n", xnGetStatusString(rc));
    return 1;
  }

  setup_uinput_device();

  g_pSessionManager = new XnVSessionManager();
  rc = ((XnVSessionManager*)g_pSessionManager)->Initialize(&context, "Wave",
                                                           "Wave");
  if (rc != XN_STATUS_OK)
  {
    printf("Session Manager couldn't initialize: %s\n", xnGetStatusString(rc));
    goto cleanup_exit;
  }

  if(NULL == g_pTrackPad)
  {
	  g_pTrackPad = new XnVSelectableSlider2D(g_TP_XDim, g_TP_YDim);
  }
  if(NULL == g_pTrackPad)
  {
    printf("Couldn't create TrackPad\n");
    goto cleanup_exit;
  }
  if(NULL == g_pWaveDetector)
  {
	  g_pWaveDetector = new XnVWaveDetector();
  }
  if(NULL == g_pWaveDetector)
  {
    printf("Couldn't create Wave Detector\n");
    goto cleanup_exit;
  }
  if(NULL == g_pFlowRouter)
  {
	  g_pFlowRouter = new XnVFlowRouter();
  }
  if(NULL == g_pFlowRouter)
  {
    printf("Couldn't create Flow Router\n");
    goto cleanup_exit;
  }

  context.StartGeneratingAll();

  g_pSessionManager->RegisterSession(NULL, &SessionStart, &SessionEnd,
                                     &SessionProgress);

  CatchSignals(&g_bQuit);

  g_pWaveDetector->RegisterPointUpdate(NULL, OnPointUpdate);
  g_pTrackPad->RegisterItemHover(NULL, &TrackPad_ItemHover);
  g_pTrackPad->RegisterItemSelect(NULL, &TrackPad_ItemSelect);

  SwitchProfile(PROFILE_MENU);

  g_pSessionManager->AddListener(g_pFlowRouter);

  printf("Please perform focus gesture to start session\n");
  printf("Hit any key to exit\n");

  // Main loop
  while ((!_kbhit()) && (!g_bQuit))
  {
    context.WaitAndUpdateAll();
    ((XnVSessionManager*)g_pSessionManager)->Update(&context);
  }

cleanup_exit:
  cleanup();
  context.Shutdown();
  return 0;
}
