/*
->====================================================<-
->= Mousemeter.sbar - © Copyright 2013-2020 OnyxSoft =<-
->====================================================<-
->= Version  : 1.3                                   =<-
->= File     : Mousemeter.sbar.h                     =<-
->= Author   : Stefan Blixth                         =<-
->= Compiled : 2020-11-19                            =<-
->====================================================<-

0xfed4 Stefan 'Develin' Blixth <stefan@onyxsoft.se>
*/

#ifndef _MOUSEMETERSBAR_
#define _MOUSEMETERSBAR_

/* System... */
#include <exec/interrupts.h>
#include <utility/tagitem.h>
#include <libraries/mui.h>
#include <intuition/screenbar.h>
#include <clib/alib_protos.h>
#include <clib/debug_protos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <devices/newmouse.h>
#include <devices/rawkeycodes.h>
#include <mui/Rawimage_mcc.h>
#include <mui/Crawling_mcc.h>
#include <math.h>

#define  CATCOMP_ARRAY
#include "Mousemeter.sbar_locale.h"

const char *UsedClasses[] = {MUIC_Crawling, MUIC_Rawimage, NULL};
struct Library *UtilityBase = NULL;

/*----------------------------------------------------------------------------*
 *                                  Defines                                   *
 *----------------------------------------------------------------------------*/
#define VYEARS                "2013-2020"
#define AUTHOR                "Stefan Blixth"
#define EMAIL                 "stefan@onyxsoft.se"
#define URL                   "www.onyxsoft.se"
#define COMPANY               "OnyxSoft"
#define COPYRIGHT             "© " VYEARS " " AUTHOR ", " COMPANY
#define DATE                  __AMIGADATE__
#define VERSION               1
#define STRVERSION            "1"
#define REVISION              3
#define STRREVISION           "3"
#define CLASS                 "Mousemeter.sbar"
#define SUPERCLASS            MUIC_Group
#define _Dispatcher           Mousemeter_Sbar
#define UserLibID             "\0$VER: " CLASS " " STRVERSION "." STRREVISION " (" DATE ") @ " VYEARS " " AUTHOR
#define MASTERVERSION         20
#define USEDCLASSES           UsedClasses

#define LIBQUERYCLASS         QUERYCLASS_INTUITION_SCREENBAR
#define LIBQUERYID            CLASS " " STRVERSION "." STRREVISION " (" DATE ") @ " VYEARS " " AUTHOR
#define LIBQUERYDESCRIPTION   "Mousemeter Titlebar Plugin"
#define MASTERVERSION         20

#define VERS                  LIBQUERYDESCRIPTION " "STRVERSION"."STRREVISION
#define VSTRING               VERS" ("DATE") © "VYEARS " " AUTHOR ", " COMPANY
#define VERSTAG               "$VER:" VSTRING
#define VERSTAG_MUI           "$VER: "VERS " ("DATE")"
#define VERSTAG_SCREEN        CLASS " "STRVERSION"."STRREVISION " ("DATE") "
#define SBARABOUT             "\33c\33b" CLASS " " STRVERSION "." STRREVISION " (" DATE ")\33n\n" COPYRIGHT


// MCC/SBar tags...
#define DEVELIN_SERIAL             0xFED4
#define DEVELIN_TAGBASE_PUBLIC     0xFED40001
#define DEVELIN_TAGBASE_PRIVATE    0xFED4f000
#define SBARCFG_MMETER_MONITORSIZE 0x7ED48011
#define SBARCFG_MMETER_DISTANCE    0x7ED48012
#define SBARCFG_MMETER_SPEED       0x7ED48013
#define SBARCFG_MMETER_SHOWSTART   0x7ED48014
#define SBARCFG_MMETER_SHOWCLICK   0x7ED48015
#define SBARCFG_MMETER_SHOWWHEEL   0x7ED48016

#define LUNIT_METER                0
#define LUNIT_INCH                 1
#define LUNIT_KMETER               2
#define LUNIT_MILES                3

#define STARTNUMBER               (1)
#define TAGBASE_MOUSEMETER        (TAG_USER | (STARTNUMBER<<16))
#define MUIM_TriggerSbar          (TAGBASE_MOUSEMETER | 0x0001)
#define MUIV_SbarPressed          (TAGBASE_MOUSEMETER | 0x0002)
#define MUIA_Mousemeter_Reset     (TAGBASE_MOUSEMETER | 0x0003)

#define MOUSEMETER_MAXWIDTH       400
#define MOUSEMETER_MAXHEIGHT      300
#define MOUSEMETER_MINWIDTH       20
#define MOUSEMETER_MINHEIGHT      10

static char *str_len[]= { "meter", "inch", "kilometer", "miles", NULL };
static char *str_spd[]= { "meter/min", "inch/min", "km/h", "mph", NULL};

/*----------------------------------------------------------------------------*
 *                              MouseHandler Data                             *
 *----------------------------------------------------------------------------*/
typedef struct InterruptData
{
  long sig;
  struct Task *task;
  struct ExecBase *SysBase;
  struct EmulLibEntry InterruptFunc;
  struct InputEvent *ievent;
}InterruptData;

/*----------------------------------------------------------------------------*
 *                              Global Data                                   *
 *----------------------------------------------------------------------------*/
ULONG mouseclicks = 0;
ULONG mousewheel = 0;
struct MsgPort *MPort = NULL;
struct Interrupt *IntHandler = NULL;
struct IOStdReq *IOReq = NULL;
BOOL OpenedDevice = FALSE;
BOOL FirstInstance = TRUE;
InterruptData *IData = NULL;
LONG Sig;


double GlobalXDist;
double GlobalYDist;
double GlobalXDiff;
double GlobalYDiff;
double GlobalTotal;
double GlobalSpeed;
BOOL FirstWinInst = FALSE;

/*----------------------------------------------------------------------------*
 *                              Instance Data                                 *
 *----------------------------------------------------------------------------*/
struct Data
{
   UBYTE str_totlen[256];
   UBYTE str_xlen[256];
   UBYTE str_ylen[256];
   UBYTE str_spd[256];
   UBYTE str_clicks[256];
   UBYTE str_wheel[256];

   Object *win_mmeter;
   Object *str_xdistance;
   Object *str_ydistance;
   Object *str_totdistance;
   Object *str_speed;
   Object *str_totclicks;
   Object *str_totwheel;
   APTR   sbaricon;
   APTR   resetbutton;

   double dpi;
   double totalxlength;
   double totalylength;
   double totaldiffxlength;
   double totaldiffylength;
   double totallength;

   ULONG  currentxdiff;
   ULONG  currentydiff;

   ULONG  totalxpos;
   ULONG  totalypos;
   ULONG  counter;
   ULONG  mouseclicks;
   ULONG  mousewheel;

   WORD   screenwidth;
   WORD   screenheigth;
   WORD   currentxpos;
   WORD   currentypos;
   WORD   oldxpos;
   WORD   oldypos;
   WORD   cursor;

   WORD   oldwidth;
   WORD   oldheigth;
   UBYTE  oldsize;

   UBYTE  monitorsize;      // Default = 17 Inch.
   UBYTE  lengthunit;       // 0 = meter, 1 = Inch //, 2 = km, 3 = miles
   UBYTE  iconid;           // Sbar icon ID... 
   BOOL   winopen;          // TRUE = Window Open, FALSE = Window Closed
   BOOL   localed;          // TRUE = Locale-handling initiated...
   BOOL   cleared;          // TRUE = Stats has been cleared by user, FALSE = Normal state...
   BOOL   showonstart;      // Our main windows is open on start (TRUE) or not (FALSE)
   BOOL   showclicks;       // Show mouse button clicks (TRUE) or not (FALSE)
   BOOL   showwheel;        // Show wheel mouse movements (TRUE) or not (FALSE)
   BOOL   firstinstance;    //

   struct Screen *screen;
   struct MUI_InputHandlerNode ihnode;
};

#endif // _MOUSEMETERSBAR_
