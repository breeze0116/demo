#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#include <cstdio>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm> // sort
#include <stdio.h>
#include "pthread.h"

struct GameMSG
{
	int msgID;
	std::string msg;
	bool tag;
};

class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();
	//static MsgQueue* shareMsgQueue();

	GameMSG getAFirstGameMessage();
	GameMSG getAGameMessageByType(int type);
	void addAGameMessage(GameMSG msg);
	int getSize();
	pthread_mutex_t lock; //»¥³âËø;
private:
	std::vector<GameMSG> vec;
};

#endif
