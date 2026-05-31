#include "NotificationManager.h"
#include <algorithm>
#include <ctime>

NotificationManager::NotificationManager() {
}

NotificationManager::~NotificationManager() {
}

void NotificationManager::AddNotification(const Notification& notification) {
	if (notifications.size() >= MAX_NOTIFICATIONS) {
		notifications.erase(notifications.begin());
	}

	Notification newNotif = notification;
	newNotif.timestamp = std::time(nullptr);
	newNotif.read = false;

	notifications.push_back(newNotif);

	if (onNotificationCallback) {
		onNotificationCallback(newNotif);
	}
}

void NotificationManager::ShowNotification(const Notification& notification) {
	AddNotification(notification);
}

std::vector<Notification> NotificationManager::GetUnreadNotifications() const {
	std::vector<Notification> unread;
	for (const auto& notif : notifications) {
		if (!notif.read) {
			unread.push_back(notif);
		}
	}
	return unread;
}

std::vector<Notification> NotificationManager::GetAllNotifications() const {
	return notifications;
}

void NotificationManager::MarkAsRead(size_t index) {
	if (index < notifications.size()) {
		notifications[index].read = true;
	}
}

void NotificationManager::MarkAllAsRead() {
	for (auto& notif : notifications) {
		notif.read = true;
	}
}

void NotificationManager::ClearNotifications() {
	notifications.clear();
}

size_t NotificationManager::GetUnreadCount() const {
	size_t count = 0;
	for (const auto& notif : notifications) {
		if (!notif.read) {
			count++;
		}
	}
	return count;
}

void NotificationManager::SetOnNotificationCallback(std::function<void(const Notification&)> callback) {
	onNotificationCallback = callback;
}

std::string NotificationManager::GetNotificationTypeString(NotificationType type) {
	switch (type) {
	case NotificationType::INFO:
		return "Information";
	case NotificationType::WARNING:
		return "Warning";
	case NotificationType::SUCCESS:
		return "Success";
	case NotificationType::ERROR:
		return "Error";
	case NotificationType::DOWNLOAD_COMPLETE:
		return "Download Complete";
	case NotificationType::DOWNLOAD_FAILED:
		return "Download Failed";
	case NotificationType::DOWNLOAD_STARTED:
		return "Download Started";
	case NotificationType::PEER_CONNECTED:
		return "Peer Connected";
	case NotificationType::TRACKER_ERROR:
		return "Tracker Error";
	default:
		return "Unknown";
	}
}

// EventManager implementation
EventManager::EventManager() {
}

EventManager::~EventManager() {
}

void EventManager::PublishEvent(EventType eventType, const std::string& details) {
	auto it = subscribers.find(eventType);
	if (it != subscribers.end()) {
		for (auto& handler : it->second) {
			handler(details);
		}
	}
}

void EventManager::Subscribe(EventType eventType, std::function<void(const std::string&)> handler) {
	subscribers[eventType].push_back(handler);
}
