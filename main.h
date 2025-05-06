//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Grid.hpp>
#include <FMX.Grid.Style.hpp>
#include <FMX.ImgList.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.TreeView.hpp>
#include <FMX.Types.hpp>
#include <System.ImageList.hpp>
#include <System.Rtti.hpp>

#include "handler.h"
#include <FMX.Menus.hpp>
#include <FMX.Objects.hpp>

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TLayout *LeftLayout;
	TSplitter *Splitter1;
	TLayout *TopLayout;
	TLayout *CenteralLayout;
	TStringGrid *PluginsTable;
	TTreeView *ScriptsTreeView;
	TImageList *ImageList;
	TStyleBook *StyleBook1;
	TComboBox *ProfileComboBox;
	TCheckColumn *EnabledColumn;
	TStringColumn *NameColumn;
	TStringColumn *ScriptColumn;
	TStringColumn *ConcommandsColumn;
	TPopupColumn *AdminLevelColumn;
	TSpeedButton *SettingsButton;
	TSpeedButton *AddProfileButton;
	TSpeedButton *DeleteProfileButton;
	TPopupMenu *PopupMenu1;
	TMenuItem *AddPluginButton;
	TMenuItem *DeletePluginButton;
	TMenuItem *MenuItem3;
	TMenuItem *EnableDisablePluginsButton;
	TMenuItem *MenuItem8;
	TMenuItem *ClearPluginsButton;
	TLayout *SettingsLayout;
	TLayout *ButtonsLayout;
	TLayout *ProfileLayout;
	TImage *Image1;
	TImage *Image2;
	TImage *Image3;
	TLayout *BottomLayout;
	TButton *ExportToVdfButton;
	TButton *SavePluginsListButton;
	TSpeedButton *EditProfileButton;
	TImage *Image4;
	TAniIndicator *AniLoadingIndicator;
	TButton *CloneProfileButton;
	void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
	TGUIHandler *FHandler;
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
