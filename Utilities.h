
#ifndef __UTILITIES_H__
#define __UTILITIES_H__
#include <iostream>

inline int getBit(int nr, int bit)
{
	return (nr >> bit) & 1;
}

inline int getBitL(long long int nr, int bit)
{
	return (nr >> bit) & (static_cast<long long int>(1));
}

inline void setBit(int& nr, int bit, bool value)
{
	if (value)
	{
		nr |= 1 << bit;
	}
	else
	{
		nr &= ~(1 << bit);
	}
}

inline void setBit(int& nr, int bit)
{
	nr |= 1 << bit;
}

inline void clearBit(int& nr, int bit)
{
	nr &= ~(1 << bit);
}

inline void toggleBit(int& nr, int bit)
{
	nr ^= 1 << bit;
}

inline void changeBit(int& nr, int bit, int value)
{
	nr ^= (-value ^ nr) & (1 << bit);
}

inline void changeBitL(long long int& nr, int bit, long long int value)
{
	nr ^= (-value ^ nr) & ((1LL) << bit);
}

inline void showBits(unsigned int nr)
{
	for (int i = (sizeof(int) * 2) - 1; i >= 0; --i)
	{
		(nr&(1u << i)) ? putchar('1') : putchar('0');
	}
	printf("\n");
}

inline int charHexDigitToInt(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 0;
}

inline long long int swapLong(long long int x) //swap bytes of a long
{
	long long int y = 0;
	for (int i = 0; i < sizeof(x); ++i)
		((char*)&y)[i] = ((char*)&x)[sizeof(x)-i-1];
	return y;
}

inline void swapUChars(unsigned char& a, unsigned char& b) //swap two unsigned chars
{
	int aux = a;
	a = b;
	b = aux;
}

#endif //__UTILITIES_H__