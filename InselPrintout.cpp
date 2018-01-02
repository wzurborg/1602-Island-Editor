#include "InselPrintout.h"

InselPrintout::InselPrintout(const char* title, TWindow* window)
: TPrintout(title)
{
  Window = window;
  Scale = true;
}

void
InselPrintout::PrintPage(int, TRect& rect, unsigned)
{
  // Conditionally scale the DC to the window so the printout will
  // resemble the window
  //
  int    prevMode;
  TSize  oldVExt, oldWExt;
  if (Scale) {
    prevMode = DC->SetMapMode(MM_ISOTROPIC);
    TRect windowSize = Window->GetClientRect();
    DC->SetViewportExt(PageSize, &oldVExt);
    DC->SetWindowExt(windowSize.Size(), &oldWExt);
    DC->IntersectClipRect(windowSize);
    DC->DPtoLP(rect, 2);
  }

  // Call the window to paint itself
  Window->Paint(*DC, false, rect);

  // Restore changes made to the DC
  if (Scale) {
    DC->SetWindowExt(oldWExt);
    DC->SetViewportExt(oldVExt);
    DC->SetMapMode(prevMode);
  }
}

// Do not enable page range in the print dialog since only one page is
// available to be printed
//
void
InselPrintout::GetDialogInfo(int& minPage, int& maxPage,
                               int& selFromPage, int& selToPage)
{
  minPage = 0;
  maxPage = 0;
  selFromPage = selToPage = 0;
}

