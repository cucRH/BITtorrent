#include "ScheduleManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

ScheduleManager::ScheduleManager()
	: nextScheduleId(1) {
}

ScheduleManager::~ScheduleManager() {
}

bool ScheduleManager::AddSchedule(const DownloadSchedule& schedule) {
	DownloadSchedule newSchedule = schedule;
	newSchedule.id = nextScheduleId++;
	newSchedule.lastExecuted = 0;
	schedules.push_back(newSchedule);
	return true;
}

bool ScheduleManager::RemoveSchedule(uint32_t scheduleId) {
	auto it = std::find_if(schedules.begin(), schedules.end(),
		[scheduleId](const DownloadSchedule& s) { return s.id == scheduleId; });

	if (it != schedules.end()) {
		schedules.erase(it);
		return true;
	}
	return false;
}

bool ScheduleManager::UpdateSchedule(const DownloadSchedule& schedule) {
	for (auto& s : schedules) {
		if (s.id == schedule.id) {
			s = schedule;
			return true;
		}
	}
	return false;
}

DownloadSchedule ScheduleManager::GetSchedule(uint32_t scheduleId) const {
	for (const auto& s : schedules) {
		if (s.id == scheduleId) {
			return s;
		}
	}
	return DownloadSchedule();
}

std::vector<DownloadSchedule> ScheduleManager::GetAllSchedules() const {
	return schedules;
}

std::vector<DownloadSchedule> ScheduleManager::GetEnabledSchedules() const {
	std::vector<DownloadSchedule> enabled;
	for (const auto& s : schedules) {
		if (s.enabled) {
			enabled.push_back(s);
		}
	}
	return enabled;
}

std::vector<DownloadSchedule> ScheduleManager::GetDueSchedules() {
	std::vector<DownloadSchedule> dueSchedules;
	for (auto& schedule : schedules) {
		if (schedule.enabled && IsScheduleDue(schedule)) {
			dueSchedules.push_back(schedule);
		}
	}
	return dueSchedules;
}

bool ScheduleManager::ExecuteSchedule(uint32_t scheduleId) {
	for (auto& schedule : schedules) {
		if (schedule.id == scheduleId) {
			schedule.lastExecuted = std::time(nullptr);
			UpdateScheduleNextTime(schedule);
			return true;
		}
	}
	return false;
}

bool ScheduleManager::SaveSchedules(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& schedule : schedules) {
		file << schedule.id << "|"
			 << schedule.torrentPath << "|"
			 << schedule.torrentName << "|"
			 << schedule.enabled << "|"
			 << (int)schedule.repeatType << "|"
			 << schedule.lastExecuted << "\n";
	}

	file.close();
	return true;
}

bool ScheduleManager::LoadSchedules(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;

		DownloadSchedule schedule = {};

		std::getline(ss, token, '|');
		schedule.id = std::stoul(token);

		std::getline(ss, schedule.torrentPath, '|');
		std::getline(ss, schedule.torrentName, '|');

		std::getline(ss, token, '|');
		schedule.enabled = (token == "1");

		std::getline(ss, token, '|');
		schedule.repeatType = (ScheduleRepeat)std::stoi(token);

		std::getline(ss, token, '|');
		schedule.lastExecuted = std::stoll(token);

		schedules.push_back(schedule);
	}

	file.close();
	return true;
}

bool ScheduleManager::EnableSchedule(uint32_t scheduleId) {
	for (auto& schedule : schedules) {
		if (schedule.id == scheduleId) {
			schedule.enabled = true;
			return true;
		}
	}
	return false;
}

bool ScheduleManager::DisableSchedule(uint32_t scheduleId) {
	for (auto& schedule : schedules) {
		if (schedule.id == scheduleId) {
			schedule.enabled = false;
			return true;
		}
	}
	return false;
}

void ScheduleManager::ClearSchedules() {
	schedules.clear();
}

bool ScheduleManager::IsScheduleDue(const DownloadSchedule& schedule) {
	std::time_t now = std::time(nullptr);
	struct tm currentTimeInfo;
	localtime_s(&currentTimeInfo, &now);
	struct tm* currentTime = &currentTimeInfo;

	if (currentTime->tm_hour == schedule.scheduledTime.tm_hour &&
		currentTime->tm_min == schedule.scheduledTime.tm_min) {

		if (schedule.repeatType == ScheduleRepeat::NEVER) {
			return schedule.lastExecuted == 0;
		} else if (schedule.repeatType == ScheduleRepeat::DAILY) {
			return true;
		} else if (schedule.repeatType == ScheduleRepeat::WEEKLY) {
			return currentTime->tm_wday == schedule.scheduledTime.tm_wday;
		}
	}

	return false;
}

void ScheduleManager::UpdateScheduleNextTime(DownloadSchedule& schedule) {
	// Логика обновления времени следующего выполнения
}

// TimeBasedBandwidthScheduler implementation
TimeBasedBandwidthScheduler::TimeBasedBandwidthScheduler()
	: nextSlotId(1) {
}

TimeBasedBandwidthScheduler::~TimeBasedBandwidthScheduler() {
}

bool TimeBasedBandwidthScheduler::AddTimeSlot(const TimeSlot& slot) {
	TimeSlot newSlot = slot;
	newSlot.id = nextSlotId++;
	timeSlots.push_back(newSlot);
	return true;
}

bool TimeBasedBandwidthScheduler::RemoveTimeSlot(uint32_t slotId) {
	auto it = std::find_if(timeSlots.begin(), timeSlots.end(),
		[slotId](const TimeSlot& s) { return s.id == slotId; });

	if (it != timeSlots.end()) {
		timeSlots.erase(it);
		return true;
	}
	return false;
}

TimeBasedBandwidthScheduler::TimeSlot TimeBasedBandwidthScheduler::GetCurrentTimeSlot() const {
	return GetApplicableTimeSlot();
}

std::vector<TimeBasedBandwidthScheduler::TimeSlot> TimeBasedBandwidthScheduler::GetAllTimeSlots() const {
	return timeSlots;
}

float TimeBasedBandwidthScheduler::GetCurrentMaxDownloadSpeed() const {
	TimeSlot slot = GetApplicableTimeSlot();
	return slot.maxDownloadSpeedKBps;
}

float TimeBasedBandwidthScheduler::GetCurrentMaxUploadSpeed() const {
	TimeSlot slot = GetApplicableTimeSlot();
	return slot.maxUploadSpeedKBps;
}

bool TimeBasedBandwidthScheduler::SaveTimeSlots(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& slot : timeSlots) {
		file << slot.id << "|"
			 << (int)slot.hourStart << "|"
			 << (int)slot.hourEnd << "|"
			 << slot.maxDownloadSpeedKBps << "|"
			 << slot.maxUploadSpeedKBps << "|"
			 << (int)slot.dayOfWeek << "\n";
	}

	file.close();
	return true;
}

bool TimeBasedBandwidthScheduler::LoadTimeSlots(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;

		TimeSlot slot = {};

		std::getline(ss, token, '|');
		slot.id = std::stoul(token);

		std::getline(ss, token, '|');
		slot.hourStart = std::stoi(token);

		std::getline(ss, token, '|');
		slot.hourEnd = std::stoi(token);

		std::getline(ss, token, '|');
		slot.maxDownloadSpeedKBps = std::stof(token);

		std::getline(ss, token, '|');
		slot.maxUploadSpeedKBps = std::stof(token);

		std::getline(ss, token, '|');
		slot.dayOfWeek = std::stoi(token);

		timeSlots.push_back(slot);
	}

	file.close();
	return true;
}

TimeBasedBandwidthScheduler::TimeSlot TimeBasedBandwidthScheduler::GetApplicableTimeSlot() const {
	std::time_t now = std::time(nullptr);
	struct tm currentTimeInfo;
	localtime_s(&currentTimeInfo, &now);
	struct tm* currentTime = &currentTimeInfo;

	for (const auto& slot : timeSlots) {
		if (slot.dayOfWeek == currentTime->tm_wday &&
			currentTime->tm_hour >= slot.hourStart &&
			currentTime->tm_hour < slot.hourEnd) {
			return slot;
		}
	}

	// Default unlimited
	TimeSlot defaultSlot = {};
	defaultSlot.maxDownloadSpeedKBps = 0;
	defaultSlot.maxUploadSpeedKBps = 0;
	return defaultSlot;
}
