#include "BitTorrentClient.h"
#include <algorithm>

BitTorrentClient::BitTorrentClient()
	: cacheManager("./BitTorrent_Data/cache"),
	initialized(false) {
}

BitTorrentClient::~BitTorrentClient() {
	Shutdown();
}

bool BitTorrentClient::Initialize(const std::string& configPathArg) {
	configPath = configPathArg;

	try {
		// Инициализируем хранилище
		storageManager.Initialize("./BitTorrent_Data");

		// Загружаем настройки
		settingsManager.LoadSettings(configPath);

		// Загружаем статистику
		statisticsManager.LoadStatistics("./BitTorrent_Data/statistics.dat");

		Logger::GetInstance().Info("BitTorrent Client initialized successfully", "Client");
		initialized = true;
		return true;
	} catch (...) {
		Logger::GetInstance().Error("Failed to initialize BitTorrent Client", "Client");
		return false;
	}
}

bool BitTorrentClient::Shutdown() {
	try {
		// Останавливаем все активные загрузки
		for (auto& torrent : activeTorrents) {
			torrent.second->StopDownload();
		}
		activeTorrents.clear();

		// Сохраняем настройки и статистику
		settingsManager.SaveSettings(configPath);
		statisticsManager.SaveStatistics("./BitTorrent_Data/statistics.dat");

		Logger::GetInstance().Info("BitTorrent Client shutdown successfully", "Client");
		return true;
	} catch (...) {
		return false;
	}
}

bool BitTorrentClient::LoadTorrent(const std::string& torrentPath, TorrentInfo& outTorrent) {
	TorrentParser parser;
	if (parser.LoadTorrent(torrentPath, outTorrent)) {
		Logger::GetInstance().Info("Torrent loaded: " + outTorrent.name, "Client");
		return true;
	}

	Logger::GetInstance().Error("Failed to load torrent: " + torrentPath, "Client");
	return false;
}

bool BitTorrentClient::StartDownload(const TorrentInfo& torrent) {
	try {
		auto existing = FindDownloadManager(torrent.name);
		if (existing) {
			existing->StartDownload(torrent);
			return true;
		}

		auto manager = std::make_unique<DownloadManager>();
		manager->StartDownload(torrent);
		activeTorrents.push_back(std::make_pair(torrent.name, std::move(manager)));

		Logger::GetInstance().Info("Download started: " + torrent.name, "Client");
		return true;
	} catch (...) {
		Logger::GetInstance().Error("Failed to start download", "Client");
		return false;
	}
}

bool BitTorrentClient::PauseDownload(const std::string& torrentName) {
	auto manager = FindDownloadManager(torrentName);
	if (manager) {
		manager->PauseDownload();
		Logger::GetInstance().Info("Download paused: " + torrentName, "Client");
		return true;
	}
	return false;
}

bool BitTorrentClient::ResumeDownload(const std::string& torrentName) {
	auto manager = FindDownloadManager(torrentName);
	if (manager) {
		manager->ResumeDownload();
		Logger::GetInstance().Info("Download resumed: " + torrentName, "Client");
		return true;
	}
	return false;
}

bool BitTorrentClient::StopDownload(const std::string& torrentName) {
	auto manager = FindDownloadManager(torrentName);
	if (manager) {
		manager->StopDownload();
		Logger::GetInstance().Info("Download stopped: " + torrentName, "Client");
		return true;
	}
	return false;
}

bool BitTorrentClient::RemoveTorrent(const std::string& torrentName) {
	auto it = std::find_if(
		activeTorrents.begin(),
		activeTorrents.end(),
		[&torrentName](const std::pair<std::string, std::unique_ptr<DownloadManager>>& p) {
			return p.first == torrentName;
		}
	);

	if (it != activeTorrents.end()) {
		it->second->StopDownload();
		activeTorrents.erase(it);
		Logger::GetInstance().Info("Torrent removed: " + torrentName, "Client");
		return true;
	}
	return false;
}

std::vector<TorrentInfo> BitTorrentClient::GetActiveTorrents() const {
	std::vector<TorrentInfo> torrents;
	// Здесь можно добавить логику получения информации об активных торрентах
	return torrents;
}

std::vector<DownloadHistoryEntry> BitTorrentClient::GetDownloadHistory() const {
	return storageManager.LoadDownloadHistory();
}

AppStatistics BitTorrentClient::GetStatistics() const {
	return statisticsManager.GetStatistics();
}

void BitTorrentClient::ResetStatistics() {
	statisticsManager.ResetSessionStats();
}

bool BitTorrentClient::LoadSettings() {
	return settingsManager.LoadSettings(configPath);
}

bool BitTorrentClient::SaveSettings() {
	return settingsManager.SaveSettings(configPath);
}

AppSettings BitTorrentClient::GetSettings() const {
	return settingsManager.GetSettings();
}

void BitTorrentClient::UpdateSettings(const AppSettings& newSettings) {
	settingsManager.SetSettings(newSettings);
}

DownloadManager* BitTorrentClient::FindDownloadManager(const std::string& torrentName) {
	auto it = std::find_if(
		activeTorrents.begin(),
		activeTorrents.end(),
		[&torrentName](const std::pair<std::string, std::unique_ptr<DownloadManager>>& p) {
			return p.first == torrentName;
		}
	);

	if (it != activeTorrents.end()) {
		return it->second.get();
	}
	return nullptr;
}
