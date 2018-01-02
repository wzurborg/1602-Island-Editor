#ifndef _inselview_h_
#define _inselview_h_

#include <owl/docview.h>
#include <owl/filedoc.h>
#include <owl/docmanag.h>
#include <owl/scroller.h>
#include <owl/menu.h>
#include "annoedit.h"
#include "inseldoc.h"

class FldDialog;
class InselView : public TWindowView
{
public:
	InselView( InselDoc& doc, TWindow* parent = 0 );
   virtual ~InselView();

   static const char far* StaticName();

    // Message response functions
    virtual void SetupWindow();
//	 virtual void EvDestroy();
    virtual void EvKeyDown(uint, uint, uint);
    virtual void EvLButtonDown(uint, TPoint&);
    virtual void EvLButtonUp(uint, TPoint&);
    virtual void EvRButtonUp(uint, TPoint&);
    virtual void EvMouseMove(uint, TPoint&);
    virtual void Paint(TDC&, bool, TRect&);
    virtual void EvSetFocus( THandle );
    virtual void EvSize(uint, TSize& );
    void CmStoreBitmap();
    void CmFilePrint();
    void CmUndo();
    void CeUndo( TCommandEnabler& );
    void CmInselDialog();
    void CeCopy( TCommandEnabler& );
    void CePaste( TCommandEnabler& );
    void CmZoom( WPARAM );
    void CeZoomPlus( TCommandEnabler& );
    void CeZoomMinus( TCommandEnabler& );
    void CmMark( WPARAM );
    void CmToClipboard();
    void CmRandom();
    void CmCommand( WPARAM );
    void CmPickOrDraw( WPARAM );
    void CmMarkId();
    LRESULT CmCommandTerminated(WPARAM,LPARAM);

    void fldHasChanged();
    void showFieldDetails();
    void resetDialog();
    void markId( int id );

    // redefined
    void InvalidateRect(const TRect& rect, bool erase = true);

    // Document notifications
    bool VnCommit(bool force);
    bool VnRevert(bool clear);

private:
//	static uint clipboardFormat;

   void setScrollers();
   void resetCursor();
   void DPtoLP( TPoint* p, int count=1 );
   void LPtoDP( TPoint* p, int count=1 );

   InselDoc& myDoc;

   // dragging
   TPoint dragStart;
   bool dragging;		// drag operation in process
   TPoint currentP;
   TRect focusRect;
	TPopupMenu* popupMenu_p;
   double zoomFactor;

   static FldDialog* fldDialog_p;

   DECLARE_RESPONSE_TABLE( InselView );
};

#endif