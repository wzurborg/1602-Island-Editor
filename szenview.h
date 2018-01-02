#ifndef _szenview_h_
#define _szenview_h_

#include <owl/docview.h>
#include <owl/filedoc.h>
#include <owl/docmanag.h>
#include <owl/scroller.h>
#include <owl/menu.h>
#include "annoedit.h"
#include "szendoc.h"

class SzenView : public TWindowView
{
public:
	SzenView( SzenDoc& doc, TWindow* parent = 0 );
   virtual ~SzenView();

   static const char far* StaticName();

    // Message response functions
    virtual void SetupWindow();
    virtual void EvLButtonDown(uint, TPoint&);
    virtual void Paint(TDC&, bool, TRect&);
	 virtual void EvSetFocus( THandle hWndLostFocus );

    // Document notifications
    bool VnCommit(bool force);
    bool VnRevert(bool clear);

private:
   SzenDoc& myDoc;

   DECLARE_RESPONSE_TABLE( SzenView );
};

#endif