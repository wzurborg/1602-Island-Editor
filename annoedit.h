//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//----------------------------------------------------------------------------
#ifndef __annoedit__h__
#define __annoedit__h__

class TDecoratedFrame;
extern TDecoratedFrame* theFrame;

class TStatusBar;
extern TStatusBar* msgBar;

class Bitmaps;
extern Bitmaps* theBitmaps;

class Random;
extern Random* theRandom;

extern string descrFile;
extern string randomFile;
extern string bitmapsDir;
extern string annoInstallDir;

extern int MaxIslandSize;

class TWindow;
extern const char* getString( int resId );
extern int msgBox( TWindow* win_p, unsigned int resId1, unsigned int resId2, unsigned int type );

#define MAX_ISLAND_SIZE 255

#define IDI_ICON256	1
#define IDI_ICON16	2
#define IDI_ICON2	2
#define IDI_ICON1	1

#define CM_AREALEFT 1
#define CM_AREARIGHT 2
#define CM_AREA180 3
#define CM_FIELDLEFT 4
#define CM_FIELDRIGHT 5
#define CM_FIELD180 6
#define CM_HORIZONTAL 7
#define CM_VERTICAL 8
#define CM_ZOOMPLUS	9
#define CM_ZOOMMINUS	10
#define CM_ZOOMRESET	11
#define CM_ZOOMWINDOW 12
#define CM_CLIPBOARD 13
#define CM_MARKALL	14
#define CM_CLEARALL	15
#define CM_UNDO	16
#define CM_HELPMANUAL 17
#define CM_RANDOM	18
#define CM_PICK 19
#define CM_DRAW 20
#define CM_MARKID 21
#define CM_STOREBITMAP 22

#define IDC_KEEPORI	107
#define IDM_EDITMENU	2

#define IDC_TREEADJUST	113
#define IDC_Y	107
#define IDC_X	106
#define IDC_HOEHE	104
#define IDC_BREITE	103
#define IDC_GROUPBOX3	109
#define IDC_GROUPBOX2	105
#define IDD_INSELDIALOG	2
#define IDC_INSELSUED	102
#define IDC_INSELNORD	101
#define IDC_GROUPBOX1	107
#define IDC_KLIMABOX	   108
#define IDC_DESCR	106
#define IDC_SYSTREEVIEW1	104
#define IDC_IDUNKNOWN	104
#define IDC_ID	103
#define IDC_UNDO	102
#define IDC_MARKID	108
#define IDC_TURN	101
#define IDC_BITMAPFRAME	111
#define IDC_FELDSETZEN	110
#define IDC_FELDLADEN	109
#define IDC_ORI	105
#define IDC_IDLOW	104
#define IDC_IDHIGH	103
#define IDC_YPOS	102
#define IDC_XPOS	101
#define IDD_FLDDIALOG	1
#define CM_INSELPROPERTIES 101
#define CM_ABOUT	0x4205
#define CM_ACCEPT     306
#define CM_RESTORE    307
#define IDD_ABOUT     211

#define IDC_GROUPBOX6	112
#define IDC_NEWSUED	111
#define IDC_NEWNORD	110
#define IDC_GROUPBOX5	109
#define IDC_NEWY	108
#define IDC_NEWX	107
#define IDC_LAR	105
#define IDC_BIG	104
#define IDC_MED	103
#define IDC_MIT	102
#define IDC_LIT	101
#define IDC_GROUPBOX4	106
#define IDC_LARGE	101
#define IDD_NEWINSEL	3

#define IDI_MULTIFILE       101
#define IDI_DOC             102

#define CM_TOGGLEHINT       202
#define CM_TOGGLEBAR        203
#define CM_TESTSTATE        204

#define IDA_AnnoEdit        100

#define IDM_INSELMENU	1
#define IDM_EDITFILE_DOC    100
#define IDM_EDITFILE_CHILD  101

#define IDC_EMAIL	102
#define IDC_URL	101

#define IDC_PENCIL	1

#define IDC_FLDPERCENT	101
#define IDC_FLDSELECT	102
#define IDD_RANDOM	4

#define IDS_PROGTITLE 901
#define IDS_DESCRNOTFOUND 902
#define IDS_FIELDEDITOR 903
#define IDS_MAXSIZE 904
#define IDS_MINSIZE 905
#define IDS_ISLANDFILES 906
#define IDS_WHITEFIELDS 907
#define IDS_PROBLEM 908
#define IDS_ERROR 909
#define IDS_WARNING 910
#define IDS_NOLOAD 911
#define IDS_SAVEERROR 912
#define IDS_DISCARDCHANGES 913
#define IDS_REVERT 914
#define IDS_STATUSONEMARKED 915
#define IDS_STATUSMANYMARKED 916
#define IDS_BIGFIELDWARNING 917
#define IDS_URL 918
#define IDS_MAILTO 919
#define IDS_RANDOMNOTFOUND 920
#define IDS_BITMAPERROR 921

#endif
