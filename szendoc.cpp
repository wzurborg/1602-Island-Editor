#include "SzenView.h"
#include "szendoc.h"

#include <mem.h>
#include <signal.h>
#include <list>
#include <functional>

//using namespace std;


class InselDraw : public unary_function<InselDoc*, void>
{
public:
	InselDraw( TDC& tdc ) : _tdc(tdc) {}
   void operator() ( InselDoc* insel )
   {
   	insel->drawMini( _tdc );
   }
private:
	TDC& _tdc;
};

// -----------------------------------------------------------------

DEFINE_DOC_TEMPLATE_CLASS( SzenDoc, SzenView, SzenTpl );

/*
BEGIN_REGISTRATION( InselReg )
	REGDATA( description, "Insel-Dateien (*.SCP)" )
   REGDATA( extension, ".SCP" )
   REGDATA( docfilter, "*.scp" )
   REGDOCFLAGS( dtAutoDelete|dtUpdateDir )
END_REGISTRATION

InselTpl* inselTpl = new InselTpl( InselReg );
*/

SzenTpl* szenTpl_p = new SzenTpl
( "Szenarios", "*.szs;*.szm;*.gam", 0, 0,
	dtAutoOpen|dtAutoDelete|dtUpdateDir|dtHideReadOnly|dtFileMustExist|dtOverwritePrompt );

// ------------------------------------------------------------------------

SzenDoc::SzenDoc( TDocument* parent )
: TFileDocument( parent ), myBuffer(0), bufLength(0), myInseln(0)
{
}

SzenDoc::~SzenDoc()
{
	if( myBuffer != 0 )
   	delete [] myBuffer;

   if( myInseln != 0 )
   	delete myInseln;
}


bool SzenDoc::InitDoc()
{
	if( GetDocPath() != 0 )
   	return true;

  	// create new document
	SetDirty( true );
   NotifyViews( vnRevert, false );
   return true;
}

bool SzenDoc::Open( int /*mode*/, const char far* path )
{
   if( path )
   {
   	// open existing document
   	SetDocPath( path );
	   if( GetDocPath() == 0 )
   	{
	   	return false;
   	}
	}

   if( GetDocPath() == 0 )
   {
   	// create new document
		SetDirty( true );
   	NotifyViews( vnRevert, false );
      return true;
   }

   TFileStatus fs;
   if( !myFile.Open( GetDocPath(), TFile::ReadOnly, TFile::PermRead )
    || !myFile.GetStatus( fs ))
   {
     	return false;
   }

   if( myBuffer != 0 )
   	delete [] myBuffer;

   bufLength = fs.size;
	myBuffer = new unsigned char[bufLength];
   if( myFile.Read( myBuffer, fs.size ) != bufLength )
   {
      myFile.Close();
      return false;
   }

   myFile.Close();

	SetDirty( false );
   NotifyViews( vnRevert, false );
	return true;
}

bool SzenDoc::IsOpen()
{
	return myBuffer != 0;
}

void SzenDoc::readInsel( unsigned char* buffer, uint buflength,
								InselDoc::FileType type )
{
	InselDoc* insel = new InselDoc( buffer, buflength, type);
   insel->readFromBuffer();
	myInseln->push_back( insel );
}

void SzenDoc::draw( TDC& tdc, TRect& r )
{
//	if( !IsOpen() ) Open(0);
	if( !IsOpen() ) return;

   if( myInseln == 0 )
   {
   	InselDoc::FileType type;
   	if( memcmp( myBuffer, "SZEN", 4 ) == 0 )
      	type = InselDoc::SZS;
      else
      	type = InselDoc::GAM;

   	myInseln = new CONTAINER<InselDoc*>;
   	int i = 0;
	   while( i < bufLength )
	   {
   	  while( i < bufLength &&
          memcmp( myBuffer+i, "INSEL5", 6 ) != 0 ) i++;
	     if( i >= bufLength ) break;
   	  readInsel( myBuffer+i, bufLength - i, type );
	     i++;
	   }
   }

//   tdc.FillRect( r, TBrush( TColor::LtBlue ));
   tdc.FillRect( r, TBrush( TColor(0,128,192) ));
	for_each( myInseln->begin(), myInseln->end(),
             InselDraw( tdc ) );

}

bool SzenDoc::Close()
{
   CONTAINER<InselDoc*>::iterator iter = myInseln->begin();
	CONTAINER<InselDoc*>::iterator end = myInseln->end();
   while( iter != end )
   {
   	delete *iter;
      iter++;
   }
   delete myInseln;
   myInseln = 0;

   delete myBuffer;
   myBuffer = 0;
	return TDocument::Close();
}

bool SzenDoc::Commit( bool force )
{
	if( !IsOpen() )
   	return false;

   if( !force && !IsDirty() )
   	return false;

   SetDirty( false );
	return TFileDocument::Commit( force );
}

bool SzenDoc::Revert( bool clear )
{
	if( !clear )
   {
		if( myView == 0 ||
   	  ( msgBox( myView, IDS_DISCARDCHANGES, IDS_REVERT,
                  MB_YESNO | MB_ICONQUESTION ) == IDYES ))
   	{
			Open(0);
	   }
      else return false;
	}

	return TFileDocument::Revert( clear );
}


void SzenDoc::setView( SzenView* view )
{
	myView = view;
}


