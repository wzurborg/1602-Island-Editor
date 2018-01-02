#include "Aboutbox.h"
#include "annoedit.h"

DEFINE_RESPONSE_TABLE1( AboutBox, TDialog )
	EV_WM_CTLCOLOR,
   EV_BN_CLICKED( IDC_URL, CmUrl ),
   EV_BN_CLICKED( IDC_EMAIL, CmEmail ),
END_RESPONSE_TABLE;

AboutBox::AboutBox( TWindow* parent, TResId resId, TModule* module )
 : TDialog( parent, resId, module )
{
	_email = new TButton( this, IDC_EMAIL );
   _url = new TButton( this, IDC_URL );
}

void AboutBox::SetupWindow()
{
	TDialog::SetupWindow();
}

void AboutBox::CmUrl()
{
	ShellExecute( 0, "open", getString(IDS_URL), 0, 0, SW_SHOW );
}

void AboutBox::CmEmail()
{
	ShellExecute( 0, "open", getString(IDS_MAILTO), 0, 0, SW_SHOW );
}

HBRUSH AboutBox::EvCtlColor(HDC hDC, HWND hWndChild, uint ctlType)
{
	if( hWndChild == *_email )
   {
   	TDC dc( hDC );
   	dc.SetTextColor( TColor::LtRed );
      return (HBRUSH) dc.GetCurrentObject(OBJ_BRUSH);
   }
   else
   if( hWndChild == *_url )
   {
   	TDC dc( hDC );
   	dc.SetTextColor( TColor::LtGreen );
      return (HBRUSH) dc.GetCurrentObject(OBJ_BRUSH);
   }
   else
   	return TDialog::EvCtlColor( hDC, hWndChild, ctlType );
}

