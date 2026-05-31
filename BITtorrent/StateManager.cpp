#include "StateManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// FileStateDatabase
FileStateDatabase::FileStateDatabase()
	: isLoaded(false) {
}

FileStateDatabase::~FileStateDatabase() {
	SaveToFile();
}

bool FileStateDatabase::Initialize(const std::string& path) {
	dbPath = path;
	return LoadFromFile();
}

bool FileStateDatabase::SaveState(const std::string& key, const std::string& value) {
	stateMap[key] = value;
	return SaveToFile();
}

bool FileStateDatabase::LoadState(const std::string& key, std::string& value) {
	auto it = stateMap.find(key);
	if (it != stateMap.end()) {
		value = it->second;
		return true;
	}
	return false;
}

bool FileStateDatabase::DeleteState(const std::string& key) {
	auto it = stateMap.find(key);
	if (it != stateMap.end()) {
		stateMap.erase(it);
		return SaveToFile();
	}
	return false;
}

bool FileStateDatabase::HasState(const std::string& key) const {
	return stateMap.find(key) != stateMap.end();
}

std::vector<std::string> FileStateDatabase::GetAllKeys() const {
	std::vector<std::string> keys;
	for (const auto& pair : stateMap) {
		keys.push_back(pair.first);
	}
	return keys;
}

bool FileStateDatabase::ClearAll() {
	stateMap.clear();
	return SaveToFile();
}

bool FileStateDatabase::Vacuum() {
	// Переписываем файл без пробелов
	return SaveToFile();
}

bool FileStateDatabase::LoadFromFile() {
	std::ifstream file(dbPath);
	if (!file.is_open()) {
		return true;  // Файл еще не создан
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		size_t delimPos = line.find('=');
		if (delimPos != std::string::npos) {
			std::string key = line.substr(0, delimPos);
			std::string value = line.substr(delimPos + 1);
			stateMap[key] = value;
		}
	}

	file.close();
	isLoaded = true;
	return true;
}

bool FileStateDatabase::SaveToFile() {
	std::ofstream file(dbPath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& pair : stateMap) {
		file << pair.first << "=" << pair.second << "\n";
	}

	file.close();
	return true;
}

// ApplicationStateManager
ApplicationStateManager::ApplicationStateManager() {
	persistenceEngine = std::make_unique<FileStateDatabase>();
}

ApplicationStateManager::~ApplicationStateManager() {
}

bool ApplicationStateManager::Initialize(const std::string& stateDbPath) {
	auto fileDb = dynamic_cast<FileStateDatabase*>(persistenceEngine.get());
	if (fileDb) {
		return fileDb->Initialize(stateDbPath);
	}
	return false;
}

bool ApplicationStateManager::SaveApplicationState() {
	// Сохраняем пользовательские параметры
	for (const auto& pref : userPreferences) {
		persistenceEngine->SaveState("pref_" + pref.first, pref.second);
	}

	// Сохраняем недавние файлы
	std::string recentFilesStr;
	for (const auto& file : recentFiles) {
		recentFilesStr += file + ";";
	}
	persistenceEngine->SaveState("recent_files", recentFilesStr);

	return true;
}

bool ApplicationStateManager::LoadApplicationState() {
	// Загружаем пользовательские параметры
	std::vector<std::string> keys;
	auto fileDb = dynamic_cast<FileStateDatabase*>(persistenceEngine.get());
	if (fileDb) {
		keys = fileDb->GetAllKeys();
	}

	for (const auto& key : keys) {
		if (key.find("pref_") == 0) {
			std::string value;
			persistenceEngine->LoadState(key, value);
			userPreferences[key.substr(5)] = value;
		}
	}

	// Загружаем недавние файлы
	std::string recentFilesStr;
	if (persistenceEngine->LoadState("recent_files", recentFilesStr)) {
		std::stringstream ss(recentFilesStr);
		std::string file;
		recentFiles.clear();
		while (std::getline(ss, file, ';')) {
			if (!file.empty()) {
				recentFiles.push_back(file);
			}
		}
	}

	return true;
}

void ApplicationStateManager::SaveUIState(const std::string& windowTitle, 
										  int x, int y, int width, int height) {
	persistenceEngine->SaveState("ui_title", windowTitle);
	persistenceEngine->SaveState("ui_x", std::to_string(x));
	persistenceEngine->SaveState("ui_y", std::to_string(y));
	persistenceEngine->SaveState("ui_width", std::to_string(width));
	persistenceEngine->SaveState("ui_height", std::to_string(height));
}

bool ApplicationStateManager::LoadUIState(std::string& windowTitle, 
										 int& x, int& y, int& width, int& height) {
	std::string title, xStr, yStr, widthStr, heightStr;

	if (persistenceEngine->LoadState("ui_title", title)) {
		windowTitle = title;
	}

	if (persistenceEngine->LoadState("ui_x", xStr)) {
		x = std::stoi(xStr);
	}

	if (persistenceEngine->LoadState("ui_y", yStr)) {
		y = std::stoi(yStr);
	}

	if (persistenceEngine->LoadState("ui_width", widthStr)) {
		width = std::stoi(widthStr);
	}

	if (persistenceEngine->LoadState("ui_height", heightStr)) {
		height = std::stoi(heightStr);
	}

	return true;
}

void ApplicationStateManager::SaveDownloadState(const std::string& torrentName, 
											   int64_t downloaded, int64_t total) {
	persistenceEngine->SaveState("dl_" + torrentName + "_down", std::to_string(downloaded));
	persistenceEngine->SaveState("dl_" + torrentName + "_total", std::to_string(total));
}

bool ApplicationStateManager::LoadDownloadState(const std::string& torrentName, 
											   int64_t& downloaded, int64_t& total) {
	std::string downStr, totalStr;

	if (persistenceEngine->LoadState("dl_" + torrentName + "_down", downStr)) {
		downloaded = std::stoll(downStr);
	}

	if (persistenceEngine->LoadState("dl_" + torrentName + "_total", totalStr)) {
		total = std::stoll(totalStr);
	}

	return true;
}

void ApplicationStateManager::AddRecentFile(const std::string& filepath) {
	auto it = std::find(recentFiles.begin(), recentFiles.end(), filepath);
	if (it != recentFiles.end()) {
		recentFiles.erase(it);
	}

	recentFiles.insert(recentFiles.begin(), filepath);

	if (recentFiles.size() > MAX_RECENT_FILES) {
		recentFiles.pop_back();
	}
}

std::vector<std::string> ApplicationStateManager::GetRecentFiles() const {
	return recentFiles;
}

void ApplicationStateManager::ClearRecentFiles() {
	recentFiles.clear();
}

void ApplicationStateManager::SetUserPreference(const std::string& key, const std::string& value) {
	userPreferences[key] = value;
}

bool ApplicationStateManager::GetUserPreference(const std::string& key, std::string& value) const {
	auto it = userPreferences.find(key);
	if (it != userPreferences.end()) {
		value = it->second;
		return true;
	}
	return false;
}

std::string ApplicationStateManager::GetUIStateKey() {
	return "ui_state";
}

std::string ApplicationStateManager::GetDownloadStateKey(const std::string& torrentName) {
	return "dl_" + torrentName;
}

// UndoRedoManager
UndoRedoManager::UndoRedoManager(size_t maxSteps)
	: maxUndoSteps(maxSteps) {
}

UndoRedoManager::~UndoRedoManager() {
}

void UndoRedoManager::RecordAction(const Action& action) {
	undoStack.push_back(action);
	redoStack.clear();

	if (undoStack.size() > maxUndoSteps) {
		undoStack.erase(undoStack.begin());
	}
}

bool UndoRedoManager::Undo() {
	if (undoStack.empty()) {
		return false;
	}

	Action action = undoStack.back();
	undoStack.pop_back();

	if (action.undoCallback) {
		action.undoCallback(action.actionData);
	}

	redoStack.push_back(action);
	return true;
}

bool UndoRedoManager::Redo() {
	if (redoStack.empty()) {
		return false;
	}

	Action action = redoStack.back();
	redoStack.pop_back();

	if (action.redoCallback) {
		action.redoCallback(action.actionData);
	}

	undoStack.push_back(action);
	return true;
}

bool UndoRedoManager::CanUndo() const {
	return !undoStack.empty();
}

bool UndoRedoManager::CanRedo() const {
	return !redoStack.empty();
}

std::string UndoRedoManager::GetUndoDescription() const {
	if (!undoStack.empty()) {
		return "Undo: " + undoStack.back().actionName;
	}
	return "Nothing to undo";
}

std::string UndoRedoManager::GetRedoDescription() const {
	if (!redoStack.empty()) {
		return "Redo: " + redoStack.back().actionName;
	}
	return "Nothing to redo";
}

void UndoRedoManager::Clear() {
	undoStack.clear();
	redoStack.clear();
}

// HotkeyManager
HotkeyManager::HotkeyManager()
	: nextHotkeyId(1) {
}

HotkeyManager::~HotkeyManager() {
}

bool HotkeyManager::RegisterHotkey(int virtualKey, Modifier modifier, 
								   const std::string& actionName) {
	int key = CombineModifier(virtualKey, modifier);

	Hotkey hotkey;
	hotkey.id = nextHotkeyId++;
	hotkey.virtualKey = virtualKey;
	hotkey.modifier = modifier;
	hotkey.actionName = actionName;
	hotkey.enabled = true;

	hotkeyMap[{virtualKey, (int)modifier}] = hotkey;
	return true;
}

bool HotkeyManager::UnregisterHotkey(int virtualKey, Modifier modifier) {
	auto it = hotkeyMap.find({virtualKey, (int)modifier});
	if (it != hotkeyMap.end()) {
		hotkeyMap.erase(it);
		return true;
	}
	return false;
}

std::string HotkeyManager::GetAction(int virtualKey, Modifier modifier) const {
	auto it = hotkeyMap.find({virtualKey, (int)modifier});
	if (it != hotkeyMap.end()) {
		return it->second.actionName;
	}
	return "";
}

std::vector<HotkeyManager::Hotkey> HotkeyManager::GetAllHotkeys() const {
	std::vector<Hotkey> hotkeys;
	for (const auto& pair : hotkeyMap) {
		hotkeys.push_back(pair.second);
	}
	return hotkeys;
}

bool HotkeyManager::SaveHotkeys(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& pair : hotkeyMap) {
		const Hotkey& hk = pair.second;
		file << hk.virtualKey << "|"
			 << (int)hk.modifier << "|"
			 << hk.actionName << "|"
			 << (hk.enabled ? "1" : "0") << "\n";
	}

	file.close();
	return true;
}

bool HotkeyManager::LoadHotkeys(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;

		int vKey, mod;
		std::string action;
		bool enabled;

		std::getline(ss, token, '|');
		vKey = std::stoi(token);

		std::getline(ss, token, '|');
		mod = std::stoi(token);

		std::getline(ss, action, '|');

		std::getline(ss, token, '|');
		enabled = (token == "1");

		RegisterHotkey(vKey, (Modifier)mod, action);
	}

	file.close();
	return true;
}

void HotkeyManager::EnableHotkey(uint32_t hotkeyId) {
	for (auto& pair : hotkeyMap) {
		if (pair.second.id == hotkeyId) {
			pair.second.enabled = true;
			break;
		}
	}
}

void HotkeyManager::DisableHotkey(uint32_t hotkeyId) {
	for (auto& pair : hotkeyMap) {
		if (pair.second.id == hotkeyId) {
			pair.second.enabled = false;
			break;
		}
	}
}

int HotkeyManager::CombineModifier(int virtualKey, Modifier modifier) const {
	return virtualKey | (int)modifier;
}

// PluginManager
PluginManager::PluginManager()
	: nextPluginId(1) {
}

PluginManager::~PluginManager() {
}

bool PluginManager::LoadPlugin(const std::string& pluginPath) {
	Plugin plugin;
	plugin.id = nextPluginId++;
	plugin.filepath = pluginPath;
	plugin.enabled = false;
	plugin.loaded = false;

	plugins.push_back(plugin);
	return true;
}

bool PluginManager::UnloadPlugin(uint32_t pluginId) {
	auto it = std::find_if(plugins.begin(), plugins.end(),
		[pluginId](const Plugin& p) { return p.id == pluginId; });

	if (it != plugins.end()) {
		it->loaded = false;
		return true;
	}
	return false;
}

std::vector<PluginManager::Plugin> PluginManager::GetInstalledPlugins() const {
	return plugins;
}

std::vector<PluginManager::Plugin> PluginManager::GetLoadedPlugins() const {
	std::vector<Plugin> loaded;
	for (const auto& p : plugins) {
		if (p.loaded) {
			loaded.push_back(p);
		}
	}
	return loaded;
}

bool PluginManager::EnablePlugin(uint32_t pluginId) {
	for (auto& p : plugins) {
		if (p.id == pluginId) {
			p.enabled = true;
			p.loaded = true;
			return true;
		}
	}
	return false;
}

bool PluginManager::DisablePlugin(uint32_t pluginId) {
	for (auto& p : plugins) {
		if (p.id == pluginId) {
			p.enabled = false;
			p.loaded = false;
			return true;
		}
	}
	return false;
}

PluginManager::Plugin PluginManager::GetPluginInfo(uint32_t pluginId) const {
	for (const auto& p : plugins) {
		if (p.id == pluginId) {
			return p;
		}
	}
	return Plugin();
}

void PluginManager::ScanPluginsDirectory(const std::string& pluginDir) {
	// Реализация поиска плагинов в директории
}
