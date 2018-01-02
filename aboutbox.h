#ifndef __aboutbox__h__
#define __aboutbox__h__

#include <owl/dialog.h>
#include <owl/button.h>

class AboutBox : public TDialog
{
public:
	AboutBox( TWindow* parent, TResId resId, TModule* module = 0 );

   virtual void SetupWindow();
   virtual HBRUSH EvCtlColor(HDC hDC, HWND hWndChild, uint ctlType);

   void CmUrl();
   void CmEmail();

private:
	TButton* _email;
   TButton* _url;

   DECLARE_RESPONSE_TABLE( AboutBox );
};
#endif