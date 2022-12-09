
// ChatServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"
#include "afxdialogex.h"
#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatServerDlg 대화 상자



CChatServerDlg::CChatServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSERVER_DIALOG, pParent)
	, m_strSend(_T(""))
	, m_strStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
}

BEGIN_MESSAGE_MAP(CChatServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_ACCEPT, OnAccept)
	ON_MESSAGE(UM_RECEIVE, OnReceive)

	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatServerDlg::OnClickedButtonSend)
END_MESSAGE_MAP()


// CChatServerDlg 메시지 처리기

BOOL CChatServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
		m_socServer.m_index.push_back(i+1);
	}

	isready = 0;
	// 서버 소켓을 생성(포트번호 5000)
	m_socServer.Create(5000);
	// 클라이언트의 접속을 기다림
	m_socServer.Listen();
	// 소켓 클래스와 메인 윈도우(여기에서는 CChatServerDlg)를 연결
	m_socServer.Init(this->m_hWnd);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CChatServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CChatServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CChatServerDlg::OnAccept(WPARAM wParam, LPARAM lParam) {
	// 클라이언트에서 접속 요청이 왔을 때

	try {
		// 통신용 소켓을 생선한 뒤
		int tmp = m_socServer.m_index.front();

		CString number;                // 클라이언트 번호
		number.Format(_T("%d"), tmp);

		m_socCom[tmp] = new CSocCom();
		// 서버소켓과 통신소켓을 연결한다.
		m_socCom[tmp] = m_socServer.GetAcceptSocCom();

		m_socServer.m_index.pop_front();
		m_using.push_back(tmp);

		m_socCom[tmp]->m_index = tmp;
		m_socCom[tmp]->Init(this->m_hWnd);

		// 클라이언트(사용자)에게 연결 성공 메시지를 보낼때 클라이언트 번호도 같이 보냄.
		m_socCom[tmp]->Send((_T(SOC_CLIENT_CONNECT) + number), 256);
		
	}
	catch (CException* ex) {
		ex->ReportError();
	}
	UpdateData(FALSE);
	return TRUE;
}


// 데이터를 보내는 것은 소켓 클래스의 멤버 함수인 Send를 이용
// 데이터를 받을 때는 통신 소켓 클래스에 오버라이딩한 OnReceive 메시지 함수를 사용
LRESULT CChatServerDlg::OnReceive(WPARAM wParam, LPARAM lParam) {
	// 접속된 곳에서 데이터가 도착했을 때
	UpdateData(TRUE);
	
	CString strTmp;
	
	unsigned char check, msg_id;//message_id
	unsigned short int body_size;

	m_socCom[wParam]->Receive((char*)&check,1,0);    // wParam = 클라이언트 번호/헤더 1바이트 읽기
	m_socCom[wParam]->Receive((int*)&body_size, 1, 0);
	char* pTmp = new char[body_size];
	memset(pTmp, '\0', body_size);
	m_socCom[wParam]->Receive(pTmp, body_size, 0);
	strTmp = CH2CS(pTmp);

	showChat(strTmp, wParam, lParam, check, body_size, pTmp);
	switch (check) 
	{
	case(20): { //채팅
		showChat(strTmp, wParam, lParam, check, body_size, pTmp);
		break;
	}
	case(91)://준비 87시작 88선턴 89 후턴
	{ 
		isready += 1;
		if (isready == 2) {
			
			
			unsigned char stch = 87; //게임시작(87,88,89)
			CString str = _T("게임을 시작합니다. 당신의 턴입니다.");
			CString str2 = _T("게임을 시작합니다. 상대의 턴입니다.");
			unsigned int strlen = str.GetLength();
			unsigned int strlen2 = str2.GetLength();

			int len_s = (sizeof(char) * strlen * 2) + 1;
			int len_s2 = (sizeof(char) * strlen2 * 2) + 1;
			char* stsTemp = new char[strlen];
			char* stsTemp2 = new char[strlen2];
			stsTemp = CS2CHAR(str);
			stsTemp2 = CS2CHAR(str2);
			m_list.AddString(_T("게임시작"));

			

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(1, 2);
			int j = dis(gen);
			
			if (j == 1) {
				char* buff = new char[len_s + 2];
				*buff = 88;
				*(buff + 1) = len_s;
				memcpy(buff + 2, stsTemp, len_s);
				m_socCom[1]->Send(buff, len_s + 2);
				delete[] buff;
				buff = NULL;

				char* buff2 = new char[len_s + 2];
				*buff2 = 89;
				*(buff2 + 1) = len_s2;
				memcpy(buff2 + 2, stsTemp2, len_s2);
				m_socCom[2]->Send(buff2, len_s2 + 2);
				delete[] buff2;
			}
			else if (j == 2) {
				char* buff = new char[len_s + 2];
				*buff = 88;
				*(buff + 1) = len_s;
				memcpy(buff + 2, stsTemp, len_s);
				m_socCom[2]->Send(buff, len_s + 2);
				delete[] buff;
				buff = NULL;

				char* buff2 = new char[len_s + 2];
				*buff2 = 89;
				*(buff2 + 1) = len_s2;
				memcpy(buff2 + 2, stsTemp2, len_s2);
				m_socCom[1]->Send(buff2, len_s2 + 2);
				delete[] buff2;
			}

			
			
			
		}
		break;
	}
	case(90): //준비해제
	{
		isready -= 1;
		break;
	}
	
	case(100): //턴 전환
	{
		unsigned char stch = 100; //게임시작(87,88,89)
		CString ststr = (_T("당신의 턴입니다."));
		int st_size = (sizeof(char) * ststr.GetLength() * 2) + 1;
		char* ctemp = CS2CHAR(ststr);

		showChat(ststr, wParam, lParam, stch, st_size, ctemp);
		delete ctemp;
		break;
	}

	case(11): //말 움직이기
	{
		break;
	}


	case(99): //승리
	{
		unsigned char stch = 99; 
		CString ststr = (wParam + _T("번 플레이어가 승리하였습니다. 당신의 패배입니다."));
		int st_size = (sizeof(char) * ststr.GetLength() * 2) + 1;
		char* ctemp = CS2CHAR(ststr);
		
		showChat(ststr, wParam, lParam, stch, st_size, ctemp);
		delete ctemp;
		break;
	}
	case(6):
	case(5):
	case(4):
	case(3):
	case(2):
	case(1):
	{
		CString ststr = (wParam + _T("번 플레이어 - ")+ strTmp);
		break;
	}

	}


	UpdateData(FALSE);
	return TRUE;
}



void CChatServerDlg::showChat(CString str, WPARAM wParam, LPARAM lParam, 
	unsigned char check, unsigned short int body_size, char * body)
{
	CString id;
	id.Format(_T("%d"), wParam);

	m_list.AddString((_T("사용자") + id + " : " + str));

	
	for each (int i in m_using) {
		if (i != _ttoi(id)) {    // 보낸 클라이언트 제외 모든 클라이언트한테 보냄
			int b_len = body_size + 2;
			char* buff = new char[b_len];
			*buff = check;
			*(buff + 1) = body_size;
			memcpy(buff + 2, body, b_len);
			m_socCom[i]->Send(buff, b_len);
		}
	}
}

CString CChatServerDlg::CH2CS(char* strInput)

{

	// ## char* -> wchar_t -> CString ##

	wchar_t* strWCHAR;
	CString strOutput;
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, strInput, strlen(strInput), NULL, NULL);
	strWCHAR = SysAllocStringLen(NULL, iLength);
	MultiByteToWideChar(CP_ACP, 0, strInput, strlen(strInput), strWCHAR, iLength);

	strOutput.Format(_T("%s"), strWCHAR);

	return strOutput;
}

char* CChatServerDlg::CS2CHAR(CString strInput)

{

	// ## CString -> wchar_t -> char* ##
	wchar_t* strWCHAR;
	char* strOutput;
	int iLength;

	strWCHAR = strInput.GetBuffer(strInput.GetLength());

	iLength = WideCharToMultiByte(CP_ACP, 0, strWCHAR, -1, NULL, 0, NULL, NULL);

	strOutput = new char[iLength];

	WideCharToMultiByte(CP_ACP, 0, strWCHAR, -1, strOutput, iLength, 0, 0);

	return strOutput;

}



void CChatServerDlg::OnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	char pTmp[256];
	CString strTmp;

	// pTmp에 전송할 데이터 입력
	memset(pTmp, '\0', 256);
	strcpy_s(pTmp, (char*)(LPCTSTR)(_T("관리자 : ") + m_strSend));
	m_strSend = "";

	// 전송
	for each (int i in m_using) {
		m_socCom[i]->Send(pTmp, 256);
	}
	// 전송한 데이터도 리스트박스에 보여준다.
	strTmp.Format(_T("%s"), pTmp);
	int i = m_list.GetCount();
	m_list.InsertString(i, strTmp);

	UpdateData(FALSE);
}
