#include "UIManager.h"
#include <sstream>
#include <iomanip>
#include <shellapi.h>
#include <time.h>
#include <thread>
#include "Logger.h"

#define IDC_LIST_DOWNLOADS 1001
#define IDC_BTN_ADD_TORRENT 1002
#define IDC_BTN_START_DOWNLOAD 1003
#define IDC_BTN_PAUSE_DOWNLOAD 1004
#define IDC_BTN_REMOVE 1005
#define IDC_PROGRESS_BAR 1006
#define IDC_STATUS_BAR 1007
#define IDC_SPEED_DOWN 1008
#define IDC_SPEED_UP 1009
#define IDC_TORRENT_NAME 1010
#define IDC_TIME_REMAINING 1011

UIManager::UIManager(HWND hWnd)
	: hMainWindow(hWnd),
	hasActiveTorrent(false),
	downloadManager(),
	storageManager(),
	trackerManager() {
	storageManager.Initialize("./BitTorrent_Data");
}

UIManager::~UIManager() {
}

void UIManager::Initialize() {
	CreateListView();
	CreateButtons();
	CreateInfoLabels();
	CreateStatusBar();
}

void UIManager::CreateControls() {
}

void UIManager::CreateListView() {
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL;

	hListDownloads = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"SysListView32",
		L"",
		dwStyle,
		10, 100, 750, 200,
		hMainWindow,
		(HMENU)IDC_LIST_DOWNLOADS,
		GetModuleHandle(nullptr),
		nullptr
	);

	if (hListDownloads) {
		AddListViewColumn(hListDownloads, 0, L"Name", 200);
		AddListViewColumn(hListDownloads, 1, L"Size", 100);
		AddListViewColumn(hListDownloads, 2, L"Progress", 100);
		AddListViewColumn(hListDownloads, 3, L"Speed", 100);
		AddListViewColumn(hListDownloads, 4, L"Status", 100);
		AddListViewColumn(hListDownloads, 5, L"Seeds/Peers", 100);
	}
}

void UIManager::CreateInfoLabels() {
	hTorrentNameLabel = CreateWindowW(
		L"STATIC",
		L"No torrent loaded",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 15, 400, 20,
		hMainWindow,
		(HMENU)IDC_TORRENT_NAME,
		GetModuleHandle(nullptr),
		nullptr
	);

	hTimeRemainingLabel = CreateWindowW(
		L"STATIC",
		L"Time remaining: --",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		600, 15, 160, 20,
		hMainWindow,
		(HMENU)IDC_TIME_REMAINING,
		GetModuleHandle(nullptr),
		nullptr
	);
}

void UIManager::CreateButtons() {
	int buttonY = 310;
	int buttonWidth = 100;
	int buttonHeight = 30;
	int spacing = 10;
	int xPos = 10;

	hBtnAddTorrent = CreateWindowW(
		L"BUTTON",
		L"Add Torrent",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		xPos, buttonY, buttonWidth, buttonHeight,
		hMainWindow,
		(HMENU)IDC_BTN_ADD_TORRENT,
		GetModuleHandle(nullptr),
		nullptr
	);

	xPos += buttonWidth + spacing;

	hBtnStartDownload = CreateWindowW(
		L"BUTTON",
		L"Start",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		xPos, buttonY, buttonWidth, buttonHeight,
		hMainWindow,
		(HMENU)IDC_BTN_START_DOWNLOAD,
		GetModuleHandle(nullptr),
		nullptr
	);

	xPos += buttonWidth + spacing;

	hBtnPauseDownload = CreateWindowW(
		L"BUTTON",
		L"Pause",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		xPos, buttonY, buttonWidth, buttonHeight,
		hMainWindow,
		(HMENU)IDC_BTN_PAUSE_DOWNLOAD,
		GetModuleHandle(nullptr),
		nullptr
	);

	xPos += buttonWidth + spacing;

	hBtnRemove = CreateWindowW(
		L"BUTTON",
		L"Remove",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		xPos, buttonY, buttonWidth, buttonHeight,
		hMainWindow,
		(HMENU)IDC_BTN_REMOVE,
		GetModuleHandle(nullptr),
		nullptr
	);

	// Progress bar
	hProgressBar = CreateWindowW(
		PROGRESS_CLASSW,
		L"",
		WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
		10, 350, 750, 25,
		hMainWindow,
		(HMENU)IDC_PROGRESS_BAR,
		GetModuleHandle(nullptr),
		nullptr
	);

	if (hProgressBar) {
		SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	}

	// Speed labels
	hSpeedLabelDown = CreateWindowW(
		L"STATIC",
		L"Down: 0 KB/s",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		10, 380, 150, 20,
		hMainWindow,
		(HMENU)IDC_SPEED_DOWN,
		GetModuleHandle(nullptr),
		nullptr
	);

	hSpeedLabelUp = CreateWindowW(
		L"STATIC",
		L"Up: 0 KB/s",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		170, 380, 150, 20,
		hMainWindow,
		(HMENU)IDC_SPEED_UP,
		GetModuleHandle(nullptr),
		nullptr
	);
}

void UIManager::CreateStatusBar() {
	hStatusBar = CreateWindowExW(
		0,
		STATUSCLASSNAMEW,
		L"Ready",
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0,
		hMainWindow,
		(HMENU)IDC_STATUS_BAR,
		GetModuleHandle(nullptr),
		nullptr
	);
}

void UIManager::AddListViewColumn(HWND hListView, int index, const wchar_t* text, int width) {
	LVCOLUMNW lvc = {};
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.cx = width;
	lvc.pszText = const_cast<wchar_t*>(text);
	ListView_InsertColumn(hListView, index, &lvc);
}

void UIManager::UpdateUI() {
	if (hasActiveTorrent) {
		std::lock_guard<std::mutex> lock(torrentMutex);

		float progress = downloadManager.GetProgress();
		SendMessage(hProgressBar, PBM_SETPOS, (int)progress, 0);

		// Update speed labels
		std::string downSpeed = "Down: " + FormatSpeed(downloadManager.GetDownloadSpeed());
		std::string upSpeed = "Up: " + FormatSpeed(downloadManager.GetUploadSpeed());

		SetWindowTextA(hSpeedLabelDown, downSpeed.c_str());
		SetWindowTextA(hSpeedLabelUp, upSpeed.c_str());

		// Update time remaining
		uint64_t timeRemaining = CalculateTimeRemaining();
		std::string timeStr = "Time remaining: " + FormatTime(timeRemaining);
		SetWindowTextA(hTimeRemainingLabel, timeStr.c_str());

		// Update list view - сначала проверим наличие элементов
		int itemCount = ListView_GetItemCount(hListDownloads);
		if (itemCount == 0) {
			// Добавляем первую строку
			LVITEMW lvi = {};
			lvi.mask = LVIF_TEXT;
			lvi.iItem = 0;
			wchar_t empty[] = L"";
			lvi.pszText = empty;
			ListView_InsertItem(hListDownloads, &lvi);
		}

		// Обновляем элемент
		LVITEMW lvi = {};
		lvi.mask = LVIF_TEXT;
		lvi.iItem = 0;

		// Name
		lvi.iSubItem = 0;
		std::wstring nameTmp(currentTorrent.name.begin(), currentTorrent.name.end());
		lvi.pszText = const_cast<wchar_t*>(nameTmp.c_str());
		ListView_SetItem(hListDownloads, &lvi);

		// Size
		lvi.iSubItem = 1;
		std::string sizeStr = FormatBytes(currentTorrent.totalLength);
		std::wstring sizeTmp(sizeStr.begin(), sizeStr.end());
		lvi.pszText = const_cast<wchar_t*>(sizeTmp.c_str());
		ListView_SetItem(hListDownloads, &lvi);

		// Progress
		lvi.iSubItem = 2;
		wchar_t progressStr[16];
		swprintf_s(progressStr, sizeof(progressStr) / sizeof(wchar_t), L"%.1f%%", progress);
		lvi.pszText = progressStr;
		ListView_SetItem(hListDownloads, &lvi);

		// Speed
		lvi.iSubItem = 3;
		std::string speed = FormatSpeed(downloadManager.GetDownloadSpeed());
		std::wstring speedTmp(speed.begin(), speed.end());
		lvi.pszText = const_cast<wchar_t*>(speedTmp.c_str());
		ListView_SetItem(hListDownloads, &lvi);

		// Status
		lvi.iSubItem = 4;
		std::string status = downloadManager.GetStatus();
		std::wstring statusTmp(status.begin(), status.end());
		lvi.pszText = const_cast<wchar_t*>(statusTmp.c_str());
		ListView_SetItem(hListDownloads, &lvi);

		// Seeds/Peers
		lvi.iSubItem = 5;
		wchar_t seedsStr[32];
		swprintf_s(seedsStr, sizeof(seedsStr) / sizeof(wchar_t), L"%d/%d", downloadManager.GetSeeds(), downloadManager.GetPeers());
		lvi.pszText = seedsStr;
		ListView_SetItem(hListDownloads, &lvi);
	}
}

void UIManager::OnCommand(int wmId, HWND hWnd) {
	switch (wmId) {
	case IDC_BTN_ADD_TORRENT:
		HandleFileOpen();
		break;
	case IDC_BTN_START_DOWNLOAD:
		if (!hasActiveTorrent) {
			hasActiveTorrent = true;
		}
		downloadManager.StartDownload(currentTorrent);
		break;
	case IDC_BTN_PAUSE_DOWNLOAD:
		downloadManager.PauseDownload();
		break;
	case IDC_BTN_REMOVE:
		downloadManager.StopDownload();
		hasActiveTorrent = false;
		break;
	}
}

void UIManager::OnPaint(HDC hdc, const RECT& rect) {
}

void UIManager::OnSize(int width, int height) {
	if (hListDownloads) {
		MoveWindow(hListDownloads, 10, 60, width - 20, 200, TRUE);
	}
	if (hProgressBar) {
		MoveWindow(hProgressBar, 10, 270, width - 20, 25, TRUE);
	}
}

void UIManager::HandleFileOpen() {
	OPENFILENAMEA ofn = {};
	char szFile[260] = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hMainWindow;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Torrent Files (*.torrent)\0*.torrent\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn)) {
		// Запускаем загрузку торрента в отдельном потоке чтобы не заморозить UI
		std::thread loadThread([this, filePath = std::string(szFile)]() {
			Logger::GetInstance().Info("Loading torrent: " + filePath, "UIManager");

			TorrentParser parser;
			TorrentInfo tempTorrent;
			if (parser.LoadTorrent(filePath, tempTorrent)) {
				Logger::GetInstance().Info("Torrent loaded successfully: " + tempTorrent.name, "UIManager");

				{
					std::lock_guard<std::mutex> lock(torrentMutex);
					currentTorrent = tempTorrent;
					hasActiveTorrent = true;
				}

				// Сохраняем метаданные
				storageManager.SaveTorrentMetadata(currentTorrent);

				// Запрашиваем информацию у трекера (асинхронно)
				std::thread trackerThread([this]() {
					trackerManager.RequestTrackerInfo(
						currentTorrent.announce,
						currentTorrent.infoHash,
						"-BT0001-1234567890ab",
						6881,
						0,
						0,
						currentTorrent.totalLength
					);
				});
				trackerThread.detach();

				// Обновляем имя торрента в UI (из основного потока!)
				std::string nameLabel = "Torrent: " + currentTorrent.name;
				SetWindowTextA(hTorrentNameLabel, nameLabel.c_str());

				// Отправляем сообщение основному потоку для обновления UI
				PostMessage(hMainWindow, WM_USER + 1, 0, 0);
			} else {
				Logger::GetInstance().Log(LogLevel::LOG_ERROR, "Failed to load torrent: " + filePath, "UIManager");
				MessageBoxA(hMainWindow, "Failed to open torrent file", "Error", MB_OK | MB_ICONERROR);
			}
		});
		loadThread.detach();
	}
}

std::string UIManager::FormatBytes(uint64_t bytes) {
	const char* units[] = { "B", "KB", "MB", "GB", "TB" };
	double size = static_cast<double>(bytes);
	int unitIndex = 0;

	while (size >= 1024 && unitIndex < 4) {
		size /= 1024;
		unitIndex++;
	}

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
	return ss.str();
}

std::string UIManager::FormatSpeed(float bytesPerSec) {
	const char* units[] = { "B/s", "KB/s", "MB/s", "GB/s" };
	float speed = bytesPerSec;
	int unitIndex = 0;

	while (speed >= 1024 && unitIndex < 3) {
		speed /= 1024;
		unitIndex++;
	}

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << speed << " " << units[unitIndex];
	return ss.str();
}

std::string UIManager::FormatTime(uint64_t seconds) {
	if (seconds == 0) return "--";

	uint64_t hours = seconds / 3600;
	uint64_t minutes = (seconds % 3600) / 60;
	uint64_t secs = seconds % 60;

	std::stringstream ss;
	if (hours > 0) {
		ss << hours << "h " << minutes << "m " << secs << "s";
	} else if (minutes > 0) {
		ss << minutes << "m " << secs << "s";
	} else {
		ss << secs << "s";
	}
	return ss.str();
}

uint64_t UIManager::CalculateTimeRemaining() {
	float speed = downloadManager.GetDownloadSpeed();
	if (speed <= 0) return 0;

	uint64_t remaining = currentTorrent.totalLength - downloadManager.GetDownloadedBytes();
	return (uint64_t)(remaining / speed);
}
