#ifndef settingsmanagerH
#define settingsmanagerH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <System.IOUtils.hpp>
#include <System.IniFiles.hpp>
#include <FMX.Types.hpp>
#include <memory>

namespace Ioutils = System::Ioutils;
namespace SysUtils = System::Sysutils;
namespace IniFiles = System::Inifiles;
namespace StrUtils = System::Strutils;

class TSettingsManager {
private:
    static TSettingsManager* FInstance;

    String FSettingsFilePath;

    String FPluginsFilePath;
    String FSvenCoopPath;
    int FFormWidth;
    int FFormHeight;
    float FSeparatorSize;
    String FPluginsFolderPath;
	String FProfilesFolderPath;
    bool FDefaultProfile;

    bool FSettingsLoaded;
    bool FInitialized;

    TSettingsManager();

    String DetectSvenCoopPath();
    void ApplyDefaultSettings();
    void DeriveRelatedPaths();
	void Initialize();

public:
    ~TSettingsManager();

    static TSettingsManager* GetInstance();
    static void FreeInstance();

    bool SaveSettings();
    bool LoadSettings();
    bool IsSettingsLoaded() const { return FSettingsLoaded; }
    bool IsInitialized() const { return FInitialized; }
	bool ValidatePath(const String& path, bool isDirectory);

    String GetPluginsFilePath() const { return FPluginsFilePath; }
    void SetPluginsFilePath(const String& Value) { FPluginsFilePath = Value; }

    String GetSvenCoopPath() const { return FSvenCoopPath; }
    void SetSvenCoopPath(const String& Value);

    int GetFormWidth() const { return FFormWidth; }
    void SetFormWidth(int Value) { FFormWidth = Value; }

    int GetFormHeight() const { return FFormHeight; }
    void SetFormHeight(int Value) { FFormHeight = Value; }

    float GetSeparatorSize() const { return FSeparatorSize; }
    void SetSeparatorSize(float Value) { FSeparatorSize = Value; }

    String GetPluginsFolderPath() const { return FPluginsFolderPath; }
    void SetPluginsFolderPath(const String& Value) { FPluginsFolderPath = Value; }

    String GetProfilesFolderPath() const { return FProfilesFolderPath; }
	void SetProfilesFolderPath(const String& Value) { FProfilesFolderPath = Value; }

	String GetGenerateDefaultProfile() const { return FDefaultProfile; }
	void SetGenerateDefaultProfile(const bool Value) { FDefaultProfile = Value; }

    __property String PluginsFilePath = {read=GetPluginsFilePath, write=SetPluginsFilePath};
    __property String SvenCoopPath = {read=GetSvenCoopPath, write=SetSvenCoopPath};
    __property int FormWidth = {read=GetFormWidth, write=SetFormWidth};
    __property int FormHeight = {read=GetFormHeight, write=SetFormHeight};
    __property float SeparatorSize = {read=GetSeparatorSize, write=SetSeparatorSize};
    __property String PluginsFolderPath = {read=GetPluginsFolderPath, write=SetPluginsFolderPath};
	__property String ProfilesFolderPath = {read=GetProfilesFolderPath, write=SetProfilesFolderPath};
    __property bool GenerateDefaltProfile = {read=FDefaultProfile, write=FDefaultProfile};
};
#endif
