#pragma once

#ifdef _DEBUG
#include <string>
#include <vector>
#include <filesystem>

class ImGuiLoadManager final {
private:
	ImGuiLoadManager();

public:
	~ImGuiLoadManager();

private:
	ImGuiLoadManager(const ImGuiLoadManager&) = delete;
	ImGuiLoadManager& operator=(const ImGuiLoadManager&) = delete;

public:
	static ImGuiLoadManager& GetInstance();

public:
	static void ShowGUI();

private:
	void show_gui();
	void get_file_list(std::vector<std::string>& list, const std::filesystem::path& path, const std::string& extension);

private:
	std::filesystem::path meshCurrentPath;
	std::vector<std::string> meshFileList;
	std::filesystem::path textureCurrentPath;
	std::vector<std::string> textureFileList;
};
#endif // _DEBUG
