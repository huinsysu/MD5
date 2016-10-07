#ifndef _MD5_H_
#define _MD5_H_

#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdio>
using namespace std;

class MD5 {

  public:
      MD5();
      MD5(const string& message);
      void printMD5();
        
  private:
      typedef unsigned int uint32;
      typedef unsigned long long uint64;
      typedef unsigned char uchar8;

      const static int blockSize = 64;

      uint32 A, B, C, D;  //初始的链接变量。
      uint64 messageInBits;
      int remainNum;
      uint32 rotateBits[64];
      uint32 constant[64];  //计算MD5期间需要用到的常数。
      uchar8 remain[blockSize];
      uchar8 MD5Result[16];
      char MD5ResultInHex[33];  //MD5ResultInHex[32]='\0'
      
      void init();
      void generateMD5(const uchar8 input[], int textLength);
      void generateMD5(const char input[], int textLength);
      void finalize();
      void paddingMD5(const uchar8 input[], int textLength);
      void endMD5(const uchar8 input[], int textLength);
      void setResult();
      void fourCircle(const uchar8 block[]);
      void ucharToUint(const uchar8 input[], uint32 output[]);
      inline uint32 leftRotate(const uint32 x, int n);
      inline uint32 F(const uint32 x, const uint32 y, const uint32 z);
      inline uint32 G(const uint32 x, const uint32 y, const uint32 z);
      inline uint32 H(const uint32 x, const uint32 y, const uint32 z);
      inline uint32 I(const uint32 x, const uint32 y, const uint32 z);

};

#endif
