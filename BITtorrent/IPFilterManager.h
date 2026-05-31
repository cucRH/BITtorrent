#pragma once

#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <ctime>

// Тип правила фильтрации
enum class FilterAction {
	ALLOW,
	BLOCK
};

// Правило IP фильтра
struct IPFilterRule {
	uint32_t id;
	std::string ipRange;  // "192.168.1.0/24" или "192.168.1.0-192.168.1.255"
	FilterAction action;
	std::string description;
	bool enabled;
};

// Менеджер IP фильтрации
class IPFilterManager {
public:
	IPFilterManager();
	~IPFilterManager();

	// Управление правилами
	bool AddRule(const IPFilterRule& rule);
	bool RemoveRule(uint32_t ruleId);
	bool UpdateRule(const IPFilterRule& rule);

	// Проверка IP
	bool IsIPAllowed(const std::string& ip);
	FilterAction GetIPAction(const std::string& ip) const;

	// Получение правил
	std::vector<IPFilterRule> GetAllRules() const;
	std::vector<IPFilterRule> GetEnabledRules() const;

	// Управление
	bool EnableRule(uint32_t ruleId);
	bool DisableRule(uint32_t ruleId);
	void ClearRules();

	// Сохранение/загрузка
	bool SaveRules(const std::string& filepath);
	bool LoadRules(const std::string& filepath);

	// Примеры стандартных фильтров
	void AddPrivateIPFilter();    // Блокировать приватные IP
	void AddCountryFilter(const std::string& countryCode);  // Фильтр по стране

private:
	std::vector<IPFilterRule> rules;
	uint32_t nextRuleId;

	bool IPToUint32(const std::string& ip, uint32_t& result) const;
	bool IsIPInRange(uint32_t ip, uint32_t start, uint32_t end) const;
	bool ParseIPRange(const std::string& range, uint32_t& start, uint32_t& end) const;
};

// Класс для отслеживания заблокированных пиров
class BlockedPeerTracker {
public:
	BlockedPeerTracker();
	~BlockedPeerTracker();

	void BlockPeer(const std::string& peerId, const std::string& reason);
	void UnblockPeer(const std::string& peerId);
	bool IsPeerBlocked(const std::string& peerId) const;

	std::vector<std::string> GetBlockedPeers() const;
	std::string GetBlockReason(const std::string& peerId) const;

	void ClearBlockedPeers();

private:
	struct BlockedPeer {
		std::string peerId;
		std::string reason;
		std::time_t blockedTime;
	};

	std::vector<BlockedPeer> blockedPeers;
};
