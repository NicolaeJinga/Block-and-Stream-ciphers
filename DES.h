#pragma once
#include<fstream>

using namespace std;
class DES
{
public:

	//long long int Key[8] = { 43, 214, 69, 159, 130, 197, 179, 0 }; // = Keyf
	long long int Key[8] = { 128, 0, 0, 0, 0, 0, 0, 0 }; // = Keyf

	long long int Keyf = 0; // 64bit

	//long long int Message[8] = { 177, 15, 132, 48, 151, 160, 249, 50 }; // Messagef
	long long int Message[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // Messagef

	long long int Messagef = 0; // 64bit

	long long int InitVector = 0;
	long long int CBCCarryOver = 0;
	long long int DecryptCBCCarryOver = 0;

	int Left = 0; //32 bit
	int Right = 0; //32 bit

	int IP[8][8] = { //Initial Permutation
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17,  9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7 };

	int IPInverse[8][8] = { //Initial Permutation Inverse
		40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41,  9, 49, 17, 57, 25 };

	int Expansion[8][6] = { //Round Expansion
		32,  1,  2,  3,  4,  5,
		 4,  5,  6,  7,  8,  9,
		 8,  9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32,  1 };

	int Permutation[8][4] = { //Round Permutation
		16,  7, 20, 21,
		29, 12, 28, 17,
		 1, 15, 23, 26,
		 5, 18, 31, 10,
		 2,  8, 24, 14,
		32, 27,  3,  9,
		19, 13, 30,  6,
		22, 11,  4, 25 };

	int PC1[8][7] = { //Permuted Choice-1
		57, 49,  41, 33,  25,  17,  9,
		 1, 58,  50, 42,  34,  26, 18,
		10,  2,  59, 51,  43,  35, 27,
		19, 11,   3, 60,  52,  44, 36,
		63, 55,  47, 39,  31,  23, 15,
		 7, 62,  54, 46,  38,  30, 22,
		14,  6,  61, 53,  45,  37, 29,
		21, 13,   5, 28,  20,  12,  4 };

	int PC2[8][6] = { //Permuted Choice-2
		14, 17, 11, 24,  1,  5,
		 3, 28, 15,  6, 21, 10,
		23, 19, 12,  4, 26,  8,
		16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32 };

	int SBoxes[8][4][16] = { //S-Boxes
		{ 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		   0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		   4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		  15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 },

		{ 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		   3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		   0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		  13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 },

		{ 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		  13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		  13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		   1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 },

		{  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		  13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		  10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		   3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 },

		{  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		  14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		   4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		  11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 },

		{ 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		  10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		   9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		   4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },

		{  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		  13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		   1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		   6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 },

		{  13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		    1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		    7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		    2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
	};
	
	inline void SetMessage(char* msg, bool hex = false);
	inline void SetKey(char* key, bool hex = false);
	inline long long int Set3DESKey(char* key, bool hex = false ,bool lastKey = true);
	
	long long int KeyAfterPC1 = 0; // 56bit
	long long int KeyAfterLS = 0; // 56bit
	long long int KeyAfterPC2 = 0; // 48bit

	long long int SubKeys[16]; // 48bit

	inline void PermutedChoice1();
	inline void PermutedChoice2();
	inline void KeySchedule();
	//inline void KeyScheduleOptim();
	
	long long int MsgAfterIP = 0; //64 bit
	long long int RightAfterExpansion = 0; // 48bit
	long long int XorInFeistelNetwork = 0; // 48bit
	int RightAfterSBoxes = 0; // 32bit
	int RightAfterPermutation = 0; // 32bit
	int XorOutOfFeistelNetwork = 0; // 32bit
	long long int AfterRoundsLeftRightSwap = 0; // 64bit
	long long int MsgAfterInversePermutation = 0; // 64 bit
	
	inline void InitialPermutation();
	inline void SplitMessage();
	
	inline void ExpansionInF();
	inline void XorInFeistel(int round, bool decrypt = false);
	inline void XorInFeistelEnc(int round);
	inline void XorInFeistelDec(int round);
	inline int GetSBoxElement(int nr, int sbox);
	inline void SBoxComputation();
	inline void PermutationInF();
	inline void XorOutOfFeistel();
	inline void CrissCross();
	
	inline void SwapLeftRight();
	inline void InversePermutation();

	long long int Cypher(bool decrypt = false);
	long long int Encipher();
	long long int Decipher();

	enum Mode
	{
		ECB = 0,
		CBC
	};

	Mode m_mode;

	void InitInitVector(long long int iv);

	void Crypt(char* input, long long int inputSize, char* key, ofstream& out, bool decrypt = false, bool hexInput = false, bool hexKey = false);
	void Encrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey = false);
	void Decrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey = false);
	char* EncryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey = false);
	char* DecryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey = false);

	ifstream m_keyFile;
	int m_keyLen = 0;
	char m_keyBuffer[16];
	ifstream m_fileInput;
	ofstream m_fileOutput;
	long long int m_fileInputLen = 0;

	void LoadKey(char* fileInput);
	void Encrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex = false);
	void Decrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex = false);

	char m_3DESkeyBuffer[48];
	long long int m_3DESkey1;
	long long int m_3DESkey2;
	long long int m_3DESkey3;
	int m_nrOf3DESkeys = 0;
	int  Load3DESKey(char* fileInput, bool loadAsHex = false);
	void Encrypt3DES(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex = false);
	void Decrypt3DES(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex = false);
	char* Encrypt3DESPixels(char* input, long long int inputLen, char* keyFile, int mode, bool readKeyAsHex = false);
	char* Decrypt3DESPixels(char* input, long long int inputLen, char* keyFile, int mode, bool readKeyAsHex = false);
};