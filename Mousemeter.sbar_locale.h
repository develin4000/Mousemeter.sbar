/*
->====================================================<-
->= Mousemeter.sbar - © Copyright 2013-2020 OnyxSoft =<-
->====================================================<-
->= Version  : 1.2                                   =<-
->= File     : Mousemeter.sbar_locale.h              =<-
->= Author   : Stefan Blixth                         =<-
->= Compiled : 2020-11-18                            =<-
->====================================================<-
*/

#ifndef _MOUSEMETERSBAR_LOCALE_
#define _MOUSEMETERSBAR_LOCALE_ 1


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif



/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_APPLICATION_NAME 0
#define MSG_GUI_LABEL_VERTICAL 1
#define MSG_GUI_LABEL_HORIZONTAL 2
#define MSG_GUI_LABEL_TOTAL 3
#define MSG_GUI_LABEL_AVERAGESPEED 4
#define MSG_GUI_SHORTHELP_BUTTONRESET 5
#define MSG_SETTINGS_LABEL_MONITORSIZE 6
#define MSG_SETTINGS_LABEL_UNIT 7
#define MSG_SETTIGS_SLIDER_INCH 8
#define MSG_SETTINGS_LABEL_SHOWONSTARTUP 9
#define MSG_GUI_LABEL_MOUSECLICKS 10
#define MSG_SETTINGS_LABEL_SHOWCLICKS 11
#define MSG_GUI_LABEL_WHEELMOUSE 12
#define MSG_SETTINGS_LABEL_SHOWWHEEL 13

#define CATCOMP_LASTID 13

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_APPLICATION_NAME_STR "Mousemeter"
#define MSG_GUI_LABEL_VERTICAL_STR "Vertical:"
#define MSG_GUI_LABEL_HORIZONTAL_STR "Horizontal:"
#define MSG_GUI_LABEL_TOTAL_STR "Total:"
#define MSG_GUI_LABEL_AVERAGESPEED_STR "Average Speed:"
#define MSG_GUI_SHORTHELP_BUTTONRESET_STR "Resets the statistics data."
#define MSG_SETTINGS_LABEL_MONITORSIZE_STR "Monitor Size:"
#define MSG_SETTINGS_LABEL_UNIT_STR "Unit:"
#define MSG_SETTIGS_SLIDER_INCH_STR "%ld inch"
#define MSG_SETTINGS_LABEL_SHOWONSTARTUP_STR "Show window on start"
#define MSG_GUI_LABEL_MOUSECLICKS_STR "Mouse Clicks:"
#define MSG_SETTINGS_LABEL_SHOWCLICKS_STR "Show mouse clicks"
#define MSG_GUI_LABEL_WHEELMOUSE_STR "Wheel Moves:"
#define MSG_SETTINGS_LABEL_SHOWWHEEL_STR "Show mouse mouse wheel moves"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_APPLICATION_NAME,(STRPTR)MSG_APPLICATION_NAME_STR},
    {MSG_GUI_LABEL_VERTICAL,(STRPTR)MSG_GUI_LABEL_VERTICAL_STR},
    {MSG_GUI_LABEL_HORIZONTAL,(STRPTR)MSG_GUI_LABEL_HORIZONTAL_STR},
    {MSG_GUI_LABEL_TOTAL,(STRPTR)MSG_GUI_LABEL_TOTAL_STR},
    {MSG_GUI_LABEL_AVERAGESPEED,(STRPTR)MSG_GUI_LABEL_AVERAGESPEED_STR},
    {MSG_GUI_SHORTHELP_BUTTONRESET,(STRPTR)MSG_GUI_SHORTHELP_BUTTONRESET_STR},
    {MSG_SETTINGS_LABEL_MONITORSIZE,(STRPTR)MSG_SETTINGS_LABEL_MONITORSIZE_STR},
    {MSG_SETTINGS_LABEL_UNIT,(STRPTR)MSG_SETTINGS_LABEL_UNIT_STR},
    {MSG_SETTIGS_SLIDER_INCH,(STRPTR)MSG_SETTIGS_SLIDER_INCH_STR},
    {MSG_SETTINGS_LABEL_SHOWONSTARTUP,(STRPTR)MSG_SETTINGS_LABEL_SHOWONSTARTUP_STR},
    {MSG_GUI_LABEL_MOUSECLICKS,(STRPTR)MSG_GUI_LABEL_MOUSECLICKS_STR},
    {MSG_SETTINGS_LABEL_SHOWCLICKS,(STRPTR)MSG_SETTINGS_LABEL_SHOWCLICKS_STR},
    {MSG_GUI_LABEL_WHEELMOUSE,(STRPTR)MSG_GUI_LABEL_WHEELMOUSE_STR},
    {MSG_SETTINGS_LABEL_SHOWWHEEL,(STRPTR)MSG_SETTINGS_LABEL_SHOWWHEEL_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x0C"
    MSG_APPLICATION_NAME_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x0A"
    MSG_GUI_LABEL_VERTICAL_STR "\x00"
    "\x00\x00\x00\x02\x00\x0C"
    MSG_GUI_LABEL_HORIZONTAL_STR "\x00"
    "\x00\x00\x00\x03\x00\x08"
    MSG_GUI_LABEL_TOTAL_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x10"
    MSG_GUI_LABEL_AVERAGESPEED_STR "\x00\x00"
    "\x00\x00\x00\x05\x00\x1C"
    MSG_GUI_SHORTHELP_BUTTONRESET_STR "\x00"
    "\x00\x00\x00\x06\x00\x0E"
    MSG_SETTINGS_LABEL_MONITORSIZE_STR "\x00"
    "\x00\x00\x00\x07\x00\x06"
    MSG_SETTINGS_LABEL_UNIT_STR "\x00"
    "\x00\x00\x00\x08\x00\x0A"
    MSG_SETTIGS_SLIDER_INCH_STR "\x00\x00"
    "\x00\x00\x00\x09\x00\x16"
    MSG_SETTINGS_LABEL_SHOWONSTARTUP_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x0E"
    MSG_GUI_LABEL_MOUSECLICKS_STR "\x00"
    "\x00\x00\x00\x0B\x00\x12"
    MSG_SETTINGS_LABEL_SHOWCLICKS_STR "\x00"
    "\x00\x00\x00\x0C\x00\x0E"
    MSG_GUI_LABEL_WHEELMOUSE_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x1E"
    MSG_SETTINGS_LABEL_SHOWWHEEL_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */
/****************************************************************************/



struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};



#ifdef CATCOMP_CODE

#include <libraries/locale.h>
#include <proto/locale.h>


STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */

BOOL Locale_Open(STRPTR, ULONG, ULONG);
void Locale_Close(void);
STRPTR Locale_GetString(long);


/****************************************************************************/



#endif /* _MOUSEMETERSBAR_LOCALE_ */

