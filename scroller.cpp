//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision:   10.4  $
//
// Implementation of class TScroller.
//----------------------------------------------------------------------------
#pragma hdrignore SECTION
#include <owl/pch.h>
#if !defined(OWL_SCROLLER_H)
# include <owl/scroller.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif
#include <stdlib.h>

#define max(a,b)   (((a) > (b)) ? (a) : (b))
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#define abs(a)     ((a) >= 0 ? (a) : -(a))

#if !defined(SECTION) || SECTION == 1
//
// constructs a TScroller object
//
TScroller::TScroller(TWindow* window,
                     int      xUnit,
                     int      yUnit,
                     long     xRange,
                     long     yRange)
{
  Window = window;
  XPos = YPos = 0;
  XUnit = xUnit;
  YUnit = yUnit;
  XRange = xRange;
  YRange = yRange;
  XTotalUnits = 0;
  YTotalUnits = 0;
  XLine = 1;  YLine = 1;
  XPage = 1;  YPage = 1;
  AutoMode = true;
  TrackMode = true;
  AutoOrg = true;
  HasHScrollBar = ToBool(Window && (Window->Attr.Style&WS_HSCROLL));
  HasVScrollBar = ToBool(Window && (Window->Attr.Style&WS_VSCROLL));
}

//
// destructs a TScroller object
//
// sets owning Window's Scroller member to 0
//
TScroller::~TScroller()
{
  if (Window)
    Window->Scroller = 0;
}

//
// sets the number of units per page (amount by which to scroll on a page
// scroll request) according to the current size of the Window's client area
//
void
TScroller::SetPageSize()
{
  if (Window) {
    TRect  clientRect;
    Window->GetClientRect(clientRect);
    int width = clientRect.Width();
    int height = clientRect.Height();

    if (width && XUnit > 0) {
      XPage = max(1, (width+1) / XUnit - 1);
#if defined(BI_PLAT_WIN32)
//      if (XTotalUnits && XTotalUnits > width) {
//        Window->SetScrollPage(SB_HORZ, width / XTotalUnits * XRange, true);
//      }
//      else
//        Window->SetScrollPage(SB_HORZ, XUnit, false);
#endif
    }

    if (height && YUnit > 0) {
      YPage = max(1, (height+1) / YUnit - 1);
#if defined(BI_PLAT_WIN32)
//      if (YTotalUnits && YTotalUnits > height) {
//        Window->SetScrollPage(SB_VERT, height / YTotalUnits * YRange, true);
//      }
//      else
//        Window->SetScrollPage(SB_VERT, YUnit, false);
#endif
    }
  }
}

//
// sets the range of the TScroller and also sets the range of its
// Window's scrollbars
//
void
TScroller::SetRange(long xRange, long yRange)
{
  XRange = xRange;
  YRange = yRange;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// Sets the total units of the window
//
void
TScroller::SetTotalRangeOfUnits(long xTotalUnits, long yTotalUnits)
{
  XTotalUnits = xTotalUnits;
  YTotalUnits = yTotalUnits;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// resets the X and Y scroll unit size (in device units) to the passed
// parameters
//
// calls SetPageSize to update the X and Y page size, which are specified
// in scroll units
//
void
TScroller::SetUnits(int xUnit, int yUnit)
{
  XUnit = xUnit;
  YUnit = yUnit;
  SetPageSize();
}

//
// sets the range of the Window's scrollbars & repaints as necessary
//
void
TScroller::SetSBarRange()
{
  if (Window) {
    if (HasHScrollBar) {
      int curMin, curMax;
      Window->GetScrollRange(SB_HORZ, curMin, curMax);
      int newMax = max(0, (int)min(XRange, SHRT_MAX));
      if (newMax != curMax || curMin != 0)
        Window->SetScrollRange(SB_HORZ, 0, newMax, true);
    }

    if (HasVScrollBar) {
      int curMin, curMax;
      Window->GetScrollRange(SB_VERT, curMin, curMax);
      int newMax = max(0, (int)min(YRange, SHRT_MAX));
      if (newMax != curMax || curMin != 0)
        Window->SetScrollRange(SB_VERT, 0, newMax, true);
    }
  }
}

//
// sets the origin for the paint display context according to XPos, YPos
//
void
TScroller::BeginView(TDC& dc, TRect& rect)
{
  long  xOrg = XPos * XUnit;
  long  yOrg = YPos * YUnit;

  if (AutoOrg && xOrg <= SHRT_MAX && yOrg <= SHRT_MAX) {
    TPoint offset(int(-xOrg), int(-yOrg));
    dc.SetViewportOrg(offset);
    rect -= offset;
  }
}

//
// updates the position of the Window's scrollbar(s)
//
void
TScroller::EndView()
{
  if (Window) {
    if (HasHScrollBar) {
      int  newPos = XRange > SHRT_MAX ? XScrollValue(XPos) : (int)XPos;
      if (newPos != Window->GetScrollPos(SB_HORZ))
        Window->SetScrollPos(SB_HORZ, newPos, true);
    }

    if (HasVScrollBar) {
      int  newPos = YRange > SHRT_MAX ? YScrollValue(YPos) : (int)YPos;
      if (newPos != Window->GetScrollPos(SB_VERT))
        Window->SetScrollPos(SB_VERT, newPos, true);
    }
  }
}

//
// scrolls vertically according to scroll action and thumb position
//
void
TScroller::VScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINEDOWN:
      ScrollBy(0, YLine);
      break;

    case SB_LINEUP:
      ScrollBy(0, -YLine);
      break;

    case SB_PAGEDOWN:
      ScrollBy(0, YPage);
      break;

    case SB_PAGEUP:
      ScrollBy(0, -YPage);
      break;

    case SB_TOP:
      ScrollTo(XPos, 0);
      break;

    case SB_BOTTOM:
      ScrollTo(XPos, YRange);
      break;

    case SB_THUMBPOSITION:
      if (YRange > SHRT_MAX)
        ScrollTo(XPos, YRangeValue(thumbPos));

      else
        ScrollTo(XPos, thumbPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (YRange > SHRT_MAX)
         ScrollTo(XPos, YRangeValue(thumbPos));

        else
         ScrollTo(XPos, thumbPos);

      if (Window && HasVScrollBar)
        Window->SetScrollPos(SB_VERT, thumbPos, true);
      break;
  }
}

//
// scrolls horizontally according to scroll action and thumb position
//
void
TScroller::HScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINEDOWN:
      ScrollBy(XLine, 0);
      break;

    case SB_LINEUP:
      ScrollBy(-XLine, 0);
      break;

    case SB_PAGEDOWN:
      ScrollBy(XPage, 0);
      break;

    case SB_PAGEUP:
      ScrollBy(-XPage, 0);
      break;

    case SB_TOP:
      ScrollTo(0, YPos);
      break;

    case SB_BOTTOM:
      ScrollTo(XRange, YPos);
      break;

    case SB_THUMBPOSITION:
      if (XRange > SHRT_MAX)
        ScrollTo(XRangeValue(thumbPos), YPos);

      else
        ScrollTo(thumbPos, YPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (XRange > SHRT_MAX)
          ScrollTo(XRangeValue(thumbPos), YPos);

        else
          ScrollTo(thumbPos, YPos);

      if (Window && HasHScrollBar)
        Window->SetScrollPos(SB_HORZ, thumbPos, true);
      break;
  }
}

//
// scrolls to an (x,y) position, after checking boundary conditions
// causes a WM_PAINT message to be sent
//
// first scrolls the contents of the client area, if a portion of the client
// area will remain visible
//
void
TScroller::ScrollTo(long x, long y)
{
  if (Window) {
    long  newXPos = max(0, min(x, XRange));
    long  newYPos = max(0, min(y, YRange));

    if (newXPos != XPos || newYPos != YPos) {
      //
      // scaling isn't needed here. if condition is met, ScrollWindow()
      // will succeed since XPage and YPage are ints
      //
      // if condition isn't met, ScrollWindow() is called in EndScroll()
      // as a result of calling UpdateWindow()
      //
      // EndScroll() performs the necessary scaling
      //
      if (AutoOrg || abs(YPos-newYPos) < YPage && abs(XPos-newXPos) < XPage)
        Window->ScrollWindow((int)(XPos - newXPos) * XUnit,
                (int)(YPos - newYPos) * YUnit, 0, 0);

      else
        Window->Invalidate();

      XPos = newXPos;
      YPos = newYPos;
      Window->UpdateWindow();
    }
  }
}

//
//
//
bool
TScroller::IsAutoMode()
{
  return AutoMode;
}

//
// performs "auto-scrolling" (dragging the mouse from within the client
// client area of the Window to without results in auto-scrolling when
// the AutoMode data member of the Scroller is true)
//
void
TScroller::AutoScroll()
{
  if (AutoMode && Window) {
    TRect  clientRect;
    TPoint cursorPos;
    long  dx = 0, dy = 0;

    GetCursorPos(&cursorPos);
    Window->ScreenToClient(cursorPos);
    Window->GetClientRect(clientRect);

    if (cursorPos.y < 0)
      dy = min(-YLine, max(-YPage, cursorPos.y / 10 * YLine));

    else if (cursorPos.y > clientRect.bottom)
      dy = max(YLine, min(YPage, (cursorPos.y-clientRect.bottom)/10 * YLine));

    if (cursorPos.x < 0)
      dx = min(-XLine, max(-XPage, cursorPos.x / 10 * XLine));

    else if (cursorPos.x > clientRect.right)
      dx = max(XLine, min(XPage, (cursorPos.x-clientRect.right)/10 * XLine));

    ScrollBy(dx, dy);
  }
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE(TScroller);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TScroller from the passed ipstream
//
void*
TScroller::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TScroller* o = GetObject();
  is >> o->XPos >> o->YPos >>
      o->XUnit >> o->YUnit >>
      o->XRange >> o->YRange >>
      o->XLine >> o->YLine >>
      o->XPage >> o->YPage >>
      o->AutoMode >> o->TrackMode >>
      o->AutoOrg >>
      o->HasHScrollBar >> o->HasVScrollBar;

  o->Window = 0;
  return o;
}

//
// writes the TScroller to the passed opstream
//
void
TScroller::Streamer::Write(opstream& os) const
{
  TScroller* o = GetObject();
  os << o->XPos << o->YPos <<
      o->XUnit << o->YUnit <<
      o->XRange << o->YRange <<
      o->XLine << o->YLine <<
      o->XPage << o->YPage <<
      o->AutoMode << o->TrackMode <<
      o->AutoOrg <<
      o->HasHScrollBar << o->HasVScrollBar;
}


#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

