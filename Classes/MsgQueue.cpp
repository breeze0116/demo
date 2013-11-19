#include "MsgQueue.h"

//pthread_mutex_t MsgQueue::lock;
MsgQueue::MsgQueue()
{
	pthread_mutex_init(&lock,NULL);
}

MsgQueue::~MsgQueue()
{
	pthread_mutex_destroy(&lock);
}

/*
static MsgQueue _sharedContext;
MsgQueue* MsgQueue::shareMsgQueue() 
{
	static bool s_bFirstUse = true;
	if (s_bFirstUse) 
	{
		s_bFirstUse = false;
	}
	return &_sharedContext;
}
*/

void MsgQueue::addAGameMessage(GameMSG msg)
{
	pthread_mutex_lock(&lock);
	vec.push_back(msg);
	pthread_mutex_unlock(&lock);
}

int MsgQueue::getSize()
{
	return vec.size();
}

GameMSG MsgQueue::getAFirstGameMessage()
{
	pthread_mutex_lock(&lock);
	GameMSG msg;
	msg.tag = false;
	
	if (vec.size() > 0)
	{
		msg = *(vec.begin());
		vec.erase(vec.begin());
	}
	pthread_mutex_unlock(&lock);
	return msg;
}


GameMSG MsgQueue::getAGameMessageByType(int sn)
{
	GameMSG msg;
	msg.tag = false;
	pthread_mutex_lock(&lock);
	for (std::vector<GameMSG>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->msgID == sn)
		{
			msg = *it;
			it = vec.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&lock);
	return msg;
}