#ifndef __TCPUTIL_H__
#define __TCPUTIL_H__

#include "cocos2d.h"
#include "ODSocket.h"
#include "pthread.h"
#include "Message.h"
#include "MsgQueue.h"

// #define SAFE_DELETE(p) {if(p != NULL){delete p; p = NULL;}}
// #define SAFE_DELETE_ARRAY(p) {if(p){ delete[] p; p = NULL; }}
// #define SAFE_DELETE_VEC(p) {for(int i = 0; i < p->size();i++){delete (p+i);}delete[] p;}

#define SLEEP_TIME 50

//TCP������IP&&�˿�;
#define SERVER_HOST "192.168.1.188"
#define SERVER_PORT 11009//8006//

/********************************************************************
*	Copyright (R) Breeze 2013
*
*	����:	    �����;
*	�ļ�����: 	TcpUtil.h;
*   ��������:	2013/11/11 12:09;
*	
*   ����:       TCP������;
*	˵��:	   ���ڹ���SOCKET���Ӽ������շ�;
*********************************************************************/
class TcpUtil
{
public:
	TcpUtil();
	~TcpUtil();

	static TcpUtil* shareTcpUtil();
	void tcp_start();
	void tcp_stop();

	bool isRuning();

	//��ȡ������Ϣ����;
	MsgQueue* getRecvQueue();
	MsgQueue* getSendQueue();

	void pushSendQueue(std::string str,int msgType);//���뷢�Ͷ���;
	
protected:
	void recvFunc(void);//������Ϣ����;
	void sendFunc(void);//������Ϣ����;
	void tcpCheck(void);//TCP״̬��ⷽ��;
	void pushRecvQueue(std::string str,int msgType);//������ն���;
	//�����Ϣ;
	Message* constructMessage(const char* data,int commandId);
	//������Ϣ����;
	MsgQueue* m_msgRecvQueue;
	//������Ϣ����;
	MsgQueue* m_msgSendQueue;
private:
	bool m_isRuning;//�߳����б�־;
	ODSocket m_socket;

	static pthread_mutex_t mutex; //������;
	pthread_t pidRecv,pidSend;
	static void* th_recv(void *r);//���������߳�;
	static void* th_send(void *r);//���������߳�;
};

#endif