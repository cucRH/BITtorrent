#include "DownloadManager.h"
#include "Logger.h"
#include <chrono>

DownloadManager::DownloadManager()
	: state(DownloadState::IDLE),
	isRunning(false),
	isPaused(false),
	downloadedBytes(0),
	uploadedBytes(0),
	downloadSpeed(0.0f),
	uploadSpeed(0.0f),
	peersCount(0),
	seedsCount(0) {
}

DownloadManager::~DownloadManager() {
	StopDownload();
}

bool DownloadManager::StartDownload(const TorrentInfo& torrentInfo) {
	if (state == DownloadState::DOWNLOADING) {
		return false;
	}

	currentTorrent = torrentInfo;
	state = DownloadState::DOWNLOADING;
	isRunning = true;
	isPaused = false;
	downloadedBytes = 0;
	uploadedBytes = 0;

	// Логируем запуск загрузки
	Logger::GetInstance().Info("StartDownload called: name=" + currentTorrent.name + ", size=" + std::to_string(currentTorrent.totalLength), "DownloadManager");

	downloadThread = std::thread(&DownloadManager::DownloadThread, this);
	return true;
}

void DownloadManager::PauseDownload() {
	if (state == DownloadState::DOWNLOADING) {
		isPaused = true;
		state = DownloadState::PAUSED;
	}
}

void DownloadManager::ResumeDownload() {
	if (state == DownloadState::PAUSED) {
		isPaused = false;
		state = DownloadState::DOWNLOADING;
	}
}

void DownloadManager::StopDownload() {
	isRunning = false;
	if (downloadThread.joinable()) {
		downloadThread.join();
	}
	state = DownloadState::IDLE;
}

DownloadState DownloadManager::GetState() const {
	return state;
}

float DownloadManager::GetProgress() const {
	if (currentTorrent.totalLength == 0) {
		return 0.0f;
	}
	float progress = (float)downloadedBytes / (float)currentTorrent.totalLength * 100.0f;
	return progress > 100.0f ? 100.0f : progress;
}

float DownloadManager::GetDownloadSpeed() const {
	return downloadSpeed;
}

float DownloadManager::GetUploadSpeed() const {
	return uploadSpeed;
}

uint64_t DownloadManager::GetDownloadedBytes() const {
	return downloadedBytes;
}

uint64_t DownloadManager::GetUploadedBytes() const {
	return uploadedBytes;
}

std::string DownloadManager::GetStatus() const {
	switch (state) {
	case DownloadState::IDLE:
		return "Idle";
	case DownloadState::DOWNLOADING:
		return "Downloading";
	case DownloadState::PAUSED:
		return "Paused";
	case DownloadState::COMPLETED:
		return "Completed";
	case DownloadState::STATE_ERROR:
		return "Error";
	default:
		return "Unknown";
	}
}

int DownloadManager::GetPeers() const {
	return peersCount;
}

int DownloadManager::GetSeeds() const {
	return seedsCount;
}

void DownloadManager::DownloadThread() {
	auto lastUpdate = std::chrono::high_resolution_clock::now();
	uint64_t lastDownloadedBytes = 0;
	uint64_t lastUploadedBytes = 0;

	while (isRunning) {
		if (!isPaused) {
			// Имитация скачивания
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			// Проверяем, что totalLength > 0
			if (currentTorrent.totalLength > 0 && downloadedBytes < currentTorrent.totalLength) {
				downloadedBytes += 1024 * 10; // 10 KB за итерацию
				if (downloadedBytes > currentTorrent.totalLength) {
					downloadedBytes = currentTorrent.totalLength;
				}
			}

			// Обновляем скорость каждую секунду
			auto now = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();

			if (elapsed >= 1000) {
				downloadSpeed = (float)(downloadedBytes - lastDownloadedBytes) / (elapsed / 1000.0f);
				uploadSpeed = (float)(uploadedBytes - lastUploadedBytes) / (elapsed / 1000.0f);

				lastDownloadedBytes = downloadedBytes;
				lastUploadedBytes = uploadedBytes;
				lastUpdate = now;
			}

			// Проверяем завершение - только если totalLength > 0
			if (currentTorrent.totalLength > 0 && downloadedBytes >= currentTorrent.totalLength) {
				state = DownloadState::COMPLETED;
				isRunning = false;
			}
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}

void DownloadManager::UpdateStats() {
	peersCount = 15;
	seedsCount = 3;
}
