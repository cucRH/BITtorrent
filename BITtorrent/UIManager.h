#pragma once

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <mutex>
#include "DownloadManager.h"
#include "StorageManager.h"
#include "TrackerManager.h"

#pragma comment(lib, "comctl32.lib")

class UIManager {
public:
	UIManager(HWND hWnd);
	~UIManager();

	void Initialize();
	void CreateControls();
	void UpdateUI();
	void OnCommand(int wmId, HWND hWnd);
	void OnPaint(HDC hdc, const RECT& rect);
	void OnSize(int width, int height);
	void HandleFileOpen();

	DownloadManager& GetDownloadManager() { return downloadManager; }

private:
	HWND hMainWindow;
	HWND hListDownloads;
	HWND hBtnAddTorrent;
	HWND hBtnStartDownload;
	HWND hBtnPauseDownload;
	HWND hBtnRemove;
	HWND hProgressBar;
	HWND hStatusBar;
	HWND hSpeedLabelDown;
	HWND hSpeedLabelUp;
	HWND hTorrentNameLabel;
	HWND hTimeRemainingLabel;

	DownloadManager downloadManager;
	StorageManager storageManager;
	TrackerManager trackerManager;
	TorrentInfo currentTorrent;
	bool hasActiveTorrent;
	std::mutex torrentMutex;

	void CreateListView();
	void CreateButtons();
	void CreateStatusBar();
	void CreateInfoLabels();
	void AddListViewColumn(HWND hListView, int index, const wchar_t* text, int width);
	void UpdateListViewItem(int index);
	std::string FormatBytes(uint64_t bytes);
	std::string FormatSpeed(float bytesPerSec);
	std::string FormatTime(uint64_t seconds);
	uint64_t CalculateTimeRemaining();
};
