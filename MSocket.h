#pragma once

/**********************************************************************************************************/
/*                                      此文件由kbk导入                                                   */
#include <afxsock.h>            // MFC 套接字扩展
class CSuperOttoDlg;
class MSocket : public CAsyncSocket
{
private:
	CSuperOttoDlg* m_dlg;
public:
	MSocket();
	
	virtual ~MSocket();
	virtual void OnAccept(int nErrorCode);	//Server
	virtual void OnConnect(int nErrorCode);	//Client
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	void SetDlg(CSuperOttoDlg* dlg);
};


