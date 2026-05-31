#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

// Тип повторения расписания
enum class ScheduleRepeat {
	NEVER,
	DAILY,
	WEEKLY,
	MONTHLY
};

// Структура для расписания загрузок
struct DownloadSchedule {
	uint32_t id;
	std::string torrentPath;
	std::string torrentName;
	struct tm scheduledTime;
	ScheduleRepeat repeatType;
	bool enabled;
	std::time_t lastExecuted;
	std::string downloadPath;
};

// Менеджер расписаний
class ScheduleManager {
public:
	ScheduleManager();
	~ScheduleManager();

	// Управление расписаниями
	bool AddSchedule(const DownloadSchedule& schedule);
	bool RemoveSchedule(uint32_t scheduleId);
	bool UpdateSchedule(const DownloadSchedule& schedule);

	// Получение расписаний
	DownloadSchedule GetSchedule(uint32_t scheduleId) const;
	std::vector<DownloadSchedule> GetAllSchedules() const;
	std::vector<DownloadSchedule> GetEnabledSchedules() const;

	// Проверка и выполнение
	std::vector<DownloadSchedule> GetDueSchedules();
	bool ExecuteSchedule(uint32_t scheduleId);

	// Сохранение/загрузка
	bool SaveSchedules(const std::string& filepath);
	bool LoadSchedules(const std::string& filepath);

	// Управление
	bool EnableSchedule(uint32_t scheduleId);
	bool DisableSchedule(uint32_t scheduleId);
	void ClearSchedules();

private:
	std::vector<DownloadSchedule> schedules;
	uint32_t nextScheduleId;

	bool IsScheduleDue(const DownloadSchedule& schedule);
	void UpdateScheduleNextTime(DownloadSchedule& schedule);
};

// Класс для управления полосой пропускания по времени
class TimeBasedBandwidthScheduler {
public:
	TimeBasedBandwidthScheduler();
	~TimeBasedBandwidthScheduler();

	struct TimeSlot {
		uint32_t id;
		uint8_t hourStart;
		uint8_t hourEnd;
		float maxDownloadSpeedKBps;
		float maxUploadSpeedKBps;
		uint8_t dayOfWeek;  // 0-6, где 0 - воскресенье
	};

	bool AddTimeSlot(const TimeSlot& slot);
	bool RemoveTimeSlot(uint32_t slotId);

	TimeSlot GetCurrentTimeSlot() const;
	std::vector<TimeSlot> GetAllTimeSlots() const;

	float GetCurrentMaxDownloadSpeed() const;
	float GetCurrentMaxUploadSpeed() const;

	bool SaveTimeSlots(const std::string& filepath);
	bool LoadTimeSlots(const std::string& filepath);

private:
	std::vector<TimeSlot> timeSlots;
	uint32_t nextSlotId;

	TimeSlot GetApplicableTimeSlot() const;
};
