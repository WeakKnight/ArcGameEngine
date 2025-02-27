#pragma once

#include <EASTL/stack.h>
#include <imgui/imgui.h>

#include "BasePanel.h"

namespace ArcEngine
{
	class AssetPanel : public BasePanel
	{
	public:
		explicit AssetPanel(const char* name = "Assets");

		virtual ~AssetPanel() override = default;

		AssetPanel(const AssetPanel& other) = delete;
		AssetPanel(AssetPanel&& other) = delete;
		AssetPanel& operator=(const AssetPanel& other) = delete;
		AssetPanel& operator=(AssetPanel&& other) = delete;

		virtual void OnUpdate([[maybe_unused]] Timestep ts) override;
		virtual void OnImGuiRender() override;

	private:
		std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask, ImGuiTreeNodeFlags flags);
		void RenderHeader();
		void RenderSideView();
		void RenderBody(bool grid);
		void UpdateDirectoryEntries(const std::filesystem::path& directory);

	private:
		struct File
		{
			eastl::string Name;
			eastl::string Filepath;
			eastl::string Extension;
			std::filesystem::directory_entry DirectoryEntry;
			Ref<Texture2D> Thumbnail = nullptr;
			bool IsDirectory = false;

			File(const eastl::string& name,
				const eastl::string& filepath,
				const eastl::string& extension,
				const std::filesystem::directory_entry& directoryEntry,
				const Ref<Texture2D> thumbnail,
				bool isDir)
				:	Name(name), Filepath(filepath), Extension(extension),
					DirectoryEntry(directoryEntry), Thumbnail(thumbnail), IsDirectory(isDir)
			{
			}
		};

		std::filesystem::path m_CurrentDirectory;
		eastl::stack<std::filesystem::path> m_BackStack;
		eastl::vector<File> m_DirectoryEntries;
		uint32_t m_CurrentlyVisibleItemsTreeView = 0;
		float m_ThumbnailSize = 128.0f;
		ImGuiTextFilter m_Filter;
		float m_ElapsedTime = 0.0f;

		Ref<Texture2D> m_WhiteTexture;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
