#include <stdio.h>
#include "flddialog.h"
#include "annoedit.h"
#include "bitmaps.h"

DEFINE_RESPONSE_TABLE1( FldDialog, TDialog )
	EV_COMMAND(CM_ACCEPT, CmAccept),
   EV_COMMAND_ENABLE(CM_ACCEPT, CmAcceptEnable),
//   EV_COMMAND(IDC_UNDO, CmUndo),
//   EV_COMMAND_ENABLE(IDC_UNDO, CmUndoEnable),
   EV_BN_CLICKED(IDC_TURN, CmTurn),
   EV_COMMAND_ENABLE(IDC_TURN, CmTurnEnable),
   EV_COMMAND(IDC_KEEPORI, CmKeepOri),
//   EV_COMMAND(IDC_DETAILS, CmDetails),
//   EV_COMMAND_ENABLE(IDC_DETAILS, CeDetails),
   EV_COMMAND(IDC_MARKID, CmMarkId ),
   EV_COMMAND_ENABLE(IDC_MARKID, CeMarkId),
   EV_COMMAND_AND_ID(IDC_FELDLADEN, CmLadenSetzen),
   EV_COMMAND_AND_ID(IDC_FELDSETZEN, CmLadenSetzen),
   EV_TVN_SELCHANGED(IDC_SYSTREEVIEW1, CmFieldSelect),
   EV_WM_SETFOCUS,
   EV_WM_KILLFOCUS,
   EV_WM_ACTIVATE,
END_RESPONSE_TABLE;

FldDialog::FldDialog( TWindow* parent, TResId resId )
: TDialog( parent, resId ), fieldBg(0),
  _id(0), _ori(-1), _id_sav(0), _ori_sav(-1),
  currentView(0)
{
	editId = new TEdit( this, IDC_ID );
   editOri = new TEdit( this, IDC_ORI );
   editDescr = new TEdit( this, IDC_DESCR );
#ifdef DEBUG
   editExt = new TEdit( this, IDC_EXT );
#endif
	cbKeepOri = new TCheckBox( this, IDC_KEEPORI );
   treeWindow = new TTreeWindow( this, IDC_SYSTREEVIEW1 );

   feldLaden = new TRadioButton( this, IDC_FELDLADEN, 0 );
   feldSetzen = new TRadioButton( this, IDC_FELDSETZEN, 0 );

   bitmapFrame = new TStatic( this, IDC_BITMAPFRAME );
}

void FldDialog::clearAll()
{
	_id = _id_sav = 0;
   _ori = _ori_sav = -1;
   updateWindow();
}

void FldDialog::updateWindow()
{
   char buffer[100];

   if( _id > 0 )
   {
   	sprintf( buffer, "%d", _id );
   	editId->SetText( buffer );
      if( _ori < 0 ) _ori = 0;
   }
   else
   {
   	editId->Clear();
//      editDescr->Clear();
   }
		editDescr->SetText( theBitmaps->getDescr( _id ).c_str() );

   if( _ori >= 0 )
   {
   	sprintf( buffer, "%d", _ori );
   	editOri->SetText( buffer );
   } else editOri->Clear();

   TRect dr( bitmapFrame->GetWindowRect() );
   TPoint tl( dr.TopLeft() );
   ScreenToClient( tl );
   TPoint m = tl.OffsetBy( dr.Width()/2, dr.Height()/2 );	// Mittelpunkt der Bitmap
   m.Offset( GetSystemMetrics(SM_CXDLGFRAME),
   	       GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME) );

   TWindowDC dc( *this );
   TSize fs( InselFeld::unitSize(), InselFeld::unitSize() );
   if( _id > 0 && _ori >=0 )
   {
	 	if( fieldBg == 0 )
   	{
	     	// save actual background of field bitmap
	      TMemoryDC mdc( dc );
	      fieldBg = new TBitmap( dc, fs.cx*3, fs.cy*3 );
	      mdc.SelectObject( *fieldBg );
	      mdc.BitBlt( 0, 0, fs.cx*3, fs.cy*3, dc,
		     	m.x-fieldBg->Width()/2,
	      	m.y-fieldBg->Height()/2 );
   	}
	   else
   	{
	     	// restore original bitmap
	      TMemoryDC mdc( *fieldBg );
	      TSize sz( fieldBg->Size() );
	      TPoint tl = m.OffsetBy( -sz.cx/2, -sz.cy/2 );
	      dc.BitBlt( TRect( tl,sz ), mdc, TPoint(0,0) );
		}

   	TBitmap* bm_p = theBitmaps->getBitmap( dc, _id, _ori );
	   if( bm_p != 0 )
  		{
  			TMemoryDC mdc( *bm_p );
         TSize sz( bm_p->Size() );
         TPoint bmOrg( 0, 0 );
         if( sz.cx > fs.cx*3 )
         {
            bmOrg.Offset( (sz.cx-fs.cx*3)/2, (sz.cy-fs.cy*3)/2 );
         	sz.cx = sz.cy = fs.cx*3;
         }
         TPoint tl = m.OffsetBy( -sz.cx/2, -sz.cy/2 );
	      dc.BitBlt( TRect(tl,sz), mdc, bmOrg );
	   }
   }
   else
   if( fieldBg != 0 )
   {
   	// clear bitmap field
      TMemoryDC mdc( *fieldBg );
      TSize sz( fieldBg->Size() );
      TPoint tl = m.OffsetBy( -sz.cx/2, -sz.cy/2 );
      dc.BitBlt( TRect( tl,sz ), mdc, TPoint(0,0) );
	}

   // Select tree item
/*
   TTreeNode node( *treeWindow );
   TTreeItem item( buffer, 100 );
   while( node )
   {
      node.GetItem( &item );
   	int id;
	   if( isdigit( buffer[0] ) )
   	{
   		sscanf( buffer, "%d", &id );
         if( id == _id )
         {
         	 node.SelectItem( TTreeNode::Caret );
             break;
         }
   	}
      node = node.GetNextItem( TTreeNode::Next );
   }
*/
}

void FldDialog::SetupWindow()
{
	TDialog::SetupWindow();

   SetCaption( getString(IDS_FIELDEDITOR) );

   // position window in lower right corner of parent
   TRect myRect = GetClientRect();
   myRect.Inflate( 0, GetSystemMetrics(SM_CYCAPTION)/2 );
   myRect.Inflate( GetSystemMetrics(SM_CXBORDER),
                   GetSystemMetrics(SM_CYBORDER) );
   TRect parentRect = Parent->GetClientRect();
   TPoint bottomRight = parentRect.BottomRight();
   bottomRight.Offset( -GetSystemMetrics( SM_CXVSCROLL ),
                       -GetSystemMetrics( SM_CYHSCROLL ) );
   bottomRight.Offset( -GetSystemMetrics(SM_CXBORDER),
                       -GetSystemMetrics(SM_CYBORDER) - 20 );
   Parent->ClientToScreen( bottomRight );
   TPoint topLeft = bottomRight.OffsetBy( -myRect.Width(), -myRect.Height() );
   SetWindowPos( 0, TRect( topLeft, bottomRight ), SWP_NOZORDER );

   editId->LimitText(4);
   editOri->LimitText(1);

   cbKeepOri->SetCheck( BF_UNCHECKED );

   feldLaden->SetCheck( BF_CHECKED );
   feldSetzen->SetCheck( BF_UNCHECKED );

   TTreeNode* level[100];
   char c;
   int i;
   for( i=0; i<100; i++ ) level[i]=0;

   level[0] = new TTreeNode(treeWindow->GetRoot());
   ifstream file( descrFile.c_str() );
   file.unsetf( ios::skipws );
   while( true )
   {
   	int lcount = 1;
   	while( true )
      {
      	file >> c;
         if( file.good() && c == ' ' )
         	lcount++;
         else
         	break;
      }

      if( !file.good() ) break;
      file.putback( c );
      string descr;
      descr.read_line( file );
      if( level[lcount-1] != 0 )
      {
      	TTreeItem* item = new TTreeItem( descr.c_str(), -1 );
         delete level[lcount];
         level[lcount] = new TTreeNode( level[lcount-1]->AddChild( *item ) );
      }
   }

   treeWindow->Update();
   for( i=0; i<100; i++ ) delete level[i];
   updateWindow();
}

void FldDialog::CmAccept()
{
	char buffer[20];

	editId->GetText( buffer, 20 );
   sscanf( buffer, "%d", &_id );

   editOri->GetText( buffer, 20 );
   sscanf( buffer, "%d", &_ori );

   if( _id_sav != _id || _ori_sav != _ori )
   {
   	_id_sav = _id;
   	_ori_sav = _ori;

   	currentView->fldHasChanged();
   	updateWindow();
   }
}

void FldDialog::CmAcceptEnable( TCommandEnabler& enabler )
{
   bool enabled = (     _id != _id_sav
   					   || _ori != _ori_sav
   					   || editId->IsModified()
                     || editOri->IsModified()
                   )
   			       &&
                   ( editId->GetLineLength(0) > 0 &&
                     editOri->GetLineLength(0) > 0
                   )
                   &&
                   ( feldSetzen->GetCheck() == BF_UNCHECKED
                   );

	enabler.Enable( enabled );
//   cbKeepOri->EnableWindow( enabled );
}

void FldDialog::CmUndo()
{
	_id = _id_sav;
   _ori = _ori_sav;
	updateWindow();
}

void FldDialog::CmUndoEnable( TCommandEnabler& enable )
{
	enable.Enable( ( _id_sav > 0 && _ori_sav > 0 )
   					&&
                    (   _id != _id_sav
   				   	|| _ori != _ori_sav
 					   	|| editId->IsModified()
                  	|| editOri->IsModified()
                    )
                   &&
                   ( feldSetzen->GetCheck() == BF_UNCHECKED
                   )
   			     );
}

void FldDialog::CmTurn()
{
	char buffer[20];
	editOri->GetText( buffer, 20 );
   sscanf( buffer, "%d", &_ori );
   _ori = ( _ori + 1 ) % 4;
   updateWindow();
}

void FldDialog::CmTurnEnable( TCommandEnabler& enable )
{
	enable.Enable( editOri->GetLineLength(0) > 0 &&
   				   cbKeepOri->GetCheck() == BF_UNCHECKED );
}

void FldDialog::CmFieldSelect(TTwNotify& /*nmHdr*/)
{
	TTreeNode selNode = treeWindow->GetSelection();
   char buffer[100];
   TTreeItem selItem( buffer, 100 );
   selNode.GetItem( &selItem );
   if( isdigit( buffer[0] ) )
   {
   	sscanf( buffer, "%d", &_id );
	   updateWindow();
   }
}

void FldDialog::CmKeepOri()
{
   editOri->EnableWindow( cbKeepOri->GetCheck() == BF_UNCHECKED );
}

void FldDialog::EvSetFocus(HWND hWndLostFocus)
{
   TDialog::EvSetFocus( hWndLostFocus );
}

void FldDialog::EvKillFocus(HWND hWndGetFocus)
{
	// deselect selected item
	treeWindow->SelectItem( TTreeNode::Caret, 0 );
   TDialog::EvKillFocus( hWndGetFocus );
}

void FldDialog::EvActivate(uint active, bool minimized, HWND hWndOther)
{
	// deselect selected item
	treeWindow->SelectItem( TTreeNode::Caret, 0 );
   TDialog::EvActivate( active, minimized, hWndOther );
}

unsigned int FldDialog::id() const
{
	return _id;
}

unsigned int FldDialog::ori() const
{
	return _ori;
}

bool FldDialog::keepOri() const
{
	return cbKeepOri->GetCheck() == BF_CHECKED;
}

void FldDialog::setId( unsigned int id )
{
	_id = _id_sav = id;
}

void FldDialog::setOri( unsigned int ori )
{
	_ori = _ori_sav = ori;
}

void FldDialog::setView( InselView* view )
{
	currentView = view;
}

void FldDialog::CmDetails()
{
	if( currentView != 0 && _id != 0 )
   {
   	currentView->showFieldDetails();
   }
}

void FldDialog::CeDetails( TCommandEnabler& enabler )
{
	enabler.Enable( _id != 0 );
}

void FldDialog::CmLadenSetzen( WPARAM command )
{
	if( currentView == 0 ) return;

	switch( command )
   {
   case IDC_FELDLADEN:
   	feldLaden->SetCheck( BF_CHECKED );
      feldSetzen->SetCheck( BF_UNCHECKED );
   	currentView->SetCursor( 0, IDC_ARROW );
      break;

   case IDC_FELDSETZEN:
   	feldLaden->SetCheck( BF_UNCHECKED );
      feldSetzen->SetCheck( BF_CHECKED );
   	currentView->SetCursor( GetApplication(), IDC_PENCIL );
      break;
   }
}

bool FldDialog::loading() const
{
	return feldLaden->GetCheck() == BF_CHECKED;
}

void FldDialog::CmMarkId()
{
	currentView->markId( _id );
}

void FldDialog::CeMarkId( TCommandEnabler& enabler )
{
	enabler.Enable( editId->GetLineLength(0) > 0 );
}
