//---------------------------------------------------------------------------

#ifndef filetreeH
#define filetreeH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.TreeView.hpp>
#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>
#include <memory>
#include <functional>
#include <vector>

namespace FileTreeUtils {

    struct TreeNodeInfo {
		UnicodeString text;
		int imageIndex;
		UnicodeString fullPath;
		UnicodeString relativePath;
		bool isFolder;
    };

	using TreeBuildCallback = std::function<void()>;
	void BuildFileTree(
        TTreeView* treeView,
        const UnicodeString& rootFolder,
        const UnicodeString& filePattern = L"*.as",
        TreeBuildCallback onComplete = nullptr);
	std::unique_ptr<TreeNodeInfo> GetSelectedNodeInfo(TTreeView* treeView);
}

#endif
