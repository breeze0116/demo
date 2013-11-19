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

//TCP服务器IP&&端口;
#define SERVER_HOST "192.168.1.188"
#define SERVER_PORT 11009//8006//

/********************************************************************
 *        Copyright (R) Breeze 2013
 *
 *        作者:         刘青峰;
 *        文件名称:         TcpUtil.h;
 * 创建日期:        2013/11/11 12:09;
 *
 * 类名: TCP工具类;
 *        说明:         用于管理SOCKET连接及数据收发;
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
    
    //获取接收消息队列;
    MsgQueue* getRecvQueue();
    MsgQueue* getSendQueue();
    
    void pushSendQueue(std::string str,int msgType);//加入发送队列;
    
protected:
    void recvFunc(void);//接收消息方法;
    void sendFunc(void);//发送消息方法;
    void tcpCheck(void);//TCP状态检测方法;
    void pushRecvQueue(std::string str,int msgType);//加入接收队列;
    //打包消息;
    Message* constructMessage(const char* data,int commandId);
    //接收消息队列;
    MsgQueue* m_msgRecvQueue;
    //发送消息队列;
    MsgQueue* m_msgSendQueue;
private:
    bool m_isRuning;//线程运行标志;
    ODSocket m_socket;
    
    static pthread_mutex_t mutex; //互斥锁;
    pthread_t pidRecv,pidSend;
    static void* th_recv(void *r);//接收数据线程;
    static void* th_send(void *r);//发送数据线程;
};

#endif