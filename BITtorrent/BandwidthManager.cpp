#include "BandwidthManager.h"
#include <algorithm>
#include <numeric>

BandwidthManager::BandwidthManager()
	: maxDownloadSpeed(0),
	maxUploadSpeed(0) {
	info.currentDownloadSpeed = 0;
	info.currentUploadSpeed = 0;
	info.averageDownloadSpeed = 0;
	info.averageUploadSpeed = 0;
	info.totalBytesDownloaded = 0;
	info.totalBytesUploaded = 0;
	info.startTime = std::chrono::system_clock::now();
}

BandwidthManager::~BandwidthManager() {
}

void BandwidthManager::UpdateDownloadSpeed(float bytesPerSec) {
	info.currentDownloadSpeed = bytesPerSec;

	if (downloadHistory.size() >= HISTORY_SIZE) {
		downloadHistory.erase(downloadHistory.begin());
	}
	downloadHistory.push_back(bytesPerSec);

	if (!downloadHistory.empty()) {
		info.averageDownloadSpeed = std::accumulate(
			downloadHistory.begin(),
			downloadHistory.end(),
			0.0f
		) / downloadHistory.size();
	}
}

void BandwidthManager::UpdateUploadSpeed(float bytesPerSec) {
	info.currentUploadSpeed = bytesPerSec;

	if (uploadHistory.size() >= HISTORY_SIZE) {
		uploadHistory.erase(uploadHistory.begin());
	}
	uploadHistory.push_back(bytesPerSec);

	if (!uploadHistory.empty()) {
		info.averageUploadSpeed = std::accumulate(
			uploadHistory.begin(),
			uploadHistory.end(),
			0.0f
		) / uploadHistory.size();
	}
}

void BandwidthManager::AddDownloadedBytes(uint64_t bytes) {
	info.totalBytesDownloaded += bytes;
}

void BandwidthManager::AddUploadedBytes(uint64_t bytes) {
	info.totalBytesUploaded += bytes;
}

BandwidthInfo BandwidthManager::GetBandwidthInfo() const {
	return info;
}

float BandwidthManager::GetAverageDownloadSpeed() const {
	return info.averageDownloadSpeed;
}

float BandwidthManager::GetAverageUploadSpeed() const {
	return info.averageUploadSpeed;
}

bool BandwidthManager::LimitDownloadSpeed(float maxSpeedKBps) {
	maxDownloadSpeed = maxSpeedKBps;
	return true;
}

bool BandwidthManager::LimitUploadSpeed(float maxSpeedKBps) {
	maxUploadSpeed = maxSpeedKBps;
	return true;
}

void BandwidthManager::Reset() {
	info.totalBytesDownloaded = 0;
	info.totalBytesUploaded = 0;
	info.currentDownloadSpeed = 0;
	info.currentUploadSpeed = 0;
	info.averageDownloadSpeed = 0;
	info.averageUploadSpeed = 0;
	downloadHistory.clear();
	uploadHistory.clear();
	info.startTime = std::chrono::system_clock::now();
}

PeerAnalytics::PeerAnalytics()
	: totalConnections(0),
	activeConnections(0) {
	startTime = std::chrono::system_clock::now();
}

PeerAnalytics::~PeerAnalytics() {
}

void PeerAnalytics::RecordPeerConnection(const std::string& peerId) {
	totalConnections++;
	activeConnections++;
	if (peerStats.find(peerId) == peerStats.end()) {
		peerStats[peerId] = 0;
	}
}

void PeerAnalytics::RecordPeerDisconnection(const std::string& peerId) {
	if (activeConnections > 0) {
		activeConnections--;
	}
}

void PeerAnalytics::RecordBytesTransferred(const std::string& peerId, uint64_t bytes, bool isDownload) {
	peerStats[peerId] += bytes;
}

PeerStatistics PeerAnalytics::GetStatistics() const {
	PeerStatistics stats;
	stats.totalPeersConnected = totalConnections;
	stats.activePeers = activeConnections;
	stats.totalSeeders = activeConnections / 2;  // Примерный расчет
	stats.totalLeechers = activeConnections - stats.totalSeeders;
	stats.peerBytesTransferred = peerStats;

	if (!peerStats.empty()) {
		uint64_t totalBytes = 0;
		for (const auto& p : peerStats) {
			totalBytes += p.second;
		}
		stats.averagePeerSpeed = static_cast<float>(totalBytes) / (totalConnections > 0 ? totalConnections : 1);
	}

	return stats;
}

std::vector<std::string> PeerAnalytics::GetTopPeers(int count) const {
	std::vector<std::pair<std::string, uint64_t>> peerList(peerStats.begin(), peerStats.end());

	std::sort(peerList.begin(), peerList.end(),
		[](const auto& a, const auto& b) {
			return a.second > b.second;
		}
	);

	std::vector<std::string> topPeers;
	for (int i = 0; i < count && i < peerList.size(); i++) {
		topPeers.push_back(peerList[i].first);
	}

	return topPeers;
}

float PeerAnalytics::GetAveragePeerSpeed() const {
	return GetStatistics().averagePeerSpeed;
}

void PeerAnalytics::Reset() {
	peerStats.clear();
	totalConnections = 0;
	activeConnections = 0;
	startTime = std::chrono::system_clock::now();
}
