#pragma once

#include "CSocCom.h"
#include <list>
#define UM_ACCEPT WM_USER+1

// CSocServer 명령 대상
// 서버용 소켓
class CSocServer : public CSocket
{
public:
    CSocServer();
    virtual ~CSocServer();

    CSocCom m_socCom[MAX_CLIENT_COUNT];        // 연결 요청을 한 클라이언트 서버와 실제 연결이 되는 소켓
    CSocCom* GetAcceptSocCom();                // 통신 소켓 리턴

    std::list<int> m_index;

    HWND m_hWnd;                            // 메인 윈도우 핸들
    void CSocServer::Init(HWND hWnd);        // 소켓 클래스와 메인 윈도우를 연결시킴    
    virtual void OnAccept(int nErrorCode);    // 클라이언트 접속 요청 처리
};
