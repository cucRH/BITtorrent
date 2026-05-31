#pragma once

#include "TorrentCore.h"
#include <thread>
#include <mutex>
#include <atomic>

enum class DownloadState {
	IDLE,
	DOWNLOADING,
	PAUSED,
	COMPLETED,
	STATE_ERROR
};

class DownloadManager {
public:
	DownloadManager();
	~DownloadManager();

	bool StartDownload(const TorrentInfo& torrentInfo);
	void PauseDownload();
	void ResumeDownload();
	void StopDownload();

	DownloadState GetState() const;
	float GetProgress() const;
	float GetDownloadSpeed() const;
	float GetUploadSpeed() const;
	uint64_t GetDownloadedBytes() const;
	uint64_t GetUploadedBytes() const;
	std::string GetStatus() const;
	int GetPeers() const;
	int GetSeeds() const;

private:
	void DownloadThread();
	void UpdateStats();

	TorrentInfo currentTorrent;
	DownloadState state;
	std::thread downloadThread;
	std::mutex stateMutex;
	std::atomic<bool> isRunning;
	std::atomic<bool> isPaused;

	uint64_t downloadedBytes;
	uint64_t uploadedBytes;
	float downloadSpeed;
	float uploadSpeed;
	int peersCount;
	int seedsCount;
};
