#include "TrackerManager.h"
#include <algorithm>

TrackerManager::TrackerManager()
	: seeders(0), leechers(0), interval(1800) {
}

TrackerManager::~TrackerManager() {
}

bool TrackerManager::RequestTrackerInfo(const std::string& trackerUrl, const std::string& infoHash,
									   const std::string& peerId, uint16_t listeningPort,
									   uint64_t downloadedBytes, uint64_t uploadedBytes,
									   uint64_t leftBytes) {
	// Симуляция запроса к трекеру
	seeders = 5;
	leechers = 10;
	interval = 1800;

	// Создаем примеры пиров
	peers.clear();

	for (int i = 0; i < seeders; i++) {
		PeerInfo peer;
		peer.ip = "192.168." + std::to_string(i / 256) + "." + std::to_string(i % 256);
		peer.port = 6881 + i;
		peer.peerId = "-BT0001-" + std::to_string(i);
		peer.isSeeder = true;
		peers.push_back(peer);
	}

	for (int i = 0; i < leechers; i++) {
		PeerInfo peer;
		peer.ip = "10.0." + std::to_string(i / 256) + "." + std::to_string(i % 256);
		peer.port = 6881 + i;
		peer.peerId = "-CL0001-" + std::to_string(i);
		peer.isSeeder = false;
		peers.push_back(peer);
	}

	return true;
}

std::vector<PeerInfo> TrackerManager::GetPeers() const {
	return peers;
}

int TrackerManager::GetSeeders() const {
	return seeders;
}

int TrackerManager::GetLeechers() const {
	return leechers;
}

int TrackerManager::GetInterval() const {
	return interval;
}

PeerManager::PeerManager() {
}

PeerManager::~PeerManager() {
}

void PeerManager::AddPeer(const PeerInfo& peer) {
	peers[peer.peerId] = peer;
}

void PeerManager::RemovePeer(const std::string& peerId) {
	peers.erase(peerId);
}

void PeerManager::UpdatePeerStats(const std::string& peerId, uint64_t uploaded, uint64_t downloaded) {
	if (peers.count(peerId)) {
		peers[peerId].bytesUploaded = uploaded;
		peers[peerId].bytesDownloaded = downloaded;
	}
}

std::vector<PeerInfo> PeerManager::GetActivePeers() const {
	std::vector<PeerInfo> result;
	for (const auto& p : peers) {
		result.push_back(p.second);
	}
	return result;
}

std::vector<PeerInfo> PeerManager::GetSeeders() const {
	std::vector<PeerInfo> result;
	for (const auto& p : peers) {
		if (p.second.isSeeder) {
			result.push_back(p.second);
		}
	}
	return result;
}

int PeerManager::GetActivePeerCount() const {
	return peers.size();
}
