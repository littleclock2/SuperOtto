// MSocket.cpp: 实现文件
/**********************************************************************************************************/
/*                                      此文件由kbk导入                                                   */
#include "pch.h"
#include "MSocket.h"
#include "SuperOtto.h"
#include "SuperOttoDlg.h"

MSocket::MSocket()
	: m_dlg(nullptr)
{

}

MSocket::~MSocket()
{
}

// MSocket 成员函数
void MSocket::OnAccept(int nErrorCode)
{
	if (nErrorCode) {
		AfxMessageBox("连接失败，请重试");
		return;
	}
	if (m_dlg != nullptr) {
		m_dlg->OnAccept();
	}
	CAsyncSocket::OnAccept(nErrorCode);
}

void MSocket::OnConnect(int nErrorCode)
{
	if (nErrorCode) {
		AfxMessageBox("连接失败，请重试");
		return;
	}
	if (m_dlg != nullptr) {
		m_dlg->OnConnect();
	}
	CAsyncSocket::OnConnect(nErrorCode);
}

void MSocket::OnReceive(int nErrorCode)
{
	if (m_dlg != nullptr) {
		m_dlg->OnReceive();
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

void MSocket::OnClose(int nErrorCode)
{
	if (m_dlg != nullptr) {
		m_dlg->OnClose();
	}
	CAsyncSocket::OnClose(nErrorCode);
}

void MSocket::SetDlg(CSuperOttoDlg * dlg)
{
	m_dlg = dlg;
}
