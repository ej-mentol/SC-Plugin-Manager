#ifndef addpluginH
#define addpluginH

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.ComboEdit.hpp>

#include <System.IOUtils.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Layouts.hpp>
#include <memory>

class TAddPluginForm : public TForm
{
__published:
    TEdit *NameEdit;
	TButton *OkButton;
	TButton *CancelButton;
	TEdit *ScriptEdit;
	TLabel *FileExistsLabel;
	TLayout *TopLayout;
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall NameEditExit(TObject *Sender);
	void __fastcall ScriptEditExit(TObject *Sender);
	void __fastcall NameEditChangeTracking(TObject *Sender);
	void __fastcall ScriptEditChangeTracking(TObject *Sender);
	void __fastcall NameEditKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
    // void __fastcall FormShow(TObject *Sender);

private:
    std::unique_ptr<TStringList> FPluginsList;
	std::unique_ptr<TStringList> FFilteredPlugins;
    String FBasePath;

    void UpdateFilteredList(const String& filterText);
    void ShowFilterPopup();
    void HideFilterPopup();
    String GetRelativePath(const String& fullPath) const;
    String GetSelectedPlugin() const;
    bool ValidateForm() const;

    void __fastcall FormCreate(TObject *Sender);
	void __fastcall ScriptEditKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift);
	void __fastcall FilterListBoxItemClick(TCustomListBox * const Sender, TListBoxItem * const Item);

	TPopup *FilterPopup;
    TListBox *FilterListBox;

public:
    __fastcall TAddPluginForm(TComponent* Owner);

    void SetFileList(const TStringDynArray& plugins, const String& basePath);
	String GetPluginName() const;
	String GetPluginPath() const;
};

#endif

