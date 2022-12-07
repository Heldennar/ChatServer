#pragma once

// CSocCom 명령 대상
// 통신용 소켓
#define WM_USER       0x0400
#define UM_CLOSE   WM_USER+3
#define UM_RECEIVE WM_USER+2
#define MAX_CLIENT_COUNT 2        // 서버 수용 가능 최대 클라이언트 수

#define SOC_CLIENT_CONNECT      "접속성공"            // 클라이언트(사용자)가 접속 
#define SOC_CLIENT_DISCONNECT "클라이언트 종료"        // 클라이언트 종료 시 보낼 메세지

class CSocCom : public CSocket
{
public:
    CSocCom();
    virtual ~CSocCom();

    HWND m_hWnd;                            // 메인 윈도우 핸들
    void CSocCom::Init(HWND hWnd);            // 소켓 클래스와 메인 윈도우를 연결시킴    
    virtual void OnReceive(int nErrorCode);    // 데이터가 도착했다는 것을 알려줌

    int m_index;
};
