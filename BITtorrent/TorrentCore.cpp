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
	Logger::GetInstance().Debug("File content (first 200 chars): " + data.substr(0, data.size() > 200 ? 200 : data.size()), "TorrentParser");

	try {
		// Устанавливаем значения по умолчанию
		torrentInfo.name = "Unknown";
		torrentInfo.announce = "";
		torrentInfo.comment = "";
		torrentInfo.createdBy = "";
		torrentInfo.totalLength = 0;
		torrentInfo.pieceLength = 0;
		torrentInfo.isMultiFile = false;

		size_t pos = 0;
		if (data.empty() || data[0] != 'd') {
			Logger::GetInstance().Log(LogLevel::LOG_ERROR, "Invalid torrent format: doesn't start with 'd'", "TorrentParser");
			return false;
		}

		auto dict = DecodeBencodeDict(data, pos);

		Logger::GetInstance().Debug("Bencoding parsed, dict size: " + std::to_string(dict.size()), "TorrentParser");

		// Логируем все ключи в словаре
		for (const auto& pair : dict) {
			Logger::GetInstance().Debug("Dict key: '" + pair.first + "', value size: " + std::to_string(pair.second.size()), "TorrentParser");
		}

		// Получаем основные поля
		if (dict.count("announce") && !dict["announce"].empty()) {
			torrentInfo.announce = dict["announce"];
			Logger::GetInstance().Debug("Found announce: " + torrentInfo.announce, "TorrentParser");
		}

		if (dict.count("comment") && !dict["comment"].empty()) {
			torrentInfo.comment = dict["comment"];
		}

		if (dict.count("created by") && !dict["created by"].empty()) {
			torrentInfo.createdBy = dict["created by"];
		}

		// Парсинг info словаря
		if (dict.count("info")) {
			auto infoData = dict["info"];
			Logger::GetInstance().Debug("Info data found, size: " + std::to_string(infoData.size()), "TorrentParser");

			if (!infoData.empty()) {
				size_t previewLen = infoData.size() > 100 ? 100 : infoData.size();
				Logger::GetInstance().Debug("Info data content: " + infoData.substr(0, previewLen), "TorrentParser");

				try {
					size_t infoPos = 0;
					auto infoDict = DecodeBencodeDict(infoData, infoPos);

					Logger::GetInstance().Debug("Info dict parsed, size: " + std::to_string(infoDict.size()), "TorrentParser");

					// Логируем все ключи в info словаре
					for (const auto& pair : infoDict) {
						Logger::GetInstance().Debug("  Info key: '" + pair.first + "', value size: " + std::to_string(pair.second.size()), "TorrentParser");
					}

					// Извлекаем имя
					if (infoDict.count("name") && !infoDict["name"].empty()) {
						torrentInfo.name = infoDict["name"];
						Logger::GetInstance().Debug("Found name: " + torrentInfo.name, "TorrentParser");
					} else {
						// Попробуем альтернативные ключи (utf-8 и др.)
						if (infoDict.count("name.utf-8") && !infoDict["name.utf-8"].empty()) {
							torrentInfo.name = infoDict["name.utf-8"];
							Logger::GetInstance().Debug("Found name (name.utf-8): " + torrentInfo.name, "TorrentParser");
						} else if (infoDict.count("name.utf8") && !infoDict["name.utf8"].empty()) {
							torrentInfo.name = infoDict["name.utf8"];
							Logger::GetInstance().Debug("Found name (name.utf8): " + torrentInfo.name, "TorrentParser");
						}
					}

					// Извлекаем длину куска
					if (infoDict.count("piece length") && !infoDict["piece length"].empty()) {
						try {
							torrentInfo.pieceLength = std::stoul(infoDict["piece length"]);
							Logger::GetInstance().Debug("Found piece length: " + std::to_string(torrentInfo.pieceLength), "TorrentParser");
						} catch (...) {
							Logger::GetInstance().Debug("Failed to parse piece length: " + infoDict["piece length"], "TorrentParser");
						}
					}

					// Определяем, это мульти-файл или одиночный файл
					if (infoDict.count("files")) {
						// Multi-file: суммируем все длины файлов в списке
						torrentInfo.isMultiFile = true;
						torrentInfo.totalLength = 0;
						Logger::GetInstance().Debug("Multi-file torrent detected, parsing files list", "TorrentParser");
						try {
							std::string filesData = infoDict["files"];
							size_t filesPos = 0;
							// Ожидаем, что filesData начинается с 'l'
							if (filesPos < filesData.size() && filesData[filesPos] == 'l') {
								filesPos++; // skip 'l'
								while (filesPos < filesData.size() && filesData[filesPos] != 'e') {
									if (filesData[filesPos] == 'd') {
										// Decode each file dict using existing DecodeBencodeDict (works on substring)
										size_t dictPos = filesPos;
										auto fileDict = DecodeBencodeDict(filesData, dictPos);
										filesPos = dictPos; // advance filesPos
										// В fileDict может быть ключ "length" как число или строкой
										if (fileDict.count("length") && !fileDict["length"].empty()) {
											try {
												uint64_t len = std::stoull(fileDict["length"]);
												torrentInfo.totalLength += len;
												Logger::GetInstance().Debug("Found file length: " + std::to_string(len), "TorrentParser");
											} catch (...) {
												Logger::GetInstance().Debug("Failed to parse file length: " + fileDict["length"], "TorrentParser");
											}
										}
									} else {
										// Если встречаем что-то неожиданное, двигаемся дальше безопасно
										filesPos++;
									}
								}
							}
							Logger::GetInstance().Debug("Total multi-file size: " + std::to_string(torrentInfo.totalLength), "TorrentParser");
						} catch (const std::exception& e) {
							Logger::GetInstance().Log(LogLevel::LOG_ERROR, std::string("Failed to parse files list: ") + e.what(), "TorrentParser");
						}
					} else if (infoDict.count("length")) {
						torrentInfo.isMultiFile = false;
						try {
							torrentInfo.totalLength = std::stoull(infoDict["length"]);
							Logger::GetInstance().Debug("Single-file torrent, length: " + std::to_string(torrentInfo.totalLength), "TorrentParser");
						} catch (...) {
							Logger::GetInstance().Debug("Failed to parse length: " + infoDict["length"], "TorrentParser");
						}
					}
				} catch (const std::exception& e) {
					Logger::GetInstance().Log(LogLevel::LOG_ERROR, std::string("Failed to parse info dict: ") + e.what(), "TorrentParser");
				}
			}
		}

		// Если имя не найдено в метаданных, используем имя файла .torrent
		if (torrentInfo.name.empty() || torrentInfo.name == "Unknown") {
			size_t p = filePath.find_last_of("\\/");
			std::string fname = (p == std::string::npos) ? filePath : filePath.substr(p + 1);
			size_t dot = fname.find_last_of('.');
			if (dot != std::string::npos) fname = fname.substr(0, dot);
			torrentInfo.name = fname;
			Logger::GetInstance().Debug("Name not present in metadata, using filename: " + torrentInfo.name, "TorrentParser");
		}

		Logger::GetInstance().Info("Torrent loaded: name=" + torrentInfo.name + ", size=" + std::to_string(torrentInfo.totalLength), "TorrentParser");
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
