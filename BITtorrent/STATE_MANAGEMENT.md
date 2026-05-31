# Компоненты управления состоянием и конфигурацией

## StateManager.h/cpp

### 1. FileStateDatabase
Легкая встроенная база данных для сохранения состояния приложения.

```cpp
// Использование
FileStateDatabase db;
db.Initialize("app_state.db");

// Сохранение
db.SaveState("user_name", "John");
db.SaveState("window_pos", "100,200");

// Загрузка
std::string username;
if (db.LoadState("user_name", username)) {
	std::cout << "User: " << username << std::endl;
}

// Проверка наличия ключа
if (db.HasState("window_pos")) {
	// Ключ существует
}
```

### 2. ApplicationStateManager
Интегрированный менеджер состояния приложения.

**Возможности:**
- Сохранение/загрузка состояния UI (позиция, размер окна)
- Отслеживание загрузок
- История недавних файлов
- Пользовательские параметры

**Пример использования:**

```cpp
ApplicationStateManager stateManager;
stateManager.Initialize("app_state.db");

// Сохранить состояние окна
stateManager.SaveUIState("BitTorrent Client", 100, 200, 800, 600);

// Загрузить состояние окна
std::string title;
int x, y, width, height;
stateManager.LoadUIState(title, x, y, width, height);

// Работа с недавними файлами
stateManager.AddRecentFile("C:\\Downloads\\ubuntu.torrent");
auto recentFiles = stateManager.GetRecentFiles();

// Пользовательские параметры
stateManager.SetUserPreference("theme", "dark");
std::string theme;
stateManager.GetUserPreference("theme", theme);

// Сохранение состояния загрузок
stateManager.SaveDownloadState("ubuntu.torrent", 524288000, 1048576000);
int64_t downloaded, total;
stateManager.LoadDownloadState("ubuntu.torrent", downloaded, total);
```

### 3. UndoRedoManager
Система отката и повтора действий.

**Возможности:**
- Регистрация действий с callback'ами
- История отката/повтора
- Ограничение размера стека
- Описание действий

**Пример использования:**

```cpp
UndoRedoManager undoRedo(100);  // Максимум 100 действий

// Записать действие удаления загрузки
UndoRedoManager::Action removeAction;
removeAction.actionName = "Remove download";
removeAction.actionData = "ubuntu.torrent";

removeAction.undoCallback = [](const std::string& data) {
	// Восстановить загрузку
	std::cout << "Undo remove: " << data << std::endl;
};

removeAction.redoCallback = [](const std::string& data) {
	// Удалить загрузку снова
	std::cout << "Redo remove: " << data << std::endl;
};

undoRedo.RecordAction(removeAction);

// Проверить возможность отката
if (undoRedo.CanUndo()) {
	std::cout << undoRedo.GetUndoDescription() << std::endl;  // "Undo: Remove download"
	undoRedo.Undo();
}

// Проверить возможность повтора
if (undoRedo.CanRedo()) {
	std::cout << undoRedo.GetRedoDescription() << std::endl;  // "Redo: Remove download"
	undoRedo.Redo();
}
```

### 4. HotkeyManager
Управление горячими клавишами приложения.

**Возможности:**
- Регистрация горячих клавиш
- Поддержка модификаторов (Ctrl, Shift, Alt)
- Сохранение/загрузка конфигурации
- Включение/отключение горячих клавиш

**Пример использования:**

```cpp
HotkeyManager hotkeyManager;

// Регистрировать горячие клавиши
hotkeyManager.RegisterHotkey(VK_O, HotkeyManager::Modifier::CTRL, "open_file");
hotkeyManager.RegisterHotkey(VK_P, HotkeyManager::Modifier::CTRL, "pause_download");
hotkeyManager.RegisterHotkey(VK_S, HotkeyManager::Modifier::CTRL_SHIFT, "start_download");

// Получить действие по горячей клавише
std::string action = hotkeyManager.GetAction(VK_O, HotkeyManager::Modifier::CTRL);
if (action == "open_file") {
	// Выполнить открытие файла
}

// Получить все горячие клавиши
auto hotkeys = hotkeyManager.GetAllHotkeys();
for (const auto& hk : hotkeys) {
	std::cout << "ID: " << hk.id << ", Action: " << hk.actionName << std::endl;
}

// Сохранить конфигурацию
hotkeyManager.SaveHotkeys("hotkeys.ini");

// Загрузить конфигурацию
hotkeyManager.LoadHotkeys("hotkeys.ini");

// Отключить горячую клавишу
hotkeyManager.DisableHotkey(1);  // Отключить по ID
```

### 5. PluginManager
Система управления плагинами и расширениями.

**Возможности:**
- Загрузка/выгрузка плагинов
- Управление жизненным циклом плагина
- Отслеживание установленных плагинов
- Сканирование директории плагинов

**Пример использования:**

```cpp
PluginManager pluginManager;

// Загрузить плагин
pluginManager.LoadPlugin("C:\\Plugins\\AdBlockPlugin.dll");
pluginManager.LoadPlugin("C:\\Plugins\\ThemePlugin.dll");

// Получить все установленные плагины
auto plugins = pluginManager.GetInstalledPlugins();
for (const auto& plugin : plugins) {
	std::cout << "Plugin: " << plugin.name 
			  << " v" << plugin.version 
			  << " by " << plugin.author << std::endl;
}

// Включить плагин
pluginManager.EnablePlugin(1);  // Включить по ID

// Получить загруженные плагины
auto loadedPlugins = pluginManager.GetLoadedPlugins();
std::cout << "Loaded: " << loadedPlugins.size() << " plugins" << std::endl;

// Выключить плагин
pluginManager.DisablePlugin(1);

// Выгрузить плагин
pluginManager.UnloadPlugin(1);
```

## Интеграция с главным приложением

### В BitTorrentClient::Initialize()

```cpp
bool BitTorrentClient::Initialize(const std::string& configPath) {
	// ... существующий код ...

	// Инициализировать управление состоянием
	stateManager.Initialize("app_state.db");
	stateManager.LoadApplicationState();

	// Загрузить горячие клавиши
	hotkeyManager.LoadHotkeys("hotkeys.ini");

	// Загрузить состояние загрузок
	auto recentFiles = stateManager.GetRecentFiles();
	for (const auto& file : recentFiles) {
		// Восстановить загрузки
	}

	return true;
}
```

### В BITtorrent.cpp WndProc()

```cpp
case WM_KEYDOWN:
	{
		HotkeyManager::Modifier mod = HotkeyManager::Modifier::NONE;
		if (GetKeyState(VK_CONTROL) & 0x8000) {
			mod = HotkeyManager::Modifier::CTRL;
		}
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			mod = (HotkeyManager::Modifier)((int)mod | (int)HotkeyManager::Modifier::SHIFT);
		}

		std::string action = g_pHotkeyManager->GetAction(wParam, mod);
		if (!action.empty()) {
			if (action == "open_file") {
				// Открыть файл
			} else if (action == "pause_download") {
				// Пауза загрузки
			}
		}
	}
	break;
```

### При закрытии приложения

```cpp
case WM_DESTROY:
	// ... существующий код ...

	// Сохранить состояние приложения
	if (g_pStateManager) {
		g_pStateManager->SaveApplicationState();
	}

	// Сохранить состояние окна перед выходом
	RECT rect;
	GetWindowRect(hWnd, &rect);
	if (g_pStateManager) {
		g_pStateManager->SaveUIState(
			"BitTorrent Client",
			rect.left, rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top
		);
	}

	PostQuitMessage(0);
	break;
```

## Расширенные сценарии использования

### Сценарий 1: Восстановление после перезагрузки

```cpp
// При запуске приложения
void RestoreApplicationState() {
	ApplicationStateManager stateManager;
	stateManager.Initialize("app_state.db");
	stateManager.LoadApplicationState();

	// Восстановить размер и позицию окна
	std::string title;
	int x, y, width, height;
	stateManager.LoadUIState(title, x, y, width, height);

	// Восстановить позицию окна (если она была сохранена)
	MoveWindow(hwnd, x, y, width, height, TRUE);

	// Восстановить недавние файлы
	auto recentFiles = stateManager.GetRecentFiles();
	for (const auto& file : recentFiles) {
		// Восстановить торренты
		LoadTorrent(file);
	}
}
```

### Сценарий 2: Система отката действий в UI

```cpp
void DeleteDownload(const std::string& torrentName) {
	// Создать действие для отката
	UndoRedoManager::Action action;
	action.actionName = "Delete: " + torrentName;
	action.actionData = torrentName;

	// Сохранить данные торрента перед удалением
	auto torrentData = SaveTorrentData(torrentName);

	action.undoCallback = [torrentData](const std::string& data) {
		RestoreTorrentData(data, torrentData);
	};

	action.redoCallback = [](const std::string& data) {
		DeleteTorrentPermanently(data);
	};

	// Зарегистрировать действие и выполнить удаление
	undoRedoManager.RecordAction(action);
	DeleteTorrentPermanently(torrentName);
}
```

### Сценарий 3: Персональные горячие клавиши

```cpp
void SetupDefaultHotkeys(HotkeyManager& manager) {
	// Основные операции
	manager.RegisterHotkey(VK_O, HotkeyManager::Modifier::CTRL, "open_file");
	manager.RegisterHotkey(VK_N, HotkeyManager::Modifier::CTRL, "new_download");

	// Управление загрузками
	manager.RegisterHotkey(VK_SPACE, HotkeyManager::Modifier::NONE, "pause_resume");
	manager.RegisterHotkey(VK_DELETE, HotkeyManager::Modifier::NONE, "delete_download");

	// Программа
	manager.RegisterHotkey(VK_F5, HotkeyManager::Modifier::NONE, "refresh");
	manager.RegisterHotkey(VK_F1, HotkeyManager::Modifier::NONE, "show_help");

	// Сохранить
	manager.SaveHotkeys("hotkeys.ini");
}
```

## Заметки о производительности

- **StateManager:** Оптимизирован для минимального использования памяти
- **UndoRedoManager:** Ограничивает количество хранимых действий
- **HotkeyManager:** Использует быстрый поиск по ассоциативному массиву
- **PluginManager:** Поддерживает ленивую загрузку плагинов

## Безопасность

- Все файлы состояния сохраняются в защищенном месте
- Чувствительные данные могут быть зашифрованы
- Валидация при загрузке сохраненного состояния

---

**Версия:** 1.0
**Автор:** BitTorrent Client Development Team
**Дата:** 2024
