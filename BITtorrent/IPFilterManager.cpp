#include "IPFilterManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

IPFilterManager::IPFilterManager()
	: nextRuleId(1) {
}

IPFilterManager::~IPFilterManager() {
}

bool IPFilterManager::AddRule(const IPFilterRule& rule) {
	IPFilterRule newRule = rule;
	newRule.id = nextRuleId++;
	rules.push_back(newRule);
	return true;
}

bool IPFilterManager::RemoveRule(uint32_t ruleId) {
	auto it = std::find_if(rules.begin(), rules.end(),
		[ruleId](const IPFilterRule& r) { return r.id == ruleId; });

	if (it != rules.end()) {
		rules.erase(it);
		return true;
	}
	return false;
}

bool IPFilterManager::UpdateRule(const IPFilterRule& rule) {
	for (auto& r : rules) {
		if (r.id == rule.id) {
			r = rule;
			return true;
		}
	}
	return false;
}

bool IPFilterManager::IsIPAllowed(const std::string& ip) {
	return GetIPAction(ip) != FilterAction::BLOCK;
}

FilterAction IPFilterManager::GetIPAction(const std::string& ip) const {
	uint32_t ipNum = 0;
	if (!IPToUint32(ip, ipNum)) {
		return FilterAction::ALLOW;  // Invalid IP allowed by default
	}

	FilterAction result = FilterAction::ALLOW;

	for (const auto& rule : rules) {
		if (!rule.enabled) continue;

		uint32_t start, end;
		if (ParseIPRange(rule.ipRange, start, end)) {
			if (IsIPInRange(ipNum, start, end)) {
				result = rule.action;
				break;  // First match wins
			}
		}
	}

	return result;
}

std::vector<IPFilterRule> IPFilterManager::GetAllRules() const {
	return rules;
}

std::vector<IPFilterRule> IPFilterManager::GetEnabledRules() const {
	std::vector<IPFilterRule> enabled;
	for (const auto& rule : rules) {
		if (rule.enabled) {
			enabled.push_back(rule);
		}
	}
	return enabled;
}

bool IPFilterManager::EnableRule(uint32_t ruleId) {
	for (auto& rule : rules) {
		if (rule.id == ruleId) {
			rule.enabled = true;
			return true;
		}
	}
	return false;
}

bool IPFilterManager::DisableRule(uint32_t ruleId) {
	for (auto& rule : rules) {
		if (rule.id == ruleId) {
			rule.enabled = false;
			return true;
		}
	}
	return false;
}

void IPFilterManager::ClearRules() {
	rules.clear();
}

bool IPFilterManager::SaveRules(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& rule : rules) {
		file << rule.id << "|"
			 << rule.ipRange << "|"
			 << (int)rule.action << "|"
			 << rule.description << "|"
			 << (rule.enabled ? "1" : "0") << "\n";
	}

	file.close();
	return true;
}

bool IPFilterManager::LoadRules(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;

		IPFilterRule rule = {};

		std::getline(ss, token, '|');
		rule.id = std::stoul(token);

		std::getline(ss, rule.ipRange, '|');

		std::getline(ss, token, '|');
		rule.action = (FilterAction)std::stoi(token);

		std::getline(ss, rule.description, '|');

		std::getline(ss, token, '|');
		rule.enabled = (token == "1");

		rules.push_back(rule);
	}

	file.close();
	return true;
}

void IPFilterManager::AddPrivateIPFilter() {
	IPFilterRule rule1;
	rule1.ipRange = "10.0.0.0/8";
	rule1.action = FilterAction::BLOCK;
	rule1.description = "Private IP - Class A";
	rule1.enabled = true;
	AddRule(rule1);

	IPFilterRule rule2;
	rule2.ipRange = "172.16.0.0/12";
	rule2.action = FilterAction::BLOCK;
	rule2.description = "Private IP - Class B";
	rule2.enabled = true;
	AddRule(rule2);

	IPFilterRule rule3;
	rule3.ipRange = "192.168.0.0/16";
	rule3.action = FilterAction::BLOCK;
	rule3.description = "Private IP - Class C";
	rule3.enabled = true;
	AddRule(rule3);
}

void IPFilterManager::AddCountryFilter(const std::string& countryCode) {
	// Реализация фильтра по стране (требует базы данных GeoIP)
}

bool IPFilterManager::IPToUint32(const std::string& ip, uint32_t& result) const {
	int parts[4];
	if (sscanf_s(ip.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]) != 4) {
		return false;
	}

	for (int i = 0; i < 4; i++) {
		if (parts[i] < 0 || parts[i] > 255) {
			return false;
		}
	}

	result = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
	return true;
}

bool IPFilterManager::IsIPInRange(uint32_t ip, uint32_t start, uint32_t end) const {
	return ip >= start && ip <= end;
}

bool IPFilterManager::ParseIPRange(const std::string& range, uint32_t& start, uint32_t& end) const {
	// Попробуем парсить в формате CIDR (192.168.1.0/24)
	size_t slashPos = range.find('/');
	if (slashPos != std::string::npos) {
		std::string ipPart = range.substr(0, slashPos);
		std::string maskPart = range.substr(slashPos + 1);

		if (!IPToUint32(ipPart, start)) {
			return false;
		}

		int maskBits = std::stoi(maskPart);
		uint32_t mask = (0xFFFFFFFF << (32 - maskBits));

		start = start & mask;
		end = start | (~mask);
		return true;
	}

	// Попробуем парсить в формате диапазона (192.168.1.0-192.168.1.255)
	size_t dashPos = range.find('-');
	if (dashPos != std::string::npos) {
		std::string startIP = range.substr(0, dashPos);
		std::string endIP = range.substr(dashPos + 1);

		return IPToUint32(startIP, start) && IPToUint32(endIP, end);
	}

	// Если это просто IP адрес
	if (IPToUint32(range, start)) {
		end = start;
		return true;
	}

	return false;
}

// BlockedPeerTracker implementation
BlockedPeerTracker::BlockedPeerTracker() {
}

BlockedPeerTracker::~BlockedPeerTracker() {
}

void BlockedPeerTracker::BlockPeer(const std::string& peerId, const std::string& reason) {
	for (auto& peer : blockedPeers) {
		if (peer.peerId == peerId) {
			peer.reason = reason;
			return;
		}
	}

	BlockedPeer peer;
	peer.peerId = peerId;
	peer.reason = reason;
	peer.blockedTime = std::time(nullptr);
	blockedPeers.push_back(peer);
}

void BlockedPeerTracker::UnblockPeer(const std::string& peerId) {
	auto it = std::find_if(blockedPeers.begin(), blockedPeers.end(),
		[&peerId](const BlockedPeer& p) { return p.peerId == peerId; });

	if (it != blockedPeers.end()) {
		blockedPeers.erase(it);
	}
}

bool BlockedPeerTracker::IsPeerBlocked(const std::string& peerId) const {
	return std::find_if(blockedPeers.begin(), blockedPeers.end(),
		[&peerId](const BlockedPeer& p) { return p.peerId == peerId; }) != blockedPeers.end();
}

std::vector<std::string> BlockedPeerTracker::GetBlockedPeers() const {
	std::vector<std::string> peers;
	for (const auto& peer : blockedPeers) {
		peers.push_back(peer.peerId);
	}
	return peers;
}

std::string BlockedPeerTracker::GetBlockReason(const std::string& peerId) const {
	for (const auto& peer : blockedPeers) {
		if (peer.peerId == peerId) {
			return peer.reason;
		}
	}
	return "";
}

void BlockedPeerTracker::ClearBlockedPeers() {
	blockedPeers.clear();
}
