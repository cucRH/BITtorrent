#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ctime>
#include <map>

// Типы уведомлений
enum class NotificationType {
	INFO,
	WARNING,
	SUCCESS,
	ERROR,
	DOWNLOAD_COMPLETE,
	DOWNLOAD_FAILED,
	DOWNLOAD_STARTED,
	PEER_CONNECTED,
	TRACKER_ERROR
};

// Структура уведомления
struct Notification {
	NotificationType type;
	std::string title;
	std::string message;
	std::time_t timestamp;
	bool read;
	std::string relatedTorrent;
};

// Система уведомлений
class NotificationManager {
public:
	NotificationManager();
	~NotificationManager();

	void AddNotification(const Notification& notification);
	void ShowNotification(const Notification& notification);

	std::vector<Notification> GetUnreadNotifications() const;
	std::vector<Notification> GetAllNotifications() const;

	void MarkAsRead(size_t index);
	void MarkAllAsRead();
	void ClearNotifications();

	size_t GetUnreadCount() const;

	// Callbacks
	void SetOnNotificationCallback(std::function<void(const Notification&)> callback);

private:
	std::vector<Notification> notifications;
	std::function<void(const Notification&)> onNotificationCallback;
	const size_t MAX_NOTIFICATIONS = 100;

	std::string GetNotificationTypeString(NotificationType type);
};

// Класс для системных событий
class EventManager {
public:
	EventManager();
	~EventManager();

	enum class EventType {
		DOWNLOAD_STARTED,
		DOWNLOAD_PAUSED,
		DOWNLOAD_RESUMED,
		DOWNLOAD_COMPLETED,
		DOWNLOAD_FAILED,
		PEER_CONNECTED,
		PEER_DISCONNECTED,
		TRACKER_UPDATED,
		SETTINGS_CHANGED,
		APPLICATION_STARTED,
		APPLICATION_STOPPED
	};

	void PublishEvent(EventType eventType, const std::string& details = "");
	void Subscribe(EventType eventType, std::function<void(const std::string&)> handler);

private:
	std::map<EventType, std::vector<std::function<void(const std::string&)>>> subscribers;
};
