//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step12.cpp
//----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dir.h>
#include <owl/applicat.h>
#include <owl/module.h>
#include <owl/decmdifr.h>
#include <owl/mdi.h>
#include <owl/mdichild.h>
#include <owl/dialog.h>
#include <owl/controlb.h>
#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <owl/docmanag.h>
#include <owl/docview.rh>

#include "AnnoEdit.h"
#include "Bitmaps.h"
#include "Aboutbox.h"
#include "Random.h"

TDecoratedFrame* theFrame = 0;
Bitmaps* theBitmaps = 0;
Random* theRandom = 0;
string descrFile;
string randomFile;
string bitmapsDir;
string annoInstallDir;
TStatusBar* msgBar = 0;
int MaxIslandSize = MAX_ISLAND_SIZE;	// default set with macro

static TApplication* theModule_p = 0;

static string resString( uint resId )
{
        if( theModule_p == 0 )
                return string();

        return theModule_p->LoadString( resId );
}

const char* getString( int resId )
{
        static string s;
        s = resString( resId );
        return s.c_str();
}

int msgBox( TWindow* win_p, uint resId1, uint resId2, uint type )
{
        // need to copy the strings
        string s1( resString( resId1 ) );
        string s2( resString( resId2 ) );

        return ::MessageBox( win_p ? (HWND)*win_p : 0,
        			 s1.c_str(), s2.c_str(), type );
}

static void getAnnoInstallDir()
{
 	const string keyStr = "Software\\Anno1602";
 	TRegKey rkey( TRegKey::CurrentUser, keyStr.c_str(),
    					KEY_READ, TRegKey::NoCreate );
 	TRegValue installdirRv( rkey, "INSTALL_DIR" );
   if( installdirRv.GetDataSize() > 0 )
         annoInstallDir = (const char far*)installdirRv;
   annoInstallDir += "ANNO1602\\";
}

static const char far* szUniqueAppString = "SirHenryAnnoEdit";
static uint WM_MY_ACTIVATEAPP = 0;

class AnnoEditFrame : public TDecoratedMDIFrame
{
public:
	AnnoEditFrame( const char* title, TResId menuResId,
   					 TMDIClient& clientWnd, bool track, TModule* module = 0 )
    : TDecoratedMDIFrame( title, menuResId, clientWnd, track, module ) {}
   void SetupWindow();

	LRESULT EvNewInstance( WPARAM atom, LPARAM );

   DECLARE_RESPONSE_TABLE( AnnoEditFrame );
};

DEFINE_RESPONSE_TABLE1( AnnoEditFrame, TDecoratedMDIFrame )
	EV_REGISTERED( szUniqueAppString, EvNewInstance ),
END_RESPONSE_TABLE;

void AnnoEditFrame::SetupWindow()
{
	TDecoratedMDIFrame::SetupWindow();
   SetProp( szUniqueAppString, (HANDLE)1 );
}

LRESULT AnnoEditFrame::EvNewInstance( WPARAM atom, LPARAM )
{
	char file[256];

   if( ::GlobalGetAtomName( (ATOM)atom, file, 256 ) != 0 )
   {
		::GlobalDeleteAtom( (ATOM)atom );

      TMDIClient* client = GetClientWindow();
      if( client != 0 )
      {
      	if( IsIconic() )
         	SendMessage( WM_SYSCOMMAND, SC_RESTORE, 0 );

         SetForegroundWindow( *this );

			if( file != "" )
		  	{
  				TDocTemplate* tpl = GetApplication()->
            	GetDocManager()->MatchTemplate( file );
		      if( tpl != 0 )
		      {
		      	tpl->CreateDoc( file );
		      }
	      }
      }
   }

   return 0;
}

class TAnnoEditApp : public TApplication {
  public:
    TAnnoEditApp( char far* file ): TApplication(), theFile(file)
    {}

  protected:
    // Override methods of TApplication
    void InitInstance();
    void InitMainWindow();

    // Event handlers
    void EvNewView  (TView& view);
    void EvCloseView(TView& view);
//    void EvDropFiles(TDropInfo dropInfo);
    void CmFilePrint();
    void CmAbout();
    void CmManual();

  private:
    TMDIClient* client_p;
    string theFile;
    string manualPath;

  DECLARE_RESPONSE_TABLE(TAnnoEditApp);
};

DEFINE_RESPONSE_TABLE1(TAnnoEditApp, TApplication)
  EV_OWLVIEW(dnCreate, EvNewView),
  EV_OWLVIEW(dnClose,  EvCloseView),
//  EV_WM_DROPFILES,
  EV_COMMAND(CM_ABOUT, CmAbout),
  EV_COMMAND(CM_HELPMANUAL, CmManual),
END_RESPONSE_TABLE;

void
TAnnoEditApp::InitMainWindow()
{
	// set file paths from registry
   getAnnoInstallDir();

   const string keyStr = "SOFTWARE\\Sir Henry\\AnnoEdit";
   TRegKey rkey( TRegKey::LocalMachine, keyStr.c_str(), KEY_READ, TRegKey::NoCreate );
   TRegValue descrFileRv( rkey, "DescriptionFile" );
   if( descrFileRv.GetDataSize() > 0 )
	   descrFile = (const char far*)descrFileRv;

   TRegValue randomFileRv( rkey, "RandomFile" );
   if( randomFileRv.GetDataSize() > 0 )
   {
	   randomFile = (const char far*)randomFileRv;
      theRandom = new Random( randomFile );
   }

   TRegValue bitmapsDirRv( rkey, "BitmapDir" );
   if( bitmapsDirRv.GetDataSize() > 0 )
   	bitmapsDir = (const char far*)bitmapsDirRv;

   TRegValue manualPathRv( rkey, "ManualPath" );
   if( manualPathRv.GetDataSize() > 0 )
   	manualPath = (const char far*)manualPathRv;

   TRegValue maxIslandSize( rkey, "MaxIslandSize" );
   if( maxIslandSize.GetDataSize() > 0 )
   	MaxIslandSize = (int)(uint32)maxIslandSize;

  // Construct the decorated frame window
  client_p = new TMDIClient;
  theFrame = new AnnoEditFrame
  	( getString(IDS_PROGTITLE), 0, *client_p, true);

  // Construct a status bar
//  TStatusBar* sb = new TStatusBar(theFrame, TGadget::Recessed);
	msgBar = new TStatusBar(theFrame);

  // Construct a control bar
  TControlBar* cb = new TControlBar(theFrame);
  cb->SetHintMode( TGadgetWindow::EnterHints );
  cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVEAS, CM_FILESAVEAS, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILEREVERT, CM_FILEREVERT, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_STOREBITMAP, CM_STOREBITMAP, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_UNDO, CM_UNDO, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_CLIPBOARD, CM_CLIPBOARD, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_RANDOM, CM_RANDOM, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_AREALEFT, CM_AREALEFT, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_AREA180, CM_AREA180, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_AREARIGHT, CM_AREARIGHT, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_HORIZONTAL, CM_HORIZONTAL, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_VERTICAL, CM_VERTICAL, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_ZOOMMINUS, CM_ZOOMMINUS, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_ZOOMPLUS, CM_ZOOMPLUS, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_ZOOMWINDOW, CM_ZOOMWINDOW, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_ZOOMRESET, CM_ZOOMRESET, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_INSELPROPERTIES, CM_INSELPROPERTIES, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_ABOUT, CM_ABOUT, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);

  // Insert the status bar and control bar into the frame
  theFrame->Insert(*msgBar, TDecoratedFrame::Bottom);
  theFrame->Insert(*cb, TDecoratedFrame::Top);

  // Set the main window and its menu
  SetMainWindow(theFrame);
  theFrame->SetMenuDescr(TMenuDescr("COMMANDS",1,0,0,0,1,1));

  // Set the icon
  theFrame->SetIcon( this, IDI_ICON256 );

  // Install the document manager
  TDocManager* docMan = new TDocManager(dmMDI | dmMenu);
  SetDocManager( docMan );

  // Create the bitmaps
  theBitmaps = new Bitmaps;
}

void
TAnnoEditApp::InitInstance()
{
  TApplication::InitInstance();

  if( theFile != "" )
  {
  		TDocTemplate* tpl = GetDocManager()->MatchTemplate( theFile.c_str() );
      if( tpl != 0 )
      {
      	tpl->CreateDoc( theFile.c_str() );
      }
  }
//  GetMainWindow()->DragAcceptFiles(true);
//  GetDocManager()->CmFileNew();
}

/*
void
TAnnoEditApp::EvDropFiles(TDropInfo dropInfo)
{
  if (dropInfo.DragQueryFileCount() != 1)
    ::MessageBox(0, "Can only drop 1 file in SDI mode", "Drag/Drop Error", MB_OK);
  else {
    int fileLength = dropInfo.DragQueryFileNameLen(0)+1;
    char* filePath = new char [fileLength];
    dropInfo.DragQueryFile(0, filePath, fileLength);
    TDocTemplate* tpl = GetDocManager()->MatchTemplate(filePath);
    if (tpl)
      tpl->CreateDoc(filePath);
    delete filePath;
  }
  dropInfo.DragFinish();
}
*/

void
TAnnoEditApp::EvNewView(TView& view)
{
	TWindow* wnd_p = view.GetWindow();

	TMDIChild* child_p =
   	new TMDIChild( *client_p, 0, wnd_p );

//   child_p->Attr.Style |= WS_MAXIMIZE;

   // Set the icon
   child_p->SetIcon( this, IDI_ICON16 );

   if ( view.GetViewMenu() )
    child_p->SetMenuDescr( *view.GetViewMenu() );

	child_p->Create();
	child_p->ShowWindow( SW_SHOWMAXIMIZED );
}

void
TAnnoEditApp::EvCloseView(TView& /*view*/)
{
//  GetMainWindow()->SetCaption("Anno 1602 Insel-Editor");
}

void
TAnnoEditApp::CmAbout()
{
  AboutBox(GetMainWindow(), IDD_ABOUT).Execute();
}

void
TAnnoEditApp::CmManual()
{
	if( manualPath.length() > 0 )
		ShellExecute( 0, "open", manualPath.c_str(), 0, 0, SW_SHOW );
}

int
OwlMain(int argc, char* argv[])
{
   randomize();	// for random functions
	char* file = "";

   // File is argument no. 2 due to InstallShield bug!
   if( argc > 2 )
   {
   	file = argv[2];
   }

	theModule_p = new TAnnoEditApp( file );

   // look for the application already running
   WM_MY_ACTIVATEAPP = RegisterWindowMessage( szUniqueAppString );
   TWindow desktop( TWindow::GetDesktopWindow() );
   HWND childHnd( desktop.GetWindow( GW_CHILD ) );

	while( childHnd != 0 )
   {
   	TWindow child( childHnd );
   	if( child.GetProp( szUniqueAppString ) != 0 )
      {
      	// found the unique AnnoEdit window
         ATOM atom = GlobalAddAtom( file );
         // send the message to the running application
         child.SendMessage( WM_MY_ACTIVATEAPP, atom, 0 );
         // this instance is finished
         return 0;
      }
      childHnd = child.GetWindow( GW_HWNDNEXT );
   }

   return theModule_p->Run();
}
