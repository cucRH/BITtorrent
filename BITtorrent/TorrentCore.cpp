#include "TorrentCore.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "Logger.h"

TorrentParser::TorrentParser() {
}

TorrentParser::~TorrentParser() {
}

bool TorrentParser::LoadTorrent(const std::string& filePath, TorrentInfo& torrentInfo) {
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		Logger::GetInstance().Log(LogLevel::LOG_ERROR, "Cannot open file: " + filePath, "TorrentParser");
		return false;
	}

	// Ограничим размер до 10 МБ
	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	if (fileSize > 10 * 1024 * 1024) {
		Logger::GetInstance().Log(LogLevel::LOG_ERROR, "File too large: " + filePath, "TorrentParser");
		return false;
	}
	file.seekg(0, std::ios::beg);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string data = buffer.str();
	file.close();

	Logger::GetInstance().Debug("File read, size: " + std::to_string(data.size()), "TorrentParser");

	try {
		size_t pos = 0;
		auto dict = DecodeBencodeDict(data, pos);

		Logger::GetInstance().Debug("Bencoding parsed, dict size: " + std::to_string(dict.size()), "TorrentParser");

		// Логируем все ключи в словаре
		for (const auto& pair : dict) {
			Logger::GetInstance().Debug("Dict key: " + pair.first + ", value size: " + std::to_string(pair.second.size()), "TorrentParser");
		}

		torrentInfo.announce = dict.count("announce") ? dict["announce"] : "";
		torrentInfo.comment = dict.count("comment") ? dict["comment"] : "";
		torrentInfo.createdBy = dict.count("created by") ? dict["created by"] : "";

		Logger::GetInstance().Debug("Announce: " + torrentInfo.announce, "TorrentParser");

		// Парсинг info словаря
		if (dict.count("info")) {
			auto infoData = dict["info"];
			Logger::GetInstance().Debug("Info data found, size: " + std::to_string(infoData.size()), "TorrentParser");

			size_t previewLen = infoData.size() > 100 ? 100 : infoData.size();
			Logger::GetInstance().Debug("Info data (first 100 chars): " + infoData.substr(0, previewLen), "TorrentParser");

			size_t infoPos = 0;
			auto infoDict = DecodeBencodeDict(infoData, infoPos);

			Logger::GetInstance().Debug("Info dict parsed, size: " + std::to_string(infoDict.size()), "TorrentParser");

			// Логируем все ключи в info словаре
			for (const auto& pair : infoDict) {
				Logger::GetInstance().Debug("Info key: " + pair.first + ", value size: " + std::to_string(pair.second.size()), "TorrentParser");
			}

			torrentInfo.name = infoDict.count("name") ? infoDict["name"] : "Unknown";
			torrentInfo.pieceLength = infoDict.count("piece length") ? std::stoul(infoDict["piece length"]) : 0;

			Logger::GetInstance().Debug("Name: " + torrentInfo.name + ", Piece length: " + std::to_string(torrentInfo.pieceLength), "TorrentParser");

			// Определяем, это мульти-файл или одиночный файл
			if (infoDict.count("files")) {
				torrentInfo.isMultiFile = true;
				torrentInfo.totalLength = 0;
				Logger::GetInstance().Debug("Multi-file torrent detected", "TorrentParser");
			} else {
				torrentInfo.isMultiFile = false;
				torrentInfo.totalLength = infoDict.count("length") ? std::stoull(infoDict["length"]) : 0;
				Logger::GetInstance().Debug("Single-file torrent, length: " + std::to_string(torrentInfo.totalLength), "TorrentParser");
			}
		}

		Logger::GetInstance().Info("Torrent loaded successfully: " + torrentInfo.name, "TorrentParser");
		return true;
	} catch (const std::exception& e) {
		Logger::GetInstance().Log(LogLevel::LOG_ERROR, std::string("Exception: ") + e.what(), "TorrentParser");
		return false;
	} catch (...) {
		Logger::GetInstance().Log(LogLevel::LOG_ERROR, "Unknown exception", "TorrentParser");
		return false;
	}
}

bool TorrentParser::SaveTorrent(const std::string& filePath, const TorrentInfo& torrentInfo) {
	return true;
}

std::string TorrentParser::CalculateInfoHash(const TorrentInfo& torrent) {
	return "";
}

std::string TorrentParser::BencodeString(const std::string& str) {
	return std::to_string(str.length()) + ":" + str;
}

std::string TorrentParser::BencodeInteger(int64_t num) {
	return "i" + std::to_string(num) + "e";
}

std::string TorrentParser::DecodeBencodeString(const std::string& data, size_t& pos) {
	size_t colonPos = data.find(':', pos);
	if (colonPos == std::string::npos) {
		throw std::runtime_error("Invalid bencode string");
	}

	size_t length = std::stoul(data.substr(pos, colonPos - pos));
	pos = colonPos + 1;
	std::string result = data.substr(pos, length);
	pos += length;
	return result;
}

int64_t TorrentParser::DecodeBencodeInteger(const std::string& data, size_t& pos) {
	if (data[pos] != 'i') {
		throw std::runtime_error("Invalid bencode integer");
	}
	pos++;

	size_t ePos = data.find('e', pos);
	if (ePos == std::string::npos) {
		throw std::runtime_error("Invalid bencode integer");
	}

	int64_t result = std::stoll(data.substr(pos, ePos - pos));
	pos = ePos + 1;
	return result;
}

std::map<std::string, std::string> TorrentParser::DecodeBencodeDict(const std::string& data, size_t& pos) {
	std::map<std::string, std::string> result;

	if (pos >= data.length() || data[pos] != 'd') {
		throw std::runtime_error("Invalid bencode dict");
	}
	pos++;

	while (pos < data.length() && data[pos] != 'e') {
		std::string key = DecodeBencodeString(data, pos);
		std::string value;

		if (pos >= data.length()) {
			throw std::runtime_error("Unexpected end of data");
		}

		if (data[pos] == 'i') {
			// Integer
			value = std::to_string(DecodeBencodeInteger(data, pos));
		} else if (std::isdigit(data[pos])) {
			// String или nested dict/list
			size_t colonPos = data.find(':', pos);
			if (colonPos == std::string::npos) {
				throw std::runtime_error("Invalid bencode string");
			}
			size_t length = std::stoul(data.substr(pos, colonPos - pos));
			pos = colonPos + 1;

			// Извлекаем raw данные (может быть строка или сериализованный dict)
			if (pos + length <= data.length()) {
				value = data.substr(pos, length);
				pos += length;
			} else {
				throw std::runtime_error("Invalid bencode string length");
			}
		} else if (data[pos] == 'd') {
			// Nested dict - сохраняем как raw bytes для дальнейшей обработки
			size_t startPos = pos;
			pos++; // skip 'd'
			int depth = 1;
			while (pos < data.length() && depth > 0) {
				if (data[pos] == 'd') {
					depth++;
				} else if (data[pos] == 'e') {
					depth--;
				} else if (std::isdigit(data[pos])) {
					// Skip string
					size_t colonPos = data.find(':', pos);
					if (colonPos != std::string::npos) {
						size_t len = std::stoul(data.substr(pos, colonPos - pos));
						pos = colonPos + 1 + len;
					} else {
						throw std::runtime_error("Invalid bencode string");
					}
					continue;
				}
				pos++;
			}
			value = data.substr(startPos, pos - startPos);
		} else if (data[pos] == 'l') {
			// List - сохраняем как raw bytes
			size_t startPos = pos;
			pos++; // skip 'l'
			int depth = 1;
			while (pos < data.length() && depth > 0) {
				if (data[pos] == 'l') {
					depth++;
				} else if (data[pos] == 'e') {
					depth--;
				} else if (std::isdigit(data[pos])) {
					// Skip string
					size_t colonPos = data.find(':', pos);
					if (colonPos != std::string::npos) {
						size_t len = std::stoul(data.substr(pos, colonPos - pos));
						pos = colonPos + 1 + len;
					} else {
						throw std::runtime_error("Invalid bencode string");
					}
					continue;
				}
				pos++;
			}
			value = data.substr(startPos, pos - startPos);
		}

		result[key] = value;
	}

	if (pos < data.length() && data[pos] == 'e') {
		pos++;
	}

	return result;
}
