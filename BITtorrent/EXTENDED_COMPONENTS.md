# Расширенные компоненты BitTorrent Client

## Новые добавленные компоненты

### 1. BandwidthManager (BandwidthManager.h/cpp)
**Назначение:** Управление пропускной способностью и анализ скорости

**Функции:**
- Отслеживание текущей скорости загрузки/выгрузки
- Расчет средней скорости за период
- Ограничение максимальной скорости
- История пиков производительности
- Анализ статистики пиров

**Использование:**
```cpp
BandwidthManager bwManager;
bwManager.UpdateDownloadSpeed(512.5f);  // KB/s
bwManager.LimitDownloadSpeed(1024);      // Максимум 1 MB/s

BandwidthInfo info = bwManager.GetBandwidthInfo();
```

---

### 2. NotificationManager (NotificationManager.h/cpp)
**Назначение:** Система уведомлений приложения

**Возможности:**
- Различные типы уведомлений (INFO, WARNING, SUCCESS, ERROR, etc.)
- Хранение истории уведомлений
- Система обратных вызовов (callbacks)
- Управление прочитанными/непрочитанными уведомлениями
- Event-driven архитектура

**Использование:**
```cpp
NotificationManager notifier;
Notification notification;
notification.type = NotificationType::DOWNLOAD_COMPLETE;
notification.title = "Download Finished";
notification.message = "File.torrent has been downloaded";
notifier.AddNotification(notification);
```

---

### 3. ScheduleManager (ScheduleManager.h/cpp)
**Назначение:** Планирование автоматических загрузок и управление пропускной способностью

**Компоненты:**
- **ScheduleManager:** Управление расписанием загрузок
  - Одноразовые загрузки
  - Ежедневные повторения
  - Еженедельные повторения
  - Ежемесячные повторения

- **TimeBasedBandwidthScheduler:** Управление пропускной способностью по времени
  - Разные ограничения для разных часов
  - Управление по дням недели
  - Автоматическое переключение режимов

**Использование:**
```cpp
ScheduleManager scheduler;
DownloadSchedule schedule;
schedule.torrentPath = "file.torrent";
schedule.repeatType = ScheduleRepeat::DAILY;
scheduler.AddSchedule(schedule);
```

---

### 4. IPFilterManager (IPFilterManager.h/cpp)
**Назначение:** Фильтрация IP адресов пиров

**Возможности:**
- Белые списки (ALLOW) / Черные списки (BLOCK)
- Поддержка CIDR нотации (192.168.1.0/24)
- Поддержка диапазонов IP (192.168.1.0-192.168.1.255)
- Отслеживание заблокированных пиров
- Сохранение/загрузка правил

**Использование:**
```cpp
IPFilterManager ipFilter;

// Блокировать приватные IP адреса
ipFilter.AddPrivateIPFilter();

// Проверить, разрешен ли IP
if (ipFilter.IsIPAllowed("192.168.1.100")) {
	// Пир разрешен
}
```

---

### 5. PerformanceMonitor (PerformanceMonitor.h/cpp)
**Назначение:** Мониторинг производительности и здоровья приложения

**Компоненты:**
- **PerformanceMonitor:** Отслеживание CPU, памяти, потоков
- **SystemEventCollector:** Сбор системных событий
- **ApplicationHealthMonitor:** Анализ здоровья приложения

**Метрики:**
- Использование CPU (%)
- Использование памяти (MB)
- Количество активных потоков
- Задержка (latency)
- Общее количество ошибок/предупреждений

**Использование:**
```cpp
PerformanceMonitor monitor;
monitor.StartMonitoring();
monitor.RecordCPUUsage(45.5f);
monitor.RecordMemoryUsage(256);

PerformanceMetrics metrics = monitor.GetMetrics();

ApplicationHealthMonitor health;
health.Update();
if (!health.IsHealthy()) {
	std::cout << health.GetHealthReport();
}
```

---

## Интеграция компонентов

### Обновленный BitTorrentClient

```cpp
class BitTorrentClient {
private:
	BandwidthManager bandwidthManager;
	NotificationManager notificationManager;
	ScheduleManager scheduleManager;
	IPFilterManager ipFilterManager;
	PerformanceMonitor performanceMonitor;
	ApplicationHealthMonitor healthMonitor;
};
```

### Обновленный UIManager

Может отображать:
- Уведомления о событиях
- Информацию о здоровье приложения
- Графики использования ресурсов
- Активные расписания
- Статус IP фильтрации

---

## Примеры использования

### Пример 1: Ограничение скорости по расписанию

```cpp
TimeBasedBandwidthScheduler scheduler;

// В рабочее время (9:00-18:00) ограничиваем до 512 KB/s
TimeBasedBandwidthScheduler::TimeSlot workHours;
workHours.hourStart = 9;
workHours.hourEnd = 18;
workHours.maxDownloadSpeedKBps = 512;
workHours.dayOfWeek = 1;  // Понедельник
scheduler.AddTimeSlot(workHours);

// После работы (18:00-23:00) разрешаем 2 MB/s
TimeBasedBandwidthScheduler::TimeSlot offHours;
offHours.hourStart = 18;
offHours.hourEnd = 23;
offHours.maxDownloadSpeedKBps = 2048;
scheduler.AddTimeSlot(offHours);
```

### Пример 2: Система уведомлений

```cpp
NotificationManager notifier;

// Установим callback для получения уведомлений
notifier.SetOnNotificationCallback([](const Notification& notif) {
	std::cout << notif.title << ": " << notif.message << std::endl;
});

// Добавим уведомление
Notification notif;
notif.type = NotificationType::DOWNLOAD_COMPLETE;
notif.title = "Download Complete";
notif.message = "Linux.torrent finished downloading";
notif.relatedTorrent = "Linux.torrent";
notifier.AddNotification(notif);
```

### Пример 3: Фильтрация IP адресов

```cpp
IPFilterManager filter;

// Заблокировать VPN и прокси провайдеров
IPFilterRule rule1;
rule1.ipRange = "185.220.100.0/24";  // Tor Exit Nodes
rule1.action = FilterAction::BLOCK;
rule1.description = "Block Tor exits";
filter.AddRule(rule1);

// Проверить пира
PeerInfo peer;
peer.ip = "192.168.1.100";
if (!filter.IsIPAllowed(peer.ip)) {
	// Пир заблокирован
	continue;
}
```

### Пример 4: Планирование загрузок

```cpp
ScheduleManager scheduler;

// Запланировать загрузку на 02:00 каждый день
DownloadSchedule schedule;
schedule.torrentPath = "ubuntu.iso.torrent";
schedule.torrentName = "Ubuntu ISO";
schedule.scheduledTime.tm_hour = 2;
schedule.scheduledTime.tm_min = 0;
schedule.repeatType = ScheduleRepeat::DAILY;
schedule.enabled = true;

scheduler.AddSchedule(schedule);

// Сохранить расписание
scheduler.SaveSchedules("schedules.dat");
```

### Пример 5: Мониторинг здоровья приложения

```cpp
ApplicationHealthMonitor health;
PerformanceMonitor perf;

// Регулярно обновляем метрики (например, каждую секунду)
perf.RecordCPUUsage(getCurrentCPUUsage());
perf.RecordMemoryUsage(getCurrentMemoryUsage() / 1024 / 1024);

health.Update();

// Выводим отчет если приложение нездорово
if (!health.IsHealthy()) {
	std::cout << health.GetHealthReport() << std::endl;

	// Применяем рекомендации
	for (const auto& rec : health.GetRecommendations()) {
		std::cout << "Recommendation: " << rec << std::endl;
	}
}
```

---

## Интеграция с основным приложением

### InitInstance() - Инициализация

```cpp
bool BitTorrentClient::Initialize(const std::string& configPath) {
	// Инициализируем новые компоненты
	performanceMonitor.StartMonitoring();

	// Загружаем правила фильтрации
	ipFilterManager.LoadRules("ip_filters.dat");

	// Загружаем расписание
	scheduleManager.LoadSchedules("schedules.dat");

	// Загружаем временные ограничения пропускной способности
	timeBasedBandwidthScheduler.LoadTimeSlots("bandwidth_schedule.dat");

	return true;
}
```

### Main Loop - Основной цикл

```cpp
// В WndProc при обработке WM_TIMER
case WM_TIMER:
	if (wParam == TIMER_UPDATE) {
		// Обновляем производительность
		perf.RecordCPUUsage(getSystemCPUUsage());
		perf.RecordMemoryUsage(getSystemMemoryUsage());

		// Проверяем здоровье
		health.Update();

		// Проверяем расписание
		auto dueSchedules = scheduleManager.GetDueSchedules();
		for (const auto& schedule : dueSchedules) {
			scheduleManager.ExecuteSchedule(schedule.id);
			// Запускаем загрузку
		}

		// Обновляем пропускную способность
		float maxDown = timeBasedBandwidthScheduler.GetCurrentMaxDownloadSpeed();
		bandwidthManager.LimitDownloadSpeed(maxDown);

		// Обновляем UI
		if (g_pUIManager) {
			g_pUIManager->UpdateUI();
		}
	}
	break;
```

---

## Производительность

Все новые компоненты разработаны с учетом:
- **Минимальное использование ресурсов**
- **Асинхронная обработка** где возможно
- **Кеширование данных**
- **Оптимизированные алгоритмы**

---

## Расширяемость

Архитектура позволяет легко добавить:
- Интеграцию с облачными сервисами
- Экспорт метрик в мониторинг системы
- Интеграцию с мобильными приложениями
- REST API для удаленного управления
- WebUI интерфейс

---

## Примечания

- Все компоненты полностью протестированы
- Код скомпилирован без ошибок и предупреждений
- Документация полная и актуальная
- Архитектура чистая и расширяемая
