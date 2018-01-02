#include "RandomDialog.h"
#include "Random.h"
#include "AnnoEdit.h"

RandomDialog::RandomDialog( InselView* parent, TResId resId )
: TDialog( parent, resId ), _probability(-1)
{
	percentSel = new TComboBox( this, IDC_FLDPERCENT );
   fldSel = new TComboBox( this, IDC_FLDSELECT );
}

void RandomDialog::SetupWindow()
{
	TDialog::SetupWindow();

   percentSel->AddString( "25" );
   percentSel->AddString( "50" );
   percentSel->AddString( "75" );
   percentSel->AddString( "100" );
   percentSel->SetText( "100" );

   if( theRandom != 0 )
   {
	   CatIndex::iterator iter = theRandom->catIndex.begin();
      while( iter != theRandom->catIndex.end() )
      {
      	fldSel->AddString( (*iter).c_str() );
         iter++;
      }
      fldSel->SetSelIndex( 0 );
   }
}

bool RandomDialog::CanClose()
{
	char buffer[80];
   percentSel->GetText( buffer, 80 );
   string ps( buffer );
   fldSel->GetText( buffer, 80 );
   string fs( buffer );

	_probability = atoi( ps.c_str() );
   _category = fs;
   return true;
}

int RandomDialog::getProbability() const
{
	return _probability;
}

const string& RandomDialog::getCategory() const
{
	return _category;
}


