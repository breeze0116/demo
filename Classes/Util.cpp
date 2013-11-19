#include "Util.h"

string byteToHexStr(unsigned char *byte_arr, int arr_len)
{
	string  hexstr;
	for (int i=0;i<arr_len;i++)
	{
		char hex1;
		char hex2;
		int value=byte_arr[i];
		int v1=value/16;
		int v2=value % 16;
		if (v1>=0&v1<=9)
			hex1=(char)(48+v1);
		else
			hex1=(char)(55+v1);

		if (v2>=0&&v2<=9)
			hex2=(char)(48+v2);
		else
			hex2=(char)(55+v2);

		hexstr+= hex1;
		hexstr+= hex2;
		
	}
	return hexstr;

}

int bytesToInt(byte* bytes)
{

	int addr = bytes[3] & 0xFF;

	addr |= ((bytes[2] << 8) & 0xFF00);

	addr |= ((bytes[1] << 16) & 0xFF0000);

	addr |= ((bytes[0] << 24) & 0xFF000000);

	return addr;

}

byte* intToByte(int i)
{
	byte* abyte0 = new byte[4];
	abyte0[3] = (byte) (0xff & i);
	abyte0[2] = (byte) ((0xff00 & i) >> 8);
	abyte0[1] = (byte) ((0xff0000 & i) >> 16);
	abyte0[0] = (byte) ((0xff000000 & i) >> 24);
	return abyte0;
}

