#include "TcpUtil.h"
#include "Util.h"
/********************************************************************
*	Copyright (R) Breeze 2013
*
*	����:	    �����;
*	�ļ�����: 	TcpUtil.cpp;
*   ��������:	2013/11/11 17:17;
*	
*   ����:        ;
*	˵��:	    ;
*********************************************************************/
pthread_mutex_t TcpUtil::mutex;//������;

static TcpUtil _sharedContext;
TcpUtil* TcpUtil::shareTcpUtil() 
{
	static bool s_bFirstUse = true;
	if (s_bFirstUse) 
	{
		s_bFirstUse = false;
	}
	return &_sharedContext;
}

TcpUtil::TcpUtil()
	: m_isRuning( false )
	, m_msgRecvQueue( NULL )
	, m_msgSendQueue( NULL )
{
	pthread_mutex_init(&mutex,NULL);//��ʼ��������;
	m_msgRecvQueue = new MsgQueue();//MsgQueue::shareMsgQueue();
	m_msgSendQueue = new MsgQueue();
}

TcpUtil::~TcpUtil()
{
	pthread_mutex_destroy(&mutex);
	SAFE_DELETE_ELEMENT(m_msgRecvQueue);
	SAFE_DELETE_ELEMENT(m_msgSendQueue);
}

void TcpUtil::tcp_start()
{
	m_socket.Connect(SERVER_HOST, SERVER_PORT);
	m_isRuning = true;
	//��ʼ���������շ��߳�;
	pthread_create(&pidRecv,NULL,th_recv,this);
	pthread_create(&pidSend,NULL,th_send,this);
}

void TcpUtil::tcp_stop()
{
	m_isRuning = false;
	m_socket.Close();
}

bool TcpUtil::isRuning()
{
	return m_isRuning;
}

MsgQueue* TcpUtil::getRecvQueue() 
{
	return m_msgRecvQueue;
}

MsgQueue* TcpUtil::getSendQueue() 
{
	return m_msgSendQueue;
}

//��Ϣ���;
Message* TcpUtil::constructMessage(const char* data,int commandId)
{
	Message* msg = new Message();

	msg->HEAD0=78;
	msg->HEAD1=37;
	msg->HEAD2=38;
	msg->HEAD3=48;
	msg->ProtoVersion=9;

	int a=0;
	msg->serverVersion[3]=(byte)(0xff&a);
	msg->serverVersion[2]=(byte)((0xff00&a)>>8);
	msg->serverVersion[1]=(byte)((0xff0000&a)>>16);
	msg->serverVersion[0]=(byte)((0xff000000&a)>>24);

	int b=strlen(data)+4;

	msg->length[3]=(byte)(0xff&b);
	msg->length[2]=(byte)((0xff00&b)>>8);
	msg->length[1]=(byte)((0xff0000&b)>>16);
	msg->length[0]=(byte)((0xff000000&b)>>24);

	int c=commandId;
	msg->commandId[3]=(byte)(0xff&c);
	msg->commandId[2]=(byte)((0xff00&c)>>8);
	msg->commandId[1]=(byte)((0xff0000&c)>>16);
	msg->commandId[0]=(byte)((0xff000000&c)>>24);

	msg->data = new char[msg->datalength()];
	memset(msg->data,0,msg->datalength());
	memcpy(msg->data+0,&msg->HEAD0,1);
	memcpy(msg->data+1,&msg->HEAD1,1);
	memcpy(msg->data+2,&msg->HEAD2,1);
	memcpy(msg->data+3,&msg->HEAD3,1);
	memcpy(msg->data+4,&msg->ProtoVersion,1);
	memcpy(msg->data+5,&msg->serverVersion,4);
	memcpy(msg->data+9,&msg->length,4);
	memcpy(msg->data+13,&msg->commandId,4);
	memcpy(msg->data+17,data,strlen(data));
	return msg;
}

void TcpUtil::tcpCheck(void)
{
	if( !m_socket.Check() )
	{
		CCLog("Socket connection Error:%d",m_socket.GetError());
		//m_socket = NULL;

		bool status = false;
		while(!status)
		{
			if (m_socket.Connect(SERVER_HOST,SERVER_PORT)) 
			{
				status = true;
				CCLog("Reconnection Success");
			}else
			{
				CCLog("Reconnection Failure");
				return;
			}
		}
	}
}

void TcpUtil::pushSendQueue(std::string str,int msgType)
{
	GameMSG msg;
	msg.msgID = msgType;
	msg.msg = str;
	msg.tag = true;
	if (m_msgSendQueue != NULL)
	{
		m_msgSendQueue->addAGameMessage(msg);
		//CCLog("sendQueue size:%d",m_msgSendQueue->getSize());
	}
}

void TcpUtil::pushRecvQueue(std::string str,int msgType)
{
	GameMSG msg;
	msg.msgID = msgType;
	msg.msg = str;
	msg.tag = true;
	if (m_msgRecvQueue != NULL)
	{
		m_msgRecvQueue->addAGameMessage(msg);
		//CCLog("recvQueue size:%d",m_msgRecvQueue->getSize());
	}
}

//���������߼�;
void TcpUtil::recvFunc(void) 
{
	//������;
	tcpCheck();

	//ȡ��Ϣͷ,ǰ��17���ֽ�Ϊ��Ϣͷ;
	char recvHeadBuf[17] = "\0";
	int recvLen = m_socket.Recv(recvHeadBuf,sizeof(recvHeadBuf),0);
	if (recvLen != 17 && recvLen != -1)
	{
		while (recvLen < 17)
		{
			int tLen = m_socket.Recv(recvHeadBuf+recvLen,sizeof(recvHeadBuf-recvLen),0);//һֱ��������Ϣͷ������;
			if (tLen != -1)
			{
				recvLen += tLen;
			}
		}
	}

	if (recvLen != -1)
	{
		//ȡ��Ϣ����;
		int msgLen, t;
		memcpy(&t,recvHeadBuf+9,4);//�ӵ�9���ֽڿ�ʼ��4���ֽ�Ϊ����;
		msgLen = ntohl(t);
		msgLen -= 4;//����������Ϣ��������+4����;

		memcpy(&t,recvHeadBuf+13,4);//�ӵ�13���ֽڿ�ʼ��4���ֽ�Ϊ��Ϣ����;
		int msgType = ntohl(t);

		//������Ϣ��;
		int readSize = 0; //��ǰ��ȡ��Ϣ�ĳ���;
		std::string rec_msg;
		//�����Ϣ�峤�ȴ���0;
		if( msgLen > 0 ) 
		{
			char* msgBody = new char[msgLen+1];
			memset(msgBody,0,msgLen+1);
			while ( readSize < msgLen )
			{
				int recvLen = m_socket.Recv(msgBody + readSize,msgLen - readSize,0);//һֱ��������Ϣ�������;
				if ( recvLen == msgLen )
				{
					rec_msg.assign((const char *)msgBody, msgLen);
					readSize = recvLen;
				}
			}
			SAFE_DELETE_ARRAY(msgBody);
			//CCLog("RECV: %d,%s",msgLen,rec_msg.c_str());
			pushRecvQueue(rec_msg,msgType);//����Ϣ��ӵ�������;
		}
	}
}

//���������߼�;
void TcpUtil::sendFunc(void)
{
	//������;
	tcpCheck();
	if (m_msgSendQueue != NULL && m_msgSendQueue->getSize() > 0)
	{
		GameMSG gameMsg = m_msgSendQueue->getAFirstGameMessage();
		if (gameMsg.tag)
		{
			std::string data = gameMsg.msg;
			Message *msg=this->constructMessage(data.c_str(), gameMsg.msgID);
			int s = m_socket.Send((char*)msg->data,msg->datalength(),0);
			SAFE_DELETE_ELEMENT(msg);
		}
	}
}


//���������߳�;
void* TcpUtil::th_send(void *r)
{
	TcpUtil* tcpUtil = (TcpUtil*) r;
	while( tcpUtil->m_isRuning )
	{
		pthread_mutex_lock(&mutex);
		tcpUtil->sendFunc();
		//CCLog("send thread________________________");
		pthread_mutex_unlock(&mutex);
#ifdef WIN32 
		Sleep(SLEEP_TIME);
#else
		sleep(SLEEP_TIME/1000.0f);
#endif
	}
	return NULL;
}

//���������߳�;
void* TcpUtil::th_recv(void *r)
{
	TcpUtil* tcpUtil = (TcpUtil*) r;
	while( tcpUtil->m_isRuning )
	{
		pthread_mutex_lock(&mutex);
		//CCLog("recv thread........................");
		tcpUtil->recvFunc();
		pthread_mutex_unlock(&mutex);
#ifdef WIN32 
		Sleep(SLEEP_TIME);
#else
		sleep(SLEEP_TIME/1000.0f);
#endif
	}
	return NULL;
}