//---------------------------------------------------------------------------

#pragma hdrstop

#include "settingsmanager.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#ifdef _WIN32
    #include <System.Win.Registry.hpp>
namespace Registry = System::Win::Registry;
#endif

TSettingsManager* TSettingsManager::FInstance = nullptr;

TSettingsManager::TSettingsManager() :
    FSettingsLoaded(false), FInitialized(false)
{
    FSettingsFilePath = Ioutils::TPath::Combine(
        Ioutils::TPath::GetDirectoryName(ParamStr(0)), "settings.ini");

    Initialize();
}

void TSettingsManager::Initialize()
{
    ApplyDefaultSettings();
    if (Ioutils::TFile::Exists(FSettingsFilePath)) {
        FSettingsLoaded = LoadSettings();
    }
    if (FSvenCoopPath.IsEmpty() || !ValidatePath(FSvenCoopPath, true)) {
        FSvenCoopPath = DetectSvenCoopPath();

        if (!FSvenCoopPath.IsEmpty()) {
            DeriveRelatedPaths();
        }
    }

    if (!FSettingsLoaded && !FSvenCoopPath.IsEmpty()) {
        SaveSettings();
    }

    FInitialized = true;
}

TSettingsManager::~TSettingsManager()
{
    if (FInitialized)
        SaveSettings();
}

TSettingsManager* TSettingsManager::GetInstance()
{
    if (FInstance == nullptr) {
        FInstance = new TSettingsManager();
    }
    return FInstance;
}

void TSettingsManager::FreeInstance()
{
    if (FInstance != nullptr) {
        delete FInstance;
        FInstance = nullptr;
    }
}

String TSettingsManager::DetectSvenCoopPath()
{
#if defined(_WIN32)
    String steamPath = "";

    auto findSteamInRegistry = [&](HKEY rootKey) -> String {
        try {
            std::unique_ptr<Registry::TRegistry> registry(
				new Registry::TRegistry(KEY_READ));
			registry->RootKey = rootKey;
			if (registry->OpenKeyReadOnly("Software\\Valve\\Steam")) {
				if (registry->ValueExists("SteamPath")) {
					return registry->ReadString("SteamPath");
				}
			}
		} catch (const Exception &e) {
			OutputDebugString(("Registry search error: " + e.Message).c_str());
		}
		return "";
	};

	steamPath = findSteamInRegistry(HKEY_CURRENT_USER);
	if (steamPath.IsEmpty()) {
		steamPath = findSteamInRegistry(HKEY_LOCAL_MACHINE);
	}

	if (!steamPath.IsEmpty()) {
		String svenPath =
			Ioutils::TPath::Combine(StringReplace(steamPath, L"/", L"\\",
										TReplaceFlags() << rfReplaceAll),
				"steamapps\\common\\Sven Co-op");

		if (Ioutils::TDirectory::Exists(svenPath)) {
			return svenPath;
		}
	}

#elif defined(__linux__)
	try {
		String homeDir = Ioutils::TPath::GetHomePath();
		if (!homeDir.IsEmpty()) {
			String commonPaths[] = {
				Ioutils::TPath::Combine(
					homeDir, ".steam/steam/steamapps/common/Sven Co-Op"),
				Ioutils::TPath::Combine(
					homeDir, ".local/share/Steam/steamapps/common/Sven Co-Op"),
				Ioutils::TPath::Combine(
					homeDir, "Steam/steamapps/common/Sven Co-Op")
			};

			for (const auto &path : commonPaths) {
				if (Ioutils::TDirectory::Exists(path)) {
					return path;
				}
			}
		}
	} catch (Exception &e) {
	}
#endif

	return "";
}

void TSettingsManager::ApplyDefaultSettings()
{
    FFormWidth = 800;
    FFormHeight = 600;
    FSeparatorSize = 150.0f;

    FSvenCoopPath = "";
    FPluginsFilePath = "";
    FPluginsFolderPath = "";
    FDefaultProfile = true;

    FProfilesFolderPath = Ioutils::TPath::Combine(
        Ioutils::TPath::GetDirectoryName(ParamStr(0)), "profiles");

    if (!Ioutils::TDirectory::Exists(FProfilesFolderPath)) {
        try {
            Ioutils::TDirectory::CreateDirectory(FProfilesFolderPath);
        } catch (Exception &e) {
            FProfilesFolderPath = Ioutils::TPath::GetDirectoryName(ParamStr(0));
        }
    }
}

void TSettingsManager::DeriveRelatedPaths()
{
    if (!FSvenCoopPath.IsEmpty()) {
        String derivedPluginsFile = Ioutils::TPath::Combine(
            FSvenCoopPath, "svencoop\\default_plugins.txt");
        if (FPluginsFilePath.IsEmpty() ||
            !ValidatePath(FPluginsFilePath, false))
            FPluginsFilePath = StringReplace(derivedPluginsFile, L"/", L"\\",
                TReplaceFlags() << rfReplaceAll);
        String derivedPluginsFolder = Ioutils::TPath::Combine(
            FSvenCoopPath, "svencoop\\scripts\\plugins");
        if (FPluginsFolderPath.IsEmpty() ||
            !ValidatePath(FPluginsFolderPath, true))
            FPluginsFolderPath = StringReplace(derivedPluginsFolder, L"/",
                L"\\", TReplaceFlags() << rfReplaceAll);
    }
}

bool TSettingsManager::ValidatePath(const String &path, bool isDirectory)
{
    if (path.IsEmpty()) {
        return false;
    }

    try {
        if (isDirectory) {
            return Ioutils::TDirectory::Exists(path);
        } else {
            return Ioutils::TFile::Exists(path);
        }
    } catch (Exception &e) {
        return false;
    }
}

void TSettingsManager::SetSvenCoopPath(const String &Value)
{
    if (FSvenCoopPath != Value) {
        FSvenCoopPath = Value;
        DeriveRelatedPaths();
    }
}

bool TSettingsManager::LoadSettings()
{
    if (!Ioutils::TFile::Exists(FSettingsFilePath)) {
        return false;
    }

    try {
        std::unique_ptr<TMemIniFile> iniFile(
            new TMemIniFile(FSettingsFilePath));

        FPluginsFilePath =
            iniFile->ReadString("Paths", "PluginsFile", FPluginsFilePath);
        FPluginsFolderPath =
            iniFile->ReadString("Paths", "PluginsFolder", FPluginsFolderPath);
        FProfilesFolderPath =
            iniFile->ReadString("Paths", "ProfilesFolder", FProfilesFolderPath);

        FFormWidth = iniFile->ReadInteger("UI", "FormWidth", FFormWidth);
        FFormHeight = iniFile->ReadInteger("UI", "FormHeight", FFormHeight);
        FSeparatorSize =
            iniFile->ReadFloat("UI", "SeparatorSize", FSeparatorSize);
        FDefaultProfile =
            iniFile->ReadBool("UI", "DefaultProfile", FDefaultProfile);

        return true;
    } catch (Exception &e) {
        OutputDebugString(("Error loading settings: " + e.Message).c_str());
        return false;
    }
}

bool TSettingsManager::SaveSettings()
{
    try {
        String settingsDir =
            Ioutils::TPath::GetDirectoryName(FSettingsFilePath);
        if (!Ioutils::TDirectory::Exists(settingsDir)) {
            Ioutils::TDirectory::CreateDirectory(settingsDir);
        }

        std::unique_ptr<TMemIniFile> iniFile(
            new TMemIniFile(FSettingsFilePath));

        iniFile->WriteString("Paths", "PluginsFile", FPluginsFilePath);
        iniFile->WriteString("Paths", "PluginsFolder", FPluginsFolderPath);
        iniFile->WriteString("Paths", "ProfilesFolder", FProfilesFolderPath);

        iniFile->WriteInteger("UI", "FormWidth", FFormWidth);
        iniFile->WriteInteger("UI", "FormHeight", FFormHeight);
        iniFile->WriteFloat("UI", "SeparatorSize", FSeparatorSize);
        iniFile->WriteBool("UI", "DefaultProfile", FDefaultProfile);

        iniFile->UpdateFile();
        FSettingsLoaded = true;

        return true;
    } catch (Exception &e) {
        OutputDebugString(("Error saving settings: " + e.Message).c_str());
        return false;
    }
}

