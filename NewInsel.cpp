#include "newinsel.h"
#include "annoedit.h"

DEFINE_RESPONSE_TABLE1( NewInsel, TDialog )
	EV_BN_CLICKED( IDC_LAR, CmTypeChanged ),
	EV_BN_CLICKED( IDC_BIG, CmTypeChanged ),
	EV_BN_CLICKED( IDC_MED, CmTypeChanged ),
	EV_BN_CLICKED( IDC_MIT, CmTypeChanged ),
	EV_BN_CLICKED( IDC_LIT, CmTypeChanged ),
   EV_EN_UPDATE( IDC_NEWX, CmSizeChanged ),
END_RESPONSE_TABLE;

NewInsel::NewInsel( TWindow* parent )
 : TDialog( parent, IDD_NEWINSEL )
{
	sizeLar = new TRadioButton( this, IDC_LAR, 0 );
   sizeBig = new TRadioButton( this, IDC_BIG, 0 );
   sizeMed = new TRadioButton( this, IDC_MED, 0 );
   sizeMit = new TRadioButton( this, IDC_MIT, 0 );
   sizeLit = new TRadioButton( this, IDC_LIT, 0 );

   sizeX = new TEdit( this, IDC_NEWX );
   sizeY = new TEdit( this, IDC_NEWY );

   klimaNord = new TRadioButton( this, IDC_NEWNORD, 0 );
   klimaSued = new TRadioButton( this, IDC_NEWSUED, 0 );
}

void NewInsel::SetupWindow()
{
	TDialog::SetupWindow();
   
   // center window to parent
   TRect parentRect = Parent->GetClientRect();
   TRect myRect = GetClientRect();

   int x = parentRect.Left() + parentRect.Width()/2 - myRect.Width()/2;
   int y = parentRect.Top() + parentRect.Height()/2 + myRect.Height()/2;
   myRect.MoveTo( x, y );
   SetWindowPos( 0, myRect, SWP_NOZORDER | SWP_NOSIZE );

	sizeLar->SetCheck( BF_UNCHECKED );
	sizeBig->SetCheck( BF_UNCHECKED );
	sizeMed->SetCheck( BF_CHECKED );
	sizeMit->SetCheck( BF_UNCHECKED );
	sizeLit->SetCheck( BF_UNCHECKED );

   sizeX->LimitText( 3 );
   sizeX->SetText( "50" );
   sizeY->LimitText( 3 );
   sizeY->SetText( "50" );

   klimaNord->SetCheck( BF_CHECKED );
   klimaSued->SetCheck( BF_UNCHECKED );
}

void NewInsel::CmTypeChanged()
{
   char* sizeText;
	if( sizeLar->GetCheck() == BF_CHECKED )
      sizeText = "100";
   else
	if( sizeBig->GetCheck() == BF_CHECKED )
      sizeText = "70";
   else
	if( sizeMed->GetCheck() == BF_CHECKED )
      sizeText = "50";
   else
	if( sizeMit->GetCheck() == BF_CHECKED )
      sizeText = "40";
   else
	if( sizeLit->GetCheck() == BF_CHECKED )
      sizeText = "30";
	else return;

   sizeX->SetText( sizeText );
   sizeY->SetText( sizeText );
}

void NewInsel::CmSizeChanged()
{
	char buffer[5];
   sizeX->GetText( buffer, 5 );
   int newX = atoi( buffer );

	sizeLar->SetCheck( BF_UNCHECKED );
	sizeBig->SetCheck( BF_UNCHECKED );
	sizeMed->SetCheck( BF_UNCHECKED );
	sizeMit->SetCheck( BF_UNCHECKED );
	sizeLit->SetCheck( BF_UNCHECKED );

   if( newX <= 32 )
   	sizeLit->SetCheck( BF_CHECKED );
   else
   if( newX <= 42 )
   	sizeMit->SetCheck( BF_CHECKED );
   else
   if( newX <= 55 )
   	sizeMed->SetCheck( BF_CHECKED );
   else
   if( newX <= 75 )
   	sizeBig->SetCheck( BF_CHECKED );
   else
//   if( newX <= 110 )
   	sizeLar->SetCheck( BF_CHECKED );
}

bool NewInsel::CanClose()
{
	char buffer[5];
   sizeX->GetText( buffer, 5 );
   mySize.cx = atoi( buffer );
   sizeY->GetText( buffer, 5 );
   mySize.cy = atoi( buffer );

   if( mySize.cx > MaxIslandSize || mySize.cy > MaxIslandSize )
   {
   	char buffer[256];
      const char* str = getString( IDS_MAXSIZE );
      sprintf( buffer, str, MaxIslandSize );
      str = getString( IDS_ERROR );
   	MessageBox( buffer, str, MB_ICONEXCLAMATION );
   	return false;
   }

   if( mySize.cx < 1 || mySize.cy < 1 )
   {
   	msgBox( this, IDS_MINSIZE, IDS_ERROR, MB_ICONEXCLAMATION );
   	return false;
   }

   myKlima = klimaNord->GetCheck() == BF_CHECKED ?
      	InselDoc::NORD : InselDoc::SUED;

   return true;
}

TSize NewInsel::size() const
{
   return mySize;
}

InselDoc::Klimatyp NewInsel::klima() const
{
	return myKlima;
}
