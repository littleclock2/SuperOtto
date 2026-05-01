/************************************************************/
/*							���ļ���KBK��д					*/
/****************************�������ͨ��*********************/
#include "pch.h"
#include "SuperOtto.h"
#include "SuperOttoDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <string>
#include <ctime>
#include "mmsystem.h"
#include <locale>//Ϊ��ʹ��setlocale(LC_CTYPE, "chs");�����������ĵ�txt��
#include <sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma comment(lib,"winmm.lib")
using std::stringstream;


void CSuperOttoDlg::OnAccept() {//���ڷ��������յ����ɹ��ڼ������Ϣ
	// TODO: �ڴ˴�����ʵ�ִ���.
	if (ID == 0)//��Ϊ�������ˣ����յ����û�����
	{
		people++;
		NoClient = false;
		
		/*����дһ����¼������ֵ�vector,�Ѿ������ˣ�����д*/
		m_ServerSocket = new MSocket;
		m_ServerSocket->SetDlg(this);
		m_ListenSocket->Accept(*m_ServerSocket);
		m_ServerSocket->AsyncSelect(FD_READ | FD_CLOSE);//ͬ��
		text += "�յ���������\r\n���ڵȴ�ͬ����ͼ\n";
		/*�ȴ������н������еĶ���*/
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(false);GetDlgItem(IDC_IPADDRESS1)->EnableWindow(false);
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
}


void CSuperOttoDlg::OnConnect() {/*���ڷÿͶ��ԣ��ɹ������Ϸ���ͻᴥ����һ����Ϣ*/
	// TODO: �ڴ˴�����ʵ�ִ���.
	/*������ƿͻ��˷���һ����ʾ��Ϣ���߷�������׼�������ˣ�Ȼ��Ϳ��Է��͵�ͼ*/
	GetDlgItem(IDC_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);//������֮��Ͳ�׼���޸�IP �Ͷ˿ڣ�ֻ�ܶϿ�֮��������
	GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);//��׼������
	GetDlgItem(IDC_EDIT_ROW)->EnableWindow(FALSE);//��׼������
	GetDlgItem(IDC_EDIT_LINE)->EnableWindow(FALSE);//��׼������
	GetDlgItem(IDC_EDIT_OBSATACLENUM)->EnableWindow(FALSE);//��׼������
	GetDlgItem(IDC_EDIT_RANGE)->EnableWindow(FALSE);//��׼������
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);//���԰��Ͽ�
	CString msg("@connected");
	robot1.position = Point(0, 0);
	robot2.position = Point(grid.size() - 1, grid[0].size() - 1);
	Invalidate();
	m_ClientSocket->Send(msg, msg.GetLength());
	if (ID == 0)//���Ƿ�����Ϊʲô�������������ӣ�
	{
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	}
	GetDlgItem(IDC_BUTTONROBOT1SETGOAL)->EnableWindow(false);
	GetDlgItem(IDC_BUTTONROBOT1FINDPATH)->EnableWindow(false);
	GetDlgItem(ID_ButtonClearObstacle2)->EnableWindow(false);//�����Ϸ����ķ��䣬������������ť
	GetDlgItem(IDC_EDIT_ROW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LINE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_OBSATACLENUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_RANGE)->EnableWindow(TRUE);
	text += "���ӷ������ɹ���\r\n����׼��ͬ����ͼ\r\n";
	GetDlgItem(IDC_TEXT)->SetWindowText(text);
	GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(false);
}


void CSuperOttoDlg::OnClose() {//�Ͽ�����
	// TODO: �ڴ˴�����ʵ�ִ���.
	if (ID == 0)//����
	{
		people--;
		if (people == 0)
			NoClient = 1;
		if (m_ServerSocket != nullptr)
			m_ServerSocket->Close();
		if (m_ServerSocket != NULL) {
			delete m_ServerSocket;
			m_ServerSocket = NULL;
		}

		GetDlgItem(IDC_CONNECT)->EnableWindow(false);
		GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDC_BUTTONROBOT2SETGOAL)->EnableWindow(true);
		GetDlgItem(IDC_BUTTONROBOT2FINDPATH)->EnableWindow(true);
		GetDlgItem(ID_ButtonClearObstacle3)->EnableWindow(true);//�˳��Ϸ����ķ��䣬�ָ���������ť
		CString temp;
		temp.Format("�ر�����\r\n");
		text += temp;
		CSuperOttoDlg::OnClickedButtonreload();
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
	}
	else
	{
		m_ClientSocket->Close();
		SocketReset();
		GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(true);//�������ؽ��
		GetDlgItem(IDC_BUTTONROBOT1SETGOAL)->EnableWindow(true);
		GetDlgItem(IDC_BUTTONROBOT1FINDPATH)->EnableWindow(true);
		GetDlgItem(IDC_CONNECT)->EnableWindow(true);
		GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(true);
		GetDlgItem(ID_ButtonClearObstacle2)->EnableWindow(true);//�˳��Ϸ����ķ��䣬�ָ���������ť

		CString temp;
		temp.Format("�����ѹرշ��䣬������\r\n");
		CSuperOttoDlg::OnClickedButtonreload();
		text += temp;
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
	}

}


void CSuperOttoDlg::OnReceive() {
	// TODO: �ڴ˴�����ʵ�ִ���.
	char szTemp[2000];
	std::string checkend;
	if (ID == 0) {//���Ƿ���
		if (m_ServerSocket == nullptr) {
			return;
		}
		int n = m_ServerSocket->Receive(szTemp, static_cast<int>(sizeof(szTemp)) - 1);
		if (n <= 0) {
			return;
		}
		szTemp[n] = '\0';
		if (szTemp[0] == '@') {//����ģʽ
			if (!strncmp("@path", szTemp, 5)) {//����·��
				stringstream s(szTemp + 6);//�ַ�������ʽ��ȡ
				path2.clear();
				s >> start2.x >> start2.y >> goal2.x >> goal2.y;
				do {//�������͸�·��2
					s >> checkend;
					if (checkend == "@end")
						break;
					else {
						int temp;
						s >> temp;
						path2.push_back(Point(atoi(checkend.c_str()), temp));
					}
				} while (s);//������ѭ��
				if (!path2.empty()) {
					start2 = robot2.position;
					selectedX = -1;
					selectedY = -1;


					MoveRobotAlongPath(path2);  // ���ƻ�������·���˶�


					robot2.SetPosition(goal2.x, goal2.y);
					isSelecting = false;
					path2.clear();
					Invalidate();
				}
				else {
					AfxMessageBox(_T("�޷��ҵ�����·����"));
				}

			}
			else if (!strncmp(szTemp, "@connected", 10)) {//����Ϣ�ǿͻ������ӳɹ����͵ķ���
				GetDlgItem(IDC_TEXT)->SetWindowText(text);
				GetDlgItem(IDC_BUTTONROBOT1SETGOAL)->EnableWindow(true);
				GetDlgItem(IDC_BUTTONROBOT1FINDPATH)->EnableWindow(true);
				GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(true);
				isloading = true;
				SendMap(grid,0);//ͬ����ͼ
				
			}
			else if (!strncmp(szTemp, "@clear", 6)) {//�������յ��û�������ϰ���
				stringstream s(szTemp + 7);//�ַ�������ʽ��ȡ
				int x_temp, y_temp;
				s >> x_temp >> y_temp;
				grid[x_temp][y_temp] = 0; // ����ϰ���  
				// ���� ComboBox  
				CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
				CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);
				pComboX->DeleteString(pComboX->GetCurSel()); // ɾ��ѡ�е���  
				pComboY->DeleteString(pComboY->GetCurSel()); // ɾ��ѡ�е���  

				// ע�⣺ɾ���������Ҫ����ѡ�������򵥵�ѡ���һ����  
				if (pComboX->GetCount() > 0) pComboX->SetCurSel(0);
				if (pComboY->GetCount() > 0) pComboY->SetCurSel(0);

				Invalidate(); // �����ػ�  
				robot1_turn = true;
				//�غϽ���
			}
			else if (!strncmp(szTemp, "@Trophy", 7)) {
				grid[Trophy_x][Trophy_y] = 0; // ����ϰ���  
				stringstream s(szTemp + 8);//�ַ�������ʽ��ȡ
				s >> Trophy_x >> Trophy_y;
				grid[Trophy_x][Trophy_y] = 2; // ����ϰ���  
				// ���� ComboBox  
				CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
				CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);
				pComboX->DeleteString(pComboX->GetCurSel()); // ɾ��ѡ�е���  
				pComboY->DeleteString(pComboY->GetCurSel()); // ɾ��ѡ�е���  

				// ע�⣺ɾ���������Ҫ����ѡ�������򵥵�ѡ���һ����  
				if (pComboX->GetCount() > 0) pComboX->SetCurSel(0);
				if (pComboY->GetCount() > 0) pComboY->SetCurSel(0);

				Invalidate(); // �����ػ� 
			}
			else if (!strncmp(szTemp, "@DONE", 5)) {//�յ��Է����ص�ͼ�ɹ�����Ϣ
				isloading = false;
				text += "ͬ����ͼ�ɹ�\r\n";
				GetDlgItem(IDC_TEXT)->SetWindowText(text);
				GetDlgItem(IDC_SEND)->EnableWindow(true);
				GetDlgItem(IDC_BUTTONROBOT1SETGOAL)->EnableWindow(true);
				GetDlgItem(IDC_BUTTONROBOT1FINDPATH)->EnableWindow(true);
				GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(true);
				GetDlgItem(IDC_AREA)->EnableWindow(true);
				GetDlgItem(IDC_TEXT)->SetWindowText(text);
			}
		}
		else {//����������Ϣ
			CString sTemp;
			sTemp.Format("������%s\r\n", szTemp);
			text += sTemp;
			GetDlgItem(IDC_TEXT)->SetWindowText(text);
			CSuperOttoDlg* pWnd = (CSuperOttoDlg*)GetDlgItem(IDC_TEXT);
			int maxPos = pWnd->GetScrollLimit(SB_VERT);
			// ���ù����������·�
			pWnd->SetScrollPos(SB_VERT, maxPos, TRUE);
		}
	}


	else//���ǹ��ڻ������
	{
		if (m_ClientSocket == nullptr) {
			return;
		}
		int n = m_ClientSocket->Receive(szTemp, static_cast<int>(sizeof(szTemp)) - 1);
		if (n <= 0) {
			return;
		}
		szTemp[n] = '\0';
		if (szTemp[0] == '@') {//����ģʽ
			if (!strncmp("@path", szTemp, 5)) {//����·��
				stringstream s(szTemp + 6);//�ַ�������ʽ��ȡ
				path1.clear();
				s >> start1.x >> start1.y >> goal1.x >> goal1.y;
				do {//�������͸�·��2
					s >> checkend;
					if (checkend == "@end")
						break;
					else {
						int temp;
						s >> temp;
						path1.push_back(Point(atoi(checkend.c_str()), temp));
					}
				} while (s);//������ѭ��
				if (!path1.empty()) {
					start1 = robot1.position;
					selectedX = -1;
					selectedY = -1;

					MoveRobotAlongPath(path1);  // ���ƻ�������·���˶�

					robot1.SetPosition(goal1.x, goal1.y);
					isSelecting = false;
					path1.clear();
					Invalidate();
				}
				else {
					AfxMessageBox(_T("�޷��ҵ�����·����"));
				}
			}
			else if (!strncmp("@map", szTemp, 4)) {//�����ͼ
				stringstream s(szTemp + 5);//�ַ�������ʽ��ȡ
				std::string checkload;
				s >> checkload;
				if (!strncmp("@Reload", checkload.c_str(), 7)) {//˳������ָ��
					text += "����Ҫ�����أ������ؽ���ͼ\r\n";
					GetDlgItem(IDC_TEXT)->SetWindowText(text);
					s >> temp_obstacle_num >> temp_range;
					s >> checkend;
					grid.clear();
					for (int i = 0; checkend != "@end"; i++) {
						std::vector<int> temp_v;
						while (checkend != "//") {
							temp_v.push_back(atoi(checkend.c_str()));
							s >> checkend;
						}
						grid.push_back(temp_v);
						temp_v.clear();
						s >> checkend;
						
					}
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
					CString temp_str;
					temp_str.Format("%d", grid.size());
					GetDlgItem(IDC_EDIT_LINE)->SetWindowText(temp_str);
					temp_str.Format("%d", grid[0].size());
					GetDlgItem(IDC_EDIT_ROW)->SetWindowText(temp_str);
					temp_str.Format("%d", temp_obstacle_num);
					GetDlgItem(IDC_EDIT_OBSATACLENUM)->SetWindowText(temp_str);
					temp_str.Format("%d", temp_range);
					GetDlgItem(IDC_EDIT_RANGE)->SetWindowText(temp_str);
					temp_str.Format("���1:%d��", robot1_score);
					GetDlgItem(IDC_SCORE1)->SetWindowText(temp_str);
					temp_str.Format("���2:%d��", robot2_score);
					GetDlgItem(IDC_SCORE2)->SetWindowText(temp_str);
					GetDlgItem(IDC_SHOW)->SetWindowText(TEXT("���1�غ�"));
					text += "�յ���������...����ͬ��...\r\n";
					GetDlgItem(IDC_TEXT)->SetWindowText(text);
					GetDlgItem(IDC_SCORE2)->SetWindowText(temp_str);
					isSelecting = false;
					if ((((CButton*)GetDlgItem(IDC_CHECK))->GetCheck()) == TRUE) {
						isPictureload = false;
					}
					else {
						isPictureload = true;
					}
					Invalidate();
				}
				else {
					checkend=checkload;
					grid.clear();
					for (int i = 0; checkend != "@end"; i++) {
						std::vector<int> temp_v;
						while (checkend != "//") {
							temp_v.push_back(atoi(checkend.c_str()));
							s >> checkend;
						}
						grid.push_back(temp_v);
						temp_v.clear();
						s >> checkend;
					}
					s >> temp_obstacle_num >> temp_range;
					temp_x=grid.size();
					temp_y = grid[0].size();
					CString temp;
					robot2.position = Point(temp_x, temp_y);
					temp.Format("%d", temp_x);
					GetDlgItem(IDC_EDIT_LINE)->SetWindowText(temp);
					temp.Format("%d", temp_y);
					temp.Format("%d", grid[0].size());
					GetDlgItem(IDC_EDIT_ROW)->SetWindowText(temp);
					temp.Format("%d", temp_obstacle_num);
					GetDlgItem(IDC_EDIT_OBSATACLENUM)->SetWindowText(temp);
					temp.Format("%d", temp_range);
					GetDlgItem(IDC_EDIT_RANGE)->SetWindowText(temp);
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
					text += "ͬ����ͼ�ɹ�\r\n";
					GetDlgItem(IDC_TEXT)->SetWindowText(text);
				}
				
				/*�����µ�ͼ*/
				CWnd* pWnd = GetDlgItem(IDC_AREA);
				CDC* pDC = pWnd->GetDC();

				// ��ȡ�ؼ��ĳߴ�
				CRect rect2;
				pWnd->GetClientRect(&rect2);
				int cellSizeX = rect2.Width() / grid.size();
				int cellSizeY = rect2.Height() / grid[0].size();
				for (int i = 0; i < grid.size(); ++i) {
					for (int j = 0; j < grid[i].size(); ++j) {
						CRect cellRect(i * cellSizeX, j * cellSizeY, (i + 1) * cellSizeX, (j + 1) * cellSizeY);
						m_dcMem.Rectangle(&cellRect);

						if (grid[i][j] == 1) {  // �����ϰ���
							m_dcMem.FillSolidRect(&cellRect, RGB(0, 0, 0));
						}
						if (grid[i][j] == 2) {  // ���ƽ���
							m_dcMem.FillSolidRect(&cellRect, RGB(0, 114, 0));
						}
					}
				}

			}
			else if (!strncmp(szTemp, "@clear", 6)) {//�û����յ�����������ϰ���
				stringstream s(szTemp + 7);//�ַ�������ʽ��ȡ
				int x_temp, y_temp;
				s >> x_temp >> y_temp;
				grid[x_temp][y_temp] = 0; // ����ϰ���  
				// ���� ComboBox  
				CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
				CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);
				pComboX->DeleteString(pComboX->GetCurSel()); // ɾ��ѡ�е���  
				pComboY->DeleteString(pComboY->GetCurSel()); // ɾ��ѡ�е���  

				// ע�⣺ɾ���������Ҫ����ѡ�������򵥵�ѡ���һ����  
				if (pComboX->GetCount() > 0) pComboX->SetCurSel(0);
				if (pComboY->GetCount() > 0) pComboY->SetCurSel(0);

				Invalidate(); // �����ػ�  
				robot1_turn = false;
				//�غϽ���
			}
			else if (!strncmp(szTemp, "@Trophy", 7)) {
				if (Trophy_x < grid.size() && Trophy_y < grid[0].size())
					grid[Trophy_x][Trophy_y] = 0; // ����ϰ���  
				stringstream s(szTemp + 8);//�ַ�������ʽ��ȡ
				s >> Trophy_x >> Trophy_y;
				grid[Trophy_x][Trophy_y] = 2; // ����ϰ���  
				// ���� ComboBox  
				CComboBox* pComboX = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_X);
				CComboBox* pComboY = (CComboBox*)GetDlgItem(IDC_COMBO_CLEAROBSTACLE_Y);
				pComboX->DeleteString(pComboX->GetCurSel()); // ɾ��ѡ�е���  
				pComboY->DeleteString(pComboY->GetCurSel()); // ɾ��ѡ�е���  

				// ע�⣺ɾ���������Ҫ����ѡ�������򵥵�ѡ���һ����  
				if (pComboX->GetCount() > 0) pComboX->SetCurSel(0);
				if (pComboY->GetCount() > 0) pComboY->SetCurSel(0);

				Invalidate(); // �����ػ� 
				text += "ͬ��Ŀ��ɹ�\r\n";
				GetDlgItem(IDC_TEXT)->SetWindowText(text);
				CString tempstr("@DONE");//���߷������Ҽ�������
				m_ClientSocket->Send(tempstr, tempstr.GetLength());
			}
			
			else if (!strncmp(szTemp, "@win", 4)) {
				if (szTemp[5] == '1') {
					robot1_score++;
					CString str;
					str.Format("玩家1:%d分", robot1_score);
					GetDlgItem(IDC_SCORE1)->SetWindowText(str);
					AfxMessageBox(_T("玩家1获得一个奖杯！"));
					grid[start1.x][start1.y] = 0;
				}
				else if(szTemp[5] == '2' ) {
					robot2_score++;
					CString str;
					str.Format("玩家2:%d分", robot2_score);
					GetDlgItem(IDC_SCORE2)->SetWindowText(str);
					AfxMessageBox(_T("玩家2获得一个奖杯！"));
					grid[start2.x][start2.y] = 0;
				}

			}
		}

		else {//����������Ϣ
			CString sTemp;
			sTemp.Format("��ң�%s\r\n", szTemp);
			text += sTemp;
			GetDlgItem(IDC_TEXT)->SetWindowText(text);
			CSuperOttoDlg* pWnd = (CSuperOttoDlg*)GetDlgItem(IDC_TEXT);
			int maxPos = pWnd->GetScrollLimit(SB_VERT);
			// ���ù����������·�
			pWnd->SetScrollPos(SB_VERT, maxPos, TRUE);
		}
	}

}
void CSuperOttoDlg::OnBnClickedConnect() {
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (!AfxSocketInit()) {//��ʼ��
		MessageBox("WindowSocket init failed!", "Receive", MB_ICONSTOP);
		return;
	}

	if (ID == 0)//������ģʽ
	{
		UpdateData(true);//������Ķ˿ڸ���
		m_ListenSocket = new MSocket;//�½�һ��ͬ��socket
		m_ListenSocket->SetDlg(this);
		BYTE nFild[4];
		CString sIP;
		m_nIPcontrol.GetAddress(nFild[0], nFild[1], nFild[2], nFild[3]); //���ÿؼ�����,��IP��nFild
		sIP.Format("%d.%d.%d.%d", nFild[0], nFild[1], nFild[2], nFild[3]);//CString ���ĸ�nFlod���CString��Sip


		m_ListenSocket->Create(_ttoi(m_port), 1, FD_ACCEPT, sIP);//��������������Socket
		m_ListenSocket->Listen(20);//Ϊʲô��������20������
		GetDlgItem(IDC_TEXT)->SetWindowText("���䴴���ɹ�\r\n��ͼ������\r\n���ڵȴ��û�����\r\n");

		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDC_SENDTEXT)->EnableWindow(true);
		GetDlgItem(IDC_SEND)->EnableWindow(true);
		CSuperOttoDlg::OnClickedButtonreload();
		GetDlgItem(IDC_BUTTONRELOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ID_COMBO)->EnableWindow(FALSE);
	}
	else
	{
		UpdateData(true);//������Ķ˿ڸ���
		m_ClientSocket = new MSocket;//�����û�
		m_ClientSocket->SetDlg(this);
		
		//���ӷ�����//
		BYTE nFile[4];
		CString sIP;
		m_nIPcontrol.GetAddress(nFile[0], nFile[1], nFile[2], nFile[3]);
		sIP.Format("%d.%d.%d.%d", nFile[0], nFile[1], nFile[2], nFile[3]);
		text += "�������ӷ�����...\r\n";
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
		m_ClientSocket->Create();
		m_ClientSocket->Connect(sIP, _ttoi(m_port));//���Ӷ�Ӧ��IP
		GetDlgItem(IDC_SEND)->EnableWindow(true);
		GetDlgItem(IDC_SENDTEXT)->EnableWindow(true);
	}
}


void CSuperOttoDlg::OnBnClickedDisconnect() {
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (ID == 0)
	{
		m_ListenSocket->Close();
		if (m_ListenSocket != NULL) {
			delete m_ListenSocket;
			m_ListenSocket = NULL;
		}
		SocketReset();
	}
	else
	{
		m_ClientSocket->Close();
		SocketReset();
	}
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_SENDTEXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ID_COMBO)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_ROW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LINE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_OBSATACLENUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_RANGE)->EnableWindow(TRUE);
}


void CSuperOttoDlg::OnBnClickedSend() {//������Ϣ
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CString str;
	GetDlgItem(IDC_SENDTEXT)->GetWindowText(str);
	if ("" == str)//�հ�
		return;
	if (ID == 0)
	{
		m_ServerSocket->Send(str, str.GetLength());//����MSocket�ķ��ͺ���
		text += (CString("������") + str + "\r\n");
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
	}
	else
	{
		m_ClientSocket->Send(str, str.GetLength());
		text += (CString("��ң�") + str + "\r\n");
		GetDlgItem(IDC_TEXT)->SetWindowText(text);
	}
}


void CSuperOttoDlg::OnCbnSelchangeIdCombo() {
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	ID = m_IDCombo.GetCurSel();
}

void CSuperOttoDlg::OnEnChangeText() {//�����������ɾ������������
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	GetDlgItem(IDC_TEXT)->SetWindowText(text);//������ʾ

}
void CSuperOttoDlg::SocketReset()
{
	if (ID == 0)//����
	{
		if (m_ServerSocket != NULL) {
			delete m_ServerSocket;
			m_ServerSocket = NULL;
		}
		if (m_ListenSocket != NULL) {
			delete m_ListenSocket;
			m_ListenSocket = NULL;
		}
	}
	else
	{
		if (m_ClientSocket != NULL) {
			delete m_ClientSocket;
			m_ClientSocket = NULL;
		}
	}

	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

	GetDlgItem(IDC_TEXT)->SetWindowText("�Ͽ����ӣ��Զ���λ");
}

// ���͵�ͼ
void CSuperOttoDlg::SendPath(std::vector<Point> sendpath, Point start, Point goal)
{
	// TODO: �ڴ˴�����ʵ�ִ���.
	CString temp_str;
	temp_str.AppendFormat("@path\n%d %d %d %d ", start.x, start.y, goal.x, goal.y);
	for (int i = 0; i < sendpath.size(); i++) {
		temp_str.AppendFormat("%d %d\n", sendpath[i].x, sendpath[i].y);
	}
	temp_str.AppendFormat("@end\n");
	if (ID == 0 && m_ServerSocket != nullptr)//��������
		m_ServerSocket->Send(temp_str, temp_str.GetLength());
	else if (m_ClientSocket != nullptr)
		m_ClientSocket->Send(temp_str, temp_str.GetLength());
}
void CSuperOttoDlg::SendMap(std::vector<std::vector<int>> sendmap,bool isreload)
{
	// TODO: �ڴ˴�����ʵ�ִ���.
	if (m_ServerSocket == nullptr) {
		return;
	}
	CString temp_str;
	temp_str.AppendFormat("@map\n");
	if (isreload) {
		temp_str.AppendFormat("@Reload\n%d\n%d\n", temp_obstacle_num, temp_range);
	}
	for (int i = 0; i < sendmap.size(); i++) {
		for (int j = 0; j < sendmap[i].size(); j++)
			temp_str.AppendFormat("%d ", sendmap[i][j]);
		temp_str.AppendFormat("// ");

	}
	temp_str.AppendFormat("@end %d %d\n", temp_obstacle_num, temp_range);
	m_ServerSocket->Send(temp_str, temp_str.GetLength());
	Sleep(10);//��ms
	temp_str.Format("@Trophy\n%d %d\n", Trophy_x, Trophy_y);//����һ������
	m_ServerSocket->Send(temp_str, temp_str.GetLength());
	Sleep(10);//��ms
	Invalidate();
}
bool CSuperOttoDlg::isPVP() {
	return (m_ClientSocket != nullptr || m_ServerSocket != nullptr) 
		;
}