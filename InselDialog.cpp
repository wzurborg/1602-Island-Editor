#include "InselDialog.h"
#include "InselDoc.h"
#include "annoedit.h"

InselDialog::InselDialog( InselView* parent, TResId resId )
 : TDialog( parent, resId ),
   inselDoc( (InselDoc&)parent->GetDocument() )
{
	klimaBox = new TGroupBox( this, IDC_KLIMABOX );
	noerdlich = new TRadioButton( this, IDC_INSELNORD );
   suedlich = new TRadioButton( this, IDC_INSELSUED );
   treeAdjust = new TCheckBox( this, IDC_TREEADJUST );
   breite = new TEdit( this, IDC_BREITE );
   hoehe = new TEdit( this, IDC_HOEHE );
   posx = new TEdit( this, IDC_X );
   posy = new TEdit( this, IDC_Y );
}

void InselDialog::SetupWindow()
{
	TDialog::SetupWindow();

   char buffer[80];
   sprintf( buffer, "%d", inselDoc.size().cx );
   breite->SetText( buffer );
   sprintf( buffer, "%d", inselDoc.size().cy );
   hoehe->SetText( buffer );
   sprintf( buffer, "%d", inselDoc.pos().x );
   posx->SetText( buffer );
   sprintf( buffer, "%d", inselDoc.pos().y );
   posy->SetText( buffer );

	InselDoc::Klimatyp klimatyp = inselDoc.klima();

   if( klimatyp == InselDoc::UNKNOWN )
   {
   	klimaBox->EnableWindow( false );
   	noerdlich->EnableWindow( false );
      suedlich->EnableWindow( false );
      treeAdjust->SetCheck( BF_UNCHECKED );
      treeAdjust->EnableWindow( false );
	}
   else
   {
   	noerdlich->SetCheck
    	( klimatyp == InselDoc::NORD ? BF_CHECKED : BF_UNCHECKED );

   	suedlich->SetCheck
    	( klimatyp == InselDoc::SUED ? BF_CHECKED : BF_UNCHECKED );

      treeAdjust->SetCheck( BF_CHECKED );
   }
}

bool InselDialog::CanClose()
{
   TSize size;
   char buffer[10];
   breite->GetText( buffer, 10 );
   size.cx = atoi( buffer );
   hoehe->GetText( buffer, 10 );
   size.cy = atoi( buffer );

   if( size.cx > MaxIslandSize || size.cy > MaxIslandSize )
   {
   	char buffer[256];
      const char* str = getString( IDS_MAXSIZE );
      sprintf( buffer, str, MaxIslandSize );
      str = getString( IDS_ERROR );
   	MessageBox( buffer, str, MB_ICONEXCLAMATION );
      return false;
   }
   else
   if( size.cx < 1 || size.cy < 1 )
   {
   	msgBox( this, IDS_MINSIZE, IDS_ERROR, MB_ICONEXCLAMATION );
   	return false;
   }
   else
	   inselDoc.setSize( size );

	InselDoc::Klimatyp klimatyp = InselDoc::UNKNOWN;
	if( (noerdlich->GetCheck()) & BF_CHECKED )
   	klimatyp = InselDoc::NORD;
   else
   if( (suedlich->GetCheck()) & BF_CHECKED )
   	klimatyp = InselDoc::SUED;

  	inselDoc.setKlima( klimatyp );

   if( (treeAdjust->GetCheck()) & BF_CHECKED )
   	inselDoc.adjustTrees();

   TPoint pos;
	posx->GetText( buffer, 10 );
   pos.x = atoi( buffer );
   posy->GetText( buffer, 10 );
   pos.y = atoi( buffer );
	inselDoc.setPos( pos );

   return TDialog::CanClose();
}
