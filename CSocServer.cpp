// CSocServer.cpp: 구현 파일
//

#include "pch.h"
#include "ChatServer.h"
#include "CSocServer.h"


// CSocServer

CSocServer::CSocServer()
{
}

CSocServer::~CSocServer()
{
}

void CSocServer::Init(HWND hWnd) {
    // 메인 윈도우 포인터 받기
    m_hWnd = hWnd;
}

// 클라이언트에서 접속 요청이 올 경우 OnAccept 함수가 호출됨
// OnAccept 함수가 호출되면 접속 요청할 한 소켓과 다른 소켓을 연결하기 위해 Accept 함수를 호출한 뒤 메인 윈도우에 OnAccept 함수가 호출되었다는 것을 알려줌
void CSocServer::OnAccept(int nErrorCode)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    Accept(m_socCom[m_index.front()]);                        // m_socCom은 연결 요청을 한 클라이언트 서버와 실제 연결이 되는 소켓
    SendMessage(m_hWnd, UM_ACCEPT, 0, 0);
    CSocket::OnAccept(nErrorCode);
}

// 메인 윈도우에서는 m_socCom을 얻어서 통신을 처리
CSocCom* CSocServer::GetAcceptSocCom() {
    // 통신소켓을 return
    // 반환되는 통신 소켓은 클라이언트와 연결됨
    return &m_socCom[m_index.front()];
}