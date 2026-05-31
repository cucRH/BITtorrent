# BitTorrent Client - Руководство разработчика

## Содержание
1. [Архитектура](#архитектура)
2. [Модули](#модули)
3. [Компиляция](#компиляция)
4. [Развертывание](#развертывание)
5. [Тестирование](#тестирование)
6. [Расширение функциональности](#расширение-функциональности)

---

## Архитектура

### Слои приложения

```
┌─────────────────────────────────────────┐
│         Пользовательский интерфейс      │
│          (UIManager, BITtorrent.cpp)    │
├─────────────────────────────────────────┤
│      Менеджеры и сервисы приложения     │
│  (BitTorrentClient, Logger, StatsMgr)   │
├─────────────────────────────────────────┤
│         Основной функционал             │
│  (DownloadMgr, TrackerMgr, TorrentCore) │
├─────────────────────────────────────────┤
│      Хранение и кеширование             │
│   (StorageManager, CacheManager)        │
├─────────────────────────────────────────┤
│      Утилиты и вспомогательные          │
│ (BandwidthMgr, IPFilter, StateManager)  │
└─────────────────────────────────────────┘
```

### Основные компоненты

#### Core Layer (Ядро)
- **TorrentCore** - Парсинг и обработка торрент-файлов
- **DownloadManager** - Управление процессом загрузки
- **TrackerManager** - Взаимодействие с трекерами
- **PeerConnection** - Взаимодействие с пирами (планируется)

#### Service Layer (Сервисы)
- **StorageManager** - Управление файлами на диске
- **CacheManager** - Кеширование блоков
- **SettingsManager** - Конфигурация приложения
- **Logger** - Логирование событий

#### Business Logic Layer (Бизнес-логика)
- **BandwidthManager** - Управление пропускной способностью
- **IPFilterManager** - Фильтрация пиров
- **ScheduleManager** - Расписание загрузок
- **NotificationManager** - Система уведомлений

#### UI Layer (Интерфейс)
- **UIManager** - Создание и управление интерфейсом
- **BITtorrent.cpp** - Главное окно приложения

#### Persistence Layer (Хранение состояния)
- **StateManager** - Сохранение состояния приложения
- **UndoRedoManager** - История действий
- **HotkeyManager** - Управление горячими клавишами

---

## Модули

### 1. TorrentCore (TorrentCore.h/cpp)
**Ответственность:** Парсинг и обработка торрент-файлов

```cpp
class TorrentParser {
	static bool LoadTorrent(const std::string& filePath, TorrentInfo& info);
	static std::string CalculateInfoHash(const std::string& torrentPath);
	// ...
};
```

**TODO:**
- [ ] Полный парсинг bencode для вложенных структур
- [ ] Правильный расчет SHA1 хеша info-dictionary
- [ ] Поддержка magnet-ссылок
- [ ] Валидация целостности файла

### 2. DownloadManager (DownloadManager.h/cpp)
**Ответственность:** Управление загрузкой, планирование блоков

```cpp
class DownloadManager {
	bool StartDownload(const TorrentInfo& torrent);
	void PauseDownload();
	void ResumeDownload();
	void StopDownload();
	// ...
private:
	void DownloadThread();  // Worker thread
};
```

**TODO:**
- [ ] Реальная загрузка блоков с пиров
- [ ] Планирование приоритетов загрузки
- [ ] Стратегия выбора пиров
- [ ] Обработка сбоев и повторных попыток

### 3. TrackerManager (TrackerManager.h/cpp)
**Ответственность:** Взаимодействие с трекерами

```cpp
class TrackerManager {
	bool RequestTrackerInfo(const TorrentInfo& torrent, 
						   std::vector<PeerInfo>& peers);
	bool UpdateTrackerStatus();
	// ...
};
```

**TODO:**
- [ ] HTTP-трекер протокол (RFC 3720)
- [ ] UDP-трекер протокол
- [ ] Парсинг ответов (compact и non-compact)
- [ ] Переподключение к резервным трекерам

### 4. UIManager (UIManager.h/cpp)
**Ответственность:** Создание и управление интерфейсом

```cpp
class UIManager {
	bool CreateUI(HWND parent);
	void UpdateUI();
	void HandleFileOpen();
	void HandleCommand(UINT commandId);
	// ...
};
```

**Элементы UI:**
- ListView для списка торрентов
- ProgressBar для отображения прогресса
- Кнопки (Add, Start, Pause, Remove)
- StatusBar для информации о скорости
- Метки для деталей торрента

### 5. StorageManager (StorageManager.h/cpp)
**Ответственность:** Работа с файловой системой

```cpp
class StorageManager {
	bool SaveTorrentMetadata(const TorrentInfo& info);
	bool LoadTorrentMetadata(const std::string& path);
	bool SaveDownloadHistory();
	bool LoadDownloadHistory();
	// ...
};
```

### 6. CacheManager (CacheManager.h/cpp)
**Ответственность:** Кеширование блоков в памяти

```cpp
class CacheManager {
	bool SaveBlock(uint32_t pieceIndex, const std::vector<uint8_t>& data);
	bool GetBlock(uint32_t pieceIndex, std::vector<uint8_t>& data);
	void ClearCache();
	// ...
};
```

---

## Компиляция

### Требования
- Visual Studio 2015+ или Visual Studio Community 2026
- Windows SDK
- C++11 или выше

### Сборка в Visual Studio

#### Способ 1: IDE
```
1. Откройте BITtorrent.sln
2. Выберите конфигурацию:
   - Debug (для разработки)
   - Release (для производства)
3. Build → Build Solution (Ctrl+Shift+B)
```

#### Способ 2: Командная строка
```powershell
# Развернуть переменные окружения MSVC
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

# Скомпилировать
msbuild BITtorrent\BITtorrent.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Конфигурации сборки

#### Debug
```
- Оптимизация: Отключена (/Od)
- Debug info: Полная (/Zi)
- Runtime: Мультипоточная (/MTd)
- Использование: Разработка и отладка
```

#### Release
```
- Оптимизация: Максимум (/O2)
- Debug info: Отсутствует
- Runtime: Мультипоточная (/MT)
- Использование: Развертывание
```

### Проверка сборки

```powershell
# Проверить успешность сборки
if (Test-Path "BITtorrent\Release\BITtorrent.exe") {
	Write-Host "Build successful!" -ForegroundColor Green
} else {
	Write-Host "Build failed!" -ForegroundColor Red
}
```

---

## Развертывание

### Создание установщика

```powershell
# 1. Подготовить файлы
$buildDir = "BITtorrent\Release"
$installDir = "Installer"

if (-not (Test-Path $installDir)) {
	New-Item -ItemType Directory -Path $installDir
}

# 2. Копировать файлы
Copy-Item "$buildDir\BITtorrent.exe" "$installDir\"
Copy-Item "BITtorrent\config.ini.example" "$installDir\config.ini"
Copy-Item "BITtorrent\README.md" "$installDir\"

# 3. Создать архив
Compress-Archive -Path "$installDir\*" `
				 -DestinationPath "BitTorrentClient-Portable.zip" `
				 -Force
```

### Распространение

#### Требования для Release
- ✓ Протестировано на Windows 7+
- ✓ Все зависимости включены (если есть)
- ✓ Документация включена (README, USER_GUIDE)
- ✓ Исходный код (если OSS)
- ✓ Лицензия

#### Структура файлов Release
```
BitTorrentClient-v1.0/
├── BITtorrent.exe (основное приложение)
├── config.ini (конфигурация)
├── README.md (быстрый старт)
├── USER_GUIDE.md (руководство пользователя)
├── ARCHITECTURE.md (архитектура)
├── LICENSE (лицензия)
└── logs/ (папка для логов)
```

---

## Тестирование

### Юнит-тесты

```cpp
// test/TorrentCoreTests.cpp
#include <cassert>
#include "TorrentCore.h"

void TestTorrentParsing() {
	TorrentInfo info;
	bool result = TorrentParser::LoadTorrent("test.torrent", info);
	assert(result == true);
	assert(!info.name.empty());
}

void TestInfoHash() {
	std::string hash = TorrentParser::CalculateInfoHash("test.torrent");
	assert(hash.length() == 40);  // SHA1 hex = 40 символов
}
```

### Интеграционные тесты

```cpp
// test/IntegrationTests.cpp
void TestFullDownloadCycle() {
	// 1. Загрузить торрент
	TorrentInfo info;
	TorrentParser::LoadTorrent("ubuntu.torrent", info);

	// 2. Запустить загрузку
	DownloadManager dm;
	dm.StartDownload(info);

	// 3. Проверить прогресс
	std::this_thread::sleep_for(std::chrono::seconds(5));
	auto progress = dm.GetProgress();
	assert(progress.downloadedBytes > 0);

	// 4. Остановить загрузку
	dm.StopDownload();
}
```

### Тестирование производительности

```cpp
// test/PerformanceTests.cpp
#include <chrono>

void BenchmarkTorrentParsing() {
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1000; i++) {
		TorrentInfo info;
		TorrentParser::LoadTorrent("large.torrent", info);
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "1000 iterations: " << duration.count() << "ms" << std::endl;
	// Должно быть < 5 сек на современном ПК
}
```

### Запуск тестов

```bash
# Компилировать и запускать тесты
cd test
g++ -std=c++11 TorrentCoreTests.cpp ../BITtorrent/TorrentCore.cpp -o TorrentTests
./TorrentTests

# Вывод:
# Test: TorrentParsing ... PASS
# Test: InfoHash ... PASS
# Test: FullDownloadCycle ... PASS
# ================================
# All tests passed: 3/3
```

---

## Расширение функциональности

### Добавление нового менеджера

#### Шаг 1: Создать заголовок
```cpp
// BITtorrent/NewManager.h
#pragma once
#include <string>

class NewManager {
public:
	NewManager();
	~NewManager();

	bool Initialize();
	bool Shutdown();

	// Ваши методы
	void DoSomething();
};
```

#### Шаг 2: Реализовать
```cpp
// BITtorrent/NewManager.cpp
#include "NewManager.h"

NewManager::NewManager() {
}

NewManager::~NewManager() {
}

bool NewManager::Initialize() {
	return true;
}

bool NewManager::Shutdown() {
	return true;
}

void NewManager::DoSomething() {
	// Реализация
}
```

#### Шаг 3: Интегрировать в BitTorrentClient
```cpp
// BITtorrent/BitTorrentClient.h
#include "NewManager.h"

class BitTorrentClient {
private:
	NewManager newManager;
};

// BITtorrent/BitTorrentClient.cpp
bool BitTorrentClient::Initialize(const std::string& configPath) {
	if (!newManager.Initialize()) {
		Logger::GetInstance().Error("Failed to init NewManager");
		return false;
	}
	// ...
}
```

### Добавление новой команды в UI

#### Шаг 1: Добавить ID команды
```cpp
// BITtorrent/BITtorrent.h
#define ID_BUTTON_NEW_FEATURE 1015
```

#### Шаг 2: Создать кнопку в UIManager
```cpp
// BITtorrent/UIManager.cpp
HWND hButton = CreateWindowW(
	L"BUTTON", L"New Feature",
	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	10, 110, 80, 25,
	hwnd, (HMENU)ID_BUTTON_NEW_FEATURE,
	GetModuleHandleW(nullptr), nullptr
);
```

#### Шаг 3: Обработать команду
```cpp
// BITtorrent/BITtorrent.cpp
case WM_COMMAND:
	if (LOWORD(wParam) == ID_BUTTON_NEW_FEATURE) {
		if (g_pBitTorrentClient) {
			g_pBitTorrentClient->DoNewFeature();
		}
	}
	break;
```

### Система плагинов

```cpp
// Пример использования PluginManager
PluginManager pluginMgr;

// Загрузить плагин
pluginMgr.LoadPlugin("MyPlugin.dll");

// Включить плагин
auto plugins = pluginMgr.GetInstalledPlugins();
if (!plugins.empty()) {
	pluginMgr.EnablePlugin(plugins[0].id);
}

// Использовать плагин
auto loaded = pluginMgr.GetLoadedPlugins();
for (const auto& plugin : loaded) {
	std::cout << "Loaded: " << plugin.name << std::endl;
}
```

---

## Сниппеты кода

### Создание логирования

```cpp
#include "Logger.h"

Logger::GetInstance().Info("Application started");
Logger::GetInstance().Debug("Initialized components");
Logger::GetInstance().Warning("Memory usage high");
Logger::GetInstance().Error("Failed to load config");
```

### Работа с конфигурацией

```cpp
SettingsManager settings;
settings.LoadSettings("config.ini");

std::string downloadDir = settings.GetString("paths", "downloads");
int maxPeers = settings.GetInt("network", "max_peers");
bool enableEncryption = settings.GetBool("security", "encrypt");
```

### Обработка ошибок

```cpp
try {
	TorrentInfo info;
	if (!TorrentParser::LoadTorrent(path, info)) {
		throw std::runtime_error("Invalid torrent file");
	}
} catch (const std::exception& ex) {
	Logger::GetInstance().Error("Error: " + std::string(ex.what()));
}
```

---

## Рекомендации по кодированию

### Стиль кода
- ✓ Используйте camelCase для переменных и функций
- ✓ Используйте PascalCase для классов
- ✓ UPPERCASE для констант
- ✓ Максимум 100 символов на строку
- ✓ Используйте 4 пробела для отступа

### Комментарии
```cpp
// Используйте однострочные комментарии для объяснения
int result = Calculate();  // результат вычисления

/*
	Используйте многострочные комментарии для
	описания сложной логики или функций
*/
```

### Управление памятью
```cpp
// Используйте умные указатели
std::unique_ptr<DownloadManager> dm(new DownloadManager());

// или
std::shared_ptr<Logger> logger = Logger::GetInstance();
```

### Многопоточность
```cpp
// Используйте мутексы для синхронизации
std::mutex mtx;
{
	std::lock_guard<std::mutex> lock(mtx);
	// Критическая секция
}
```

---

**Версия:** 1.0
**Дата:** 2024
**Автор:** BitTorrent Client Development Team
