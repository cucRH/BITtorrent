# 📚 BitTorrent Client - Полный индекс проекта

## 📁 Структура файлов

### 🔧 Исходные коды (Source Files)

#### Основное приложение
```
BITtorrent/
├── BITtorrent.cpp           [Win32 главное окно и message loop]
├── BITtorrent.h             [Определения и макросы приложения]
├── framework.h              [Общие заголовки фреймворка]
└── BITtorrent.vcxproj       [Visual Studio project файл]
```

#### Координация
```
BITtorrent/
├── BitTorrentClient.h       [Интерфейс координатора приложения]
└── BitTorrentClient.cpp     [Реализация координатора]
```

#### Основной функционал
```
BITtorrent/
├── TorrentCore.h/cpp        [Парсинг torrent файлов, bencode]
├── DownloadManager.h/cpp    [Управление загрузками, threading]
├── TrackerManager.h/cpp     [Взаимодействие с трекерами]
├── CacheManager.h/cpp       [Кеширование блоков в памяти]
└── StorageManager.h/cpp     [Управление файловой системой]
```

#### Менеджеры и сервисы
```
BITtorrent/
├── UIManager.h/cpp          [Создание и управление интерфейсом]
├── Logger.h/cpp             [Система логирования (singleton)]
├── SettingsManager.h/cpp    [Управление конфигурацией]
├── StatisticsManager.h/cpp  [Сбор статистики использования]
└── BandwidthManager.h/cpp   [Управление пропускной способностью]
```

#### Расширенные компоненты
```
BITtorrent/
├── NotificationManager.h/cpp    [Система уведомлений и событий]
├── ScheduleManager.h/cpp        [Расписание загрузок и БВ]
├── IPFilterManager.h/cpp        [Фильтрация IP адресов]
├── PerformanceMonitor.h/cpp     [Мониторинг производительности]
├── StateManager.h/cpp           [Управление состоянием приложения]
├── HotkeyManager                [Управление горячими клавишами (в StateManager)]
└── PluginManager                [Система плагинов (в StateManager)]
```

---

## 📖 Документация

### Пользовательские руководства
```
├── QUICKSTART.md                [⭐ Начните отсюда (5 минут)]
├── README.md                    [Обзор проекта и установка]
├── USER_GUIDE.md                [Полное руководство пользователя]
└── FAQ.md                       [Часто задаваемые вопросы]
```

### Технические документы
```
├── ARCHITECTURE.md              [Архитектура системы]
├── EXTENDED_COMPONENTS.md       [Описание расширенных компонентов]
├── STATE_MANAGEMENT.md          [Управление состоянием]
├── DEVELOPER_GUIDE.md           [Руководство для разработчиков]
├── REFERENCE.md                 [API справочник]
└── EXTENDING.md                 [Как расширить функциональность]
```

### Проектные документы
```
├── PROJECT_COMPLETION_STATUS.md [Статус завершения проекта]
├── COMPLETION_REPORT.md         [Отчет о завершении]
└── ARCHITECTURE_DIAGRAM.txt     [ASCII диаграмма архитектуры]
```

---

## ⚙️ Конфигурация

```
BITtorrent/
└── config.ini.example           [Пример конфигурационного файла]
```

---

## 📊 Статистика проекта

### По типам файлов

```
┌──────────────────────────────────┐
│ Тип файла    │ Количество       │
├──────────────────────────────────┤
│ .cpp         │ 15 файлов        │
│ .h           │ 15 файлов        │
│ .md          │ 10+ файлов       │
│ .ini         │ 1 файл           │
│ .vcxproj     │ 1 файл           │
│ Всего        │ 50+ файлов       │
└──────────────────────────────────┘
```

### По строкам кода

```
Основные исходники:       ~3000 строк
Расширенные компоненты:   ~2000 строк
Документация:             ~5000 строк
Всего:                    ~10000 строк
```

### По функциональности

```
Классы:                   15+
Методов:                  200+
Функций:                  50+
Интерфейсов:             8+
```

---

## 🎯 Карта использования

### Для новичков
```
START HERE → QUICKSTART.md
	↓
README.md (установка)
	↓
USER_GUIDE.md (основное использование)
	↓
FAQ.md (решение проблем)
```

### Для разработчиков
```
START HERE → DEVELOPER_GUIDE.md
	↓
ARCHITECTURE.md (понимание структуры)
	↓
API REFERENCE.md (справка по API)
	↓
EXTENDING.md (добавление функций)
	↓
Source Code (реализация)
```

### Для архитекторов
```
START HERE → ARCHITECTURE.md
	↓
EXTENDED_COMPONENTS.md (доп. компоненты)
	↓
STATE_MANAGEMENT.md (управление состоянием)
	↓
PROJECT_COMPLETION_STATUS.md (статус)
	↓
Source Code (детали реализации)
```

---

## 🔍 Быстрая справка по модулям

### TorrentCore
**Файлы:** TorrentCore.h/cpp
**Отвечает за:** Парсинг .torrent файлов
**Ключевые функции:**
- LoadTorrent() - загрузка и парсинг
- DecodeBencode*() - декодирование bencode
- CalculateInfoHash() - расчет SHA1 хеша

**Использование:**
```cpp
TorrentInfo info;
TorrentParser::LoadTorrent("file.torrent", info);
```

### DownloadManager
**Файлы:** DownloadManager.h/cpp
**Отвечает за:** Управление процессом загрузки
**Ключевые функции:**
- StartDownload() - начать загрузку
- PauseDownload() - поставить на паузу
- GetProgress() - получить прогресс

**Использование:**
```cpp
DownloadManager dm;
dm.StartDownload(torrentInfo);
auto progress = dm.GetProgress();
```

### UIManager
**Файлы:** UIManager.h/cpp
**Отвечает за:** Создание и управление интерфейсом
**Ключевые функции:**
- CreateUI() - создание элементов UI
- UpdateUI() - обновление информации
- HandleCommand() - обработка команд пользователя

**Использование:**
```cpp
UIManager ui;
ui.CreateUI(hwnd);
ui.UpdateUI();
```

### StorageManager
**Файлы:** StorageManager.h/cpp
**Отвечает за:** Управление файловой системой
**Ключевые функции:**
- SaveTorrentMetadata() - сохранение метаданных
- LoadTorrentMetadata() - загрузка метаданных
- GetTorrentPath() - путь к торренту

**Использование:**
```cpp
StorageManager sm;
sm.SaveTorrentMetadata(torrentInfo);
```

### StateManager
**Файлы:** StateManager.h/cpp
**Отвечает за:** Сохранение состояния приложения
**Ключевые компоненты:**
- FileStateDatabase - сохранение в файл
- ApplicationStateManager - управление состоянием
- UndoRedoManager - история действий
- HotkeyManager - горячие клавиши
- PluginManager - система плагинов

**Использование:**
```cpp
ApplicationStateManager stateManager;
stateManager.Initialize("app_state.db");
stateManager.SaveUIState("Window", 100, 200, 800, 600);
```

### BandwidthManager
**Файлы:** BandwidthManager.h/cpp
**Отвечает за:** Управление пропускной способностью
**Ключевые функции:**
- UpdateDownloadSpeed() - обновить скорость
- LimitDownloadSpeed() - установить лимит
- GetBandwidthInfo() - получить информацию

**Использование:**
```cpp
BandwidthManager bwMgr;
bwMgr.UpdateDownloadSpeed(1024);
bwMgr.LimitDownloadSpeed(2048);
```

### IPFilterManager
**Файлы:** IPFilterManager.h/cpp
**Отвечает за:** Фильтрацию IP адресов
**Ключевые функции:**
- AddRule() - добавить правило
- IsIPAllowed() - проверить IP
- SaveRules() - сохранить правила

**Использование:**
```cpp
IPFilterManager filter;
filter.AddPrivateIPFilter();
if (filter.IsIPAllowed(ip)) { /* разрешено */ }
```

### NotificationManager
**Файлы:** NotificationManager.h/cpp
**Отвечает за:** Система уведомлений
**Ключевые функции:**
- AddNotification() - добавить уведомление
- GetUnreadNotifications() - получить непрочитанные
- SetOnNotificationCallback() - установить callback

**Использование:**
```cpp
NotificationManager notifier;
Notification notif;
notif.type = NotificationType::DOWNLOAD_COMPLETE;
notifier.AddNotification(notif);
```

### ScheduleManager
**Файлы:** ScheduleManager.h/cpp
**Отвечает за:** Расписание загрузок
**Ключевые функции:**
- AddSchedule() - добавить расписание
- GetDueSchedules() - получить готовые к выполнению
- ExecuteSchedule() - выполнить расписание

**Использование:**
```cpp
ScheduleManager scheduler;
DownloadSchedule schedule;
schedule.repeatType = ScheduleRepeat::DAILY;
scheduler.AddSchedule(schedule);
```

### PerformanceMonitor
**Файлы:** PerformanceMonitor.h/cpp
**Отвечает за:** Мониторинг производительности
**Ключевые компоненты:**
- PerformanceMonitor - сбор метрик
- SystemEventCollector - события
- ApplicationHealthMonitor - здоровье приложения

**Использование:**
```cpp
PerformanceMonitor monitor;
monitor.RecordCPUUsage(45.5f);
auto metrics = monitor.GetMetrics();
```

---

## 🚀 Начало работы

### Минимально требуемое знание
```
1. Скачайте проект
2. Откройте BITtorrent.sln в Visual Studio
3. Build → Build Solution
4. F5 для запуска
5. Ctrl+O для открытия торрента
6. Space для паузы/продолжения
```

### Рекомендуемое изучение (по порядку)
```
1. QUICKSTART.md (5 минут)
2. README.md (10 минут)
3. USER_GUIDE.md (30 минут)
4. ARCHITECTURE.md (30 минут)
5. DEVELOPER_GUIDE.md (1 час)
```

### Для опытных разработчиков
```
1. Используйте find_symbol для анализа
2. Посмотрите ARCHITECTURE.md
3. Изучите BitTorrentClient.cpp
4. Начните с понравившегося модуля
5. Расширяйте!
```

---

## 📞 Справка и поддержка

### Встроенные ресурсы
- F1 в приложении - встроенная справка
- Help menu - доступна справка
- Status bar - текущая информация

### Локальная документация
- Все .md файлы в папке BITtorrent/
- config.ini.example - конфигурация
- Source code с комментариями

### Где найти ответы на вопросы
```
Вопрос: Как установить?
Ответ: README.md, QUICKSTART.md

Вопрос: Как использовать?
Ответ: USER_GUIDE.md

Вопрос: Как расширить?
Ответ: EXTENDING.md, DEVELOPER_GUIDE.md

Вопрос: Как что-то работает?
Ответ: ARCHITECTURE.md, исходный код

Вопрос: У меня ошибка!
Ответ: FAQ.md, DEVELOPER_GUIDE.md
```

---

## 🎓 Структура обучения

### Уровень 1: Пользователь (Beginner)
```
📱 QUICKSTART.md
📖 USER_GUIDE.md
❓ FAQ.md
Время: ~1 час
```

### Уровень 2: Новичок (Novice Developer)
```
🏗️ ARCHITECTURE.md
📚 DEVELOPER_GUIDE.md
🔍 REFERENCE.md
Время: ~3 часа
```

### Уровень 3: Разработчик (Intermediate)
```
🔧 Source Code Analysis
📝 EXTENDING.md
🎯 EXTENDED_COMPONENTS.md
Время: ~1 день
```

### Уровень 4: Архитектор (Advanced)
```
🏛️ Full Architecture Review
📊 STATE_MANAGEMENT.md
🚀 PROJECT_COMPLETION_STATUS.md
Время: ~1 неделя
```

---

## ✅ Контрольный список

### Перед первым использованием
- [ ] Скачана последняя версия
- [ ] Проверены требования системы
- [ ] Прочитан QUICKSTART.md
- [ ] Установлена папка загрузок

### Перед расширением функционала
- [ ] Прочитан ARCHITECTURE.md
- [ ] Изучен DEVELOPER_GUIDE.md
- [ ] Понят интересующий модуль
- [ ] Подготовлена среда разработки

### Перед развертыванием
- [ ] Код протестирован
- [ ] Документация актуальна
- [ ] Version бамп выполнен
- [ ] Release notes написаны

---

## 🎉 Поздравляем!

Вы изучили полный индекс проекта BitTorrent Client!

### Теперь вы можете:
✅ Начать использовать приложение
✅ Понять архитектуру и дизайн
✅ Расширить функциональность
✅ Участвовать в разработке
✅ Развертывать в production

---

**Версия:** 1.0.0
**Последнее обновление:** 2024
**Статус:** ✅ Готово к использованию

**Навигация по проекту успешно завершена! 🚀**
