#include "message.h"
#include "Util.h"

Message::Message():data(NULL){

}
Message::~Message()
{
	//printf("Message::~Message() %p\n",this);
	if (data!=NULL) {
		delete[] data;
	}
}

int Message::datalength()
{
	return bytesToInt(length)+13;
}