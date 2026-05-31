#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// Информация о пире
struct PeerInfo {
	std::string ip;
	uint16_t port;
	std::string peerId;
	bool isSeeder;
	uint64_t bytesUploaded;
	uint64_t bytesDownloaded;
};

// Класс для взаимодействия с трекерами
class TrackerManager {
public:
	TrackerManager();
	~TrackerManager();

	bool RequestTrackerInfo(const std::string& trackerUrl, const std::string& infoHash,
						   const std::string& peerId, uint16_t listeningPort,
						   uint64_t downloadedBytes, uint64_t uploadedBytes,
						   uint64_t leftBytes);

	std::vector<PeerInfo> GetPeers() const;
	int GetSeeders() const;
	int GetLeechers() const;
	int GetInterval() const;

private:
	std::vector<PeerInfo> peers;
	int seeders;
	int leechers;
	int interval;
};

// Класс для управления пирами
class PeerManager {
public:
	PeerManager();
	~PeerManager();

	void AddPeer(const PeerInfo& peer);
	void RemovePeer(const std::string& peerId);
	void UpdatePeerStats(const std::string& peerId, uint64_t uploaded, uint64_t downloaded);

	std::vector<PeerInfo> GetActivePeers() const;
	std::vector<PeerInfo> GetSeeders() const;
	int GetActivePeerCount() const;

private:
	std::map<std::string, PeerInfo> peers;
};
