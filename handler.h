#ifndef handlerH
#define handlerH

#include <System.IOUtils.hpp>
#include <FMX.TreeView.hpp>
#include <FMX.ListBox.hpp>
#include "pluginmanager.h"
#include "settingsmanager.h"

struct TProfileInfo { UnicodeString Name; bool IsDefault; UnicodeString ProfileFilePath; };

class TGUIHandler
{
public:
    // Constructor/Destructor
    __fastcall TGUIHandler();
    __fastcall ~TGUIHandler();

    // Properties
    __property TTreeView *ScriptsTreeView = { write = FScriptsTreeView, read = FScriptsTreeView };
    __property TStringGrid *PluginsTable = { write = FPluginsTable, read = FPluginsTable };
    __property TComboBox *ProfileComboBox = { write = FProfileComboBox, read = FProfileComboBox };

private:
    // Enums
    enum class EMsgType { mtInfo, mtError, mtWarning, mtConfirm, mtExit };

    // Member variables - Managers
    TSettingsManager *FSettings;
    TPluginManager *FPluginManager;

    // Member variables - UI Controls
    TTreeView *FScriptsTreeView;
    TComboBox *FProfileComboBox;
    TStringGrid *FPluginsTable;
    TAniIndicator *FAniLoadingIndicator;
    TPopupMenu *FPopupMenu;

    TSpeedButton *FAddProfileButton;
    TSpeedButton *FDeleteProfileButton;
    TSpeedButton *FEditProfileButton;
	TSpeedButton *FSettingsButton;
    TButton *FSavePluginsListButton;
    TButton *FExportToVdfButton;
	TButton *FCloneProfileButton;

    TMenuItem *FAddPluginButton;
    TMenuItem *FDeletePluginButton;
    // TMenuItem *FEditPluginButton;
	TMenuItem *FEnableDisablePluginsButton;
	TMenuItem *FClearPluginsButton;

    // Settings methods
    bool SaveSettings();
    bool LoadSettings();

    // Profile methods
    void __fastcall LoadDefaultProfile();
    void __fastcall LoadProfileList();
    TProfileInfo __fastcall GetCurrentProfileInfo();
    void __fastcall ToggleDefaultProfile(bool AShowDefault);

    // Plugin management methods
    PluginManagerResult __fastcall LoadPluginsList();
    PluginManagerResult __fastcall SavePluginsList();
    void __fastcall ShowPluginsList(bool loadFromFile = true);
    bool AddPlugin(const String& name, const String& path);
    bool IsPluginExists(const String& name, const String& script);
    void __fastcall ClearPluginsTable();
    void __fastcall GetFilesTree();

    // UI event handlers
    void __fastcall OnProfileChanged(TObject *Sender);
    void __fastcall OnCellClick(TColumn * const Column, const int Row);
    void __fastcall OnAddProfileButton(TObject *Sender);
    void __fastcall OnDeleteProfile(TObject *Sender);
    void __fastcall OnShowSettingsForm(TObject *Sender);
    void __fastcall OnSavePluginsListButton(TObject *Sender);
    void __fastcall OnExportToVdfButton(TObject *Sender);
    void __fastcall OnAddPluginButton(TObject *Sender);
    void __fastcall OnDeletePluginButton(TObject *Sender);
    void __fastcall OnEditPluginButton(TObject *Sender);
	void __fastcall OnEnableDisablePluginsButton(TObject *Sender);
	void __fastcall OnPluginsTableEditingDone(TObject *Sender, const int ACol, const int ARow);
	//void __fastcall PluginsTableEditingDone(TObject *Sender, const int ACol, const int ARow);
	void __fastcall OnTreeViewDblClick( TObject *Sender );
	void __fastcall OnTreeViewMouseDown( TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y );
	void __fastcall OnPopupMenu(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift);
    void __fastcall OnEditProfileButton(TObject *Sender);
    void __fastcall OnCloneProfileButton( TObject *Sender );
};
#endif
