#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// Статус куска данных
enum class PieceStatus {
	MISSING,      // Отсутствует
	DOWNLOADING,  // Загружается
	DOWNLOADED,   // Загружен
	VERIFIED      // Проверен
};

// Структура для отслеживания куска
struct PieceData {
	uint32_t index;
	PieceStatus status;
	std::vector<uint8_t> data;
	std::string hash;
	uint64_t offset;
	uint32_t length;
};

// Класс для управления кешем и кусками файлов
class CacheManager {
public:
	CacheManager(const std::string& cachePath);
	~CacheManager();

	bool AddPiece(const PieceData& piece);
	PieceData GetPiece(uint32_t index);
	bool VerifyPiece(uint32_t index, const std::string& expectedHash);

	std::vector<uint32_t> GetDownloadedPieces() const;
	std::vector<uint32_t> GetMissingPieces() const;

	float GetCacheUsagePercent() const;
	uint64_t GetTotalCacheSize() const;

	void ClearCache();
	bool LoadFromDisk(const std::string& filename);
	bool SaveToDisk(const std::string& filename);

private:
	std::string cacheDirectory;
	std::map<uint32_t, PieceData> pieces;
	uint64_t maxCacheSize;
	uint64_t currentCacheSize;

	std::string GetPieceFilePath(uint32_t index);
	bool WritePieceToDisk(const PieceData& piece);
	bool ReadPieceFromDisk(uint32_t index, PieceData& piece);
};

// Структура для отслеживания блоков внутри куска
struct BlockInfo {
	uint32_t offset;
	uint32_t length;
	bool downloaded;
};

// Класс для управления загрузкой по блокам
class BlockManager {
public:
	BlockManager(uint32_t pieceSize);
	~BlockManager();

	std::vector<BlockInfo> GetMissingBlocks(uint32_t pieceIndex);
	void MarkBlockDownloaded(uint32_t pieceIndex, uint32_t blockOffset, uint32_t blockLength);
	bool IsPieceComplete(uint32_t pieceIndex);

	float GetPieceProgress(uint32_t pieceIndex);

private:
	std::map<uint32_t, std::vector<BlockInfo>> pieceBlocks;
	uint32_t blockSize;
	uint32_t pieceSize;
};
