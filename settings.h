//---------------------------------------------------------------------------

#ifndef settingsH
#define settingsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Edit.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>

#include <System.IOUtils.hpp>
#include <FMX.Dialogs.hpp>
//---------------------------------------------------------------------------

class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TCheckBox *IsDefailtProfileEnabledCheckBox;
	TEdit *DefaultProfileEdit;
	TEdit *PluginsPathEdit;
	TLabel *Label1;
	TLabel *Label2;
	TButton *OkButton;
	TButton *CancelButton;
	TEdit *ProfilesPathEdit;
	TLabel *Label3;
	TEllipsesEditButton *EllipsesEditButton1;
	TEllipsesEditButton *EllipsesEditButton2;
	TEllipsesEditButton *EllipsesEditButton3;
	TOpenDialog *OpenDialog;
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall EllipsesEditButton2Click(TObject *Sender);
	void __fastcall OnChangedTracking( TObject *Sender );
private:	// User declarations
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
// extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
