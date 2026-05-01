
// SuperOttoDlg.h: 头文件
//

#pragma once
#include "Robot.h"
#include <vector>

#include "AStar.h"
#include "afxdialogex.h"

/****************************
	kbk:非常重要的头文件
***************/
#include "MSocket.h"
#define WM_SOCKET (WM_USER + 1)
// CSuperOttoDlg 对话框

#include <shlwapi.h>  // 用于 PathRemoveFileSpec
#pragma comment(lib, "shlwapi.lib")  // 链接shlwapi.lib

inline CString GetExeDirectory() {
	TCHAR exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH);  // 获取EXE完整路径
	PathRemoveFileSpec(exePath);                 // 去除文件名，只保留目录
	return CString(exePath);
}

// 获取资源目录（picturetest）- 自动创建
inline CString GetPictureDirectory() {
	CString picDir = GetExeDirectory() + _T("\\picturetest\\");
	// 自动创建目录
	if (!PathFileExists(picDir)) {
		CreateDirectory(picDir, NULL);
	}
	return picDir;
}

// 获取音乐目录 - 自动创建
inline CString GetMusicDirectory() {
	CString musicDir = GetExeDirectory() + _T("\\music\\");
	if (!PathFileExists(musicDir)) {
		CreateDirectory(musicDir, NULL);
	}
	return musicDir;
}

/* kbk :非常重要的预声明*/
class MSocket;
class CSuperOttoDlg : public CDialogEx
{
// 构造
public:
	CSuperOttoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUPEROTTO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//变量声明
	void ReSize();		//对话框最大化时控件随比例最大化或者还原
	POINT old;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedArea();
	afx_msg void OnClickedButtonFindPath();
	afx_msg void OnClickedButtonSetGoal();
	afx_msg void OnClickedButtonSetObstacle();
	afx_msg void OnClickedButtonSetStart();
	afx_msg void OnClickedButtonclearobstacle();

private:
	//void DrawGrid(CDC* pDC);  // 绘制网格
	void DrawPath(CDC* pDC, const std::vector<Point>& path, int cellSizeX, int cellSizeY);  // 绘制路径
	void MoveRobotAlongPath(const std::vector<Point>& path);  // 控制机器人沿路径运动
	void UpdateDisplayArea();//自动更新显示区域
	void PlaceRandomTrophy();
	void ValidateAndFixCoordinates(int& x, int& y, const CString& context);  // 验证坐标有效性

	std::vector<std::vector<int>> grid;  // 表示工作环境的网格
	Point start,start1,start2,goal,goal1,goal2;  // 起点和终点
	Robot robot, robot1, robot2;  // 机器人对象
	std::vector<Point> path0,path1,path2;  // 路径
	const int cellSize = 20;  // 每个网格单元的大小
	Point clearobstacle1, clearobstacle2;
	int clearobstacle1X = 0, clearobstacle1Y = 0; // 初始化变量
	int clearobstacle2X = 0, clearobstacle2Y = 0; // 初始化变量
	UINT_PTR m_nTimerID0, m_nTimerID1, m_nTimerID2;
	int m_nPathIndex;
	std::vector<Point> m_path;

	CDC m_dcMem;     // 内存DC
	CBitmap m_bitmap; // 位图
	CBitmap* m_pOldBitmap; // 用于保存旧的位图

	//鼠标选择
	int selectedX;
	int selectedY;
	bool isSelecting;
	bool robot1_turn = true; // 开始时由robot1操作

	bool IsValidGoal(const Point& robot_pos, const Point& goal);

	bool IsValidObstacle(const Point& robot_pos, const Point& goal) {
		int dx = abs(robot_pos.x - goal.x);
		int dy = abs(robot_pos.y - goal.y);
		return (dx <= temp_range && dy <= temp_range) && grid[goal.x][goal.y] == 1;
	}

	void HighlightSurroundingCells(const Point& start);
	void PlaceRandomObstacles(int numObstacles);
	void CheckRobotReachedTrophy();
	void HighlightSelectedCell(int x, int y, COLORREF fillColor);
	//玩家分数
	int robot1_score;
	int robot2_score;

	//重载函数变量
	int temp_x, temp_y, temp_obstacle_num,temp_range;
	//
	bool isPictureload;
public:
	CComboBox m_combo_start_x;
	CComboBox m_combo_start_y;
	CComboBox m_combo_setobstacle_x;
	CComboBox m_combo_setobstacle_y;
	CComboBox m_combo_clearobstacle_x;
	CComboBox m_combo_clearobstacle_y;
	CComboBox m_combo_setgoal_x;
	CComboBox m_combo_setgoal_y;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	SOCKET s;
	SOCKET s1;
	int n;
	sockaddr_in addr;
	sockaddr_in addr1;
	CButton m_btn_robot1_setgoal;
	CButton m_btn_robot1_findpath;
	CButton m_btn_robot2_setgoal;
	CButton m_btn_robot2_findpath;
	afx_msg void OnBnClickedButtonrobot1findpath();
	afx_msg void OnBnClickedButtonrobot2setgoal();
	afx_msg void OnBnClickedButtonrobot2findpath();
	afx_msg void OnClickedButtonrobot1setgoal();
	afx_msg void OnBnClickedButtonclearobstacle3();
	afx_msg void OnBnClickedButtonclearobstacle2();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClickedButtonreload();
	//音乐播放器
	std::vector<CString>preLoadMusicPath;
	std::vector<CString>preLoadMusicName;
	CListBox listBox;
	CSliderCtrl m_volumeSlider;
	CStatic m_volumeLabel;
	int curPlayPos;
	afx_msg void OnBnClickedButtonadd();
	afx_msg void OnBnClickedButtonplay();
	afx_msg void OnBnClickedButtonpause();
	afx_msg void OnBnClickedButtonstop();
	afx_msg void OnBnClickedButtondel();
	afx_msg void OnBnClickedButtonlast();
	afx_msg void OnBnClickedButtonnext();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void UpdateVolumeFromSlider();

	/*********************************************************************/
	/**********************************************************************/
	/*							以下函数以及变量kbk声明						*/
	void OnAccept();
	void OnConnect();
	void OnClose();
	void OnReceive();
	// 控制ip输入的空间----------kbk
	CIPAddressCtrl m_nIPcontrol;
	// 端口的值----------------------------kbk
	MSocket* m_ServerSocket;
	MSocket* m_ListenSocket;
	MSocket* m_ClientSocket;//三种不同身份的Socket
	CString m_port;
	CComboBox m_IDCombo;
	bool no_map;
	std::vector<CString> namevetor;//观众ID列表
	int people;//观众人数

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedSend();
	afx_msg void OnCbnSelchangeIdCombo();
	int ID;
	// 服务器状态
	CString text;
	afx_msg void OnEnChangeText();
	void SocketReset();
	void SendPath(std::vector<Point> sendpath, Point start, Point goal);
	// 发送地图
	void SendMap(std::vector<std::vector<int>> sendmap, bool isreload);
	bool isPVP();
	int Trophy_x;
	int Trophy_y;
	bool NoClient;
	bool isloading;//对于服务器端来说，这代表客户端还在加载地图，既然如此，再客户端加载出地图之前，服务器端什么都不准做
	afx_msg void OnBnClickedCheck();
};
