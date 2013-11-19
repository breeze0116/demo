#ifndef _UTIL_H_
#define _UTIL_H_
#include "cocos2d.h"
#include <string>

#ifndef WIN32
typedef unsigned char byte;
#endif
typedef std::string String;

using namespace std;

// Helpers to safely delete objects and arrays
#define SQ_SAFE_DELETE(x)       {if(x){ delete x; x = 0; }}
#define SQ_SAFE_DELETE_ARRAY(x) {if(x){ delete[] x; x = 0; }}
#define SQ_SAFE_DELETE_VEC(x) {for(int i = 0; i < x->size();i++){delete (x+i);}delete[] x;}

extern string byteToHexStr(unsigned char *byte_arr, int arr_len);

#define SAFE_DELETE_ELEMENT( ptr ) if (ptr != NULL) {delete ptr; ptr = NULL;}
#define SAFE_DELETE_ARRAY( ptr )if (ptr != NULL){delete[] ptr;ptr = NULL;}
#define SAFE_DELETE_VEC(x) {for(int i = 0; i < x->size();i++){delete (x+i);}delete[] x;}

int bytesToInt(byte* bytes);
byte* intToByte(int i);

#endif
