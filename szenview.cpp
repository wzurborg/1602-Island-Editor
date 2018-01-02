#include <stdio.h>
#include <math.h>
#include <owl/scroller.h>
#include <owl/controlb.h>
#include <owl/decframe.h>
#include <owl/docview.rh>
#include <owl/statusba.h>
#include <owl/except.h>
#include "szenview.h"
#include "annoedit.h"

SzenView::SzenView( SzenDoc& doc, TWindow* parent )
: TWindowView( doc, parent ), myDoc( doc )
{
}

DEFINE_RESPONSE_TABLE1(SzenView, TWindowView)
  EV_WM_LBUTTONDOWN,
  EV_WM_SETFOCUS,
END_RESPONSE_TABLE;

const char far* SzenView::StaticName()
{
	return "Insel View";
}

void SzenView::SetupWindow()
{
	TWindowView::SetupWindow();
}

SzenView::~SzenView()
{
}

void SzenView::EvLButtonDown( uint modKeys, TPoint& point)
{
   TPoint realP = point;
   TWindowView::EvLButtonDown( modKeys, point );
}

void SzenView::Paint( TDC& tdc, bool, TRect& rect )
{
   // Draw
	myDoc.draw( tdc, rect );
}

bool SzenView::VnCommit( bool /*force*/ )
{
   return true;
}

bool SzenView::VnRevert( bool clear )
{
	Invalidate( clear );
   return true;
}

void SzenView::EvSetFocus( THandle hWndLostFocus )
{
   myDoc.setView( this );
	TWindowView::EvSetFocus( hWndLostFocus );
}


