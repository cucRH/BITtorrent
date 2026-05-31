#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <chrono>

// Структура для отслеживания пропускной способности
struct BandwidthInfo {
	float currentDownloadSpeed;
	float currentUploadSpeed;
	float averageDownloadSpeed;
	float averageUploadSpeed;
	uint64_t totalBytesDownloaded;
	uint64_t totalBytesUploaded;
	std::chrono::system_clock::time_point startTime;
};

// Класс для управления пропускной способностью
class BandwidthManager {
public:
	BandwidthManager();
	~BandwidthManager();

	void UpdateDownloadSpeed(float bytesPerSec);
	void UpdateUploadSpeed(float bytesPerSec);
	void AddDownloadedBytes(uint64_t bytes);
	void AddUploadedBytes(uint64_t bytes);

	BandwidthInfo GetBandwidthInfo() const;
	float GetAverageDownloadSpeed() const;
	float GetAverageUploadSpeed() const;

	bool LimitDownloadSpeed(float maxSpeedKBps);
	bool LimitUploadSpeed(float maxSpeedKBps);

	void Reset();

private:
	BandwidthInfo info;
	float maxDownloadSpeed;  // KB/s, 0 = unlimited
	float maxUploadSpeed;    // KB/s, 0 = unlimited

	std::vector<float> downloadHistory;
	std::vector<float> uploadHistory;
	const size_t HISTORY_SIZE = 60;  // Last 60 seconds
};

// Статистика по пирам
struct PeerStatistics {
	uint32_t totalPeersConnected;
	uint32_t totalSeeders;
	uint32_t totalLeechers;
	uint32_t activePeers;
	float averagePeerSpeed;
	std::map<std::string, uint64_t> peerBytesTransferred;
};

// Класс для анализа пиров
class PeerAnalytics {
public:
	PeerAnalytics();
	~PeerAnalytics();

	void RecordPeerConnection(const std::string& peerId);
	void RecordPeerDisconnection(const std::string& peerId);
	void RecordBytesTransferred(const std::string& peerId, uint64_t bytes, bool isDownload);

	PeerStatistics GetStatistics() const;
	std::vector<std::string> GetTopPeers(int count) const;
	float GetAveragePeerSpeed() const;

	void Reset();

private:
	std::map<std::string, uint64_t> peerStats;
	uint32_t totalConnections;
	uint32_t activeConnections;
	std::chrono::system_clock::time_point startTime;
};
