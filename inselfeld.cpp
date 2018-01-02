#include <strstrea.h>
#include <iomanip.h>

#include "flddialog.h"
#include "inselfeld.h"
#include "bitmaps.h"

unsigned InselFeld::_unitSize = 15;

unsigned InselFeld::unitSize()
{
	return _unitSize;
}

void InselFeld::setUnitSize( unsigned unitSize )
{
	_unitSize = unitSize;
}

unsigned InselFeld::fldSize( int id )
{
	switch( id )
   {
   	case 1110:
			return 6;

   	case 1108:
      case 1109:
			return 4;

   	case 1103:
      case 1106:
      case 1107:
      	return 3;

      case  904:
      case  905:
   	case 1104:
      case 1105:
      	return 2;

      default:
      	return 1;
   }
}

TColor InselFeld::pixelColor() const
{
	uint16 id = myStruct._id;

   if( ( id > 1200 && id < 1215 ) ||
       ( id > 1237 && id < 1300 ) ||
       ( id >  900 && id <  906 ) )
//   	return TColor::LtBlue;
   	return TColor(0,128,192);

   if( ( id > 1103 && id < 1111 ) ||
       ( id > 2300 && id < 2400 ) )
   	return TColor::LtGray;

   if( ( id >  106 && id <  161 ) ||
       ( id > 2761 && id < 2801 ) )
      return TColor::LtYellow;
      
   return TColor::LtGreen;
}

InselFeld::InselFeld( uint inselnr )
 : _selected(false), _size(1,1)
{
   memset( &myStruct, 0, sizeof(myStruct) );
   myStruct._inselnr = inselnr;
   myStruct._stadtnr = 7;
   myStruct._randnr = random(32);
   myStruct._playernr = 15;
}

InselFeld::InselFeld( unsigned char*& inAddr )
 : _selected(false), _size(1,1)
{
	// initialises the object from a buffer
   // increments buffer
   memcpy( &myStruct, inAddr, sizeof( myStruct ));
   inAddr += sizeof( myStruct );
   setSize();
}

void InselFeld::setSelectedFrom( const InselFeld& other )
{
	myStruct._id = other.myStruct._id;
   myStruct._ori = other.myStruct._ori;
   myStruct._animcnt = other.myStruct._animcnt;
//   myStruct._inselnr = other.myStruct._inselnr;
   myStruct._stadtnr = other.myStruct._stadtnr ;
   myStruct._randnr = other.myStruct._randnr ;
   myStruct._playernr = other.myStruct._playernr ;
   _selected = other._selected;
   _size = other._size;
}

bool InselFeld::operator ==( const InselFeld& other ) const
{
	return (memcmp(&myStruct, &other.myStruct, sizeof(myStruct)) == 0);
}

bool InselFeld::operator !=( const InselFeld& other ) const
{
	return !(*this == other);
}

bool InselFeld::operator <( const InselFeld& other ) const
{
	if( myStruct._posX < other.myStruct._posX )
   	return true;
   else
   if( myStruct._posX > other.myStruct._posX )
   	return false;
   else
   if( myStruct._posY < other.myStruct._posY )
   	return true;
   else
   	return false;
}

void InselFeld::write( unsigned char*& outAddr ) const
{
     	if( ( myStruct._id >= 1201 && myStruct._id <= 1204 ) ||
          ( myStruct._id == 1209 ||
            myStruct._id == 1253 ||
            myStruct._id == 1254 )
        )
		{
/*
static ofstream file( "wzb.tmp" );
file << "(" << (int)myStruct._posX << "/" << (int)myStruct._posY << ") "
     << (int)myStruct._animcnt << endl;
*/
			myStruct._animcnt = random( 5 );
		}
      else
			myStruct._animcnt = 0;	// no animation

		memcpy( outAddr, &myStruct, sizeof( myStruct ) );
   	outAddr += sizeof( myStruct );
}

TRect InselFeld::rect() const
{
	return TRect(
   		   TPoint(myStruct._posX*_unitSize, myStruct._posY*_unitSize),
   			TSize(_unitSize*_size.cx, _unitSize*_size.cy) );
}

TRect InselFeld::fldRect() const
{
	return TRect(
   			TPoint( myStruct._posX, myStruct._posY ),
            TSize( _size.cx-1, _size.cy-1 ) );
}

bool InselFeld::isSelected() const
{
	return _selected;
}

void InselFeld::setSelected( bool sel )
{
	_selected = sel;
}

bool InselFeld::isDummy() const
{
	return myStruct._id == 0;
}

int InselFeld::id() const
{
	return ( myStruct._id );
}

int InselFeld::ori() const
{
	return myStruct._ori;
}

string InselFeld::debugInfo()
{
   ostrstream os;
   os << "Id        \t" << myStruct._id
      << "\nposX    \t" << (uint) myStruct._posX
      << "\nposY    \t" << (uint) myStruct._posY
      << "\nori     \t" << myStruct._ori
      << "\nanimcnt \t" << myStruct._animcnt
      << "\ninselnr \t" << myStruct._inselnr
      << "\nstadtnr \t" << myStruct._stadtnr
      << "\nrandnr  \t" << myStruct._randnr
      << "\nplayernr\t" << myStruct._playernr
      << ends;

   char* str = os.str();
   string s( str );
   free( str );
   return s;
}

TPoint InselFeld::pos() const
{
	return TPoint( myStruct._posX, myStruct._posY );
}

vector<TPoint> InselFeld::allPos() const
{
	vector<TPoint> liste;
   for( int x=0; x<_size.cx; x++ )
   {
   	for( int y=0; y<_size.cy; y++ )
      {
      	liste.push_back( pos().OffsetBy(x,y) );
      }
   }
   return liste;
}

void InselFeld::setId( int id )
{
	myStruct._id = (uint16) id;
   setSize();
}

void InselFeld::setOri( int ori )
{
	myStruct._ori = (uint) ori;
}

void InselFeld::setPos( const TPoint& pos )
{
	myStruct._posX = (uint8) pos.x;
   myStruct._posY = (uint8) pos.y;
}

void InselFeld::takeValuesFrom( FldDialog& d )
{
   myStruct._id = (uint16) d.id();
   if( ! d.keepOri() )
   {
   	// Übernahme der Orientierung vom Dialogfenster
	   myStruct._ori = (uint) d.ori();
   }
   else
   if( d.ori() != -1 )
   {
   	// Übernahme der Orientierung in das Dialogfenster
   	d.setOri( myStruct._ori );
   }

   myStruct._animcnt = 0;	// no animation

   setSize();
}

void InselFeld::draw( TDC& tdc ) const
{
	TRect r( rect() );
   TBitmap* bm_p = theBitmaps->getBitmap( tdc, myStruct._id, myStruct._ori );
   if( bm_p != 0 )
   {
     	TMemoryDC mdc( tdc );
      mdc.SelectObject( *bm_p );
      tdc.BitBlt( r, mdc, TPoint(0,0) );
   }
   else
   {
     	tdc.FillRect( r, TBrush( TColor( TColor::LtRed )));
   }

   if( _selected )
   {
   	tdc.OWLFastWindowFrame( TBrush( TColor::Black ), r, 2, 2 );
   }
}

ostream& operator <<( ostream& str, const InselFeld& feld )
{
	str << (int)feld.myStruct._id << " ("
   	 << (int)feld.myStruct._posX << "|"
       << (int)feld.myStruct._posY << ") "
       << (int)feld.myStruct._ori
       ;
   return str;
}

const TSize& InselFeld::size() const
{
	return _size;
}

void InselFeld::setSize()
{
	int size = fldSize( myStruct._id );
	_size = TSize( size, size );
}

void InselFeld::drawMini( TDC& tdc, const TPoint& offset, int factor ) const
{
	TSize s( myStruct._posX * factor, myStruct._posY * factor );
   TPoint p( offset + s );

   TSize sz( (_size.cx) * factor, (_size.cy) * factor );

   if( sz.cx == 1 )
	   tdc.SetPixel( p, pixelColor() );
   else
   	tdc.FillRect( TRect( p, sz ), pixelColor() );
}
