/*
->====================================================<-
->= Mousemeter.sbar - © Copyright 2013-2020 OnyxSoft =<-
->====================================================<-
->= Version  : 1.2                                   =<-
->= File     : Mousemeter.sbar.c                     =<-
->= Author   : Stefan Blixth                         =<-
->= Compiled : 2020-11-18                            =<-
->====================================================<-

0xfed4 Stefan 'Develin' Blixth <stefan@onyxsoft.se>
*/

#include "debug.h"
#include "Mousemeter.sbar.h"
#include "Mousemeter.sbar_gfx.h"

/*=----------------------------- Patches and Macros()-------------------------*
 * Patching some incompatible functions and adds some useful macros           *
 *----------------------------------------------------------------------------*/
 #ifndef RedrawQ
 #define RedrawQ(m, o, flg) \
   do { \
   if ((((flg)==0) || ((flg)==1 && muiRenderInfo(o))) && (m->redrawcount < (LONG)(sizeof(m->redrawobj)/sizeof(m->redrawobj[0])))) \
   { \
      m->redrawflags[m->redrawcount] = flg; \
      m->redrawobj[m->redrawcount++] = o; \
   } } while(0)
 #endif

#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright (obj)) \
                     && _between(_mtop(obj) ,(y),_mbottom(obj)))

#define HORIZ_SPACING   2
#define GETWIDTH(id)    (_Mousemeter_icon_gfx[id][3]) // Fetch width from the gfx-data array header...
#define GETHEIGHT(id)   (_Mousemeter_icon_gfx[id][7]) // Fetch height from the gfx-data array header...

#define DISPATCHERNAME_BEGIN(Name)         \
ULONG Name(void) { Class *cl=(Class*) REG_A0; Msg msg=(Msg) REG_A1; Object *obj=(Object*) REG_A2; switch (msg->MethodID) {

#define DISPATCHER_END } return DoSuperMethodA(cl,obj,msg);}

/*=*/

#include <proto/exec.h> // To make pre/post functions to work...

/*=------------------------ SetInt() ---------------------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
void SetInt(InterruptData *data, struct Interrupt *inte, void *f, char *name)
{
   data->SysBase = SysBase;
   data->InterruptFunc.Trap = TRAP_LIB;
   data->InterruptFunc.Extension = 0;
   data->InterruptFunc.Func = (void(*)(void)) f;
   inte->is_Code = (void(*)(void)) &data->InterruptFunc;
   inte->is_Node.ln_Type = NT_UNKNOWN;
}
/*=*/

/*=------------------------ IEvent() ---------------------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
struct InputEvent *IEvent(void)
{
   struct InputEvent *ie     = (APTR) REG_A0;
   struct MsgPort *port      = (APTR) REG_A1;
   struct ExecBase *SysBase  = SysBase;
   struct InputEvent *iet    = ie;
   
   (void)port;
   
   if (iet->ie_Class == IECLASS_RAWMOUSE)
   {
      switch (iet->ie_Code)
      {
         case IECODE_LBUTTON :
            mouseclicks++;
            debug_print("MouseMeter.sbar : %s (%d) - IECODE_LBUTTON\n", __func__, __LINE__);
            break;
  
         case IECODE_RBUTTON :
            mouseclicks++;
            debug_print("MouseMeter.sbar : %s (%d) - IECODE_RBUTTON\n", __func__, __LINE__);
            break;
 
         case IECODE_MBUTTON :
            mouseclicks++;
            debug_print("MouseMeter.sbar : %s (%d) - IECODE_MBUTTON\n", __func__, __LINE__);
            break;
      }
   }
 
   if (iet->ie_Class == IECLASS_NEWMOUSE)
   {
      switch (iet->ie_Code)
      {
         case NM_WHEEL_UP :
            mousewheel++;
            debug_print("MouseMeter.sbar : %s (%d) - NM_WHEEL_UP\n", __func__, __LINE__);
            break;

         case NM_WHEEL_DOWN :
            mousewheel++;
            debug_print("MouseMeter.sbar : %s (%d) - NM_WHEEL_DOWN\n", __func__, __LINE__);
            break;

         case NM_WHEEL_LEFT :
            mousewheel++;
            debug_print("MouseMeter.sbar : %s (%d) - NM_WHEEL_LEFT\n", __func__, __LINE__);
            break;

         case NM_WHEEL_RIGHT :
            mousewheel++;
            debug_print("MouseMeter.sbar : %s (%d) - NM_WHEEL_RIGHT\n", __func__, __LINE__);
            break;

         case NM_BUTTON_FOURTH :
            mouseclicks++;
            debug_print("MouseMeter.sbar : %s (%d) - NM_BUTTON_FOURTH\n", __func__, __LINE__);
            break;
      }
   }

   return (ie);
}
/*=*/

#define PostClassExit
void PostClassExitFunc(void)
{
   if (OpenedDevice)
   {
      debug_print("MouseMeter.sbar : %s (%d) - MouseHandler disabled!\n", __func__, __LINE__);
      IOReq->io_Data = (APTR)IntHandler;
      IOReq->io_Command = IND_REMHANDLER;
      DoIO((struct IORequest *)IOReq);

      if (OpenedDevice) CloseDevice((struct IORequest*)IOReq);
      if (IOReq) DeleteIORequest ((struct IORequest*)IOReq);
      if (IntHandler) FreeVec(IntHandler);
      if (MPort) DeleteMsgPort(MPort);
      if (IData) FreeVec(IData);
   }
}
#define PreClassInit
BOOL PreClassInitFunc(void)
{
   if ((IData = (InterruptData *) AllocVec(sizeof(InterruptData), MEMF_PUBLIC|MEMF_CLEAR)))
   {
      if ((MPort = CreateMsgPort()))
      {
         if ((IntHandler = AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR)))
         {
            if ((IOReq = (struct IOStdReq *) CreateIORequest(MPort, sizeof(struct IOStdReq))))
            {
               if ((!OpenDevice ("input.device", 0, (struct IORequest *)IOReq, 0)))
                  OpenedDevice = TRUE;
            }
         }
      }
   }

   if (OpenedDevice)
   {
      IData->sig = Sig; // Kolla denna
      IData->task = FindTask(0);
      SetInt(IData, IntHandler, &IEvent, "MousemeterHandler");
        
      IData->ievent = NULL;
      IntHandler->is_Data = IData;
      IntHandler->is_Node.ln_Name = "MousemeterHandler";
      IntHandler->is_Node.ln_Type = NT_UNKNOWN;
      IntHandler->is_Node.ln_Pri = 60;

      IOReq->io_Message.mn_ReplyPort = MPort;
      IOReq->io_Command = IND_ADDHANDLER;
      IOReq->io_Data = (APTR)IntHandler;

      DoIO((struct IORequest*)IOReq);

      SetSignal(0, (1L << MPort->mp_SigBit));

      FirstInstance = TRUE;
      FirstWinInst  = FALSE;

      return (TRUE);
   }
  
   return (FALSE);
}

#include <mui/mccheader.c>


/*=------------------------ CreateDistanceString() -------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
void CreateDistanceString(char *strtemp, double value, UBYTE unit)
{
   if (unit > LUNIT_MILES)
      unit = LUNIT_MILES;

   if (unit > LUNIT_INCH)
      NewRawDoFmt("%.5lf %s", NULL, strtemp, value, str_len[unit]);
   else
      NewRawDoFmt("%.2lf %s", NULL, strtemp, value, str_len[unit]);
}
/*=*/

/*=------------------------ CreateSpeedString() ----------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
void CreateSpeedString(char *strtemp, double value, UBYTE unit)
{
   if (unit > LUNIT_MILES)
      unit = LUNIT_MILES;

   if (unit > LUNIT_INCH)
      NewRawDoFmt("%.5lf %s", NULL, strtemp, value, str_spd[unit]);
   else
      NewRawDoFmt("%.2lf %s", NULL, strtemp, value, str_spd[unit]);
}
/*=*/

/*=------------------------ CreateClickWheelString() -----------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
void CreateClickWheelString(char *strtemp, unsigned long value)
{
   NewRawDoFmt("%d", NULL, strtemp, value);
}
/*=*/

/*=------------------------ mousemeter_sbar_new() --------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_new(struct IClass *cl, Object *obj, struct opSet *msg)
{
   APTR imagebutton;
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   if (!(UtilityBase = (struct Library *)OpenLibrary("utility.library", 39)))
      return(FALSE);

   Locale_Open("Mousemeter_sbar.catalog", 1, 0);

   obj = DoSuperNew(cl, obj,
          InnerSpacing(HORIZ_SPACING, 0),
          MUIA_Group_Horiz,   FALSE,
          MUIA_Group_Spacing, 0,
          MUIA_ShortHelp,     TRUE,
          Child, VSpace(0),
          Child, imagebutton = RawimageObject,
             MUIA_DoubleBuffer, 0,
             MUIA_Rawimage_Data, _Mousemeter_icon_gfx[0],
             MUIA_InputMode, MUIV_InputMode_RelVerify, 
          End,
          Child, VSpace(0),

          TAG_MORE, msg->ops_AttrList);
        
   if (obj)
   {
      struct Data *data = INST_DATA(cl, obj);

      data->screen       = NULL;
      data->monitorsize  = 17;

      data->totalxpos    = 0;
      data->totalypos    = 0;
      data->oldxpos      = 0;
      data->oldypos      = 0;
      data->counter      = 0;
      data->cursor       = 0;
      data->mouseclicks  = 0;
      data->mousewheel   = 0;
   
      data->iconid       = 0;

      data->cleared      = FALSE;
      data->localed      = TRUE;
      data->showonstart  = FALSE;
      data->showclicks   = TRUE;
      data->showwheel    = TRUE;

      data->lengthunit   = LUNIT_METER;
      data->sbaricon     = imagebutton;
      data->winopen      = FALSE;
      
      if (FirstInstance)
      {
         data->firstinstance = TRUE;
         FirstInstance = FALSE;
      }
      else
      {
         data->firstinstance = FALSE;
      }

      DoMethod(imagebutton, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_HandleInput);
   }
   return((ULONG)obj);
}
/*=*/

/*=------------------------ mousemeter_sbar_get() ----------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_get(struct IClass *cl, Object *obj, Msg msg)
{
   struct Data *data = INST_DATA(cl, obj);
   ULONG *store = ((struct opGet *)msg)->opg_Storage;
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   switch (((struct opGet *)msg)->opg_AttrID)
   {
      case MUIA_Screenbar_DisplayedImage:
         *store = (LONG)RawimageObject,
         MUIA_Rawimage_Data, _Mousemeter_icon_gfx[2],
         End;
         return TRUE;

      case MUIA_Version:
         *store = (LONG)VERSION;
         debug_print("MouseMeter.sbar : %s (%d) - MUIA_Version\n", __func__, __LINE__);
         break;

      case MUIA_Revision:
         *store = (LONG)REVISION;
         debug_print("MouseMeter.sbar : %s (%d) - MUIA_Revision\n", __func__, __LINE__);
         break;

      case MUIA_Screenbar_DisplayedName:
         *store = (LONG)Locale_GetString(MSG_APPLICATION_NAME);
         debug_print("MouseMeter.sbar : %s (%d) - MUIA_Screenbar_DisplayedName\n", __func__, __LINE__);
         return TRUE;
         break;
   }
   return(DoSuperMethodA(cl,obj,msg));
}
/*=*/

/*=------------------------ mousemeter_sbar_set() ----------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_set(struct IClass *cl, Object *obj, Msg msg)
{
   struct Data *data = INST_DATA(cl, obj);
   struct TagItem *tags, *tag;
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   for ((tags=((struct opSet *)msg)->ops_AttrList); (tag=NextTagItem(&tags)); )
   {
      switch (tag->ti_Tag)
      {
         case MUIA_Mousemeter_Reset :
            data->totalxpos        = 0;
            data->totalypos        = 0;
            data->totalxlength     = 0;
            data->totalylength     = 0;
            data->totallength      = 0;
            data->totaldiffxlength = 0;
            data->totaldiffylength = 0;
            data->currentypos      = 0;
            data->currentxpos      = 0;
            data->oldxpos          = 0;
            data->oldypos          = 0;
            data->counter          = 0;
            data->cursor           = 0;

            mouseclicks            = 0;
            mousewheel             = 0;

            data->cleared          = TRUE;
            break;
      }
   }

   return(DoSuperMethodA(cl, obj, msg));
}
/*=*/

/*=------------------------ mousemeter_sbar_dispose() ----------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_dispose(struct IClass *cl, Object *obj, Msg msg)
{
   struct Data *data = INST_DATA(cl, obj);
   debug_print("Mousemeter.sbar : %s (%d)\n", __func__, __LINE__);
   if (UtilityBase) CloseLibrary((struct Library *) UtilityBase);
   return (DoSuperMethodA(cl, obj, msg));
}
/*=*/

/*=------------------------ mousemeter_sbar_setup() ------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_setup(struct IClass *cl, Object *obj, Msg msg)
{
   struct Data *data = INST_DATA(cl, obj);
   ULONG rc = DoSuperMethodA(cl, obj, msg);
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   if (data->localed == FALSE)
   {
      data->localed = TRUE;
      Locale_Open("Mousemeter_sbar.catalog", 1, 0);
   }

   if (rc)
   {
      ULONG newmonsize, newdistunit, showstartup, showclicks, showwheel;
      ULONG barheight = _screen(obj)->BarHeight + 1;
      ULONG newid     = 0;

      DoMethod(obj, MUIM_GetConfigItem, SBARCFG_MMETER_MONITORSIZE, (ULONG)&newmonsize);
      DoMethod(obj, MUIM_GetConfigItem, SBARCFG_MMETER_DISTANCE,    (ULONG)&newdistunit);
      DoMethod(obj, MUIM_GetConfigItem, SBARCFG_MMETER_SHOWSTART,   (ULONG)&showstartup);
      DoMethod(obj, MUIM_GetConfigItem, SBARCFG_MMETER_SHOWCLICK,   (ULONG)&showclicks);
      DoMethod(obj, MUIM_GetConfigItem, SBARCFG_MMETER_SHOWWHEEL,   (ULONG)&showwheel);

      data->monitorsize = newmonsize;
      data->lengthunit  = newdistunit;
      data->showonstart = showstartup;
      data->showclicks  = showclicks;
      data->showwheel   = showwheel;

      data->ihnode.ihn_Object  = obj;
      data->ihnode.ihn_Millis  = 100;
      data->ihnode.ihn_Method  = MUIM_TriggerSbar;
      data->ihnode.ihn_Flags   = MUIIHNF_TIMER;
      DoMethod(_app(obj), MUIM_Application_AddInputHandler, &data->ihnode);

      if      (barheight >= 26) { newid = 5; } // 22 pixels
      else if (barheight >= 24) { newid = 4; } // 20 pixels
      else if (barheight >= 22) { newid = 3; } // 18 pixels
      else if (barheight >= 18) { newid = 2; } // 15 pixels
      else if (barheight >= 15) { newid = 1; } // 13 pixels
      else                      { newid = 0; } // 11 pixels

      set(data->sbaricon, MUIA_Rawimage_Data, _Mousemeter_icon_gfx[newid]);
      data->iconid = newid;

      if (data->showonstart)
      {
         if (data->firstinstance)
            DoMethod(obj, MUIM_HandleInput);
      }

      return rc;
   }
   return FALSE;
}
/*=*/

/*=------------------------ mousemeter_sbar_cleanup() ----------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_cleanup(struct IClass *cl, Object *obj, Msg msg)
{
   struct Data *data = INST_DATA(cl, obj);
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   if (data->winopen)
   {
      set(data->win_mmeter, MUIA_Window_Open, FALSE);
      DoMethod(_app(obj), OM_REMMEMBER, data->win_mmeter);

      if (data->win_mmeter) MUI_DisposeObject(data->win_mmeter);
      data->win_mmeter = NULL;

      data->winopen = FALSE;
 
      if (data->firstinstance)
         FirstWinInst = FALSE;
   }

   Locale_Close();
   data->localed = FALSE;

   DoMethod(_app(obj), MUIM_Application_RemInputHandler, &data->ihnode);
   return DoSuperMethodA(cl, obj, msg);
}
/*=*/

/*=------------------------ mousemeter_sbar_handler() ------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_handler(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
   struct Data *data = INST_DATA(cl, obj);
   static ULONG counter;
   double xdist, ydist, xdiff, ydiff, spddiff, tdist;
   //APTR mclicks, mwheel;
   debug_print("MouseMeter.sbar : %s (%d) - Instance = %d\n", __func__, __LINE__, data->firstinstance);

   if (data->winopen)
   {
      set(data->win_mmeter, MUIA_Window_Open, FALSE);
      DoMethod(_app(obj), OM_REMMEMBER, data->win_mmeter);

      if (data->win_mmeter) MUI_DisposeObject(data->win_mmeter);
      data->win_mmeter = NULL;
      data->winopen = FALSE;
 
      if (data->firstinstance)
         FirstWinInst = FALSE;
   }
   else
   {
      if (data->firstinstance)
      {
         data->totallength += (data->totaldiffxlength + data->totaldiffylength);

         if (data->lengthunit == LUNIT_METER)
         {
            xdist = (data->totalxlength*0.0254);
            ydist = (data->totalylength*0.0254);
            xdiff = (data->totaldiffxlength*0.0254);
            ydiff = (data->totaldiffylength*0.0254);
            tdist = (data->totallength*0.0254);
         }
         else if (data->lengthunit == LUNIT_KMETER)
         {
            xdist = (data->totalxlength*0.0000254);
            ydist = (data->totalylength*0.0000254);
            xdiff = (data->totaldiffxlength*0.0000254);
            ydiff = (data->totaldiffylength*0.0000254);
            tdist = (data->totallength*0.0000254);
         }
         else if (data->lengthunit == LUNIT_MILES)
         {
            xdist = (data->totalxlength*0.0000157828283);
            ydist = (data->totalylength*0.0000157828283);
            xdiff = (data->totaldiffxlength*0.0000157828283);
            ydiff = (data->totaldiffylength*0.0000157828283);
            tdist = (data->totallength*0.0000157828283);
         }
         else
         {
            xdist = data->totalxlength;
            ydist = data->totalylength;
            xdiff = data->totaldiffxlength;
            ydiff = data->totaldiffylength;
            tdist = data->totallength;
         }
         
         GlobalXDist = xdist;
         GlobalYDist = ydist;
         GlobalXDiff = xdiff;
         GlobalYDiff = ydiff;
         GlobalTotal = tdist; //data->totallength;
         
         if (data->counter == 0)
            counter = 1;
         else
            counter = data->counter;

         spddiff = (double)(tdist*600) / (double)counter;

         if (data->lengthunit == LUNIT_KMETER)
            spddiff *= 0.06;
         else if (data->lengthunit == LUNIT_MILES)
            spddiff *= 0.0166666667;
            
         GlobalSpeed = spddiff;
         FirstWinInst = TRUE;
      }
      else
      {
         xdist = GlobalXDist;
         ydist = GlobalYDist;
         xdiff = GlobalXDiff;
         ydiff = GlobalYDiff;
         tdist = GlobalTotal;
         spddiff = GlobalSpeed;
      }

      CreateDistanceString(&data->str_xlen, xdist, data->lengthunit);
      CreateDistanceString(&data->str_ylen, ydist, data->lengthunit);
      CreateDistanceString(&data->str_totlen, data->totallength, data->lengthunit);
      CreateSpeedString(&data->str_spd, spddiff, data->lengthunit);
      CreateClickWheelString(&data->str_clicks, data->mouseclicks);
      CreateClickWheelString(&data->str_wheel, data->mousewheel);

      data->win_mmeter = WindowObject,
         MUIA_Window_ID,            MAKE_ID('M','A','I','N'),
         MUIA_Window_ScreenTitle,   VERSTAG_SCREEN,
         MUIA_Window_Title,         "Mousemeter",
         MUIA_Window_AppWindow,     FALSE,
         MUIA_Window_CloseGadget,   TRUE,
         MUIA_Window_DepthGadget,   FALSE,
         MUIA_Window_SizeGadget,    TRUE,

         WindowContents, VGroup,
            Child, HGroup, GroupFrame, MUIA_Group_Columns, 2,

               Child, data->resetbutton = RawimageObject,
                  MUIA_DoubleBuffer, 0,
                  MUIA_Rawimage_Data, _Mousemeter_button_gfx_bz2,
                  MUIA_InputMode, MUIV_InputMode_RelVerify,
                  MUIA_ShortHelp, Locale_GetString(MSG_GUI_SHORTHELP_BUTTONRESET),
               End,

               Child, VGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_VERTICAL),
                     End,
                     Child, data->str_xdistance = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_xlen,
                     End,
                  End,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_HORIZONTAL),
                     End,
                     Child, data->str_ydistance = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_ylen,
                     End,
                  End,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_TOTAL),
                     End,
                     Child, data->str_totdistance = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_totlen,
                     End,
                  End,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_AVERAGESPEED),
                     End,
                     Child, data->str_speed = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_spd,
                     End,
                  End,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     MUIA_ShowMe, data->showclicks,
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_MOUSECLICKS),
                     End,
                     Child, data->str_totclicks = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_clicks,
                     End,
                  End,
                  Child, HGroup, NoFrame, MUIA_Group_SameSize, TRUE,
                     MUIA_ShowMe, data->showwheel, 
                     Child, TextObject, NoFrame,
                        MUIA_Text_PreParse, "\033r",
                        MUIA_Text_Contents, Locale_GetString(MSG_GUI_LABEL_WHEELMOUSE),
                     End,
                     Child, data->str_totwheel = TextObject, NoFrame,
                        MUIA_Text_Contents, data->str_wheel,
                     End,
                  End,
               End,
            End,
         End,
      End,

      DoMethod(_app(obj), OM_ADDMEMBER, data->win_mmeter);
      DoMethod(data->win_mmeter, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 1, MUIM_HandleInput);
      DoMethod(data->resetbutton, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Set, MUIA_Mousemeter_Reset);

      set(data->win_mmeter, MUIA_Window_Open, TRUE);
      data->winopen = TRUE;
   }
   return 0;
}

/*=------------------------ mousemeter_sbar_askminmax() --------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_askminmax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
   ULONG rc = DoSuperMethodA(cl, obj, msg);
   struct MUI_MinMax *mm = msg->MinMaxInfo;
   struct Data *data = INST_DATA(cl, obj); 
   debug_print("MouseMeter.sbar : %s (%d) -> BarHeight = %d\n", __func__, __LINE__, _screen(obj)->BarHeight);

   mm->MinWidth  = mm->DefWidth  = mm->MaxWidth  = GETWIDTH(data->iconid) + (HORIZ_SPACING*2);
   mm->MinHeight = mm->DefHeight = mm->MaxHeight =  _screen(obj)->BarHeight + 1;
   return rc;
}
/*=*/

/*=------------------------ mousemeter_sbar_show() -------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_show(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
   struct Data *data = INST_DATA(cl, obj);
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);
   return(DoSuperMethodA(cl, obj, (Msg)msg));
}
/*=*/

/*=------------------------ mousemeter_sbar_trigger() ----------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_trigger(struct IClass *cl, Object *obj)
{
   struct Data *data = INST_DATA(cl, obj);
   double xpow, ypow, srot;
   double xdist, ydist, xdiff, ydiff, spddiff, tdist;
   BOOL newcalc = FALSE;
   BOOL newclick = FALSE;
#ifdef EXTDEBUG
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);
#endif

   if ((data->screen = _screen(obj)) != NULL)
   {
      if (data->cleared)
      {
         newcalc = TRUE;
         data->oldxpos = data->currentxpos = data->screen->MouseX;
         data->oldypos = data->currentypos = data->screen->MouseY;
         data->cleared = FALSE;
      }
      else
      {
         data->currentxpos  = data->screen->MouseX;
         data->currentypos  = data->screen->MouseY;
         data->screenwidth  = data->screen->Width;
         data->screenheigth = data->screen->Height;
      }

      // Calculate DPI with a little ugly formula =) (when needed),
      if ( (data->screenwidth != data->oldwidth) || (data->screenheigth != data->oldheigth) || (data->monitorsize != data->oldsize) )
      {
         xpow = pow(data->screenwidth, 2);
         ypow = pow(data->screenheigth, 2);
         srot = sqrt(xpow + ypow);
         data->dpi = srot / data->monitorsize;

         data->oldwidth = data->screenwidth;
         data->oldheigth = data->screenheigth;
         data->oldsize = data->monitorsize;
      }

      data->counter++;

      if (data->currentxpos != data->oldxpos)
      {
         data->currentxdiff = abs(data->currentxpos - data->oldxpos);
         data->totalxpos += data->currentxdiff;
         data->oldxpos = data->currentxpos;
         data->totalxlength = data->totalxpos / data->dpi;
         data->totaldiffxlength = data->currentxdiff / data->dpi;
         newcalc = TRUE;
      }

      if (data->currentypos != data->oldypos)
      {
         data->currentydiff = abs(data->currentypos - data->oldypos);
         data->totalypos += data->currentydiff;
         data->oldypos = data->currentypos;
         data->totalylength = data->totalypos / data->dpi;
         data->totaldiffylength = data->currentydiff / data->dpi;
         newcalc = TRUE;
      }

      if (data->mouseclicks != mouseclicks)
         newclick = TRUE;
         
      if (data->mousewheel != mousewheel)
         newclick = TRUE;

      if (newcalc)
      {
         data->cursor++;

         if (data->firstinstance)
         {
            data->totallength += (data->totaldiffxlength + data->totaldiffylength);

            if (data->lengthunit == LUNIT_METER)
            {
               xdist = (data->totalxlength*0.0254);
               ydist = (data->totalylength*0.0254);
               xdiff = (data->totaldiffxlength*0.0254);
               ydiff = (data->totaldiffylength*0.0254);
               tdist = (data->totallength*0.0254);
            }
            else if (data->lengthunit == LUNIT_KMETER)
            {
               xdist = (data->totalxlength*0.0000254);
               ydist = (data->totalylength*0.0000254);
               xdiff = (data->totaldiffxlength*0.0000254);
               ydiff = (data->totaldiffylength*0.0000254);
               tdist = (data->totallength*0.0000254);
            }
            else if (data->lengthunit == LUNIT_MILES)
            {
               xdist = (data->totalxlength*0.0000157828283);
               ydist = (data->totalylength*0.0000157828283);
               xdiff = (data->totaldiffxlength*0.0000157828283);
               ydiff = (data->totaldiffylength*0.0000157828283);
               tdist = (data->totallength*0.0000157828283);
            }
            else
            {
               xdist = data->totalxlength;
               ydist = data->totalylength;
               xdiff = data->totaldiffxlength;
               ydiff = data->totaldiffylength;
               tdist = data->totallength;
            }

            GlobalXDist = xdist;
            GlobalYDist = ydist;
            GlobalXDiff = xdiff;
            GlobalYDiff = ydiff;
            GlobalTotal = tdist; //data->totallength;
         }
         else
         {
            xdist = GlobalXDist;
            ydist = GlobalYDist;
            xdiff = GlobalXDiff;
            ydiff = GlobalYDiff;
            tdist = GlobalTotal;
         }

         if (data->winopen)
         {
            // X-Distance...
            CreateDistanceString(&data->str_xlen, xdist, data->lengthunit);
            set(data->str_xdistance, MUIA_Text_Contents, data->str_xlen);

            // Y-Distance...
            CreateDistanceString(&data->str_ylen, ydist, data->lengthunit);
            set(data->str_ydistance, MUIA_Text_Contents, data->str_ylen);

            // Total Distance...
            CreateDistanceString(&data->str_totlen, tdist, data->lengthunit);
            set(data->str_totdistance, MUIA_Text_Contents, data->str_totlen);

            // Total number of mouse click-operations (if enabled)
            if (data->showclicks)
            {
               data->mouseclicks = mouseclicks;
               CreateClickWheelString(&data->str_clicks, data->mouseclicks);
               set(data->str_totclicks, MUIA_Text_Contents, data->str_clicks);
            }

            // Total number of mouse wheel-operations (if enabled)
            if (data->showwheel)
            {
               data->mousewheel = mousewheel;
               CreateClickWheelString(&data->str_wheel, data->mousewheel);
               set(data->str_totwheel, MUIA_Text_Contents, data->str_wheel);
            }

            // Average speed calculations...
            if (data->cursor > 9)
            {
               if (data->firstinstance)
               {
                  spddiff = (double)(tdist*600) / (double)data->counter;

                  if (data->lengthunit == LUNIT_KMETER)
                     spddiff *= 0.06;
                  else if (data->lengthunit == LUNIT_MILES)
                     spddiff *= 0.0166666667;
                     
                  GlobalSpeed = spddiff;
               }
               else
               {
                  if (FirstWinInst == FALSE) // Make a temporary calculation if the first instance window is closed...
                  {
                     spddiff = (double)(tdist*600) / (double)data->counter;

                     if (data->lengthunit == LUNIT_KMETER)
                        spddiff *= 0.06;
                     else if (data->lengthunit == LUNIT_MILES)
                        spddiff *= 0.0166666667;
                  }
                  else
                  {
                     spddiff = GlobalSpeed;
                  }
               }
               
               CreateSpeedString(&data->str_spd, spddiff, data->lengthunit);

               set(data->str_speed, MUIA_Text_Contents, data->str_spd);
               data->cursor = 0;
            }
         }
      }
      if (newclick)
      {
         if (data->winopen)
         {
            // Total number of mouse click-operations (if enabled)
            if (data->showclicks)
            {
               data->mouseclicks = mouseclicks;
               CreateClickWheelString(&data->str_clicks, data->mouseclicks);
               set(data->str_totclicks, MUIA_Text_Contents, data->str_clicks);
            }

            // Total number of mouse wheel-operations (if enabled)
            if (data->showwheel)
            {
               data->mousewheel = mousewheel;
               CreateClickWheelString(&data->str_wheel, data->mousewheel);
               set(data->str_totwheel, MUIA_Text_Contents, data->str_wheel);
            }
         }
      }

      return (TRUE);
   }
   return (FALSE);
}
/*=*/

/*=------------------------ mousemeter_sbar_draw() -------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
   struct Data *data = INST_DATA(cl, obj);
   ULONG rc = DoSuperMethodA(cl, obj, msg);
#ifdef EXTDEBUG
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);
#endif

   if (msg->flags & MADF_DRAWUPDATE)
   {
      Object *parent = NULL;
      get(obj, MUIA_Parent, &parent);
   }
   return rc;
}
/*=*/

/*=------------------------ mousemeter_sbar_settings() ---------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_settings(struct IClass *cl, Object *obj, struct MUIP_Screenbar_BuildSettingsPanel *msg)
{
   Object *lbl_mon;
   Object *sli_mon;
   Object *lbl_len;
   Object *cyc_len;
   Object *lbl_opn;
   Object *chk_opn;
   Object *lbl_clk;
   Object *chk_clk;
   Object *lbl_whe;
   Object *chk_whe;

   Object *prefs = MUI_NewObject(MUIC_Mccprefs,

                     Child, HGroup, MUIA_Group_Columns, 2,
                        Child, lbl_mon = Label(Locale_GetString(MSG_SETTINGS_LABEL_MONITORSIZE)),
                        Child, sli_mon = SliderObject, MUIA_CycleChain, TRUE, SliderFrame, MUIA_Numeric_Min, 10, MUIA_Numeric_Max, 100, MUIA_Numeric_Value, 17, MUIA_Numeric_Format, Locale_GetString(MSG_SETTIGS_SLIDER_INCH), End,

                        Child, lbl_len = Label(Locale_GetString(MSG_SETTINGS_LABEL_UNIT)),
                        Child, cyc_len = CycleObject, MUIA_CycleChain, TRUE, MUIA_Cycle_Entries, str_len, End,

                        Child, HSpace(0),
                        Child, HGroup,
                           Child, chk_opn = CheckMark(TRUE),
                           Child, lbl_opn = Label(Locale_GetString(MSG_SETTINGS_LABEL_SHOWONSTARTUP)),
                           Child, HSpace(0),
                        End,
                        Child, HSpace(0),
                        Child, HGroup,
                           Child, chk_clk = CheckMark(TRUE),
                           Child, lbl_clk = Label(Locale_GetString(MSG_SETTINGS_LABEL_SHOWCLICKS)),
                           Child, HSpace(0),
                        End,

                        Child, HSpace(0),
                        Child, HGroup,
                           Child, chk_whe = CheckMark(TRUE),
                           Child, lbl_whe = Label(Locale_GetString(MSG_SETTINGS_LABEL_SHOWWHEEL)),
                           Child, HSpace(0),
                        End,
                     End,

                     Child, RectangleObject, End,

                     Child, CrawlingObject,
                        MUIA_Frame,        MUIV_Frame_Text,
                        MUIA_Background,   MUII_TextBack,
                        MUIA_FixHeightTxt, SBARABOUT,

                        Child, VGroup,
                           Child, TextObject,
                              MUIA_Text_Contents, SBARABOUT,
                           End,
                        End,
                     End, TAG_DONE);

   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   DoMethod(prefs, MUIM_Mccprefs_RegisterGadget, (ULONG)sli_mon, SBARCFG_MMETER_MONITORSIZE, 3, NULL, MUIA_Numeric_Value, (ULONG)lbl_mon);
   DoMethod(prefs, MUIM_Mccprefs_RegisterGadget, (ULONG)cyc_len, SBARCFG_MMETER_DISTANCE, 3, NULL, MUIA_Cycle_Active, (ULONG)lbl_len);
   DoMethod(prefs, MUIM_Mccprefs_RegisterGadget, (ULONG)chk_opn, SBARCFG_MMETER_SHOWSTART, 3, NULL, MUIA_Selected, (ULONG)lbl_opn);
   DoMethod(prefs, MUIM_Mccprefs_RegisterGadget, (ULONG)chk_clk, SBARCFG_MMETER_SHOWCLICK, 3, NULL, MUIA_Selected, (ULONG)lbl_clk);
   DoMethod(prefs, MUIM_Mccprefs_RegisterGadget, (ULONG)chk_whe, SBARCFG_MMETER_SHOWWHEEL, 3, NULL, MUIA_Selected, (ULONG)lbl_whe);
   return (ULONG)prefs;
}
/*=*/

/*=------------------------ mousemeter_sbar_configitem() -------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_configitem(struct IClass *cl, Object *obj, struct MUIP_Screenbar_KnowsConfigItem *msg)
{
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   switch (msg->cfgid)
   {
      case SBARCFG_MMETER_MONITORSIZE:
      case SBARCFG_MMETER_DISTANCE:
      case SBARCFG_MMETER_SHOWSTART:
      case SBARCFG_MMETER_SHOWCLICK:
      case SBARCFG_MMETER_SHOWWHEEL:
           return TRUE;
           break;
   }
   return FALSE;
}
/*=*/

/*=------------------------ mousemeter_sbar_defconfig() --------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_defconfig(struct IClass *cl, Object *obj, struct MUIP_Screenbar_DefaultConfigItem *msg)
{
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   switch (msg->cfgid)
   {
      case SBARCFG_MMETER_MONITORSIZE: return (ULONG)17; break;
      case SBARCFG_MMETER_DISTANCE:    return (ULONG)LUNIT_METER; break;
      case SBARCFG_MMETER_SHOWSTART:   return (ULONG)FALSE; break;
      case SBARCFG_MMETER_SHOWCLICK:   return (ULONG)TRUE; break;
      case SBARCFG_MMETER_SHOWWHEEL:   return (ULONG)TRUE; break;
   }
   return 0;
}
/*=*/

/*=------------------------ GetConfig()-------------------------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG GetConfig(Object *o, ULONG id)
{
   ULONG storage=0;
   DoMethod(o, MUIM_GetConfigItem, id, &storage);
   return(storage);
}
/*=*/

/*=------------------------ mousemeter_sbar_updateconfig() -----------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
static ULONG mousemeter_sbar_updateconfig(struct IClass *cl, Object *obj, struct MUIP_Screenbar_UpdateConfigItem *msg)
{
   struct Data *data = INST_DATA(cl, obj);
   debug_print("MouseMeter.sbar : %s (%d)\n", __func__, __LINE__);

   switch (msg->cfgid)
   {
      case SBARCFG_MMETER_MONITORSIZE:
         data->monitorsize = GetConfig(obj, msg->cfgid);
         DoMethod(obj, MUIM_TriggerSbar);
         break;

      case SBARCFG_MMETER_DISTANCE:
         data->lengthunit = GetConfig(obj, msg->cfgid);
         DoMethod(obj, MUIM_TriggerSbar);
         break;

      case SBARCFG_MMETER_SHOWSTART:
         data->showonstart = GetConfig(obj, msg->cfgid);
         break;
      
      case SBARCFG_MMETER_SHOWCLICK:
         data->showclicks = GetConfig(obj, msg->cfgid);
         RedrawQ(msg, obj, 1);
         break;

      case SBARCFG_MMETER_SHOWWHEEL:
         data->showwheel = GetConfig(obj, msg->cfgid);
         RedrawQ(msg, obj, 1);
         break;
   }
   return 0;
}
/*=*/

/*=------------------------ DISPATCHER Mousemeter_Sbar ---------------------*
 *                                                                          *
 *--------------------------------------------------------------------------*/
DISPATCHERNAME_BEGIN(Mousemeter_Sbar)
   case OM_NEW:                           return mousemeter_sbar_new          (cl,obj,(APTR)msg);
   case OM_GET:                           return mousemeter_sbar_get          (cl,obj,(APTR)msg);
   case OM_SET:                           return mousemeter_sbar_set          (cl,obj,(APTR)msg);
   case OM_DISPOSE:                       return mousemeter_sbar_dispose      (cl,obj,(APTR)msg);
   case MUIM_Setup:                       return mousemeter_sbar_setup        (cl,obj,(APTR)msg);
   case MUIM_Cleanup:                     return mousemeter_sbar_cleanup      (cl,obj,(APTR)msg);
   case MUIM_HandleInput:                 return mousemeter_sbar_handler      (cl,obj,(APTR)msg);
   case MUIM_AskMinMax:                   return mousemeter_sbar_askminmax    (cl,obj,(APTR)msg);
   case MUIM_Draw:                        return mousemeter_sbar_draw         (cl,obj,(APTR)msg);
   case MUIM_Show:                        return mousemeter_sbar_show         (cl,obj,(APTR)msg);
   case MUIM_Screenbar_BuildSettingsPanel:return mousemeter_sbar_settings     (cl,obj,(APTR)msg);
   case MUIM_Screenbar_KnowsConfigItem:   return mousemeter_sbar_configitem   (cl,obj,(APTR)msg);
   case MUIM_Screenbar_DefaultConfigItem: return mousemeter_sbar_defconfig    (cl,obj,(APTR)msg);
   case MUIM_Screenbar_UpdateConfigItem:  return mousemeter_sbar_updateconfig (cl,obj,(APTR)msg);
   case MUIM_TriggerSbar:                 return mousemeter_sbar_trigger      (cl, obj);
DISPATCHER_END
/*=*/
