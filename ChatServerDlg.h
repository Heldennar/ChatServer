
// ChatServerDlg.h: 헤더 파일
//

#pragma once

#include "stdAfx.h"
// CChatServerDlg 대화 상자
class CChatServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CChatServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CSocServer m_socServer;
	CSocCom* m_socCom[MAX_CLIENT_COUNT];
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnAccept(WPARAM wParam, LPARAM lParam);    // 클라이언트 접속 요청이 왔을 때 실행되는 메세지 함수
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void showChat(CString str, WPARAM wParam, LPARAM lParam, unsigned char check, unsigned short int body_size, char* body);
	CString CH2CS(char* strInput);
	char* CS2CHAR(CString strInput);
	CString m_strSend;
	CListBox m_list;
	CString m_strStatus;
	afx_msg void OnClickedButtonSend();
	std::list<int> m_using;

protected:
	int isready;
	
};
