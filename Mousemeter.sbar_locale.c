/*
->==============================================<-
->= Musemeter.sbar - © Copyright 2012 OnyxSoft =<-
->==============================================<-
->= Version  : 1.0                             =<-
->= File     : Mousemeter.sbar_locale.c        =<-
->= Author   : Stefan Blixth                   =<-
->= Compiled : 2012-03-20                      =<-
->==============================================<-
*/


#define CATCOMP_ARRAY
#define CATCOMP_NUMBERS

#include <exec/types.h>
#include <libraries/locale.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/dos.h>

struct Locale                *MousemeterSbar_Locale;
static struct Catalog        *MousemeterSbar_Catalog;
struct Library *LocaleBase = NULL;

#include "Mousemeter.sbar_locale.h"
  
/*=----------------------------- Locale_Open()--------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
BOOL Locale_Open( STRPTR catname, ULONG version, ULONG revision)
{
   if ((LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library", 0)))
   {
      if ((MousemeterSbar_Locale = OpenLocale(NULL)))
      {
         if ((MousemeterSbar_Catalog = OpenCatalogA(MousemeterSbar_Locale, catname, TAG_DONE)))
         {
            if (MousemeterSbar_Catalog->cat_Version == version && MousemeterSbar_Catalog->cat_Revision == revision)
            {
               return(TRUE);
            }

            CloseCatalog(MousemeterSbar_Catalog);
            MousemeterSbar_Catalog = NULL;
         }

         CloseLocale(MousemeterSbar_Locale);
         MousemeterSbar_Locale = NULL;
      }
      CloseLibrary((struct Library *)LocaleBase);
   }

   return(FALSE);
}
/*=*/

/*=----------------------------- Locale_Close()-------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
void Locale_Close(void)
{

   if (LocaleBase)
   {
      if (MousemeterSbar_Catalog)
      {
         CloseCatalog(MousemeterSbar_Catalog);
         MousemeterSbar_Catalog = NULL;
      }

      if (MousemeterSbar_Locale)
      {
         CloseLocale(MousemeterSbar_Locale);
         MousemeterSbar_Locale = NULL;
      }

      CloseLibrary((struct Library *)LocaleBase);
      LocaleBase = NULL;
   }
}
/*=*/

/*=----------------------------- Locale_GetString()---------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
STRPTR Locale_GetString(long id)
{
  STRPTR defstr;
  LONG strnum;

  strnum = CatCompArray[id].cca_ID;
  defstr = CatCompArray[id].cca_Str;

  if (MousemeterSbar_Catalog && LocaleBase)
     return GetCatalogStr(MousemeterSbar_Catalog, strnum, defstr);
  else
     return defstr;
}
/*=*/

