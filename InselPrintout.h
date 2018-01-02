#ifndef __InselPrintout__h__
#define __InselPrintout__h__

#include <owl/printer.h>

class InselPrintout : public TPrintout
{
  public:
    InselPrintout(const char* title, TWindow* window);

    void GetDialogInfo(int& minPage, int& maxPage,
                       int& selFromPage, int& selToPage);
    void PrintPage(int page, TRect& rect, unsigned flags);
    void SetBanding(bool b)
    {
      Banding = b;
    }
    bool HasPage(int pageNumber)
    {
      return pageNumber == 1;
    }

  protected:
    TWindow* Window;
    bool     Scale;
};

#endif
