#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

// Структура для информации о файле в торренте
struct FileInfo {
	std::string name;
	uint64_t length;
	std::vector<std::string> path;
};

// Структура для информации о куске данных
struct PieceInfo {
	std::string hash;
	uint64_t offset;
	uint32_t length;
	bool downloaded;
};

// Основная структура торрента
struct TorrentInfo {
	std::string name;
	std::string announce;
	std::vector<std::string> announceList;
	std::string comment;
	std::string createdBy;
	uint32_t creationDate;
	uint64_t totalLength;
	uint32_t pieceLength;
	std::vector<PieceInfo> pieces;
	std::vector<FileInfo> files;
	std::string infoHash;
	bool isMultiFile;
	// Путь для загрузки (выбран пользователем)
	std::string downloadPath;
};

// Класс для парсинга и управления торрент файлами
class TorrentParser {
public:
	TorrentParser();
	~TorrentParser();

	bool LoadTorrent(const std::string& filePath, TorrentInfo& torrentInfo);
	bool SaveTorrent(const std::string& filePath, const TorrentInfo& torrentInfo);
	std::string CalculateInfoHash(const TorrentInfo& torrent);

private:
	std::string BencodeString(const std::string& str);
	std::string BencodeInteger(int64_t num);
	std::string DecodeBencodeString(const std::string& data, size_t& pos);
	int64_t DecodeBencodeInteger(const std::string& data, size_t& pos);
	std::map<std::string, std::string> DecodeBencodeDict(const std::string& data, size_t& pos);
};
