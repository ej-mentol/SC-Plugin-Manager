//---------------------------------------------------------------------------

#pragma hdrstop

#include "filetree.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include <memory>
#include <unordered_map>
#include <map>
#include <System.Threading.hpp>

namespace FileTreeUtils {

struct NodeBuildInfo {
    UnicodeString text;
    int imageIndex;
    UnicodeString parentPath;
    UnicodeString fullPath;
    UnicodeString relativePath;
    bool isFolder;
};

//UnicodeString MakeRelativePath(const UnicodeString& fullPath, const UnicodeString& rootFolder) {
//	if (fullPath.Length() <= rootFolder.Length() + 1) {
//		return {};
//	}

//    UnicodeString result = fullPath.SubString(rootFolder.Length() + 2, fullPath.Length());
//	return StringReplace(result, L"\\", L"/", TReplaceFlags() << rfReplaceAll);
//}

UnicodeString MakeRelativePath(const UnicodeString& fullPath, const UnicodeString& rootFolder)
	  {
		UnicodeString normalizedRoot = System::Sysutils::ExcludeTrailingPathDelimiter( rootFolder );
		if (fullPath.Length() <= normalizedRoot.Length() + 1) {
			return {};
		}
		UnicodeString result = fullPath.SubString(normalizedRoot.Length() + 2, fullPath.Length());
		return StringReplace(result, L"\\", L"/", TReplaceFlags() << rfReplaceAll);
	}

TTreeViewItem* CreateTreeItem(const NodeBuildInfo& nodeInfo, TTreeView* treeView) {
    auto newItem = new TTreeViewItem(treeView);
    newItem->Text = nodeInfo.text;
    newItem->IsExpanded = false;
    newItem->ImageIndex = nodeInfo.imageIndex;
	newItem->TagString = nodeInfo.relativePath;
    return newItem;
}

void BuildFileTree(
    TTreeView* treeView,
    const UnicodeString& rootFolder,
    const UnicodeString& filePattern,
    TreeBuildCallback onComplete)
{
    if (!treeView || rootFolder.IsEmpty()) {
        if (onComplete) onComplete();
        return;
    }

	TThread::Synchronize(nullptr, [treeView]() {
        treeView->Clear();
    });

	TThread::CreateAnonymousThread([treeView, rootFolder, filePattern, onComplete]() {
        try {
			std::vector<NodeBuildInfo> nodeInfos;

			NodeBuildInfo rootNode;
			rootNode.text = Ioutils::TPath::GetFileName( System::Sysutils::ExcludeTrailingPathDelimiter( rootFolder ) );
			rootNode.imageIndex = 0;
            rootNode.parentPath = {};
            rootNode.fullPath = rootFolder;
            rootNode.relativePath = {};
            rootNode.isFolder = true;
            nodeInfos.push_back(rootNode);

			std::map<UnicodeString, bool> processedPaths;
            processedPaths[rootFolder] = true;

            try {
				auto files = Ioutils::TDirectory::GetFiles(rootFolder, filePattern, TSearchOption::soAllDirectories);

				for (int i = 0; i < files.Length; ++i) {
                    const auto& filePath = files[i];
                    const auto relPath = MakeRelativePath(filePath, rootFolder);

					auto pathParts = SplitString(relPath, L"/");

					UnicodeString currentPath = rootFolder;
                    UnicodeString parentPath = rootFolder;

					for (int j = 0; j < pathParts.Length - 1; ++j) {
                        currentPath = currentPath + L"\\" + pathParts[j];

						if (processedPaths.find(currentPath) == processedPaths.end()) {
                            NodeBuildInfo folderNode;
                            folderNode.text = pathParts[j];
							folderNode.imageIndex = 0;
                            folderNode.parentPath = parentPath;
                            folderNode.fullPath = currentPath;
							folderNode.isFolder = true;

							int endPos = relPath.Pos(pathParts[j]) + pathParts[j].Length() - 1;
                            folderNode.relativePath = relPath.SubString(1, endPos);

                            nodeInfos.push_back(folderNode);
                            processedPaths[currentPath] = true;
                        }

                        parentPath = currentPath;
                    }

					const auto& fileName = pathParts[pathParts.Length - 1];
                    NodeBuildInfo fileNode;
					fileNode.text = Ioutils::TPath::GetFileNameWithoutExtension(fileName);
					fileNode.imageIndex = 1;
                    fileNode.parentPath = parentPath;
					fileNode.fullPath = filePath;
					fileNode.relativePath = Ioutils::TPath::ChangeExtension(relPath, L"");
					if (!fileNode.relativePath.IsEmpty() && fileNode.relativePath[fileNode.relativePath.Length()] == L'.') fileNode.relativePath.SetLength(fileNode.relativePath.Length() - 1);
					fileNode.isFolder = false;

					nodeInfos.push_back(fileNode);
                }

				// Sort Tree
				std::unordered_map<UnicodeString, std::vector<NodeBuildInfo>> grouped;
				for (const auto& node : nodeInfos)
					grouped[node.parentPath].push_back(node);

				nodeInfos.clear();
				for (auto& [parent, group] : grouped) {
					std::sort(group.begin(), group.end(), [](const NodeBuildInfo& a, const NodeBuildInfo& b) {
						if (a.imageIndex != b.imageIndex)
							return a.imageIndex < b.imageIndex;
						return a.text.CompareIC(b.text) < 0;
					});
					nodeInfos.insert(nodeInfos.end(), group.begin(), group.end());
				}
				// ----------

				TThread::Synchronize(nullptr, [treeView, nodeInfos]() {
					std::unordered_map<UnicodeString, TTreeViewItem*> nodeMap;

                    try {
						for (const auto& nodeInfo : nodeInfos) {
                            auto newItem = CreateTreeItem(nodeInfo, treeView);
                            nodeMap[nodeInfo.fullPath] = newItem;
                        }

						for (const auto& nodeInfo : nodeInfos) {
                            auto item = nodeMap[nodeInfo.fullPath];
                            if (!item) continue;

                            if (nodeInfo.parentPath.IsEmpty()) {
								treeView->AddObject(item);
                                item->IsExpanded = true;
							} else {
                                auto parentItem = nodeMap[nodeInfo.parentPath];
                                if (parentItem) {
                                    parentItem->AddObject(item);
                                }
                            }
                        }
                    } catch (const Exception& e) {
						OutputDebugString((L"[BuildFileTree] UI build error: " + e.Message).c_str());
                    }
                });
			} catch (const Exception& e) {
				OutputDebugString((L"[BuildFileTree] File scan error: " + e.Message).c_str());
            }
        } catch (...) {
			OutputDebugString(L"[BuildFileTree] Unexpected error occurred");
        }

		if (onComplete) {
            TThread::Queue(nullptr, [onComplete]() {
                onComplete();
            });
        }
    })->Start();
}

std::unique_ptr<TreeNodeInfo> GetSelectedNodeInfo(TTreeView* treeView) {
    if (!treeView || !treeView->Selected) {
        return nullptr;
    }

    auto selectedItem = dynamic_cast<TTreeViewItem*>(treeView->Selected);
    if (!selectedItem) {
        return nullptr;
    }

    auto result = std::make_unique<TreeNodeInfo>();
    result->text = selectedItem->Text;
    result->imageIndex = selectedItem->ImageIndex;
    result->relativePath = selectedItem->TagString;
	result->isFolder = (selectedItem->ImageIndex == 0);

    return result;
}

}