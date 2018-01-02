#include <stdio.h>
#include <math.h>
#include <owl/scroller.h>
#include <owl/controlb.h>
#include <owl/decframe.h>
#include <owl/docview.rh>
#include <owl/statusba.h>
#include <owl/except.h>
#include "inselview.h"
#include "flddialog.h"
#include "InselDialog.h"
#include "RandomDialog.h"
#include "annoedit.h"

#define MIN_ZOOM  0.3
#define MAX_ZOOM  3.0

//uint InselView::clipboardFormat = 0;
FldDialog* InselView::fldDialog_p = 0;

InselView::InselView( InselDoc& doc, TWindow* parent )
: TWindowView( doc, parent ), myDoc( doc ), zoomFactor(1.0),
  dragging(false),currentP(0,0)
{
	  Attr.Style |= WS_VSCROLL | WS_HSCROLL;
     Attr.AccelTable = IDM_EDITMENU;

     Scroller = new TScroller( this, InselFeld::unitSize(),
     											 InselFeld::unitSize(),
                                     100, 100 );
     Scroller->AutoOrg = false;

	  TMenuDescr* menuDescr =
     		new TMenuDescr( IDM_EDITMENU, 0, 3, 0, 0, 0, 0 );
     SetViewMenu( menuDescr );
     popupMenu_p = new TPopupMenu( menuDescr->GetSubMenu( 0 ) );

     if( fldDialog_p == 0 )
	     fldDialog_p = new FldDialog( theFrame, IDD_FLDDIALOG );
}

DEFINE_RESPONSE_TABLE1(InselView, TWindowView)
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONUP,
  EV_WM_KEYDOWN,
  EV_WM_SIZE,
  EV_VN_COMMIT,
  EV_VN_REVERT,
  EV_WM_DESTROY,
  EV_WM_SETFOCUS,
  EV_COMMAND(CM_STOREBITMAP, CmStoreBitmap),
  EV_MESSAGE(WM_USER, CmCommandTerminated),
  EV_COMMAND(CM_UNDO, CmUndo),
  EV_COMMAND_ENABLE(CM_UNDO, CeUndo),
  EV_COMMAND(CM_INSELPROPERTIES, CmInselDialog),
  EV_COMMAND(CM_CLIPBOARD, CmToClipboard),
  EV_COMMAND_AND_ID(CM_EDITCOPY, CmCommand),
  EV_COMMAND_ENABLE(CM_EDITCOPY, CeCopy),
  EV_COMMAND_AND_ID(CM_EDITPASTE, CmCommand),
  EV_COMMAND_ENABLE(CM_EDITPASTE, CePaste),
  EV_COMMAND_AND_ID(CM_AREALEFT, CmCommand),
  EV_COMMAND_ENABLE(CM_AREALEFT, CeCopy),
  EV_COMMAND_AND_ID(CM_AREARIGHT, CmCommand),
  EV_COMMAND_ENABLE(CM_AREARIGHT, CeCopy),
  EV_COMMAND_AND_ID(CM_AREA180, CmCommand),
  EV_COMMAND_ENABLE(CM_AREA180, CeCopy),
  EV_COMMAND_AND_ID(CM_HORIZONTAL, CmCommand),
  EV_COMMAND_ENABLE(CM_HORIZONTAL, CeCopy),
  EV_COMMAND_AND_ID(CM_VERTICAL, CmCommand),
  EV_COMMAND_ENABLE(CM_VERTICAL, CeCopy),
  EV_COMMAND_AND_ID(CM_FIELDLEFT, CmCommand),
  EV_COMMAND_ENABLE(CM_FIELDLEFT, CeCopy),
  EV_COMMAND_AND_ID(CM_FIELDRIGHT, CmCommand),
  EV_COMMAND_ENABLE(CM_FIELDRIGHT, CeCopy),
  EV_COMMAND_AND_ID(CM_FIELD180, CmCommand),
  EV_COMMAND_ENABLE(CM_FIELD180, CeCopy),
  EV_COMMAND_AND_ID(CM_ZOOMPLUS, CmZoom),
  EV_COMMAND_AND_ID(CM_ZOOMMINUS, CmZoom),
  EV_COMMAND_AND_ID(CM_ZOOMRESET, CmZoom),
  EV_COMMAND_AND_ID(CM_ZOOMWINDOW, CmZoom),
  EV_COMMAND_ENABLE(CM_ZOOMPLUS, CeZoomPlus),
  EV_COMMAND_ENABLE(CM_ZOOMMINUS, CeZoomMinus),
  EV_COMMAND_AND_ID(CM_MARKALL, CmMark),
  EV_COMMAND_AND_ID(CM_CLEARALL, CmMark),
  EV_COMMAND(CM_RANDOM, CmRandom),
  EV_COMMAND_ENABLE(CM_RANDOM, CeCopy),
  EV_COMMAND_AND_ID(CM_PICK, CmPickOrDraw),
  EV_COMMAND_AND_ID(CM_DRAW, CmPickOrDraw),
  EV_COMMAND(CM_MARKID, CmMarkId),
END_RESPONSE_TABLE;

const char far* InselView::StaticName()
{
	return "Insel View";
}

void InselView::EvSize(uint sizeType, TSize& size)
{
	setScrollers();
   TWindowView::EvSize( sizeType, size );
}

void InselView::SetupWindow()
{
	TWindowView::SetupWindow();
}

InselView::~InselView()
{
}

void InselView::setScrollers()
{
	if( myDoc.size() == TSize(0,0) ) return;

	TRect r = GetClientRect();
   if( r == TRect(0,0,0,0) ) return;

   double unitSize = (double)InselFeld::unitSize() * zoomFactor;

   // total rows/columns
	double areaWidth = (double) myDoc.size().cx;
   double areaHeight = (double) myDoc.size().cy;

   // visible rows/columns = page size
   double pageWidth = r.Width() / unitSize;
   double pageHeight = r.Height() / unitSize;

   Scroller->SetUnits( unitSize+1, unitSize+1 );
   Scroller->SetRange( pageWidth  < areaWidth  ? areaWidth : 0,
   						  pageHeight < areaHeight ? areaHeight : 0 );

   // bug in TScroller::SetPageSize() - page size is incorrectly set
   if( pageWidth < areaWidth )
	   SetScrollPage( SB_HORZ, pageWidth, true );

	if( pageHeight < areaHeight )
	  	SetScrollPage( SB_VERT, pageHeight, true );
}

void InselView::InvalidateRect(const TRect& rect, bool erase)
{
	TRect r( rect );
	LPtoDP( r, 2 );
	TWindowView::InvalidateRect( r, erase );
}

void InselView::resetDialog()
{
	int numMarked = 0;
   InselFeld* firstMarked_p = 0;
   TRect r = myDoc.getSelection( numMarked, firstMarked_p );

   char buffer[100] = "";
   if( numMarked == 1 )
   {
		fldDialog_p->setId( firstMarked_p->id() );
   	fldDialog_p->setOri( firstMarked_p->ori() );
   	fldDialog_p->updateWindow();
	   sprintf( buffer, getString(IDS_STATUSONEMARKED),
   			firstMarked_p->pos().x+1, firstMarked_p->pos().y+1 );
   }
   else
   if( numMarked > 1 )
   {
     	fldDialog_p->clearAll();

		if( r.Left() != -1 )
      {
      	int rw = r.Width()+1;
         int rh = r.Height()+1;
         int rl = r.Left() + 1;
         int rt = r.Top() + 1;
         sprintf( buffer, getString(IDS_STATUSMANYMARKED),
                  numMarked, rl, rt, rw, rh );
		}
   }
   else
   {	// nothing selected
//   	fldDialog_p->Show(SW_HIDE);
	   msgBar->SetText(buffer);
  		return;
   }

   msgBar->SetText(buffer);
  	fldDialog_p->Show(SW_SHOW);
}

void InselView::EvKeyDown(uint key, uint repeatCount, uint flags)
{
	WORD wScrollNotify = 0xFFFF;
   uint msg = WM_VSCROLL;

    switch ( key )
    {
        case VK_UP:
            wScrollNotify = SB_LINEUP;
            break;

        case VK_PRIOR:
            wScrollNotify = SB_PAGEUP;
            break;

        case VK_NEXT:
            wScrollNotify = SB_PAGEDOWN;
            break;

        case VK_DOWN:
            wScrollNotify = SB_LINEDOWN;
            break;

        case VK_HOME:
            wScrollNotify = SB_TOP;
            break;

        case VK_END:
            wScrollNotify = SB_BOTTOM;
            break;

        case VK_LEFT:
        		wScrollNotify = SB_LINELEFT;
            msg = WM_HSCROLL;
            break;

        case VK_RIGHT:
        		wScrollNotify = SB_LINERIGHT;
            msg = WM_HSCROLL;
            break;
    }

    if (wScrollNotify != 0xFFFF)
        SendMessage( msg, MAKELONG(wScrollNotify, 0), 0L);
    else
	     TWindowView::EvKeyDown( key, repeatCount, flags );
}

void InselView::EvLButtonDown( uint modKeys, TPoint& point)
{
   TPoint realP = point;
   DPtoLP( &realP );

   if( !fldDialog_p->IsWindow() )
   	fldDialog_p->Create();

   fldDialog_p->ShowWindow( SW_RESTORE );

   if( fldDialog_p->loading() )
   {
	   if( modKeys & MK_CONTROL )
   	{
			myDoc.flipOne( realP );
	   }
   	else
	   {
		   if( modKeys & MK_SHIFT )
	   	{
				int numMarked = 0;
			   InselFeld* firstMarked_p = 0;
			   TRect r = myDoc.getSelection( numMarked, firstMarked_p );

         	if( numMarked != 0 )	// do only if not first marked field
	         {
					focusRect |= TRect( realP, realP );
					SetCursor( 0, IDC_WAIT );
		   	  	myDoc.markAllTouching( focusRect );
					resetCursor();
	            resetDialog();
   	         TWindowView::EvLButtonDown( modKeys, point );
      	      return;
         	}
		   }
  		   myDoc.markOne( realP );
	   }

   	// prepare for dragging
		focusRect = TRect( realP, realP );
//   	if( ! (modKeys & (MK_SHIFT|MK_CONTROL) ) )
//	   {
		  	dragStart = realP;
			dragging = true;
//	   }

		resetDialog();
   }
   else
   {
   	// setting
		InselFeld* fld_p = myDoc.fieldFromPoint( realP );
      if( fld_p != 0 )
      {
			myDoc.setOneFromDialog( *fld_p, *fldDialog_p );
      }
		dragging = true;
   }

   TWindowView::EvLButtonDown( modKeys, point );
}

void InselView::EvLButtonUp( uint modKeys, TPoint& point)
{
  	dragging = false;
   TWindowView::EvLButtonUp( modKeys, point );
}

void InselView::EvRButtonUp( uint modKeys, TPoint& point )
{
	ClientToScreen(point);
	popupMenu_p->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON,
   								  point, 0, *this);
	TWindowView::EvRButtonUp( modKeys, point );
}

void InselView::EvMouseMove( uint modKeys, TPoint& point)
{
	if( dragging )
   {
      if( modKeys & MK_LBUTTON )	// still dragging?
      {
			TPoint realP = point;
         DPtoLP( &realP );

         TPoint newP;
         newP.x = realP.x / InselFeld::unitSize();
         newP.y = realP.y / InselFeld::unitSize();
         if( newP != currentP )
         {
         	currentP = newP;
			   if( fldDialog_p->loading() )
			   {
					focusRect = TRect( dragStart, realP ).Normalize();
//        	   if( modKeys & MK_CONTROL )
//         	   	myDoc.flipAllTouching( focusRect );
//  	         else
   	   	  	myDoc.markAllTouching( focusRect );
   	      	resetDialog();
	         }
   	      else
      	   {
         		// setting
					InselFeld* f_p = myDoc.fieldFromPoint( realP );
               if( f_p != 0 )
               {
						myDoc.setOneFromDialog( *f_p, *fldDialog_p, false );
   	         	//f_p->takeValuesFrom( *fldDialog_p );
               }
      	   }
         }
      }
      else
      {
      	dragging = false;
      }
   }

  	TWindowView::EvMouseMove( modKeys, point );
}

void InselView::EvSetFocus( THandle hWndLostFocus )
{
	if( fldDialog_p != 0 )
		fldDialog_p->setView( this );
   myDoc.setView( this );
	TWindowView::EvSetFocus( hWndLostFocus );
}

void InselView::fldHasChanged()
{
   InselFeld* fld_p = 0;
	int num = 0;
   myDoc.getSelection( num, fld_p );
	if( InselFeld::fldSize( fldDialog_p->id() ) != 1
       && num > 1 )
   {
   	msgBox( this, IDS_BIGFIELDWARNING, IDS_PROBLEM, MB_ICONINFORMATION );
      return;
   }

	SetCursor( 0, IDC_WAIT );

   if( num == 1 )
   	myDoc.setOneFromDialog( *fld_p, *fldDialog_p );
   else
   if( num > 1 )
		myDoc.setAllMarkedFrom( *fldDialog_p );
   resetCursor();

//	Invalidate(false);
}

void InselView::markId( int id )
{
   SetCursor( 0, IDC_WAIT );
	myDoc.selectId( id );
   resetCursor();
}

void InselView::Paint( TDC& tdc, bool, TRect& rect )
{
   // Scroll
   int xOrg = Scroller->XPos * Scroller->XUnit;
   int yOrg = Scroller->YPos * Scroller->YUnit;
	TPoint offset( -xOrg, -yOrg );
   tdc.SetViewportOrg( offset );

   // Zoom
   TSize ws( GetClientRect().Size() );
   TSize wz( ws.cx/zoomFactor, ws.cy/zoomFactor );
   TSize oldVExt, oldWExt;
   int prevMode = tdc.SetMapMode( MM_ISOTROPIC );
   tdc.SetWindowExt( wz, &oldWExt );
	tdc.SetViewportExt( ws, &oldVExt );
	tdc.DPtoLP( rect, 2 );

   // Draw
	myDoc.draw( tdc, rect );

   // Restore
   tdc.SetWindowExt( oldWExt );
   tdc.SetViewportExt( oldVExt );
   tdc.SetMapMode( prevMode );
}

bool InselView::VnCommit( bool /*force*/ )
{
   return true;
}

bool InselView::VnRevert( bool clear )
{
	setScrollers();
	Invalidate( clear );
/*
   if( fldDialog_p != 0 && fldDialog_p->IsWindow() )
   	fldDialog_p->ShowWindow( SW_HIDE );
*/
   return true;
}

void InselView::CmFilePrint()
{
}

void InselView::CmUndo()
{
	myDoc.undo();
}

void InselView::CeUndo( TCommandEnabler& enabler )
{
	enabler.Enable( myDoc.canUndo() );
}

void InselView::CmInselDialog()
{
	InselDialog( this, IDD_INSELDIALOG ).Execute();
   setScrollers();
}

void InselView::CeCopy( TCommandEnabler& enabler )
{
	enabler.Enable( !myDoc.isBusy() && myDoc.numMarked() > 0 );
}

void InselView::CePaste( TCommandEnabler& enabler )
{
	enabler.Enable( !myDoc.isBusy() &&
   					 !myDoc.clipboardEmpty() &&
   					 myDoc.numMarked() == 1 );
}

void InselView::CmZoom( WPARAM how )
{
	double oldZoom = zoomFactor;

	switch( how )
   {
   case CM_ZOOMPLUS:
	   zoomFactor *= 1.1;
      break;

   case CM_ZOOMMINUS:
		zoomFactor /= 1.1;
      break;

   case CM_ZOOMRESET:
		zoomFactor = 1.0;
      break;

   case CM_ZOOMWINDOW:
   {
   	TRect clientRect = GetClientRect();
	   int unitSize = InselFeld::unitSize();
      int inselWidth = myDoc.size().cx * unitSize;
      int inselHeight = myDoc.size().cy * unitSize;
      int windowWidth = clientRect.Width();
      int windowHeight = clientRect.Height();
      double wf = (double)windowWidth / (double)inselWidth;
      double hf = (double)windowHeight / (double)inselHeight;
      zoomFactor = min( wf, hf );
   	break;
   }
   }

  	if( zoomFactor > MAX_ZOOM ) zoomFactor = MAX_ZOOM;
  	if( zoomFactor < MIN_ZOOM ) zoomFactor = MIN_ZOOM;
   if( zoomFactor != oldZoom )
   {
	   setScrollers();
	   Invalidate();
   }
}

void InselView::CeZoomPlus( TCommandEnabler& enabler )
{
	enabler.Enable( zoomFactor < MAX_ZOOM );
}

void InselView::CeZoomMinus( TCommandEnabler& enabler )
{
	enabler.Enable( zoomFactor > MIN_ZOOM );
}

void InselView::showFieldDetails()
{
	int numMarked = 0;
   InselFeld* firstMarked_p = 0;
   TRect r = myDoc.getSelection( numMarked, firstMarked_p );

   if( numMarked == 1 )
   {
   	string s = firstMarked_p->debugInfo();
      ::MessageBox( 0, s.c_str(), "Details", MB_OK|MB_TASKMODAL );
   }
}

void InselView::CmStoreBitmap()
{
	SetCursor( 0, IDC_WAIT );

   int unitSize = InselFeld::unitSize();
	int areaWidth = myDoc.size().cx * unitSize;
  	int areaHeight = myDoc.size().cy * unitSize;
  	TRect all( 0, 0, areaWidth, areaHeight );

   TWindowDC wdc( *this );
   TMemoryDC memDC( wdc );

	try
   {
	   /* create a bitmap compatible with the window DC */
   	TBitmap bitmap( wdc, areaWidth, areaHeight );

	   /* select new bitmap into memory DC */
	   memDC.SelectObject( bitmap );

	   /* paint Insel into bitmap */
	   myDoc.draw( memDC, all );

	   /* Create DIB from Bitmap */
	   TDib dib( bitmap );
	   /* store DIB to file */
	   const char* docPath = 0;
	   TOpenSaveDialog::TData data
	   (OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	//flags
	    "Bitmap Files (*.BMP)|*.bmp|",				// filter
	    0,													// custom filter
	    docPath,											// initial dir
	    "BMP"												// defExt
	    );

	   string scpfile( myDoc.GetTitle() );
	   size_t pos = scpfile.find( "scp" );
	   if( pos == NPOS ) pos = scpfile.find( "SCP" );
	   if( pos != NPOS )
	   {
	   	scpfile.replace( pos, 3, "bmp" );
	      data.FileName = new char[MAX_PATH];
	      strcpy( data.FileName, scpfile.c_str() );

		   if( TFileSaveDialog( this, data ).Execute() == IDOK )
			{
		   if( !(data.Flags & OFN_EXTENSIONDIFFERENT) )
		      {
					SetCursor( 0, IDC_WAIT );
				   dib.WriteFile( data.FileName );
					resetCursor();
	         }
	      }
	   }
   }
   catch( TXOwl& exp )
   {
		msgBox( this, IDS_BITMAPERROR, IDS_PROBLEM, MB_ICONERROR );
//		throw(exp);
	}
   resetCursor();
}

void InselView::CmToClipboard()
{
   TRect rectClient = GetClientRect();
	TPoint tl = rectClient.TopLeft();
   TPoint br = rectClient.BottomRight();

   /* convert client coords to screen coords */
   ClientToScreen( tl );
   ClientToScreen( br );
   TRect rectScreen( tl, br );

   /*  create a DC for the screen and create
    *  a memory DC compatible to screen DC
    */

   TScreenDC srcDC;
   TMemoryDC memDC( srcDC );

   /* create a bitmap compatible with the screen DC */
   TBitmap bitmap( srcDC, rectScreen.Width(), rectScreen.Height() );

   /* select new bitmap into memory DC */
   memDC.SelectObject( bitmap );

   /* bitblt screen DC to memory DC */
   memDC.BitBlt( 0, 0, rectScreen.Width(), rectScreen.Height(),
   				  srcDC, rectScreen.Left(), rectScreen.Top() );

   TDib dib( bitmap );

   // Copy DIB to clipboard
   TClipboard clipboard( *this );
   if( !clipboard.SetClipboardData( CF_DIB, dib ) )
   {
   	MessageBox( "Copy to clipboard failed!", "Problem", MB_ICONWARNING );
   }
}

void InselView::CmMark( WPARAM what )
{
	SetCursor( 0, IDC_WAIT );

	switch( what )
   {
   case CM_MARKALL:
   	{
		   int unitSize = InselFeld::unitSize();
			int areaWidth = myDoc.size().cx * unitSize;
	   	int areaHeight = myDoc.size().cy * unitSize;
	   	TRect all( 0, 0, areaWidth, areaHeight );
	   	myDoc.markAllTouching( all );
      }
      break;

   case CM_CLEARALL:
   	myDoc.unmarkAll();
      break;
   }

	resetDialog();
	resetCursor();
}

void InselView::CmCommand( WPARAM command )
{
	SetCursor( 0, IDC_WAIT );
	myDoc.processCommand( command );
   LockWindowUpdate();
}

void InselView::CmPickOrDraw( WPARAM what )
{
   if( !fldDialog_p->IsWindow() )
   	fldDialog_p->Create();

   fldDialog_p->ShowWindow( SW_RESTORE );

	fldDialog_p->CmLadenSetzen( what == CM_PICK ?
   	IDC_FELDLADEN : IDC_FELDSETZEN );
}

LRESULT InselView::CmCommandTerminated(WPARAM,LPARAM)
{
	resetCursor();
   myDoc.commandTerminated();
   ::LockWindowUpdate(0);
   return 0;
}

void InselView::DPtoLP( TPoint* p, int count )
{
	TWindowDC dc( *this );

   // Scroll
   int xOrg = Scroller->XPos * Scroller->XUnit;
   int yOrg = Scroller->YPos * Scroller->YUnit;
	TPoint offset( -xOrg, -yOrg );
   dc.SetViewportOrg( offset );

   // Zoom
   TSize ws( GetClientRect().Size() );
   TSize wz( ws.cx/zoomFactor, ws.cy/zoomFactor );
   dc.SetMapMode( MM_ISOTROPIC );
   dc.SetWindowExt( wz, &TSize() );
	dc.SetViewportExt( ws, &TSize() );
	dc.DPtoLP( p, count );
}

void InselView::LPtoDP( TPoint* p, int count )
{
	TWindowDC dc( *this );

   // Zoom
   TSize ws( GetClientRect().Size() );
   TSize wz( ws.cx/zoomFactor, ws.cy/zoomFactor );
   dc.SetMapMode( MM_ISOTROPIC );
   dc.SetWindowExt( wz, &TSize() );
	dc.SetViewportExt( ws, &TSize() );

      // Scroll
   int xOrg = Scroller->XPos * Scroller->XUnit;
   int yOrg = Scroller->YPos * Scroller->YUnit;
	TPoint offset( -xOrg, -yOrg );
   dc.SetViewportOrg( offset );

  	dc.LPtoDP( p, count );
}

void InselView::resetCursor()
{
	if( fldDialog_p == 0 ||
       !fldDialog_p->IsWindow() ||
       fldDialog_p->loading() )
		SetCursor( 0, IDC_ARROW );
   else
   	SetCursor( GetApplication(), IDC_PENCIL );
}

void InselView::CmRandom()
{
	RandomDialog r( this, IDD_RANDOM );
	if( r.Execute() == IDOK )
   {
   	myDoc.setRandomValues( r.getProbability(), r.getCategory() );
   	myDoc.processCommand( CM_RANDOM );
   }
}

void InselView::CmMarkId()
{
	int numMarked = 0;
   InselFeld* firstMarked_p = 0;
   TRect r = myDoc.getSelection( numMarked, firstMarked_p );

   if( numMarked == 1 )
   {
		myDoc.selectId( firstMarked_p->id() );
   }
}
