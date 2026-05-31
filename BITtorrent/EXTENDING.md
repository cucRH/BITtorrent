# Гайд по расширению BitTorrent Client

## Введение

Этот документ описывает, как расширить функциональность BitTorrent Client путем добавления новых функций и компонентов.

## Добавление новой функции загрузки

### Шаг 1: Создать новый менеджер

```cpp
// NewFeatureManager.h
#pragma once
#include <string>

class NewFeatureManager {
public:
	NewFeatureManager();
	~NewFeatureManager();

	bool Initialize();
	void Update();
	void Shutdown();

private:
	// Приватные члены
};
```

### Шаг 2: Интегрировать в BitTorrentClient

```cpp
// BitTorrentClient.h
class BitTorrentClient {
private:
	NewFeatureManager newFeatureManager;  // Добавить менеджер
};

// BitTorrentClient.cpp
bool BitTorrentClient::Initialize(const std::string& configPath) {
	// ...
	newFeatureManager.Initialize();  // Инициализировать
	// ...
}
```

### Шаг 3: Добавить UI элементы

```cpp
// UIManager.h
class UIManager {
private:
	HWND hNewFeatureButton;  // Добавить кнопку
};

// UIManager.cpp
void UIManager::CreateButtons() {
	// Существующие кнопки...

	// Новая кнопка
	hNewFeatureButton = CreateWindowW(
		L"BUTTON",
		L"New Feature",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		x, y, width, height,
		hMainWindow,
		(HMENU)IDC_BTN_NEW_FEATURE,
		GetModuleHandle(nullptr),
		nullptr
	);
}
```

## Добавление нового типа трекера

### Шаг 1: Расширить TrackerManager

```cpp
// TrackerManager.h
class TrackerManager {
private:
	bool RequestTrackerInfoHTTP(const std::string& url, ...);
	bool RequestTrackerInfoUDP(const std::string& url, ...);
	bool RequestTrackerInfoDHT(const std::string& infoHash, ...);  // Новый
};
```

### Шаг 2: Реализовать методы

```cpp
// TrackerManager.cpp
bool TrackerManager::RequestTrackerInfoDHT(const std::string& infoHash, ...) {
	// Реализация DHT запроса
	// ...
	return true;
}
```

## Добавление поддержки плагинов

### Пример структуры плагина

```cpp
// Plugin Interface
class IPlugin {
public:
	virtual ~IPlugin() {}
	virtual bool Initialize() = 0;
	virtual void Execute() = 0;
	virtual void Shutdown() = 0;
	virtual const char* GetName() = 0;
};

// PluginManager
class PluginManager {
public:
	bool LoadPlugin(const std::string& dllPath);
	void UnloadPlugin(const std::string& pluginName);
	void ExecuteAllPlugins();

private:
	std::map<std::string, HMODULE> plugins;
};
```

## Добавление поддержки WebUI

### Шаг 1: Добавить HTTP сервер

```cpp
// WebUIServer.h
class WebUIServer {
public:
	bool Start(uint16_t port);
	void Stop();
	void SetBitTorrentClient(BitTorrentClient* client);

private:
	void HandleRequest(const std::string& request);
	std::string GenerateJSON();
};
```

### Шаг 2: Интегрировать в клиент

```cpp
// BitTorrentClient.h
private:
	WebUIServer webUIServer;
```

## Добавление поддержки магнет-ссылок

### Реализация

```cpp
// MagnetLinkParser.h
class MagnetLinkParser {
public:
	static bool Parse(const std::string& magnetLink, TorrentInfo& outInfo);

private:
	static bool ExtractInfoHash(const std::string& link, std::string& hash);
	static bool ExtractTrackers(const std::string& link, std::vector<std::string>& trackers);
};

// Использование
TorrentInfo torrent;
if (MagnetLinkParser::Parse("magnet:?xt=urn:btih:...", torrent)) {
	client->StartDownload(torrent);
}
```

## Добавление поддержки IPv6

### Модификация TrackerManager

```cpp
// TrackerManager.h
struct PeerInfo {
	std::string ip;           // IPv4 или IPv6
	uint16_t port;
	bool isIPv6;              // Новое поле
	// ...
};

// Реализация
bool TrackerManager::RequestTrackerInfo(...) {
	// Проверяем поддержку IPv6
	if (supports_ipv6) {
		// Запрашиваем IPv6 пиров
	}
}
```

## Добавление RSS поддержки

### Структура

```cpp
// RSSManager.h
class RSSManager {
public:
	bool AddFeed(const std::string& url);
	bool RemoveFeed(const std::string& url);
	std::vector<TorrentInfo> CheckFeeds();

private:
	std::vector<std::string> feeds;
	std::map<std::string, std::vector<std::string>> seenTorrents;
};
```

## Улучшение поиска пиров

### Реализация улучшенного алгоритма поиска

```cpp
// PeerDiscoveryManager.h
class PeerDiscoveryManager {
public:
	std::vector<PeerInfo> FindPeers(const std::string& infoHash);

private:
	// DHT поиск
	std::vector<PeerInfo> SearchDHT(const std::string& infoHash);

	// PEX (Peer Exchange)
	std::vector<PeerInfo> SearchPEX(const std::string& peerID);

	// Трекер поиск
	std::vector<PeerInfo> SearchTracker(const std::string& infoHash);
};
```

## Оптимизация скорости

### Кеширование результатов

```cpp
// CacheLayer.h
class CacheLayer {
public:
	bool TryGetFromCache(const std::string& key, std::vector<uint8_t>& data);
	void PutInCache(const std::string& key, const std::vector<uint8_t>& data);
	void ClearExpired();

private:
	std::map<std::string, std::pair<std::vector<uint8_t>, time_t>> cache;
	const time_t CACHE_TTL = 3600;  // 1 час
};
```

## Тестирование расширений

### Unit тесты

```cpp
// TestNewFeature.cpp
#include <cassert>

void TestNewFeatureInitialization() {
	NewFeatureManager manager;
	assert(manager.Initialize() == true);
	manager.Shutdown();
}

void TestNewFeatureExecution() {
	NewFeatureManager manager;
	manager.Initialize();
	manager.Update();
	assert(/* проверка результата */);
	manager.Shutdown();
}
```

### Интеграционные тесты

```cpp
// Тестирование интеграции с BitTorrentClient
BitTorrentClient client;
client.Initialize("config.ini");

// Тест новой функции
// ...

client.Shutdown();
```

## Best Practices

### 1. Следуйте существующей архитектуре
- Создавайте новые менеджеры для новой функциональности
- Интегрируйте через BitTorrentClient
- Используйте Logger для отладки

### 2. Обработка ошибок
```cpp
try {
	newFeatureManager.Initialize();
} catch (const std::exception& e) {
	Logger::GetInstance().Error(e.what(), "NewFeature");
}
```

### 3. Потокобезопасность
```cpp
class NewFeatureManager {
private:
	std::mutex mutex;

public:
	void Update() {
		std::lock_guard<std::mutex> lock(mutex);
		// Работа с данными
	}
};
```

### 4. Документирование
- Добавляйте комментарии к новым методам
- Обновляйте README.md
- Добавляйте примеры использования

### 5. Производительность
- Минимизируйте копирование данных
- Используйте ссылки где возможно
- Профилируйте новый код

## Рекомендуемые расширения

1. **WebUI Interface** - веб-интерфейс для удаленного управления
2. **Advanced Scheduling** - планирование загрузок по времени
3. **Bandwidth Shaping** - динамический контроль скорости
4. **PEX Support** - обмен пирами между клиентами
5. **Partial Downloads** - выборочная загрузка файлов
6. **IP Filtering** - блокировка/разрешение IP адресов
7. **Encryption** - шифрование соединений
8. **Mobile App** - приложение для мобильных устройств

## Контрибьютинг

При добавлении новых функций:
1. Создавайте отдельные ветки Git
2. Пишите юнит тесты
3. Документируйте изменения
4. Делайте код ревью перед мержем
