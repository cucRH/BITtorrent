#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include <functional>

// Интерфейс для сохранения состояния
class IStatePersistence {
public:
	virtual ~IStatePersistence() = default;

	virtual bool SaveState(const std::string& key, const std::string& value) = 0;
	virtual bool LoadState(const std::string& key, std::string& value) = 0;
	virtual bool DeleteState(const std::string& key) = 0;
	virtual bool HasState(const std::string& key) const = 0;
};

// Реализация файловой базы данных состояний
class FileStateDatabase : public IStatePersistence {
public:
	FileStateDatabase();
	~FileStateDatabase();

	bool Initialize(const std::string& dbPath);

	bool SaveState(const std::string& key, const std::string& value) override;
	bool LoadState(const std::string& key, std::string& value) override;
	bool DeleteState(const std::string& key) override;
	bool HasState(const std::string& key) const override;

	std::vector<std::string> GetAllKeys() const;
	bool ClearAll();
	bool Vacuum();  // Очистить неиспользуемое место

private:
	std::map<std::string, std::string> stateMap;
	std::string dbPath;
	bool isLoaded;

	bool LoadFromFile();
	bool SaveToFile();
};

// Менеджер состояния приложения
class ApplicationStateManager {
public:
	ApplicationStateManager();
	~ApplicationStateManager();

	bool Initialize(const std::string& stateDbPath);

	// Управление состоянием
	bool SaveApplicationState();
	bool LoadApplicationState();

	// Состояние UI
	void SaveUIState(const std::string& windowTitle, int x, int y, int width, int height);
	bool LoadUIState(std::string& windowTitle, int& x, int& y, int& width, int& height);

	// Состояние загрузок
	void SaveDownloadState(const std::string& torrentName, 
						  int64_t downloaded, int64_t total);
	bool LoadDownloadState(const std::string& torrentName, 
						  int64_t& downloaded, int64_t& total);

	// Состояние рецентных файлов
	void AddRecentFile(const std::string& filepath);
	std::vector<std::string> GetRecentFiles() const;
	void ClearRecentFiles();

	// Пользовательские параметры
	void SetUserPreference(const std::string& key, const std::string& value);
	bool GetUserPreference(const std::string& key, std::string& value) const;

private:
	std::unique_ptr<IStatePersistence> persistenceEngine;
	std::map<std::string, std::string> userPreferences;
	std::vector<std::string> recentFiles;
	const size_t MAX_RECENT_FILES = 10;

	std::string GetUIStateKey();
	std::string GetDownloadStateKey(const std::string& torrentName);
};

// Менеджер отката действий
class UndoRedoManager {
public:
	struct Action {
		std::string actionName;
		std::string actionData;
		std::function<void(const std::string&)> undoCallback;
		std::function<void(const std::string&)> redoCallback;
	};

	UndoRedoManager(size_t maxUndoSteps = 50);
	~UndoRedoManager();

	void RecordAction(const Action& action);
	bool Undo();
	bool Redo();

	bool CanUndo() const;
	bool CanRedo() const;

	std::string GetUndoDescription() const;
	std::string GetRedoDescription() const;

	void Clear();

private:
	std::vector<Action> undoStack;
	std::vector<Action> redoStack;
	size_t maxUndoSteps;
};

// Менеджер горячих клавиш
class HotkeyManager {
public:
	enum class Modifier {
		NONE = 0,
		CTRL = 1,
		SHIFT = 2,
		ALT = 4,
		CTRL_SHIFT = 3,
		CTRL_ALT = 5,
		SHIFT_ALT = 6,
		CTRL_SHIFT_ALT = 7
	};

	struct Hotkey {
		uint32_t id;
		int virtualKey;
		Modifier modifier;
		std::string actionName;
		bool enabled;
	};

	HotkeyManager();
	~HotkeyManager();

	bool RegisterHotkey(int virtualKey, Modifier modifier, 
					   const std::string& actionName);
	bool UnregisterHotkey(int virtualKey, Modifier modifier);

	std::string GetAction(int virtualKey, Modifier modifier) const;
	std::vector<Hotkey> GetAllHotkeys() const;

	bool SaveHotkeys(const std::string& filepath);
	bool LoadHotkeys(const std::string& filepath);

	void EnableHotkey(uint32_t hotkeyId);
	void DisableHotkey(uint32_t hotkeyId);

private:
	std::map<std::pair<int, int>, Hotkey> hotkeyMap;
	uint32_t nextHotkeyId;

	int CombineModifier(int virtualKey, Modifier modifier) const;
};

// Менеджер плагинов/расширений
class PluginManager {
public:
	struct Plugin {
		uint32_t id;
		std::string name;
		std::string version;
		std::string author;
		std::string description;
		std::string filepath;
		bool enabled;
		bool loaded;
	};

	PluginManager();
	~PluginManager();

	bool LoadPlugin(const std::string& pluginPath);
	bool UnloadPlugin(uint32_t pluginId);

	std::vector<Plugin> GetInstalledPlugins() const;
	std::vector<Plugin> GetLoadedPlugins() const;

	bool EnablePlugin(uint32_t pluginId);
	bool DisablePlugin(uint32_t pluginId);

	Plugin GetPluginInfo(uint32_t pluginId) const;

	void ScanPluginsDirectory(const std::string& pluginDir);

private:
	std::vector<Plugin> plugins;
	uint32_t nextPluginId;
	std::vector<void*> loadedLibraries;  // Для HMODULE на Windows
};
