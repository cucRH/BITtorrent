#include "CacheManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>

CacheManager::CacheManager(const std::string& cachePath)
	: cacheDirectory(cachePath),
	maxCacheSize(1024 * 1024 * 512), // 512 MB по умолчанию
	currentCacheSize(0) {
	CreateDirectoryA(cacheDirectory.c_str(), nullptr);
}

CacheManager::~CacheManager() {
}

bool CacheManager::AddPiece(const PieceData& piece) {
	if (currentCacheSize + piece.length > maxCacheSize) {
		// Очищаем часть кеша если нужно
		ClearCache();
	}

	pieces[piece.index] = piece;
	currentCacheSize += piece.length;

	return WritePieceToDisk(piece);
}

PieceData CacheManager::GetPiece(uint32_t index) {
	PieceData piece = {};

	if (pieces.count(index)) {
		piece = pieces[index];
	} else {
		ReadPieceFromDisk(index, piece);
	}

	return piece;
}

bool CacheManager::VerifyPiece(uint32_t index, const std::string& expectedHash) {
	if (!pieces.count(index)) {
		ReadPieceFromDisk(index, pieces[index]);
	}

	if (pieces[index].hash == expectedHash) {
		pieces[index].status = PieceStatus::VERIFIED;
		return true;
	}

	return false;
}

std::vector<uint32_t> CacheManager::GetDownloadedPieces() const {
	std::vector<uint32_t> downloaded;
	for (const auto& p : pieces) {
		if (p.second.status == PieceStatus::DOWNLOADED || 
			p.second.status == PieceStatus::VERIFIED) {
			downloaded.push_back(p.first);
		}
	}
	return downloaded;
}

std::vector<uint32_t> CacheManager::GetMissingPieces() const {
	std::vector<uint32_t> missing;
	for (const auto& p : pieces) {
		if (p.second.status == PieceStatus::MISSING) {
			missing.push_back(p.first);
		}
	}
	return missing;
}

float CacheManager::GetCacheUsagePercent() const {
	if (maxCacheSize == 0) return 0.0f;
	return (float)currentCacheSize / (float)maxCacheSize * 100.0f;
}

uint64_t CacheManager::GetTotalCacheSize() const {
	return currentCacheSize;
}

void CacheManager::ClearCache() {
	pieces.clear();
	currentCacheSize = 0;
}

bool CacheManager::LoadFromDisk(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	file.close();
	return true;
}

bool CacheManager::SaveToDisk(const std::string& filename) {
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	file.close();
	return true;
}

std::string CacheManager::GetPieceFilePath(uint32_t index) {
	std::stringstream ss;
	ss << cacheDirectory << "/piece_" << index << ".dat";
	return ss.str();
}

bool CacheManager::WritePieceToDisk(const PieceData& piece) {
	std::string filepath = GetPieceFilePath(piece.index);
	std::ofstream file(filepath, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	if (!piece.data.empty()) {
		file.write(reinterpret_cast<const char*>(piece.data.data()), piece.data.size());
	}

	file.close();
	return true;
}

bool CacheManager::ReadPieceFromDisk(uint32_t index, PieceData& piece) {
	std::string filepath = GetPieceFilePath(index);
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	piece.data.resize(size);
	file.read(reinterpret_cast<char*>(piece.data.data()), size);
	file.close();

	piece.index = index;
	piece.status = PieceStatus::DOWNLOADED;
	return true;
}

BlockManager::BlockManager(uint32_t pieceSize)
	: blockSize(16384), // 16 KB
	pieceSize(pieceSize) {
}

BlockManager::~BlockManager() {
}

std::vector<BlockInfo> BlockManager::GetMissingBlocks(uint32_t pieceIndex) {
	std::vector<BlockInfo> missing;

	if (!pieceBlocks.count(pieceIndex)) {
		// Создаем блоки для куска если их нет
		uint32_t offset = 0;
		while (offset < pieceSize) {
			BlockInfo block;
			block.offset = offset;
			uint32_t remaining = pieceSize - offset;
			block.length = blockSize < remaining ? blockSize : remaining;
			block.downloaded = false;
			pieceBlocks[pieceIndex].push_back(block);
			offset += block.length;
		}
	}

	for (auto& block : pieceBlocks[pieceIndex]) {
		if (!block.downloaded) {
			missing.push_back(block);
		}
	}

	return missing;
}

void BlockManager::MarkBlockDownloaded(uint32_t pieceIndex, uint32_t blockOffset, uint32_t blockLength) {
	if (!pieceBlocks.count(pieceIndex)) {
		return;
	}

	for (auto& block : pieceBlocks[pieceIndex]) {
		if (block.offset == blockOffset && block.length == blockLength) {
			block.downloaded = true;
			break;
		}
	}
}

bool BlockManager::IsPieceComplete(uint32_t pieceIndex) {
	if (!pieceBlocks.count(pieceIndex)) {
		return false;
	}

	for (const auto& block : pieceBlocks[pieceIndex]) {
		if (!block.downloaded) {
			return false;
		}
	}

	return true;
}

float BlockManager::GetPieceProgress(uint32_t pieceIndex) {
	if (!pieceBlocks.count(pieceIndex)) {
		return 0.0f;
	}

	uint32_t downloaded = 0;
	uint32_t total = 0;

	for (const auto& block : pieceBlocks[pieceIndex]) {
		total += block.length;
		if (block.downloaded) {
			downloaded += block.length;
		}
	}

	if (total == 0) return 0.0f;
	return (float)downloaded / (float)total * 100.0f;
}
