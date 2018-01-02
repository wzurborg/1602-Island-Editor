#include "flddialog.h"
#include "InselView.h"
#include "inseldoc.h"
#include "newinsel.h"
#include "Random.h"

#include <mem.h>
#include <signal.h>
#include <list>
#include <functional>

using namespace std;

static ofstream ostr( "C:\\TEMP\\Anno.out" );

// ------------------------------------------------------------------------

class ReplaceFieldId: public unary_function<InselFeld&, void>
{
public:
	ReplaceFieldId( list<int>* v1, list<int>* v2 )
     : myv1(v1), myv2(v2) {}
   void operator()( InselFeld& f );
private:
	list<int>* myv1;
   list<int>* myv2;
};

void ReplaceFieldId::operator()( InselFeld& f )
{
   list<int>::iterator i1 = myv1->begin();
   list<int>::iterator i2 = myv2->begin();
   while( i1 != myv1->end() && i2 != myv2->end() )
   {
    	if( f.myStruct._id == *i1 )
      	f.myStruct._id = *i2;
   	i1++;
      i2++;
   }
}

// ------------------------------------------------------------------------

class TurnIfSelected: public unary_function<InselFeld&, void>
{
public:
	TurnIfSelected( int numTurns, InselView& view )
   	: _numTurns(numTurns), theView(view) {}
   void operator() ( InselFeld& f );
private:
	int _numTurns;
   InselView& theView;
};

void TurnIfSelected::operator()( InselFeld& f )
{
	if( f.isSelected() )
   {
   	f.setOri( ( f.ori() + _numTurns ) % 4 );
      theView.InvalidateRect( f.rect(), false );
	}
}
// ------------------------------------------------------------------------

class UnmarkField: public unary_function<InselFeld&, void>
{
public:
	UnmarkField( InselView& view ) : theView(view) {}
	void operator() ( InselFeld& f );
private:
	InselView& theView;
};

void UnmarkField::operator()( InselFeld& f )
{
	if( f._selected )
   {
		f._selected = false;
      theView.InvalidateRect( f.rect(), false );
   }
}

// ------------------------------------------------------------------------

class FieldIsMarked: public unary_function<const InselFeld&, bool>
{
public:
        bool operator()( const InselFeld& f )
           { return f.isSelected(); }
};

// ------------------------------------------------------------------------

class FieldIsDummy: public unary_function<const InselFeld&, bool>
{
public:
	bool operator()( const InselFeld& f )
	   { return f.isDummy(); }
};

// ------------------------------------------------------------------------

class FieldIsOutside: public unary_function<const InselFeld&, bool>
{
public:
	FieldIsOutside( const TRect& r ) : theRect(r) {}
	bool operator()( const InselFeld& f )
   	{ return !theRect.Contains( f.fldRect() ); }
private:
	const TRect& theRect;
};

// ------------------------------------------------------------------------

class FieldContains: public unary_function<const InselFeld&, bool>
{
public:
	FieldContains( const TPoint& p ) : myPoint( p ) {}
   bool operator()( const InselFeld& f ) { return f.rect().Contains( myPoint ); }
private:
	const TPoint& myPoint;
};

// ------------------------------------------------------------------------

class MarkIfTouches: public unary_function<InselFeld&, void>
{
public:
	MarkIfTouches( const TRect& r, InselView& view )
   	 : myRect( r ), theView( view ) {}
	void operator() ( InselFeld& f );
private:
   const TRect& myRect;
	InselView& theView;
};

void MarkIfTouches::operator()( InselFeld& f )
{
	if( f.rect().Touches( myRect ) )
   {
   	f._selected = true;
      theView.InvalidateRect( f.rect(), false );
   }
}

// ------------------------------------------------------------------------

class MarkIfId: public unary_function<InselFeld&, void>
{
public:
	MarkIfId( int id ) : myId( id ) {}
   void operator() ( InselFeld& f );
private:
	int myId;
};

void MarkIfId::operator() ( InselFeld& f )
{
	if( f.myStruct._id == myId )
     f._selected = true;
}

// ------------------------------------------------------------------------

class MarkIfContains: public unary_function<InselFeld&, void>
{
public:
	MarkIfContains( const TPoint& p ) : myPoint( p ) {}
	void operator() ( InselFeld& f );
   static InselFeld* lastMarked;
private:
   const TPoint& myPoint;
};

InselFeld* MarkIfContains::lastMarked;

void MarkIfContains::operator()( InselFeld& f )
{
	if( f.rect().Contains( myPoint ) )
   {
   	if( !f._selected )
      {
	  		f._selected = true;
         lastMarked = &f;
      }
   }
   else
   {
   	if( f._selected )
      {
   		f._selected = false;
      }
   }
}

// ------------------------------------------------------------------------

class FlipIfTouches: public unary_function<InselFeld&, void>
{
public:
	FlipIfTouches( const TRect& r, InselView& view )
   	: myRect( r ), theView( view ) {}
	inline void operator() ( InselFeld& f );
private:
   const TRect& myRect;
	InselView& theView;
};

void FlipIfTouches::operator()( InselFeld& f )
{
	if( f.rect().Touches( myRect ) )
   {
   	f._selected = !f._selected;
      theView.InvalidateRect( f.rect(), false );
   }
}

// ------------------------------------------------------------------------

class FieldOverlaps: public unary_function<const InselFeld&, bool>
{
public:
	FieldOverlaps( const InselFeld& f ) : refFld( f ) {}
	bool operator()( const InselFeld& f ) const;
	const InselFeld& refFld;
};

bool FieldOverlaps::operator()( const InselFeld& f ) const
{
	return( /*f.pos() != refFld.pos() &&*/
   			f.rect().Touches( refFld.rect() ) );
}

// ------------------------------------------------------------------------

DEFINE_DOC_TEMPLATE_CLASS( InselDoc, InselView, InselTpl );

/*
BEGIN_REGISTRATION( InselReg )
	REGDATA( description, "Insel-Dateien (*.SCP)" )
   REGDATA( extension, ".SCP" )
   REGDATA( docfilter, "*.scp" )
   REGDOCFLAGS( dtAutoDelete|dtUpdateDir )
END_REGISTRATION

InselTpl* inselTpl = new InselTpl( InselReg );
*/

InselTpl* inselTpl_p = new InselTpl
( "Island Files", "*.scp", 0, "SCP",
	dtAutoOpen|dtAutoDelete|dtUpdateDir|dtHideReadOnly|dtFileMustExist|dtOverwritePrompt );

// ------------------------------------------------------------------------

InselDoc::Clipboard InselDoc::clipboard;

InselDoc::InselDoc( TDocument* parent )
: TFileDocument( parent ), myInsel_p(0), myBuffer(0), bufLength(0),
  mySize(0,0), index1stField(0), myKlima(UNKNOWN), myFileType(SCP),
  myPos(0,0), myView(0), myThread(0), feldIndex(0), deleteBuffer(true)
{
	header.insel3 = 0;
   myUndo.insel_p = 0;
   myUndo.size = TSize(0,0);
}

InselDoc::InselDoc( unsigned char* buffer, uint bufl,
                    InselDoc::FileType type, TDocument* parent )
: TFileDocument( parent ), myInsel_p(0), myBuffer(buffer), bufLength(bufl),
  mySize(0,0), index1stField(0), myKlima(UNKNOWN), myFileType(type),
  myPos(0,0), myView(0), myThread(0), feldIndex(0), deleteBuffer(false)
{
	header.insel3 = 0;
   myUndo.insel_p = 0;
   myUndo.size = TSize(0,0);
   myInsel_p = new CONTAINER<InselFeld>;
}

InselDoc::~InselDoc()
{
	if( myBuffer != 0 && deleteBuffer )
   	delete [] myBuffer;

   if( myInsel_p != 0 )
   	delete myInsel_p;

   if( myUndo.insel_p != 0 )
   	delete myUndo.insel_p;
}

const TSize& InselDoc::size() const
{
	return mySize;
}

void InselDoc::setSize( const TSize& size )
{
	if( size != mySize )
   {
   	saveUndoInfo();
		mySize = size;

   	// remove all fields that are now outside
	   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
	  	CONTAINER<InselFeld>::iterator result =
			remove_if( myInsel_p->begin(), myInsel_p->end(),
     		           FieldIsOutside( inselRect ) );
	   myInsel_p->erase( result, myInsel_p->end() );

      makeDummies();
      if( myView != 0 )
	      myView->Invalidate();
      SetDirty();
   }
}

const TPoint& InselDoc::pos() const
{
	return myPos;
}

void InselDoc::setPos( const TPoint& pos )
{
	if( pos != myPos )
   {
   	saveUndoInfo();

		myPos = pos;
      SetDirty();
   }
}

bool InselDoc::canUndo() const
{
	return myUndo.insel_p != 0;
}

void InselDoc::undo()
{
	if( myUndo.insel_p == 0 ) return;

   *myInsel_p = *myUndo.insel_p;
   mySize = myUndo.size;
   delete myUndo.insel_p;
   myUndo.insel_p = 0;
   myView->Invalidate();
}

void InselDoc::saveUndoInfo()
{
	if( myUndo.insel_p != 0 )
   {
		// remove old undo info
      delete myUndo.insel_p;
   }

   myUndo.insel_p = new CONTAINER<InselFeld>;
   *myUndo.insel_p = *myInsel_p;
   myUndo.size = mySize;
}

void InselDoc::unmarkAll()
{
	if( myInsel_p != 0 )
   {
   	for_each( myInsel_p->begin(), myInsel_p->end(),
      UnmarkField( *myView ) );
   }
}

void InselDoc::selectId( int id )
{
   for_each( myInsel_p->begin(), myInsel_p->end(), MarkIfId( id ) );
   myView->resetDialog();
   myView->Invalidate( false );
}

InselFeld* InselDoc::fieldFromPoint( const TPoint& p )
{
   if( feldIndex == 0 ) return 0;

   int x = p.x / InselFeld::unitSize();
   int y = p.y / InselFeld::unitSize();
   if( x > mySize.cx || y > mySize.cy ) return 0;
   InselFeld* f_p = &(*myInsel_p)[feldIndex[y*mySize.cx + x]];
   return f_p;
}

InselFeld* InselDoc::markOne( const TPoint& p )
{
	if( myInsel_p == 0 )	return 0;

   for_each( myInsel_p->begin(), myInsel_p->end(), MarkIfContains( p ) );
   myView->Invalidate( false );
   return MarkIfContains::lastMarked;
}

InselFeld* InselDoc::flipOne( const TPoint& p )
{
	if( myInsel_p == 0 ) return 0;

   CONTAINER<InselFeld>::iterator result =
     	find_if( myInsel_p->begin(), myInsel_p->end(), FieldContains( p ) );
   if( result != myInsel_p->end() )
   {
   	InselFeld& f = *result;
      f._selected = !f._selected;

	   myView->InvalidateRect( f.rect(), false );
	   return &f;
   }

   return 0;
}

void InselDoc::markAllTouching( const TRect& r )
{
	if( myInsel_p != 0 )
   {
   	for_each( myInsel_p->begin(), myInsel_p->end(),
      			MarkIfTouches( r, *myView ) );
   }
}

void InselDoc::flipAllTouching( const TRect& r )
{
	if( myInsel_p != 0 )
   {
   	for_each( myInsel_p->begin(), myInsel_p->end(),
      	FlipIfTouches( r, *myView ) );
   }
}

void InselDoc::setAllMarkedFrom( FldDialog& d, bool saveUndo )
{
	// all fields must be size 1x1 !!

	if( saveUndo )
		saveUndoInfo();

   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   while( iter != end )
   {
   	InselFeld& f = *iter;	// this is a reference
		if( f.isSelected() )
      {
         myView->InvalidateRect( f.rect(), false );
         f.takeValuesFrom( d );
      }
      iter++;
   }

   SetDirty( true );
}

void InselDoc::setRandom( bool saveUndo )
{
	if( theRandom == 0 ) return;

	if( saveUndo )
		saveUndoInfo();

   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   randomize();
   while( iter != end )
   {
   	InselFeld& f = *iter;	// this is a reference
		if( f.isSelected() )
      {
         myView->InvalidateRect( f.rect(), false );
         int id = theRandom->getRandom( myProbability, myCategory );
         if( id >= 0 )
	         f.setId( id );
      }
      iter++;
   }

   makeDummies();
   SetDirty( true );
}

void InselDoc::setOneFromDialog( InselFeld& fld, FldDialog& d, bool saveUndo )
{
	if( saveUndo )
		saveUndoInfo();

   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
  	InselFeld newFld( fld );
   newFld.takeValuesFrom( d );
	if( !inselRect.Contains( newFld.fldRect() ) )
   	return;

  	CONTAINER<InselFeld>::iterator result =
   		remove_if( myInsel_p->begin(), myInsel_p->end(),
                    FieldOverlaps( newFld ) );
   myInsel_p->erase( result, myInsel_p->end() );
   myInsel_p->push_back( newFld );
   myView->InvalidateRect( fld.rect(), false );
  	myView->InvalidateRect( newFld.rect(), false );
   makeDummies();
   SetDirty( true );
}

void InselDoc::turnArea( int numTurns )
{
	saveUndoInfo();

	int left = mySize.cx;
   int top = mySize.cy;
   int right = -1;
   int bottom = -1;
   bool foundMarked = false;

   // 1. extract all marked fields, remember area covered
	CONTAINER<InselFeld> markedFlds;
   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
   while( iter != end )
   {
   	InselFeld f = *iter;	// this is a copy
		if( f.isSelected() )
      {
      	foundMarked = true;
         TRect r( f.fldRect() );
         if( r.Left() < left ) left = r.Left();
         if( r.Right() > right ) right = r.Right();
         if( r.Top() < top ) top = r.Top();
         if( r.Bottom() > bottom ) bottom = r.Bottom();
        	markedFlds.push_back( f );
      }
      iter++;
   }

   if( !foundMarked ) return;

   // 2. Modify the position and orientation of these fields
   int i;
   for( i = numTurns; i > 0; i-- )
   {
	   TRect markedRect( left, top, right, bottom );
   	iter = markedFlds.begin();
      while( iter != markedFlds.end() )
      {
      	InselFeld& f = *iter;
         TPoint pOld( f.pos() );
         TSize sz( f.size() );
         TPoint pNew;
         pNew.x = left + bottom - pOld.y - sz.cy + 1;
         pNew.y = top + pOld.x - left;
         f.setPos( pNew );
         f.setOri( (f.ori() + 1) % 4 );
         myView->InvalidateRect( f.rect(), false );
         iter++;
      }

   	right = markedRect.Left() + markedRect.Height();
      if( right > mySize.cy ) right = mySize.cy;
      bottom = markedRect.Top() + markedRect.Width();
      if( bottom > mySize.cy ) bottom = mySize.cy;
   }

   // 2a. remove all fields that are now outside
  	CONTAINER<InselFeld>::iterator result =
		remove_if( markedFlds.begin(), markedFlds.end(),
     	           FieldIsOutside( inselRect ) );
   markedFlds.erase( result, markedFlds.end() );

   // 3. Delete fields overlapped by marked fields
   iter = markedFlds.begin();
   while( iter != markedFlds.end() )
   {
   	CONTAINER<InselFeld>::iterator result =
   		remove_if( myInsel_p->begin(), myInsel_p->end(),
                    FieldOverlaps( *iter ) );
	   myInsel_p->erase( result, myInsel_p->end() );
      iter++;
   }

   // 4. merge container with original after unmarking all fields
	unmarkAll();
   copy( markedFlds.begin(), markedFlds.end(), back_inserter( *myInsel_p ) );

   // 5. Create dummies for empty fields
   makeDummies();
   SetDirty( true );
}

void InselDoc::mirrorArea( MirrorOperation op )
{
	saveUndoInfo();
   
	int left = mySize.cx;
   int top = mySize.cy;
   int right = -1;
   int bottom = -1;
   bool foundMarked = false;

   // 1. extract all marked fields, remember area covered
	CONTAINER<InselFeld> markedFlds;
   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
   while( iter != end )
   {
   	InselFeld f = *iter;	// this is a copy
		if( f.isSelected() )
      {
      	foundMarked = true;
         TRect r( f.fldRect() );
         if( r.Left() < left ) left = r.Left();
         if( r.Right() > right ) right = r.Right();
         if( r.Top() < top ) top = r.Top();
         if( r.Bottom() > bottom ) bottom = r.Bottom();
        	markedFlds.push_back( f );
      }
      iter++;
   }

   if( !foundMarked ) return;

   // 2. Modify the position and orientation of these fields
   TRect markedRect( left, top, right, bottom );
  	iter = markedFlds.begin();
   while( iter != markedFlds.end() )
   {
     	InselFeld& f = *iter;
      TPoint pOld( f.pos() );
      TSize sz( f.size() );
      TPoint pNew( pOld );
      if( op == HORIZONTAL )
	      pNew.x = left + right - pOld.x - sz.cx + 1;
      else
      if( op == VERTICAL )
	      pNew.y = top + bottom - pOld.y - sz.cy + 1;

      f.setPos( pNew );
      myView->InvalidateRect( f.rect(), false );
      iter++;
   }

   // 2a. remove all fields that are now outside
  	CONTAINER<InselFeld>::iterator result =
		remove_if( markedFlds.begin(), markedFlds.end(),
     	           FieldIsOutside( inselRect ) );
   markedFlds.erase( result, markedFlds.end() );

   // 3. Delete fields overlapped by marked fields
   iter = markedFlds.begin();
   while( iter != markedFlds.end() )
   {
   	CONTAINER<InselFeld>::iterator result =
   		remove_if( myInsel_p->begin(), myInsel_p->end(),
                    FieldOverlaps( *iter ) );
	   myInsel_p->erase( result, myInsel_p->end() );
      iter++;
   }

   // 4. merge container with original after unmarking all fields
	unmarkAll();
   copy( markedFlds.begin(), markedFlds.end(), back_inserter( *myInsel_p ) );

   // 5. Create dummies for empty fields
   makeDummies();
   SetDirty( true );
}

void InselDoc::turnFields( int numTurns )
{
	saveUndoInfo();
  	for_each( myInsel_p->begin(), myInsel_p->end(),
      			TurnIfSelected( numTurns, *myView ) );
   SetDirty( true );
}

int InselDoc::numMarked() const
{
	if( myInsel_p == 0 ) return 0;

   int i = 0;
   count_if( myInsel_p->begin(), myInsel_p->end(),
                         FieldIsMarked(), i );
   return i;
}

void InselDoc::makeDummies()
{
	// creates dummy fields for all empty squares
   // also sets feldIndex array

   // 1. make array for the coverage
   if( feldIndex != 0 )
	   delete [] feldIndex;

   int numFlds = mySize.cx * mySize.cy;
   feldIndex = new int[numFlds];

   int i,j;
   for( i=0; i<numFlds; i++ )
   {
      feldIndex[i] = -1;
   }

   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   while( iter != end )
   {
		const InselFeld& f = *iter;
      const TPoint& fp = f.pos();
      const TSize& fs = f.size();
      for( i=0; i<fs.cx; i++ )
      {
      	for( j=0; j<fs.cy; j++ )
         {
         	TPoint p = fp.OffsetBy( i,j );
            feldIndex[p.y*mySize.cx + p.x] = iter - myInsel_p->begin();
         }
      }
      iter++;
   }

   // 2. create dummies for all not covered
  	for( i=0; i<mySize.cx; i++ )
   {
   	for( j=0; j<mySize.cy; j++ )
	   {
      	if( feldIndex[j*mySize.cx + i] == -1 )
	      {
   	   	InselFeld f( myInselNr );	// dummy
            f._selected = false;
      	   f.setPos( TPoint(i,j) );
            f.setSize();
            if( myView != 0 )
	            myView->InvalidateRect( f.rect(), false );
         	myInsel_p->push_back( f );
            feldIndex[j*mySize.cx + i]
            	= myInsel_p->end() - myInsel_p->begin() - 1;
	      }
      }
   }
}

int InselDoc::numDummy() const
{
	int i = 0;
	count_if( myInsel_p->begin(), myInsel_p->end(),
   			 FieldIsDummy(), i );
   return i;
}

TRect InselDoc::getSelection( int& numMarked, InselFeld*& firstMarked ) const
{
	if( myInsel_p == 0 )
   	return TRect();

	int left = mySize.cx;
   int top = mySize.cy;
   int right = -1;
   int bottom = -1;
   numMarked = 0;
   firstMarked = 0;

   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
   while( iter != end )
   {
   	InselFeld& f = *iter;	// this is a reference
		if( f.isSelected() )
      {
      	numMarked++;
      	if( firstMarked == 0 ) firstMarked = &f;
         TRect r( f.fldRect() );
         if( r.Left() < left ) left = r.Left();
         if( r.Right() > right ) right = r.Right();
         if( r.Top() < top ) top = r.Top();
         if( r.Bottom() > bottom ) bottom = r.Bottom();
      }
      iter++;
   }

	if( firstMarked != 0 )
		return TRect( left, top, right, bottom );
   else
   	return TRect( -1, -1, -1, -1 );
}

void InselDoc::copyToClipboard()
{
	// empty clipboard
   clipboard.cbData.resize( 0 );

   // loop over marked fields and copy to clipboard
   // remember top left element for later paste operation
   clipboard.cbTopLeft = TPoint( mySize.cx, mySize.cy );
   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
	CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   while( iter != end )
   {
   	InselFeld& f = *iter;
		if( f.isSelected() )
      {
      	InselFeld newFld( f );
         TPoint p( f.pos() );
         TPoint& cbp( clipboard.cbTopLeft );
         if( p.x < cbp.x ) cbp.x = p.x;
         if( p.y < cbp.y ) cbp.y = p.y;
         clipboard.cbData.push_back( newFld );
      }
      iter++;
   }
}

void InselDoc::pasteFromClipboard()
{
	// find first marked field
   CONTAINER<InselFeld>::iterator result =
   find_if( myInsel_p->begin(), myInsel_p->end(), FieldIsMarked() );
   if( result == myInsel_p->end() || clipboard.cbData.empty() )
   	return;

	saveUndoInfo();

   InselFeld& fm = *result;
   TSize offset = fm.pos() - clipboard.cbTopLeft;

   // remove fields overlapped by fields from clipboard
   CONTAINER<InselFeld>::iterator iter = clipboard.cbData.begin();
   CONTAINER<InselFeld>::iterator end = clipboard.cbData.end();
   TRect inselRect( 0, 0, mySize.cx-1, mySize.cy-1 );
   while( iter != end )
   {
   	InselFeld f( myInselNr );	// this will be the new field
   	f.setSelectedFrom( *iter );
      f.setPos( (*iter).pos().OffsetBy( offset.cx, offset.cy ) );
      if( inselRect.Contains( f.fldRect() ) )
      {
	   	CONTAINER<InselFeld>::iterator result =
   			remove_if( myInsel_p->begin(), myInsel_p->end(),
      	              FieldOverlaps( f ) );
	   	myInsel_p->erase( result, myInsel_p->end() );
      	myInsel_p->push_back( f );
         myView->InvalidateRect( f.rect(), false );
      }
      iter++;
   }

   // Create dummies for empty fields
   makeDummies();
   SetDirty( true );
}

bool InselDoc::clipboardEmpty() const
{
	return clipboard.cbData.empty();
}

bool InselDoc::InitDoc()
{
	if( GetDocPath() != 0 )
   	return true;

  	// create new document
	if( myInsel_p != 0 )
   	delete myInsel_p;
   myInsel_p = new CONTAINER<InselFeld>;
   if( !createNewInsel() )
   	return false;
      
	SetDirty( true );
   NotifyViews( vnRevert, false );
   return true;
}

bool InselDoc::Open( int /*mode*/, const char far* path )
{
	if( myInsel_p != 0 )
   	delete myInsel_p;

   myInsel_p = new CONTAINER<InselFeld>;

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
      createNewInsel();
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

   if( myBuffer != 0 && deleteBuffer )
   	delete [] myBuffer;

   bufLength = fs.size;
	myBuffer = new unsigned char[bufLength];
   if( myFile.Read( myBuffer, fs.size ) != bufLength )
   {
      myFile.Close();
      return false;
   }
   myFile.Close();

   if( !readFromBuffer() )
   {
   	myFile.Close();
      return false;
   }

   makeDummies();

	SetDirty( false );
   NotifyViews( vnRevert, false );
	return true;
}

bool InselDoc::createNewInsel()
{
   NewInsel newInsel( GetDocManager().GetApplication()->GetMainWindow() );
   if( newInsel.Execute() != IDOK )
   	return false;

   if( myBuffer != 0 && deleteBuffer )
   	delete [] myBuffer;

   myKlima = newInsel.klima();
   mySize = newInsel.size();
//   myPos = TPoint( 100, 100 );
   myPos = TPoint( 10, 10 );
   myInselNr = 7;

   uint numFld = mySize.cx * mySize.cy;
   bufLength = 20 + sizeof( TINSEL5 ) + 20 + (numFld * 8);
	myBuffer = new unsigned char[bufLength];
   memset( myBuffer, 0, bufLength );

   // secrets (don't kno why)
   static const unsigned char secret1[]
   	= {0x7D, 0x21, 0xF1, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0x74};
   static const unsigned char secret2[]
      = {0x00, 0x96, 0x01, 0xE8, 0xFB, 0x12};

   // set header "INSEL5"
   memcpy( myBuffer, "INSEL5", 6 );
   memcpy( &myBuffer[8], secret1, 9 );
   int index = 20;
  	myInseltyp = 5;
  	header.insel5 = (TINSEL5*)&myBuffer[index];
   header.insel5->inselnr = (uint8)myInselNr;
   header.insel5->felderx = (uint8)mySize.cx;
   header.insel5->feldery = (uint8)mySize.cy;
   header.insel5->nofixflg = 1;	// weiss nicht warum
   header.insel5->posx = (uint16)myPos.x;
   header.insel5->posy = (uint16)myPos.y;
	int i;
   for( i=0; i<8; i++ )
   	header.insel5->stadtplayernr[i] = 7;	// weiss nicht warum
   header.insel5->rohstflags = 0x1181;	// weiss nicht warum
   header.insel5->filenr = -1;
	header.insel5->orginalflg = 1;	// weiss nicht warum


	header.insel5->klimanr = myKlima == NORD ? 0 : 1;
   index += sizeof( TINSEL5 );

   // set second header
  	memcpy( myBuffer+index, "INSELHAUS", 9 );
   memcpy( myBuffer+index+9, secret2, 6 );

   index += 16;
   *((unsigned long*) (myBuffer+index)) = numFld * 8;
   index1stField = index + 4;
//   unsigned char* bufAddr = &myBuffer[index1stField];
   InselFeld newFld( myInselNr );
   newFld.setId( 1201 );	// tiefer Ozean
   for( i=0; i<numFld; i++ )
   {
   	int x = i % mySize.cx;
      int y = i / mySize.cx;
   	newFld.setPos( TPoint(x,y));
      myInsel_p->push_back( newFld );
   }

   makeDummies(); // to create the fldIndex
   return true;
}

bool InselDoc::IsOpen()
{
	return myBuffer != 0;
}

void InselDoc::draw( TDC& tdc, TRect& r )
{
//	if( !IsOpen() ) Open(0);
	if( !IsOpen() ) return;

   if( myInsel_p == 0 ) return;
   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
   CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   while( iter != end )
   {
   	InselFeld& fld = *iter;
      if( fld.rect().Touches( r ) )
   		fld.draw( tdc );
      iter++;
   }
}

bool InselDoc::Close()
{
	delete myInsel_p;
   myInsel_p = 0;
   delete myBuffer;
   myBuffer = 0;
	return TDocument::Close();
}

bool InselDoc::Commit( bool force )
{
	if( !IsOpen() )
   	return false;

   if( !force && !IsDirty() )
   	return false;

   if( numDummy() > 0 )
   {
      if( msgBox( myView,
          IDS_WHITEFIELDS, IDS_PROBLEM,
          MB_YESNO | MB_ICONQUESTION ) != IDYES
        )
      {
         return false;
      }
      else
      {
         msgBox( myView,
          IDS_NOLOAD, IDS_WARNING, MB_ICONWARNING );
      }
   }

   if( !myFile.Open( GetDocPath(), TFile::Create|TFile::WriteOnly, TFile::PermRdWr ))
   {
   	msgBox( myView, IDS_SAVEERROR, IDS_ERROR, MB_ICONWARNING );
      return false;
   }

   if( myInseltyp == 5 )
   {
   	header.insel5->felderx = (uint8)mySize.cx;
      header.insel5->feldery = (uint8)mySize.cy;
      header.insel5->posx = (uint16)myPos.x;
      header.insel5->posy = (uint16)myPos.y;

	   // Insel kann im SzEditor nicht gedreht werden, wenn Bytes 116 und 117
	   // 0xFF sind - das Setzen auf 0 ist laut Linde möglich
      header.insel5->filenr = 0;

      if( mySize.cx <= 32 ) header.insel5->sizenr = 0;
      else if( mySize.cx <= 42 ) header.insel5->sizenr = 1;
      else if( mySize.cx <= 55 ) header.insel5->sizenr = 2;
      else if( mySize.cx <= 75 ) header.insel5->sizenr = 3;
      else if( mySize.cx <= 110 ) header.insel5->sizenr = 4;

	   if( myKlima != UNKNOWN )
   		header.insel5->klimanr = (myKlima == NORD) ? (uint8)0 : (uint8)1;
   }
   else
   if( myInseltyp == 3 )
   {
   	header.insel3->felderx = (uint8)mySize.cx;
      header.insel3->feldery = (uint8)mySize.cy;
      header.insel3->posx = (uint16)myPos.x;
      header.insel3->posy = (uint16)myPos.y;
   }

   uint numFld = myInsel_p->size();
   uint indexNumFld = index1stField - 4;
   *((unsigned long*) (myBuffer+indexNumFld)) = numFld * 8;

   // write up to here
   myFile.Write( myBuffer, index1stField );

   // sort the InselFeld array
/*
   myView->	SetCursor( 0, IDC_WAIT );
   sort( myInsel_p->begin(), myInsel_p->end() );
   myView->	SetCursor( 0, IDC_ARROW );
*/
   unsigned char* fldBuffer = new unsigned char[ numFld * 8 ];
	unsigned char* addr = fldBuffer;
   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
   while( iter != myInsel_p->end() )
   {
   	InselFeld& f = *iter;
		f.write( addr );
      iter++;
   }

   int bytesWritten = myFile.Write( fldBuffer, numFld * 8 );
   delete [] fldBuffer;
   if( bytesWritten != numFld*8 )
   {
      msgBox( myView, IDS_SAVEERROR, IDS_ERROR, MB_ICONWARNING );
   	myFile.Close();
      return false;
   }

   myFile.Close();

   SetDirty( false );
	return TFileDocument::Commit( force );
}

bool InselDoc::Revert( bool clear )
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

InselDoc::Klimatyp InselDoc::klima() const
{
	return myKlima;
}

void InselDoc::setKlima( Klimatyp klima )
{
	if( klima != myKlima )
   {
		myKlima = klima;
      SetDirty();
   }
}

void InselDoc::adjustTrees( bool saveUndo )
{
	static int nordBaum[] = { 1304, 1306, 1308, 1310, 1312, 1314, 1316, 1318, 1320, 1322, 1324 };
   static int suedBaum1[] ={ 1352, 1354, 1356, 1358, 1360, 1362, 1364, 1366, 1368, 1370, 1372 };
   static int suedBaum2[] ={ 2802, 2804, 2806, 2808, 2810, 2812, 2814, 2816, 2818, 2820, 2822 };
   static int C_numFlds = 11;

   list<int> v1, v2;
   int* a1;
   int* a2;

   switch( myKlima )
   {
   case NORD:
   	a1 = suedBaum1;
      a2 = nordBaum;
   	break;

   case SUED:
   	a1 = nordBaum;
      a2 = suedBaum1;
      break;

   default:
   	return;
   }

   for( int i=0; i<C_numFlds; i++ )
   {
   	v1.push_back( a1[i] );
      v2.push_back( a2[i] );
   }

   for_each( myInsel_p->begin(), myInsel_p->end(), ReplaceFieldId( &v1, &v2 ));

   if( myKlima == NORD )
   {
   	a1 = suedBaum2;
      list<int> v3;
	   for( int i=0; i<C_numFlds; i++ )
   		v3.push_back( a1[i] );
		for_each( myInsel_p->begin(), myInsel_p->end(), ReplaceFieldId( &v3, &v2 ));
   }

   myView->Invalidate( false );
   SetDirty( true );
}

void InselDoc::setView( InselView* view )
{
	myView = view;
}

void InselDoc::setRandomValues( int probability, const string& category )
{
	myProbability = probability;
   myCategory = category;
}

bool InselDoc::isBusy() const
{
	return ( myThread != 0 );
}

void InselDoc::processCommand( int command )
{
	if( myThread != 0 )
   {
   	myThread->WaitForExit();
      delete myThread;
   }

   myThread = new InselThread( *this, command );
   myThread->Start();
}

void InselDoc::threadCommand( int command )
{
	switch( command )
   {
   case CM_EDITCOPY:
		if( numMarked() > 0 )
			copyToClipboard();
      break;

   case CM_EDITPASTE:
	   if( !clipboardEmpty() && numMarked() == 1 )
		{
			pasteFromClipboard();
		   myView->resetDialog();
      }
		break;

   case CM_AREALEFT:
		turnArea( 3 );
      break;

   case CM_AREARIGHT:
		turnArea( 1 );
      break;

   case CM_AREA180:
		turnArea( 2 );
      break;

   case CM_FIELDLEFT:
		turnFields( 3 );
      break;

   case CM_FIELDRIGHT:
		turnFields( 1 );
      break;

   case CM_FIELD180:
		turnFields( 2 );
      break;

   case CM_HORIZONTAL:
		mirrorArea( HORIZONTAL );
      break;

   case CM_VERTICAL:
		mirrorArea( VERTICAL );
      break;

   case CM_RANDOM:
   	setRandom();
      break;

   default:
   	break;
   }

	if( myView != 0 )
   {
      myView->PostMessage( WM_USER );
   }
}

void InselDoc::commandTerminated()
{
	if( myThread != 0 )
   {
   	myThread->WaitForExit();
      delete myThread;
      myThread = 0;
   }
}

bool InselDoc::readFromBuffer()
{
   // read header "INSEL"
   if( memcmp( myBuffer, "INSEL", 5 ) != 0
   	 || bufLength < 60 )
     return false;

   int index = 20;
   char typ = myBuffer[5];
   if( typ == '5' || typ == '4' )
   {
   	myInseltyp = 5;
   	header.insel5 = (TINSEL5*)&myBuffer[index];
      myInselNr = header.insel5->inselnr;
      mySize.cx = header.insel5->felderx;
      mySize.cy = header.insel5->feldery;
      myPos.x = header.insel5->posx;
      myPos.y = header.insel5->posy;
   	myKlima = header.insel5->klimanr == 0 ? NORD : SUED;
      index += sizeof( TINSEL5 );
   }
   else
   if( typ == '3' )
   {
   	myInseltyp = 3;
   	header.insel3 = (TINSEL3*)&myBuffer[index];
      myInselNr = header.insel3->inselnr;
	   mySize.cx = header.insel3->felderx;
      mySize.cy = header.insel3->feldery;
      myPos.x = header.insel3->posx;
      myPos.y = header.insel3->posy;
      index += sizeof( TINSEL3 );
   }
   else
   {
//   	::MessageBox( 0, "Unbekannter Inseltyp", "Fehler", MB_OK );
      return false;
   }

   // check second header
  	if( memcmp( myBuffer+index, "INSELHAUS", 9 ) != 0 )
   {
//   	::MessageBox( 0, "Unbekannter Inseltyp", "Fehler", MB_OK );
      return false;
   }

   index += 16;
   uint numFld = *((unsigned long*) (myBuffer+index)) / 8;
   index1stField = index + 4;

   // Read Insel from Game NORD/SUED directory if not completely
   // defined in Szenario file *and* if not a savegame

   uint16 filenr = header.insel5->filenr;
   if( myFileType == SZS && myInseltyp == 5 &&
       filenr >= 0 && filenr < 100 )
   {
   	string sizeStr;
      switch( header.insel5->sizenr )
      {
      case 0: sizeStr = "lit"; break;
      case 1: sizeStr = "mit"; break;
      case 2: sizeStr = "med"; break;
      case 3: sizeStr = "big"; break;
      case 4: sizeStr = "lar"; break;
      default: break;
      }
      string klimastr = myKlima == NORD ? "NORD" : "SUED";
      char filenum[5];
      sprintf( filenum, "%-2.2d", filenr );
      string filename = annoInstallDir + klimastr + "\\"
      	+ sizeStr + filenum + ".scp";

		TFile inselFile;
      TFileStatus fs;
   	if( inselFile.Open( filename.c_str(), TFile::ReadOnly, TFile::PermRead )
       && inselFile.GetStatus( fs ))
   	{
		   if( myBuffer != 0 && deleteBuffer )
   		delete [] myBuffer;

		   bufLength = fs.size;
			myBuffer = new unsigned char[bufLength];
         deleteBuffer = true;
   		if( inselFile.Read( myBuffer, fs.size ) == bufLength )
   		{
         	switch( myBuffer[5] )
            {
            case '3': index = 0x4c; break;
            default:  index = 0x98; break;
            }
            numFld = *((unsigned long*) (myBuffer+index)) / 8;
            index1stField = index + 4;
         }
      }
  		inselFile.Close();
   }

   unsigned char* bufAddr = &myBuffer[index1stField];
	int i;
   for( i=0; i<numFld; i++ )
   {
   	if( bufAddr - &myBuffer[0] < bufLength )
      {
      	InselFeld fld( bufAddr );
	      myInsel_p->push_back( fld );
      }
   }

	if( bufAddr -&myBuffer[0] != bufLength )
   {
   // Insel hat einen Trailer!
	/*   	::MessageBox( 0, "Inseltyp wird nicht unterstützt!",
        				  "Sorry", MB_OK ); */
	//      return false;
   }

	return true;
}

void InselDoc::drawMini( TDC& tdc ) const
{
	int factor = 2;

   TPoint pos( myPos.x*factor, myPos.y*factor );
   TSize size( mySize.cx*factor, mySize.cy*factor );

   TRect inselrect( pos, size );
//   tdc.FillRect( inselrect, TBrush( TColor::LtGray ) );

   if( myInsel_p == 0 )
   	return;

ostr << "Dumping Insel " << myInselNr
	  << ", pos = " << myPos
     << ", filenr = " << (int)header.insel5->filenr
/*
     << ", sizenr = " << (int)header.insel5->sizenr
     << ", klimanr = " << (int)header.insel5->klimanr
     << ", rotier = " << (int)header.insel5->rotier
*/
     << endl;

//	tdc.FillRect( inselrect, TColor::LtBlue );

   CONTAINER<InselFeld>::iterator iter = myInsel_p->begin();
   CONTAINER<InselFeld>::iterator end = myInsel_p->end();
   while( iter != end )
   {
   	InselFeld& fld = *iter;
 		fld.drawMini( tdc, pos, factor );
      iter++;
   }

/*
   char sbuf[10];
   sprintf( sbuf, "%d", myInselNr );
	tdc.TextOut( inselrect.TopLeft().OffsetBy(2,2), sbuf );
*/
   if( myInseltyp == 5 )
   {
   	TINSEL5* t5 = header.insel5;
   	for( int i=0; i<t5->eisencnt; i++ )
   	{
      	TERZBERG& t = t5->eisenberg[i];
	      if( t.ident == 58 )
	      {
	      	TPoint schatzp = inselrect.TopLeft()
            	+ TSize( t.posx*factor, t.posy*factor );
	         TRect schatzr1( schatzp.OffsetBy(-1*factor,-3*factor),
            	TSize(3*factor,7*factor) );
	         tdc.FillRect( schatzr1, TColor::LtRed );
            TRect schatzr2( schatzp.OffsetBy(-3*factor,-1*factor),
            	TSize(7*factor,3*factor) );
            tdc.FillRect( schatzr2, TColor::LtRed );
	      }
   	}
   }
}

// ------------------------------------

InselThread::InselThread( InselDoc& doc, int command )
 : _doc(doc), _command(command)
{
}

int InselThread::Run()
{
	_doc.threadCommand( _command );
   return 0;
}

