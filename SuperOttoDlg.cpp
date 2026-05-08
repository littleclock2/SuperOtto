
// SuperOttoDlg.cpp: 实现文件
// 版本 3.1.0 - 优化版
// 改进：自动创建依赖文件夹、修复bug、UI优化
//
#pragma comment(lib,"winmm.lib")
#include "pch.h"
#include "SuperOtto.h"
#include "SuperOttoDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <string>
#include <ctime>
#include "mmsystem.h"
#include <locale>//为了使用setlocale(LC_CTYPE, "chs");便于输入中文到txt中
#define _WINSOCK_DEPRECATED_NO_WARNINGS
/*别报错*/
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HWND m_hWnd;//记录当前窗口的句柄
//DWORD DeviceID;//指定播放音乐的设备
MCI_OPEN_PARMS mciopenparms;//指定打开音乐文件的参数
// 用于应用程序"关于"菜单项的 CAboutDlg 对话框
CString fullPath;
MCIDEVICEID m_IDdevice;

// 用于应用程序"关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	int Trophy_x;
	int Trophy_y;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	Trophy_x = 0;
	Trophy_y = 0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)

{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSuperOttoDlg 对话框



CSuperOttoDlg::CSuperOttoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUPEROTTO_DIALOG, pParent)
	, robot(0, 0)
	,robot1(0,0)
	,robot2(1,0)
	, selectedX(-1) // 初始化选中的坐标
	, selectedY(-1)
	, temp_obstacle_num(0), m_port(_T(""))
	, ID(0)
	, text(_T(""))
	, no_map(true)
	,NoClient(true)
	,isloading(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	n = 0;
	m_nTimerID0 = 0;
	m_nTimerID1 = 0;
	m_nTimerID2 = 0;
}

void CSuperOttoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_START_X, m_combo_start_x);
	DDX_Control(pDX, IDC_COMBO_START_Y, m_combo_start_y);
	DDX_Control(pDX, IDC_COMBO_SETOBSTACLE_X, m_combo_setobstacle_x);
	DDX_Control(pDX, IDC_COMBO_SETOBSTACLE_Y, m_combo_setobstacle_y);
	DDX_Control(pDX, IDC_COMBO_CLEAROBSTACLE_X, m_combo_clearobstacle_x);
	DDX_Control(pDX, IDC_COMBO_CLEAROBSTACLE_Y, m_combo_clearobstacle_y);
	DDX_Control(pDX, IDC_COMBO_SETGOAL_X, m_combo_setgoal_x);
	DDX_Control(pDX, IDC_COMBO_SETGOAL_Y, m_combo_setgoal_y);
	DDX_Control(pDX, IDC_BUTTONROBOT1SETGOAL, m_btn_robot1_setgoal);
	DDX_Control(pDX, IDC_BUTTONROBOT1FINDPATH, m_btn_robot1_findpath);
	DDX_Control(pDX, IDC_BUTTONROBOT2SETGOAL, m_btn_robot2_setgoal);
	DDX_Control(pDX, IDC_BUTTONROBOT2FINDPATH, m_btn_robot2_findpath);
	DDX_Text(pDX, IDC_EDIT_OBSATACLENUM, temp_obstacle_num);
	DDX_Control(pDX, IDC_LISTMUSIC, listBox);
	DDX_Control(pDX, IDC_IPADDRESS1, m_nIPcontrol);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_ID_COMBO, m_IDCombo);
	DDX_CBIndex(pDX, IDC_ID_COMBO, ID);
	DDX_Text(pDX, IDC_TEXT, text);
}

BEGIN_MESSAGE_MAP(CSuperOttoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CSuperOttoDlg::OnBnClickedOk)
	ON_STN_CLICKED(IDC_AREA, &CSuperOttoDlg::OnStnClickedArea)
	ON_BN_CLICKED(ID_ButtonFindPath, &CSuperOttoDlg::OnClickedButtonFindPath)
	ON_BN_CLICKED(ID_ButtonSetGoal, &CSuperOttoDlg::OnClickedButtonSetGoal)
	ON_BN_CLICKED(ID_ButtonSetObstacle, &CSuperOttoDlg::OnClickedButtonSetObstacle)
	ON_BN_CLICKED(ID_ButtonSetStart, &CSuperOttoDlg::OnClickedButtonSetStart)
	ON_BN_CLICKED(ID_ButtonClearObstacle, &CSuperOttoDlg::OnClickedButtonclearobstacle)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_SOCKET, &CSuperOttoDlg::OnSocket)
	ON_BN_CLICKED(IDC_BUTTONROBOT1FINDPATH, &CSuperOttoDlg::OnBnClickedButtonrobot1findpath)
	ON_BN_CLICKED(IDC_BUTTONROBOT2SETGOAL, &CSuperOttoDlg::OnBnClickedButtonrobot2setgoal)
	ON_BN_CLICKED(IDC_BUTTONROBOT2FINDPATH, &CSuperOttoDlg::OnBnClickedButtonrobot2findpath)
	ON_BN_CLICKED(IDC_BUTTONROBOT1SETGOAL, &CSuperOttoDlg::OnClickedButtonrobot1setgoal)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(ID_ButtonClearObstacle3, &CSuperOttoDlg::OnBnClickedButtonclearobstacle3)
	ON_BN_CLICKED(ID_ButtonClearObstacle2, &CSuperOttoDlg::OnBnClickedButtonclearobstacle2)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTONRELOAD, &CSuperOttoDlg::OnClickedButtonreload)
	ON_BN_CLICKED(IDC_BUTTONADD, &CSuperOttoDlg::OnBnClickedButtonadd)
	ON_BN_CLICKED(IDC_BUTTONPLAY, &CSuperOttoDlg::OnBnClickedButtonplay)
	ON_BN_CLICKED(IDC_BUTTONPAUSE, &CSuperOttoDlg::OnBnClickedButtonpause)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &CSuperOttoDlg::OnBnClickedButtonstop)
	ON_BN_CLICKED(IDC_BUTTONDEL, &CSuperOttoDlg::OnBnClickedButtondel)
	ON_BN_CLICKED(IDC_BUTTONLAST, &CSuperOttoDlg::OnBnClickedButtonlast)
	ON_BN_CLICKED(IDC_BUTTONNEXT, &CSuperOttoDlg::OnBnClickedButtonnext)
	ON_BN_CLICKED(IDCANCEL, &CSuperOttoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CONNECT, &CSuperOttoDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CSuperOttoDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_SEND, &CSuperOttoDlg::OnBnClickedSend)
	ON_CBN_SELCHANGE(IDC_ID_COMBO, &CSuperOttoDlg::OnCbnSelchangeIdCombo)
	ON_EN_CHANGE(IDC_TEXT, &CSuperOttoDlg::OnEnChangeText)
	ON_BN_CLICKED(IDC_CHECK, &CSuperOttoDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// CSuperOttoDlg 消息处理程序

BOOL CSuperOttoDlg::OnInitDialog(){
	CDialogEx::OnInitDialog();
	m_hWnd = GetSafeHwnd();
	m_IDdevice = 0;
	people = 0;

	// 将"关于..."菜单项添加到系统菜单中。
	//音乐播放器读取音乐 - 自动扫描music目录
	CString musicDir = GetMusicDirectory();
	CString strFind = musicDir + _T("*.*");
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strFind, &findData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CString fileName = findData.cFileName;
			CString ext = fileName.Right(4).MakeLower();
			if (ext == _T(".mp3") || ext == _T(".wma") || ext == _T(".wav")) {
				CString fullPath = musicDir + fileName;
				preLoadMusicPath.push_back(fullPath);
				preLoadMusicName.push_back(fileName);
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}

	// 兼容旧的musicPath.txt（绝对路径）
	CString strFileName = GetExeDirectory() + _T("\\musicPath.txt");
	CStdioFile file;
	if (file.Open(strFileName, CFile::modeRead)) {
		CString strValue;
		while (file.ReadString(strValue)) {
			if (strValue.IsEmpty()) continue;
			CString fullPath = strValue;
			if (fullPath.GetLength() >= 2 && fullPath[1] != _T(':')) {
				fullPath = GetExeDirectory() + _T("\\") + fullPath;
			}
			if (!PathFileExists(fullPath)) continue;
			// 避免重复添加
			bool dup = false;
			for (auto& p : preLoadMusicPath) {
				if (p.CompareNoCase(fullPath) == 0) { dup = true; break; }
			}
			if (dup) continue;
			int pos = fullPath.ReverseFind(_T('\\'));
			preLoadMusicPath.push_back(fullPath);
			preLoadMusicName.push_back(fullPath.Mid(pos + 1));
		}
		file.Close();
	}

	for (int i = 0; i < preLoadMusicPath.size(); i++) {
		listBox.InsertString(i, preLoadMusicName[i]);
	}

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	curPlayPos = 0;

	// 创建音量控件：竖向滑块，位于"上一首"按钮左侧
	CWnd* pLastBtn = GetDlgItem(IDC_BUTTONLAST);
	CWnd* pPlayBtn = GetDlgItem(IDC_BUTTONPLAY);
	if (pLastBtn && pPlayBtn) {
		CRect lastRect, playRect;
		pLastBtn->GetWindowRect(&lastRect);
		pPlayBtn->GetWindowRect(&playRect);
		ScreenToClient(&lastRect);
		ScreenToClient(&playRect);

		int sliderW = 25;
		int sliderH = lastRect.bottom - playRect.top + 4;
		int sliderRight = lastRect.left - 6;
		int sliderLeft = sliderRight - sliderW;
		int sliderTop = playRect.top;
		CRect sliderRect(sliderLeft, sliderTop, sliderRight, sliderTop + sliderH);
		m_volumeSlider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_BOTH | TBS_AUTOTICKS, sliderRect, this, 1070);
		m_volumeSlider.SetRange(0, 100);
		m_volumeSlider.SetPos(80);
		m_volumeSlider.SetTicFreq(10);

		// 百分比标签在滑块左侧
		CRect labelRect(sliderLeft - 40, sliderTop + sliderH / 2 - 8, sliderLeft - 2, sliderTop + sliderH / 2 + 8);
		m_volumeLabel.Create(_T("80%"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT, labelRect, this, 1071);
	}
	waveOutSetVolume(NULL, (80 * 65535 / 100 << 16) | (80 * 65535 / 100));

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// // 初始化双缓冲
	CWnd* pWnd = GetDlgItem(IDC_AREA);
	CDC* pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	m_pOldBitmap = m_dcMem.SelectObject(&m_bitmap);
	pWnd->ReleaseDC(pDC);
	// 自适应屏幕大小
	//保留未自适应分辨率时的应用大小
	//CRect rect;
	GetClientRect(&rect);     //取客户区大小
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;

	// 初始化网格大小（假设30x30的网格）
	grid = std::vector<std::vector<int>>(30, std::vector<int>(30, 0));
	//填充下拉栏菜单
	m_combo_start_x.ResetContent();
	m_combo_start_y.ResetContent();
	m_combo_setobstacle_x.ResetContent();
	m_combo_setobstacle_y.ResetContent();
	m_combo_clearobstacle_x.ResetContent();
	m_combo_clearobstacle_y.ResetContent();
	m_combo_setgoal_x.ResetContent();
	m_combo_setgoal_y.ResetContent();

	for (int i = 0; i < static_cast<int>(grid.size()); ++i) {
		CString str;
		str.Format(_T("%d"), i);
		m_combo_start_x.InsertString(i,str);
		m_combo_setobstacle_x.InsertString(i, str);
		m_combo_clearobstacle_x.InsertString(i, str);
		m_combo_setgoal_x.InsertString(i, str);
	}
	for (int i = 0; i < static_cast<int>(grid[0].size()); ++i) {
		CString str;
		str.Format(_T("%d"), i);
		m_combo_start_y.InsertString(i, str);
		m_combo_setobstacle_y.InsertString(i, str);
		m_combo_clearobstacle_y.InsertString(i, str);
		m_combo_setgoal_y.InsertString(i, str);
	}
	// 设定初始起点和终点
	isSelecting = false;
	temp_range = 9;
	start = Point(0, 0);
	goal = Point(9, 9);
	goal1 = Point(0, 0);
	goal2 = Point(grid.size() - 1, grid[0].size() - 1);
	robot = Robot(start.x, start.y);
	start1 = Point(0, 0);
	start2 = Point(grid.size() - 1, grid[0].size() - 1);
	robot1 = Robot(start1.x, start1.y);
	robot2 = Robot(start2.x, start2.y);
	robot1_score = 0;
	robot2_score = 0;
	GetDlgItem(IDC_EDIT_LINE)->SetWindowText("30");
	GetDlgItem(IDC_EDIT_ROW)->SetWindowText("30");
	GetDlgItem(IDC_EDIT_OBSATACLENUM)->SetWindowText("180");
	GetDlgItem(IDC_EDIT_RANGE)->SetWindowText("9");
	CString str;
	str.Format("玩家1:%d分", robot1_score);
	GetDlgItem(IDC_SCORE1)->SetWindowText(str);
	str.Format("玩家2:%d分", robot2_score);
	GetDlgItem(IDC_SCORE2)->SetWindowText(str);
	GetDlgItem(IDC_SHOW)->SetWindowText(TEXT("玩家1回合"));
	PlaceRandomTrophy();
	PlaceRandomObstacles(200);
	((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(BST_UNCHECKED);
	isPictureload = true;

	// 检查picturetest目录是否存在必要的图片
	CString picDir = GetPictureDirectory();
	if (!PathFileExists(picDir + _T("bk.JPG")) && !PathFileExists(picDir + _T("bk.jpg"))) {
		// 创建默认背景
		AfxMessageBox(_T("提示：picturetest目录中未找到背景图片，将使用纯色背景。\n请将图片文件放入: ") + picDir);
		isPictureload = false;
		((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(BST_CHECKED);
	}

	/*****************************************************************************************/
	/*****************************************************************************************/
	/*									网络相关初始化										*/
	/*列表初始化*/
	m_IDCombo.InsertString(0, _T("房主"));
	m_IDCombo.InsertString(1, _T("玩家"));
	m_IDCombo.InsertString(2, _T("观众"));
	m_IDCombo.SetCurSel(0);

	/*指针初始化*/
	m_ServerSocket = NULL;
	m_ListenSocket = NULL;
	m_ClientSocket = NULL;

	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);/*禁止断开*/
	GetDlgItem(IDC_SEND)->EnableWindow(false);/*禁止按发送*/
	GetDlgItem(IDC_SENDTEXT)->EnableWindow(false);/*禁止输入消息*/

	CString strIP = "192.0.2.1";//在这里修改IP地址
	m_port = "7600";
	DWORD dwAddress = ntohl(inet_addr(strIP));//将CString类型的IP地址转化到 DWORD 类型
	m_nIPcontrol.SetAddress(dwAddress);//将服务器的IP设置为strIP对应的地址

	GetDlgItem(IDC_EDIT_PORT)->SetWindowText("7600");//显示7600

	// 更新窗口标题
	SetWindowText(_T("SuperOtto v3.1.0 - 优化版"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSuperOttoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSuperOttoDlg::OnPaint(){
	if (IsIconic()){
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else{
		CDialogEx::OnPaint();


		//CPaintDC dc(this);  // 绘图设备上下文
		CWnd* pWnd = GetDlgItem(IDC_AREA);
		CDC* pDC = pWnd->GetDC();

		// 获取控件的尺寸
		CRect rect2;
		pWnd->GetClientRect(&rect2);

		// 设置单元格大小（根据控件大小自适应）
		int cellSizeX = rect2.Width() / grid.size();
		int cellSizeY = rect2.Height() / grid[0].size();

		// 使用自动创建的picturetest目录
		const CString pictureDir = GetPictureDirectory();
		static CString cachedPictureDir;
		static bool assetsReady = false;
		static CImage bgImage, obImage, cupImage, goalImage, startImage, robot1Image, robot2Image;

		auto reloadPaintAssets = [&]() {
			if (assetsReady && cachedPictureDir == pictureDir) {
				return;
			}
			if (!bgImage.IsNull()) bgImage.Destroy();
			if (!obImage.IsNull()) obImage.Destroy();
			if (!cupImage.IsNull()) cupImage.Destroy();
			if (!goalImage.IsNull()) goalImage.Destroy();
			if (!startImage.IsNull()) startImage.Destroy();
			if (!robot1Image.IsNull()) robot1Image.Destroy();
			if (!robot2Image.IsNull()) robot2Image.Destroy();

			// 尝试加载图片，支持多种格式
			bgImage.Load(pictureDir + _T("bk.JPG"));
			if (bgImage.IsNull()) {
				bgImage.Load(pictureDir + _T("bk.jpg"));
			}
			if (bgImage.IsNull()) {
				bgImage.Load(pictureDir + _T("bk.png"));
			}
			obImage.Load(pictureDir + _T("Wall2.png"));
			cupImage.Load(pictureDir + _T("Cup.png"));
			goalImage.Load(pictureDir + _T("Wall3.png"));
			startImage.Load(pictureDir + _T("Wall4.png"));
			robot1Image.Load(pictureDir + _T("1.png"));
			robot2Image.Load(pictureDir + _T("2.png"));

			cachedPictureDir = pictureDir;
			assetsReady = true;
		};

		if (isPictureload) {
			reloadPaintAssets();
		}

		// 优化：使用更好的背景颜色
		if (isPictureload && !bgImage.IsNull()) bgImage.Draw(m_dcMem.GetSafeHdc(), rect2);
		else {
			m_dcMem.FillSolidRect(&rect2, RGB(240, 248, 255)); // 使用浅蓝色背景
		}

		// 绘制障碍物和奖杯
		for (int i = 0; i < grid.size(); ++i) {
			for (int j = 0; j < grid[i].size(); ++j) {
				if (grid[i][j] == 1) {
					CRect cellRect(i * cellSizeX, j * cellSizeY, (i + 1) * cellSizeX, (j + 1) * cellSizeY);
					if (isPictureload && !obImage.IsNull()) {
						obImage.Draw(m_dcMem.GetSafeHdc(), cellRect);
					}
					else {
						m_dcMem.FillSolidRect(&cellRect, RGB(64, 64, 64));
					}
				}
				else if (grid[i][j] == 2) {
					CRect cellRect(i * cellSizeX, j * cellSizeY, (i + 1) * cellSizeX, (j + 1) * cellSizeY);
					if (isPictureload && !cupImage.IsNull()) {
						cupImage.Draw(m_dcMem.GetSafeHdc(), cellRect);
					}
					else {
						m_dcMem.FillSolidRect(&cellRect, RGB(255, 215, 0));
					}
				}
			}
		}

		// 非图片模式：绘制网格线（单次绘制整个网格，而非逐格）
		if (!isPictureload) {
			CPen pen(PS_SOLID, 1, RGB(200, 200, 200));
			CPen* pOldPen = m_dcMem.SelectObject(&pen);
			int gridW = static_cast<int>(grid.size());
			int gridH = static_cast<int>(grid[0].size());
			for (int i = 0; i <= gridW; ++i) {
				m_dcMem.MoveTo(i * cellSizeX, 0);
				m_dcMem.LineTo(i * cellSizeX, gridH * cellSizeY);
			}
			for (int j = 0; j <= gridH; ++j) {
				m_dcMem.MoveTo(0, j * cellSizeY);
				m_dcMem.LineTo(gridW * cellSizeX, j * cellSizeY);
			}
			m_dcMem.SelectObject(pOldPen);
		}

		if(robot1_turn)HighlightSurroundingCells(start1);
		else { HighlightSurroundingCells(start2); }

		// 绘制路径
		if (!path0.empty()) {
			DrawPath(&m_dcMem, path0, cellSizeX, cellSizeY);
		}

		// 绘制路径1
		if (!path1.empty()) {
			DrawPath(&m_dcMem, path1, cellSizeX, cellSizeY);
		}
		// 绘制路径2
		if (!path2.empty()) {
			DrawPath(&m_dcMem, path2, cellSizeX, cellSizeY);
		}

		// 绘制终点1
		CRect goal1Rect(goal1.x * cellSizeX, goal1.y * cellSizeY, (goal1.x + 1) * cellSizeX, (goal1.y + 1) * cellSizeY);
		if (isPictureload && !goalImage.IsNull()) {
			goalImage.Draw(m_dcMem.GetSafeHdc(), goal1Rect);
		}
		else {
			// 优化终点颜色 - 使用红色
			m_dcMem.FillSolidRect(&goal1Rect, RGB(220, 20, 60));
		}



		// 绘制终点2
		CRect goal2Rect(goal2.x * cellSizeX, goal2.y * cellSizeY, (goal2.x + 1) * cellSizeX, (goal2.y + 1) * cellSizeY);
		if (isPictureload && !goalImage.IsNull()) {
			goalImage.Draw(m_dcMem.GetSafeHdc(), goal2Rect);
		}
		else {
			// 优化终点颜色 - 使用深红色
			m_dcMem.FillSolidRect(&goal2Rect, RGB(178, 34, 34));
		}



		// 绘制起点
		CRect start1Rect(start1.x * cellSizeX, start1.y * cellSizeY, (start1.x + 1) * cellSizeX, (start1.y + 1) * cellSizeY);
		if (isPictureload && !startImage.IsNull()) {
			startImage.Draw(m_dcMem.GetSafeHdc(), start1Rect);
		}
		else {
			// 优化起点颜色 - 使用绿色
			m_dcMem.FillSolidRect(&start1Rect, RGB(34, 139, 34));
		}


		// 绘制起点
		CRect start2Rect(start2.x * cellSizeX, start2.y * cellSizeY, (start2.x + 1) * cellSizeX, (start2.y + 1) * cellSizeY);
		if (isPictureload && !startImage.IsNull()) {
			startImage.Draw(m_dcMem.GetSafeHdc(), start2Rect);
		}
		else {
			// 优化起点颜色 - 使用深绿色
			m_dcMem.FillSolidRect(&start2Rect, RGB(0, 100, 0));
		}


		//绘制机器人1
		if (isPictureload && !robot1Image.IsNull()) {
			robot1Image.Draw(m_dcMem.GetSafeHdc(), robot1.GetX() * cellSizeX, robot1.GetY() * cellSizeY, cellSizeX, cellSizeY);
		}
		else {
			CRect robot1Rect(robot1.GetX() * cellSizeX, robot1.GetY() * cellSizeY, (robot1.GetX() + 1) * cellSizeX, (robot1.GetY() + 1) * cellSizeY);
			// 优化机器人1颜色 - 使用橙色
			m_dcMem.FillSolidRect(&robot1Rect, RGB(255, 140, 0));
		}
		//绘制机器人2
		if (isPictureload && !robot2Image.IsNull()) {
			robot2Image.Draw(m_dcMem.GetSafeHdc(), robot2.GetX() * cellSizeX, robot2.GetY() * cellSizeY, cellSizeX, cellSizeY);
		}
		else {
			CRect robot2Rect(robot2.GetX() * cellSizeX, robot2.GetY() * cellSizeY, (robot2.GetX() + 1) * cellSizeX, (robot2.GetY() + 1) * cellSizeY);
			// 优化机器人2颜色 - 使用紫色
			m_dcMem.FillSolidRect(&robot2Rect, RGB(148, 0, 211));
		}

		// 绘制选中的单元格
		if (selectedX >= 0 && selectedY >= 0) {
			HighlightSelectedCell(selectedX, selectedY, RGB(255 ,236, 139));
		}

		if(robot1_turn)GetDlgItem(IDC_SHOW)->SetWindowText(TEXT("玩家1回合"));
		else GetDlgItem(IDC_SHOW)->SetWindowText(TEXT("玩家2回合"));
		// 将内存DC的内容拷贝到屏幕DC
		pDC->BitBlt(0, 0, rect2.Width(), rect2.Height(), &m_dcMem, 0, 0, SRCCOPY);
		// 释放DC
		pWnd->ReleaseDC(pDC);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSuperOttoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSuperOttoDlg::OnSize(UINT nType, int cx, int cy) {
	CDialogEx::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
	{
		ReSize();
		UpdateDisplayArea();
		Invalidate(TRUE);
	}
}

void CSuperOttoDlg::UpdateDisplayArea() {
	// 确保控件存在
	CWnd* pWnd = GetDlgItem(IDC_AREA);
	if (!pWnd) {
		return;  // 控件不存在，退出
	}

	// 获取DC
	CDC* pDC = pWnd->GetDC();
	if (!pDC) {
		return;  // 获取DC失败，退出
	}

	// 释放旧的位图和内存DC
	if (m_pOldBitmap) {
		m_dcMem.SelectObject(m_pOldBitmap);
		m_pOldBitmap = NULL;
	}

	// 删除旧的位图对象
	if (m_bitmap.GetSafeHandle()) {
		m_bitmap.DeleteObject();
	}

	// 获取控件的新的尺寸
	CRect rect;
	pWnd->GetClientRect(&rect);

	// 创建新的位图和内存DC
	m_dcMem.DeleteDC();  // 确保旧的内存DC被删除
	if (!m_dcMem.CreateCompatibleDC(pDC)) {
		pWnd->ReleaseDC(pDC);  // 释放DC
		return;  // 创建兼容DC失败，退出
	}

	if (!m_bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height())) {
		pWnd->ReleaseDC(pDC);  // 释放DC
		return;  // 创建位图失败，退出
	}

	m_pOldBitmap = m_dcMem.SelectObject(&m_bitmap);
	pWnd->ReleaseDC(pDC);  // 释放DC
}

//自适应比例
void CSuperOttoDlg::ReSize() {
	POINT Newp;
	CRect recta;
	GetClientRect(&recta);
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;

	if (old.x <= 0 || old.y <= 0) return;
	float fx = (float)Newp.x / old.x;
	float fy = (float)Newp.y / old.y;
	if (fx <= 0.01f || fy <= 0.01f) return;

	// 统计子控件数量
	int count = 0;
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild) { count++; hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT); }
	if (count == 0) { old = Newp; return; }

	// 使用 DeferWindowPos 批量重定位，减少闪烁
	HDWP hdwp = BeginDeferWindowPos(count);
	if (!hdwp) { old = Newp; return; }

	hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild) {
		CRect r;
		::GetWindowRect(hwndChild, &r);
		ScreenToClient(&r);
		int nx = static_cast<int>(r.left * fx);
		int ny = static_cast<int>(r.top * fy);
		int nw = static_cast<int>(r.Width() * fx);
		int nh = static_cast<int>(r.Height() * fy);
		if (nw > 0 && nh > 0) {
			hdwp = DeferWindowPos(hdwp, hwndChild, NULL, nx, ny, nw, nh,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}

	EndDeferWindowPos(hdwp);
	old = Newp;
}

void CSuperOttoDlg::OnBnClickedOk() {
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CSuperOttoDlg::OnStnClickedArea() {
	// TODO: 在此添加控件通知处理程序代码
}


// 绘制路径
void CSuperOttoDlg::DrawPath(CDC* pDC, const std::vector<Point>& path, int cellSizeX, int cellSizeY) {

	CString strImagePath = GetPictureDirectory() + _T("path.png");  // 自动组合路径
	static CString s_cachedPathImage;
	static CImage s_pathImage;
	if (s_cachedPathImage != strImagePath) {
		if (!s_pathImage.IsNull()) {
			s_pathImage.Destroy();
		}
		s_pathImage.Load(strImagePath);
		s_cachedPathImage = strImagePath;
	}

	for (const Point& p : path) {
		CRect rect(p.x * cellSizeX, p.y * cellSizeY, (p.x + 1) * cellSizeX, (p.y + 1) * cellSizeY);
		if (isPictureload && !s_pathImage.IsNull()) {
			s_pathImage.Draw(m_dcMem.GetSafeHdc(), rect);
		}
		else {
			// 优化路径颜色 - 使用天蓝色
			m_dcMem.FillSolidRect(&rect, RGB(135, 206, 235));
		}
	}
}


// 控制机器人沿路径运动
void CSuperOttoDlg::MoveRobotAlongPath(const std::vector<Point>& path) {
	m_path = path;
	m_nPathIndex = 0;
	// 每五百秒移动一次
	if(path == path0)m_nTimerID0 = SetTimer(0, 300, NULL);
	else if (path == path1)m_nTimerID1 = SetTimer(1, 300, NULL);
	else if (path == path2)m_nTimerID2 = SetTimer(2, 300, NULL);

}

void CSuperOttoDlg::OnClickedButtonFindPath() {
	// TODO: 在此添加控件通知处理程序代码
	// 调用A*算法计算路径
	path0 = AStar(start, goal, grid);
	if (!path0.empty()) {
		MoveRobotAlongPath(path0);  // 控制机器人沿路径运动
	}
	else {
		AfxMessageBox(_T("无法找到可行路径！"));
	}
}

void CSuperOttoDlg::OnClickedButtonSetStart() {
	// TODO: 在此添加控件通知处理程序代码
	// 获取用户输入的起点位置
	int startX = 0;
	int startY = 0;
	startX = ((CComboBox*)GetDlgItem(IDC_COMBO_START_X))->GetCurSel();
	startY = ((CComboBox*)GetDlgItem(IDC_COMBO_START_Y))->GetCurSel();
	if (startX < 0 || startY < 0) {
		AfxMessageBox(_T("请选择有效的起点坐标"));
		return;
	}
	start = Point(startX, startY);
	robot = Robot(start.x, start.y);  // 重置机器人的位置
	Invalidate();  // 触发重绘
}

void CSuperOttoDlg::OnClickedButtonSetObstacle() {
	// TODO: 在此添加控件通知处理程序代码
	// 获取用户输入的障碍物位置，通过控件获取输入值
	int setobstacleX = 1;
	int setobstacleY  =1;
	setobstacleX = ((CComboBox*)GetDlgItem(IDC_COMBO_SETOBSTACLE_X))->GetCurSel();
	setobstacleY = ((CComboBox*)GetDlgItem(IDC_COMBO_SETOBSTACLE_Y))->GetCurSel();
	if (setobstacleX < 0 || setobstacleY < 0) {
		AfxMessageBox(_T("请选择有效的障碍物坐标"));
		return;
	}
	if (setobstacleX >= 0 && setobstacleX < grid.size() && setobstacleY >= 0 && setobstacleY < grid[0].size()) {
		// 检查是否是起点或终点
		if ((setobstacleX == start1.x && setobstacleY == start1.y) ||
			(setobstacleX == start2.x && setobstacleY == start2.y) ||
			(setobstacleX == goal1.x && setobstacleY == goal1.y) ||
			(setobstacleX == goal2.x && setobstacleY == goal2.y)) {
			AfxMessageBox(_T("不能在起点或终点位置设置障碍物！"));
			return;
		}
		// 清除旧的路径
		path0.clear();
		grid[setobstacleX][setobstacleY] = 1;
		//设置后添加菜单
		CString strX, strY;
		strX.Format(_T("%d"), setobstacleX);
		strY.Format(_T("%d"), setobstacleY);
		if (m_combo_clearobstacle_x.FindString(-1, strX) == CB_ERR) {
			m_combo_clearobstacle_x.AddString(strX);
		}

		if (m_combo_clearobstacle_y.FindString(-1, strY) == CB_ERR) {
			m_combo_clearobstacle_y.AddString(strY);
		}
		Invalidate();  // 触发重绘
	}
}

void CSuperOttoDlg::OnClickedButtonclearobstacle() {
	// TODO: 在此添加控件通知处理程序代码
	CString strX, strY;
	int clearobstacleX = 0, clearobstacleY = 0; // 初始化变量

	// 获取用户通过ComboBox选择的障碍物位置（假设存储的是文本坐标）
	CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
	CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);

	int selX = pComboX->GetCurSel();
	int selY = pComboY->GetCurSel();

	// 检查是否成功获取了选定的索引
	if (selX == CB_ERR || selY == CB_ERR) {
		AfxMessageBox(_T("请选择有效的障碍物坐标"));
		return;
	}

	// 从ComboBox中获取文本并转换为整数
	pComboX->GetLBText(selX, strX);
	pComboY->GetLBText(selY, strY);

	// 尝试将文本转换为整数
	clearobstacleX = _ttoi(strX);
	clearobstacleY = _ttoi(strY);

	 // 假设 grid 是一个二维数组，且已正确初始化
	 if (clearobstacleX >= 0 && clearobstacleX < grid.size() && clearobstacleY >= 0 && clearobstacleY < grid[0].size()) {
		 grid[clearobstacleX][clearobstacleY] = 0; // 清除障碍物

		 // 更新 ComboBox
		 CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
		 CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);
		 pComboX->DeleteString(pComboX->GetCurSel()); // 删除选中的项
		 pComboY->DeleteString(pComboY->GetCurSel()); // 删除选中的项

		 // 注意：删除后可能需要调整选中项，这里简单地选择第一个项
		 if (pComboX->GetCount() > 0) pComboX->SetCurSel(0);
		 if (pComboY->GetCount() > 0) pComboY->SetCurSel(0);

		 Invalidate(); // 触发重绘
	 }
	 else {
		 AfxMessageBox(_T("选择的障碍物坐标超出范围"));
	 }

}

/******************************************************************/
/*修改*/
void CSuperOttoDlg::OnBnClickedButtonclearobstacle2() {
	if (IsValidObstacle(robot1.position, clearobstacle1)) {
		// 假设 grid 是一个二维数组，且已正确初始化
		if (clearobstacle1X >= 0 && clearobstacle1X < grid.size() && clearobstacle1Y >= 0 && clearobstacle1Y < grid[0].size()) {
			grid[clearobstacle1X][clearobstacle1Y] = 0; // 清除障碍物
			Invalidate(); // 触发重绘
			robot1_turn = false;
			//房主清除障碍物
			if (isPVP()) {
				CString msg;
				msg.AppendFormat("@clear\n%d %d", clearobstacle1X, clearobstacle1Y);
				m_ServerSocket->Send(msg, msg.GetLength());
			}
		}
		else {
			AfxMessageBox(_T("选择的障碍物坐标超出范围"));
		}
	}
	else {
		AfxMessageBox(_T("选中方块非障碍物"));
	}
	isSelecting = false;
}
/*****************************************************/
/*修改*/
void CSuperOttoDlg::OnBnClickedButtonclearobstacle3() {
	if (IsValidObstacle(robot2.position, clearobstacle2)) {
		// 假设 grid 是一个二维数组，且已正确初始化
		if (clearobstacle2X >= 0 && clearobstacle2X < grid.size() && clearobstacle2Y >= 0 && clearobstacle2Y < grid[0].size()) {
			grid[clearobstacle2X][clearobstacle2Y] = 0; // 清除障碍物
			robot1_turn = true;
			//访客清除障碍物
			if (isPVP()) {
				CString msg;
				msg.AppendFormat("@clear\n%d %d", clearobstacle2X, clearobstacle2Y);
				m_ClientSocket->Send(msg, msg.GetLength());
			}

			Invalidate(); // 触发重绘
		}
		else {
			AfxMessageBox(_T("选择的障碍物坐标超出范围"));
		}
	}
	else {
		AfxMessageBox(_T("选中方块非障碍物"));
	}
	isSelecting = false;
}

void CSuperOttoDlg::OnClickedButtonSetGoal() {
	// TODO: 在此添加控件通知处理程序代码
	// 获取用户输入的终点位置，这里假设通过控件获取输入值
	// 获取用户输入的终点位置
	int goalX = 19;
	int goalY = 19;
	goalX = ((CComboBox*)GetDlgItem(IDC_COMBO_SETGOAL_X))->GetCurSel();
	goalY = ((CComboBox*)GetDlgItem(IDC_COMBO_SETGOAL_Y))->GetCurSel();
	if (goalX < 0 || goalY < 0) {
		AfxMessageBox(_T("请选择有效的终点坐标"));
		return;
	}
	goal = Point(goalX, goalY);
	Invalidate();  // 触发重绘
}

void CSuperOttoDlg::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == m_nTimerID1) {
		if (m_nPathIndex < m_path.size()) {
			Point p1 = m_path[m_nPathIndex];
			robot1.MoveTo(p1.x, p1.y);
			m_nPathIndex++;
			Invalidate(FALSE);
		}
		else {
			start1 = goal1;
			robot1.SetPosition(goal1.x, goal1.y);
			path1.clear();
			robot1_turn = false;
			KillTimer(m_nTimerID1);
			m_nTimerID1 = 0;
			CheckRobotReachedTrophy();
			Invalidate(FALSE);
		}
	}
	if (nIDEvent == m_nTimerID2) {
		if (m_nPathIndex < m_path.size()) {
			Point p2 = m_path[m_nPathIndex];
			robot2.MoveTo(p2.x, p2.y);
			m_nPathIndex++;
			Invalidate(FALSE);
		}
		else {
			start2 = goal2;
			robot2.SetPosition(goal2.x, goal2.y);
			path2.clear();
			robot1_turn = true;
			KillTimer(m_nTimerID2);
			m_nTimerID2 = 0;
			CheckRobotReachedTrophy();
			Invalidate(FALSE);
		}
	}
	if (text.GetLength() > 500) {
		text.Empty();
		text += "详细过多，自动删除\r\n";
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CSuperOttoDlg::OnMouseMove(UINT nFlags, CPoint point) {
	CWnd* pWnd = GetDlgItem(IDC_AREA);
	if (pWnd == nullptr || grid.empty() || grid[0].empty()) {
		CDialogEx::OnMouseMove(nFlags, point);
		return;
	}

	CRect areaRect;
	pWnd->GetWindowRect(&areaRect);
	ScreenToClient(&areaRect);
	if (!areaRect.PtInRect(point)) {
		if (selectedX != -1 || selectedY != -1) {
			selectedX = -1;
			selectedY = -1;
			if (!m_nTimerID1 && !m_nTimerID2 && !GetUpdateRect(NULL, FALSE)) {
				Invalidate(FALSE);
			}
		}
		CDialogEx::OnMouseMove(nFlags, point);
		return;
	}

	const int localX = point.x - areaRect.left;
	const int localY = point.y - areaRect.top;
	const int cellSizeX = max(1, areaRect.Width() / static_cast<int>(grid.size()));
	const int cellSizeY = max(1, areaRect.Height() / static_cast<int>(grid[0].size()));
	const int x = localX / cellSizeX;
	const int y = localY / cellSizeY;

	if (x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && isSelecting == false) {
		if (x != selectedX || y != selectedY) {
			selectedX = x;
			selectedY = y;
			// 只在没有待处理的WM_PAINT时才刷新，避免鼠标快速移动时大量重绘
			if (!m_nTimerID1 && !m_nTimerID2 && !GetUpdateRect(NULL, FALSE)) {
				Invalidate(FALSE);
			}
		}
		CString strX, strY;
		strX.Format(_T("%d"), selectedX);
		strY.Format(_T("%d"), selectedY);

		// 将选中的坐标显示在下拉列表中
		((CComboBox*)GetDlgItem(IDC_COMBO_START_X))->SelectString(0, strX);
		((CComboBox*)GetDlgItem(IDC_COMBO_START_Y))->SelectString(0, strY);
		((CComboBox*)GetDlgItem(IDC_COMBO_SETOBSTACLE_X))->SelectString(0, strX);
		((CComboBox*)GetDlgItem(IDC_COMBO_SETOBSTACLE_Y))->SelectString(0, strY);
		((CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X))->SelectString(0, strX);
		((CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y))->SelectString(0, strY);
		((CComboBox*)GetDlgItem(IDC_COMBO_SETGOAL_X))->SelectString(0, strX);
		((CComboBox*)GetDlgItem(IDC_COMBO_SETGOAL_Y))->SelectString(0, strY);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CSuperOttoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
	CWnd* pArea = GetDlgItem(IDC_AREA);
	CRect areaRect;
	if (pArea != nullptr) {
		pArea->GetWindowRect(&areaRect);
		ScreenToClient(&areaRect);
	}
	if (pArea != nullptr && areaRect.PtInRect(point)) {
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		isSelecting = isSelecting ? false : true;
		if (robot1_turn) {
			// 设置goal1，确保其在有效范围内
			Point new_goal(selectedX, selectedY);
			if (selectedX != -1 && selectedY != -1 && IsValidGoal(robot1.position, new_goal)) {
				goal1 = new_goal;
				Invalidate();
			}
			else if (selectedX != -1 && selectedY != -1 && IsValidObstacle(robot1.position, new_goal)) {
				clearobstacle1 = new_goal;
				clearobstacle1X = new_goal.x;
				clearobstacle1Y = new_goal.y;
				Invalidate();
			}
			else {
				AfxMessageBox(_T("目标选择超范围"));
				isSelecting = false;
			}
		}
		else if (!robot1_turn) {
			// 设置goal2，确保其在有效范围内
			Point new_goal(selectedX, selectedY);
			if (selectedX != -1 && selectedY != -1 && IsValidGoal(robot2.position, new_goal)) {
				goal2 = new_goal;
				Invalidate();
			}
			else if (selectedX != -1 && selectedY != -1 && IsValidObstacle(robot2.position, new_goal)) {
				clearobstacle2 = new_goal;
				clearobstacle2X = new_goal.x;
				clearobstacle2Y = new_goal.y;
				Invalidate();
			}
			else {
				AfxMessageBox(_T("目标选择超范围"));
				isSelecting = false;
			}
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CSuperOttoDlg::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//isSelecting = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


LRESULT CSuperOttoDlg::OnSocket(WPARAM wParam, LPARAM lParam) {
	// 处理你的自定义消息
	return 0;
}


void CSuperOttoDlg::OnClickedButtonrobot1setgoal() {
	if (robot1_turn) {
		// 设置goal1，确保其在有效范围内
		Point new_goal(selectedX, selectedY);
		if (selectedX != -1 && selectedY != -1 && IsValidGoal(robot1.position, new_goal)) {
			goal1 = new_goal;
			Invalidate();
		}
		else {
			AfxMessageBox(_T("终点设置错误"));
		}
	}
	else {
		AfxMessageBox(_T("玩家2 回合"));
	}
}
/*修改*/
void CSuperOttoDlg::OnBnClickedButtonrobot1findpath() {
	// TODO: 在此添加控件通知处理程序代码
	if (IsValidObstacle(robot1.position, clearobstacle1)) {
		AfxMessageBox(_T("选中方块为障碍物"));
	}
	else {
		if (robot1_turn) {
			path1 = AStar(robot1.position, goal1, grid);
			if (!path1.empty() && !IsValidObstacle(robot1.position, goal1)) {
				if (isPVP())
					SendPath(path1, start1, goal1);//发送
				start1 = robot1.position;
				selectedX = -1;
				selectedY = -1;
				MoveRobotAlongPath(path1);  // 控制机器人沿路径运动
				isSelecting = false;
				path2.clear();
				Invalidate();
			}
			else {
				AfxMessageBox(_T("无法找到可行路径！"));
			}
		}
		else {
			AfxMessageBox(_T("玩家2 回合"));
		}
	}
}


void CSuperOttoDlg::OnBnClickedButtonrobot2setgoal() {
	// TODO: 在此添加控件通知处理程序代码
	if (!robot1_turn) {
		// 设置goal2，确保其在有效范围内
		Point new_goal(selectedX, selectedY);
		if (selectedX != -1 && selectedY != -1 && IsValidGoal(robot2.position, new_goal)) {
			goal2 = new_goal;
			Invalidate();
		}
		else {
			AfxMessageBox(_T("终点设置错误"));
		}
	}
	else {
		AfxMessageBox(_T("玩家1 回合"));
	}
}
/*修改*/
void CSuperOttoDlg::OnBnClickedButtonrobot2findpath() {
	// TODO: 在此添加控件通知处理程序代码
	if (IsValidObstacle(robot2.position, clearobstacle2)) {
		AfxMessageBox(_T("选中方块为障碍物"));
	}
	else {
		if (!robot1_turn) {
			path2 = AStar(robot2.position, goal2, grid);
			if (!path2.empty()) {

				if (isPVP())
					SendPath(path2, start2, goal2);


				start2 = robot2.position;
				selectedX = -1;
				selectedY = -1;
				MoveRobotAlongPath(path2);  // 控制机器人沿路径运动
				path1.clear();
				isSelecting = false;
				Invalidate();


			}
			else {
				AfxMessageBox(_T("无法找到可行路径！"));
			}
		}
		else {
			AfxMessageBox(_T("玩家1 回合"));
		}
	}
}

/****************************************************/
/*修改*/
//将奖杯的位置记录
void CSuperOttoDlg::PlaceRandomTrophy() {
	if (!isPVP() || ID == 0) {
		do {
			Trophy_x = RandomNumber(0, static_cast<int>(grid.size()) - 1);
			Trophy_y = RandomNumber(0, static_cast<int>(grid[0].size()) - 1);
		} while (grid[Trophy_x][Trophy_y] != 0);
		grid[Trophy_x][Trophy_y] = 2;
		Invalidate();
		CString TrophyPlace;
		TrophyPlace.Format("@Trophy\n%d %d", Trophy_x, Trophy_y);
		if (ID == 0 && isPVP() && !NoClient)
			m_ServerSocket->Send(TrophyPlace, TrophyPlace.GetLength());
	}
}




void CSuperOttoDlg::HighlightSurroundingCells(const Point& start) {
	int x = start.x;
	int y = start.y;

	CWnd* pWnd = GetDlgItem(IDC_AREA);
	CRect rect2;
	pWnd->GetClientRect(&rect2);

	int gridW = static_cast<int>(grid.size());
	int gridH = static_cast<int>(grid[0].size());
	int cellSizeX = rect2.Width() / gridW;
	int cellSizeY = rect2.Height() / gridH;

	const CString rangeImagePath = GetPictureDirectory() + _T("range.png");
	static CString s_cachedRangeImage;
	static CImage s_rangeImage;
	if (s_cachedRangeImage != rangeImagePath) {
		if (!s_rangeImage.IsNull()) {
			s_rangeImage.Destroy();
		}
		s_rangeImage.Load(rangeImagePath);
		s_cachedRangeImage = rangeImagePath;
	}

	// BFS 计算实际可达区域（考虑障碍物阻挡路径）
	std::vector<std::vector<int>> dist(gridW, std::vector<int>(gridH, -1));
	std::queue<Point> q;
	dist[x][y] = 0;
	q.push(start);

	const int dx4[] = { 1, -1, 0, 0 };
	const int dy4[] = { 0, 0, 1, -1 };

	while (!q.empty()) {
		Point cur = q.front();
		q.pop();
		int curDist = dist[cur.x][cur.y];
		if (curDist >= temp_range) continue;

		for (int d = 0; d < 4; d++) {
			int nx = cur.x + dx4[d];
			int ny = cur.y + dy4[d];
			if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH &&
				dist[nx][ny] == -1 && grid[nx][ny] != 1 && grid[nx][ny] != 2) {
				dist[nx][ny] = curDist + 1;
				q.push(Point(nx, ny));
			}
		}
	}

	// 绘制可达区域
	for (int i = 0; i < gridW; ++i) {
		for (int j = 0; j < gridH; ++j) {
			if (dist[i][j] >= 0) {
				CRect rect(i * cellSizeX, j * cellSizeY, (i + 1) * cellSizeX, (j + 1) * cellSizeY);
				if (isPictureload && !s_rangeImage.IsNull()) {
					s_rangeImage.Draw(m_dcMem.GetSafeHdc(), rect);
				}
				else {
					m_dcMem.FillSolidRect(&rect, RGB(175, 238, 238));
				}
			}
		}
	}
}
bool CSuperOttoDlg::IsValidGoal(const Point& robot_pos, const Point& goal) {
	std::vector<Point> ispath_range;
	if (goal.x >= 0 && goal.x < grid.size() &&
		goal.y >= 0 && goal.y < grid[0].size() &&
		grid[goal.x][goal.y] != 1) {
		ispath_range = AStar(robot_pos, goal, grid);
	}
	return (ispath_range.size() <= temp_range + 1 && !ispath_range.empty());
}

// BFS 检查从起点能否到达终点（不经过障碍物）
static bool CanReach(const std::vector<std::vector<int>>& grid, Point from, Point to) {
	int w = static_cast<int>(grid.size());
	int h = static_cast<int>(grid[0].size());
	if (from.x < 0 || from.x >= w || from.y < 0 || from.y >= h) return false;
	if (to.x < 0 || to.x >= w || to.y < 0 || to.y >= h) return false;
	if (grid[from.x][from.y] == 1 || grid[to.x][to.y] == 1) return false;

	std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, false));
	std::queue<Point> q;
	visited[from.x][from.y] = true;
	q.push(from);
	const int dx4[] = { 1, -1, 0, 0 };
	const int dy4[] = { 0, 0, 1, -1 };
	while (!q.empty()) {
		Point cur = q.front(); q.pop();
		if (cur.x == to.x && cur.y == to.y) return true;
		for (int d = 0; d < 4; d++) {
			int nx = cur.x + dx4[d], ny = cur.y + dy4[d];
			if (nx >= 0 && nx < w && ny >= 0 && ny < h &&
				!visited[nx][ny] && grid[nx][ny] != 1) {
				visited[nx][ny] = true;
				q.push(Point(nx, ny));
			}
		}
	}
	return false;
}

void CSuperOttoDlg::PlaceRandomObstacles(int numObstacles) {
	int gridW = static_cast<int>(grid.size());
	int gridH = static_cast<int>(grid[0].size());
	int count = 0;
	int maxAttempts = numObstacles * 10;
	int attempts = 0;

	while (count < numObstacles && attempts < maxAttempts) {
		attempts++;
		int x = RandomNumber(0, gridW - 1);
		int y = RandomNumber(0, gridH - 1);

		// 跳过起点、终点、奖杯和已有障碍物
		if (grid[x][y] != 0) continue;
		if (x == start1.x && y == start1.y) continue;
		if (x == start2.x && y == start2.y) continue;
		if (x == goal1.x && y == goal1.y) continue;
		if (x == goal2.x && y == goal2.y) continue;

		// 临时放置障碍物，检查是否会导致玩家被困
		grid[x][y] = 1;
		bool ok = CanReach(grid, start1, start2) &&
		          CanReach(grid, start1, goal1) &&
		          CanReach(grid, start2, goal2);
		if (ok) {
			count++;
		} else {
			grid[x][y] = 0;  // 撤销：会导致困死
		}
	}
	Invalidate();
}

void CSuperOttoDlg::CheckRobotReachedTrophy() {
	if (grid[start1.x][start1.y] == 2) {
		robot1_score++;
		CString str;
		str.Format("玩家1:%d分", robot1_score);
		GetDlgItem(IDC_SCORE1)->SetWindowText(str);
		AfxMessageBox(_T("机器人1获得一个奖杯！"));
		grid[start1.x][start1.y] = 0;
		PlaceRandomTrophy();  // 重新生成奖杯
		if (isPVP() && ID == 0) {
			CString msg("@win 1");
			m_ServerSocket->Send(msg, msg.GetLength());
		}
	}
	else if (grid[start2.x][start2.y] == 2) {
		robot2_score++;
		CString str;
		str.Format("玩家2:%d分", robot2_score);
		GetDlgItem(IDC_SCORE2)->SetWindowText(str);
		AfxMessageBox(_T("机器人2获得一个奖杯！"));
		grid[start2.x][start2.y] = 0;
		PlaceRandomTrophy();  // 重新生成奖杯
		if (isPVP() && ID == 0) {
			CString msg("@win 2");
			m_ServerSocket->Send(msg, msg.GetLength());
		}
	}
}

void CSuperOttoDlg::HighlightSelectedCell(int x, int y, COLORREF fillColor) {
	// 获取控件的DC
	CWnd* pWnd = GetDlgItem(IDC_AREA);
	CDC* pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);

	int cellSizeX = rect.Width() / grid.size();
	int cellSizeY = rect.Height() / grid[0].size();
	// 确保cellSizeX和cellSizeY是奇数，以便圆心位于单元格中央
	if (cellSizeX % 2 == 0) cellSizeX++;
	if (cellSizeY % 2 == 0) cellSizeY++;

	// 绘制选中的单元格（使用圆代替填充）
	CRect cellRect(x * cellSizeX, y * cellSizeY, (x + 1) * cellSizeX, (y + 1) * cellSizeY);
	// 创建一个CBrush对象用于填充圆
	CBrush brush(fillColor);
	CBrush* pOldBrush = m_dcMem.SelectObject(&brush); // 保存旧的刷子并选入新的刷子
	// 计算圆心的位置
	int centerX = cellRect.left + cellSizeX / 2;
	int centerY = cellRect.top + cellSizeY / 2;
	// 圆的半径，这里简单地取宽或高的一半（取较小值以避免溢出边界）
	int radius = min(cellSizeX, cellSizeY) / 2;

	// 绘制圆形
	m_dcMem.Ellipse(centerX - radius, centerY - radius, centerX + radius, centerY + radius);
	// 恢复旧的刷子
	m_dcMem.SelectObject(pOldBrush);

	pWnd->ReleaseDC(pDC);
}



BOOL CSuperOttoDlg::OnEraseBkgnd(CDC* pDC) {
	return FALSE;
}


void CSuperOttoDlg::OnClickedButtonreload() {
	// TODO: 在此添加控件通知处理程序代码
	if (ID == 0) {
		people = 0;
	}
	CString strX, strY,strNUM,strRANGE;
	GetDlgItemText(IDC_EDIT_LINE, strX);
	GetDlgItemText(IDC_EDIT_ROW, strY);
	GetDlgItemText(IDC_EDIT_OBSATACLENUM, strNUM);
	GetDlgItemText(IDC_EDIT_RANGE, strRANGE);
	temp_x = _ttoi(strX);
	temp_y = _ttoi(strY);
	temp_obstacle_num = _ttoi(strNUM);
	temp_range = _ttoi(strRANGE);

	// 动态障碍物上限：总格数的30%，至少留出路径空间
	int maxObstacles = max(1, temp_x * temp_y * 3 / 10);
	if (temp_obstacle_num > maxObstacles) {
		CString msg;
		msg.Format(_T("障碍物数目过多！当前地图最大建议 %d 个，已自动调整。"), maxObstacles);
		AfxMessageBox(msg);
		temp_obstacle_num = maxObstacles;
		strNUM.Format(_T("%d"), temp_obstacle_num);
	}
	else if (temp_obstacle_num < 0) {
		AfxMessageBox(_T("障碍物数目过少！"));
	}
	else if (temp_range > temp_x || temp_range > temp_y) {
		AfxMessageBox(_T("行动范围过大！"));
	}
	else if (temp_range <0){
		AfxMessageBox(_T("行动范围过小！"));
	}
	else if (temp_x < 3 || temp_y<3) {
		AfxMessageBox(_T("地图范围过小！"));
	}
	else {// 初始化网格大小（假设30x30的网格）
		if (temp_x > 40 || temp_y > 40) {
			AfxMessageBox(_T("地图范围过大！会卡卡卡卡卡的"));
	    }
		grid = std::vector<std::vector<int>>(temp_x, std::vector<int>(temp_y, 0));
		//填充下拉栏菜单
		m_combo_start_x.ResetContent();
		m_combo_start_y.ResetContent();
		m_combo_setobstacle_x.ResetContent();
		m_combo_setobstacle_y.ResetContent();
		m_combo_clearobstacle_x.ResetContent();
		m_combo_clearobstacle_y.ResetContent();
		m_combo_setgoal_x.ResetContent();
		m_combo_setgoal_y.ResetContent();

		for (int i = 0; i < static_cast<int>(grid.size()); ++i) {
			CString str;
			str.Format(_T("%d"), i);
			m_combo_start_x.InsertString(i, str);
			m_combo_setobstacle_x.InsertString(i, str);
			m_combo_clearobstacle_x.InsertString(i, str);
			m_combo_setgoal_x.InsertString(i, str);
		}
		for (int i = 0; i < static_cast<int>(grid[0].size()); ++i) {
			CString str;
			str.Format(_T("%d"), i);
			m_combo_start_y.InsertString(i, str);
			m_combo_setobstacle_y.InsertString(i, str);
			m_combo_clearobstacle_y.InsertString(i, str);
			m_combo_setgoal_y.InsertString(i, str);
		}
		// 设定初始起点和终点
		start = Point(0, 0);
		goal = Point(9, 9);
		goal1 = Point(0, 0);
		goal2 = Point(grid.size() - 1, grid[0].size() - 1);
		robot = Robot(start.x, start.y);
		start1 = Point(0, 0);
		start2 = Point(grid.size() - 1, grid[0].size() - 1);
		robot1 = Robot(start1.x, start1.y);
		robot2 = Robot(start2.x, start2.y);
		robot1_score = 0;
		robot2_score = 0;
		robot1_turn = true;/////
		GetDlgItem(IDC_EDIT_LINE)->SetWindowText(strX);
		GetDlgItem(IDC_EDIT_ROW)->SetWindowText(strY);
		GetDlgItem(IDC_EDIT_OBSATACLENUM)->SetWindowText(strNUM);
		GetDlgItem(IDC_EDIT_RANGE)->SetWindowText(strRANGE);
		CString str;
		str.Format("玩家1:%d分", robot1_score);
		GetDlgItem(IDC_SCORE1)->SetWindowText(str);
		str.Format("玩家2:%d分", robot2_score);
		GetDlgItem(IDC_SCORE2)->SetWindowText(str);
		GetDlgItem(IDC_SHOW)->SetWindowText(TEXT("玩家1回合"));
		PlaceRandomTrophy();
		PlaceRandomObstacles(temp_obstacle_num);
		isSelecting = false;
		if (isPVP() && !NoClient) {
			SendMap(grid, 1);

			Sleep(1);
			text += "正在重置地图，等待对方同步...";
			GetDlgItem(IDC_TEXT)->SetWindowText(text);
			isloading = 1;
			GetDlgItem(IDC_TEXT)->SetWindowText(text);
			GetDlgItem(IDC_IPADDRESS1)->EnableWindow(false); GetDlgItem(IDC_IPADDRESS1)->EnableWindow(false);
			GetDlgItem(IDC_ID_COMBO)->EnableWindow(false);
			GetDlgItem(IDC_SEND)->EnableWindow(false);
			GetDlgItem(IDC_CONNECT)->EnableWindow(false);
			GetDlgItem(IDC_BUTTONROBOT1SETGOAL)->EnableWindow(false);
			GetDlgItem(IDC_BUTTONROBOT1FINDPATH)->EnableWindow(false);
			GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(false);
			GetDlgItem(IDC_BUTTONROBOT2SETGOAL)->EnableWindow(false);
			GetDlgItem(IDC_BUTTONROBOT2FINDPATH)->EnableWindow(false);
			GetDlgItem(ID_ButtonClearObstacle3)->EnableWindow(false);
			GetDlgItem(IDC_AREA)->EnableWindow(false);
		}
		Invalidate();
	}
}


void CSuperOttoDlg::OnBnClickedButtonadd() {
	// TODO: 在此添加控件通知处理程序代码

	CString musicName;
	CFileDialog dlgFile(TRUE, NULL, NULL,
	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER,
		_T("All files|*.mp3;*.wma;*.wav|MP3 (*.MP3)|*.mp3|WMA (*.wma)|*.wma||"),
		NULL);
	if (dlgFile.DoModal() == IDOK) {
		fullPath = dlgFile.GetPathName();
		musicName = dlgFile.GetFileName();

		preLoadMusicPath.push_back(fullPath);
		int pos = fullPath.ReverseFind(_T('\\'));

		preLoadMusicName.push_back(fullPath.Mid(pos + 1, fullPath.GetLength()));
		listBox.InsertString(preLoadMusicName.size()-1, musicName);
		SetDlgItemText(IDC_EDIT1, fullPath);//img_select为选择的文件的路径，IDC_IMGSELECTEDIT为edit控件名称

		// 确保 fullPath 有效，且指向的文件存在
		if (fullPath.IsEmpty()) {
			AfxMessageBox(_T("请选择一个有效的音乐文件。"));
			return;
		}
		// 关闭之前的设备
		if (m_IDdevice != 0) {
			mciSendCommand(m_IDdevice, MCI_CLOSE, 0, 0);
		}

		//打开一首歌曲
		MCI_OPEN_PARMS mciOpenParms = {0};
		mciOpenParms.lpstrElementName = fullPath;
		mciOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;

		curPlayPos = preLoadMusicName.size() - 1;//当前的播放位置，播放第几首歌曲，这的意思就是播放刚添加的歌曲
		DWORD dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT, (DWORD_PTR)&mciOpenParms);
		if (dwReturn != 0) {
			// 处理错误
			TCHAR errorText[128];
			mciGetErrorString(dwReturn, errorText, sizeof(errorText));
			AfxMessageBox(errorText);
			return;
		}
		m_IDdevice = mciOpenParms.wDeviceID;
	}
}


void CSuperOttoDlg::OnBnClickedButtonplay() {
	// TODO: 在此添加控件通知处理程序代码

	if (curPlayPos < 0 || curPlayPos >= preLoadMusicName.size()) {
		MessageBox(_T("Invalid play position"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	SetDlgItemText(IDC_EDIT2, preLoadMusicName[curPlayPos]);

	// 如果设备未打开（预加载歌曲），先打开设备
	if (m_IDdevice == 0 && curPlayPos < preLoadMusicPath.size()) {
		CString musicPath = preLoadMusicPath[curPlayPos];
		if (!PathFileExists(musicPath)) {
			AfxMessageBox(_T("音乐文件不存在: ") + musicPath);
			return;
		}
		MCI_OPEN_PARMS mciOpenParms = {0};
		mciOpenParms.lpstrElementName = musicPath;
		mciOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
		DWORD dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT, (DWORD_PTR)&mciOpenParms);
		if (dwReturn != 0) {
			TCHAR errorText[128];
			mciGetErrorString(dwReturn, errorText, sizeof(errorText));
			AfxMessageBox(errorText);
			return;
		}
		m_IDdevice = mciOpenParms.wDeviceID;
	}

	MCI_PLAY_PARMS mciPlayParms = {0};
	mciPlayParms.dwCallback = NULL;
	mciPlayParms.dwFrom = 0;

	mciSendCommand(m_IDdevice, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD_PTR)&mciPlayParms);
}


void CSuperOttoDlg::UpdateVolumeFromSlider() {
	int pos = m_volumeSlider.GetPos();
	int vol = pos;
	DWORD dwVol = (DWORD)(vol * 65535 / 100);
	DWORD dwFullVol = (dwVol << 16) | dwVol;
	waveOutSetVolume(NULL, dwFullVol);
	CString label;
	label.Format(_T("%d%%"), vol);
	m_volumeLabel.SetWindowText(label);
}

void CSuperOttoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (pScrollBar && pScrollBar->GetDlgCtrlID() == 1070) {
		UpdateVolumeFromSlider();
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSuperOttoDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (pScrollBar && pScrollBar->GetDlgCtrlID() == 1070) {
		UpdateVolumeFromSlider();
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CSuperOttoDlg::OnBnClickedButtonpause() {
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_BUTTONPAUSE, str);
	if (str == _T("暂停")) {
		mciSendCommand(m_IDdevice, MCI_PAUSE, 0, 0);
		SetDlgItemText(IDC_BUTTONPAUSE, _T("继续"));
	}
	else {
		mciSendCommand(m_IDdevice, MCI_RESUME, 0, 0);
		SetDlgItemText(IDC_BUTTONPAUSE, _T("暂停"));
	}
}


void CSuperOttoDlg::OnBnClickedButtonstop() {
	// TODO: 在此添加控件通知处理程序代码
	//SetDlgItemText(IDC_EDIT2, "");
	mciSendCommand(m_IDdevice, MCI_STOP, 0, 0);
    if (m_IDdevice != 0) {
        mciSendCommand(m_IDdevice, MCI_CLOSE, 0, 0);
        m_IDdevice = 0;  // Reset device ID
    }
}


void CSuperOttoDlg::OnBnClickedButtondel() {
	//// TODO: 在此添加控件通知处理程序代码
	int pos = listBox.GetCurSel();
	if (pos < 0 || pos >= preLoadMusicName.size()) {
		MessageBox(_T("Invalid selection"), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}

	listBox.ResetContent();
	preLoadMusicPath.erase(preLoadMusicPath.begin() + pos);
	preLoadMusicName.erase(preLoadMusicName.begin() + pos);

	for (int i = 0; i < preLoadMusicName.size(); i++) {
		listBox.InsertString(i,preLoadMusicName[i]);
	}

	curPlayPos = 0;
}


void CSuperOttoDlg::OnBnClickedButtonlast() {
	//// TODO: 在此添加控件通知处理程序代码
	if (curPlayPos <= 0) {
		MessageBox(_T("已经到达第一首歌曲"));
		return;
	}

	curPlayPos--;
	listBox.SetCurSel(curPlayPos);
	mciSendCommand(m_IDdevice, MCI_CLOSE, 0, 0);

	MCI_OPEN_PARMS mciOpenParms = { 0 };
	mciOpenParms.lpstrElementName = preLoadMusicPath[curPlayPos];
	DWORD dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT, (DWORD_PTR)&mciOpenParms);
	if (dwReturn != 0) {
		TCHAR errorText[128];
		mciGetErrorString(dwReturn, errorText, sizeof(errorText));
		AfxMessageBox(errorText);
		return;
	}
	m_IDdevice = mciOpenParms.wDeviceID;
	OnBnClickedButtonplay();
}


void CSuperOttoDlg::OnBnClickedButtonnext() {
	// TODO: 在此添加控件通知处理程序代码
	if (curPlayPos >= preLoadMusicName.size() - 1) {
		MessageBox(_T("已经到达最后一首歌曲"));
		return;
	}

	curPlayPos++;
	listBox.SetCurSel(curPlayPos);
	mciSendCommand(m_IDdevice, MCI_CLOSE, 0, 0);

	MCI_OPEN_PARMS mciOpenParms = { 0 };
	mciOpenParms.lpstrElementName = preLoadMusicPath[curPlayPos];
	DWORD dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT, (DWORD_PTR)&mciOpenParms);
	if (dwReturn != 0) {
		TCHAR errorText[128];
		mciGetErrorString(dwReturn, errorText, sizeof(errorText));
		AfxMessageBox(errorText);
		return;
	}
	m_IDdevice = mciOpenParms.wDeviceID;
	OnBnClickedButtonplay();
}


void CSuperOttoDlg::OnBnClickedCancel() {
	// TODO: 在此添加控件通知处理程序代码
	CString filename = GetExeDirectory() + _T("\\musicPath.txt");
	CStdioFile csFile;
	CFileException cfException;
	if (csFile.Open(filename, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite, &cfException)) {
		csFile.SetLength(0);
		csFile.SeekToBegin();
		setlocale(LC_CTYPE, "chs");

		for (const auto& path : preLoadMusicPath) {
			csFile.WriteString(path + _T("\n"));
		}
	}
	csFile.Close();
	CDialogEx::OnCancel();
}
/*****************************************************************************************************************/
/*										以下函数为网络通信相关														*/




void CSuperOttoDlg::OnBnClickedCheck() {
	// TODO: 在此添加控件通知处理程序代码
	if ((((CButton*)GetDlgItem(IDC_CHECK))->GetCheck()) == TRUE) {
		isPictureload = false;
	}
	else {
		isPictureload = true;
	}
	Invalidate();
}

// 验证坐标有效性
void CSuperOttoDlg::ValidateAndFixCoordinates(int& x, int& y, const CString& context) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= grid.size()) x = grid.size() - 1;
	if (y >= grid[0].size()) y = grid[0].size() - 1;
}
