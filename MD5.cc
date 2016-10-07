#include "MD5.h"

MD5::MD5() {  //默认message为空格。

    string str = " ";
    init();
    generateMD5(str.c_str(), str.length());
    finalize();

}

MD5::MD5(const string& message) {

    init(); //初始化即将使用到的数据。
    generateMD5(message.c_str(), message.length());  //对消息划分成一个个512bits的分组，不包含1，0的填充。包含对已经划分好的分组的处理。
    finalize();  //对不满512bits的分组的填充，填充1,0以及消息的长度。同时也包含对分组的处理。

}

void MD5::printMD5() {  //打印出MD5序列。
     cout << MD5ResultInHex << endl;
}

void MD5::init() {

    //A, B, C, D的值可以设为不同的值。
    A = 0x01234567;
    B = 0x89abcdef;
    C = 0xfedcba98;
    D = 0x76543210;

    for (int i = 0; i < 16; i++) {
        int mod = i % 4;
        if (mod == 0) rotateBits[i] = 7;
        else if (mod == 1) rotateBits[i] = 12;
        else if (mod == 2) rotateBits[i] = 17;
        else rotateBits[i] = 22;
    }
    for (int i = 16; i < 32; i++) {
        int mod = i % 4;
        if (mod == 0) rotateBits[i] = 5;
        else if (mod == 1) rotateBits[i] = 9;
        else if (mod == 2) rotateBits[i] = 14;
        else rotateBits[i] = 20;
    }
    for (int i = 32; i < 48; i++) {
        int mod = i % 4;
        if (mod == 0) rotateBits[i] = 4;
        else if (mod == 1) rotateBits[i] = 11;
        else if (mod == 2) rotateBits[i] = 16;
        else rotateBits[i] = 23;
    }
    for (int i = 48; i < 64; i++) {
        int mod = i % 4;
        if (mod == 0) rotateBits[i] = 6;
        else if (mod == 1) rotateBits[i] = 10;
        else if (mod == 2) rotateBits[i] = 15;
        else rotateBits[i] = 21;
    }

    for (int i = 0 ; i < 64; i++)
        constant[i] = floor(abs(sin(i + 1)) * pow(2, 32));

}

void MD5::generateMD5(const char input[], int textLength) {  //在构造函数中传给generateMD5的字符串为char类型，需要转为uchar8。
    generateMD5((const uchar8 *)input, textLength);
}

void MD5::generateMD5(const uchar8 input[], int textLength) {

    messageInBits = (textLength << 3);  //message的bit长，用于之后填充512bits的分组的最后64bits。
    int firstBlock = blockSize;  //第一个分组的字节长，64bytes, 512bits。

    if (firstBlock <= textLength) {
        memcpy(remain, input, blockSize);
        fourCircle(remain);

        int i;
        for (i = firstBlock; i <= textLength - blockSize; i += blockSize)
            fourCircle(&input[i]);

        remainNum = textLength - i;
        memcpy(remain, &input[i], remainNum);
    } else {  //message不够一个512bits分组的长度，之后直接填充就可以。
        remainNum = textLength;
        memcpy(remain, input, remainNum);
    }
    
}

void MD5::finalize() {
    
    uchar8 padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int paddingNum = 56 - remainNum;  //56 = (512-64)/8
    if (paddingNum > 0) {
        paddingMD5(padding, paddingNum);
    } else {
        paddingMD5(padding, paddingNum + 64);
    }

    uchar8 endOfMD5[8];
    //将64位的整型用8位的字符型表示。
    for (int i = 0; i < 8; i++) {
        endOfMD5[i] = ((messageInBits >> (8 * i)) & 0xff);
    }
    endMD5(endOfMD5, 8);

    setResult();

    //将结果以16进制数表示，但实际还是字符。
    for (int i = 0; i < 16; i++)
        sprintf(MD5ResultInHex + i * 2, "%02x", MD5Result[i]);
    MD5ResultInHex[32] = '\0';

}

void MD5::paddingMD5(const uchar8 input[], int textLength) {

    if (textLength > 56) {  //表示原来remain中的字符数大于56,需要再创建一个512bits的分组。
        int firstPadding = textLength - 56;
        memcpy(&remain[remainNum], input, firstPadding);
        fourCircle(remain);
        memcpy(remain, &input[firstPadding], 56);
    } else {
        memcpy(&remain[remainNum], input, textLength);
    }

}

void MD5::endMD5(const uchar8 input[], int textLength) {

    memcpy(&remain[56], input, textLength);
    fourCircle(remain);

}

void MD5::setResult() {  //将uint32的A，B，C，D转换位uchar8类型。

    for(int i = 0; i < 4; i++) {
        MD5Result[i] = (A >> (8 * i)) & 0xff;
    }
    for (int i = 0; i < 4; i++) {
        MD5Result[i + 4] = (B >> (8 * i)) & 0xff;
    }
    for (int i = 0; i < 4; i++) {
        MD5Result[i + 8] = (C >> (8 * i)) & 0xff;
    }
    for (int i = 0; i < 4; i++) {
        MD5Result[i + 12] = (D >> (8 * i)) & 0xff;
    }

}

//对每个分组的4次循环运算。每次循环中又有16次循环。四次循环采用的非线性函数分别为F, G, H, I。
void MD5::fourCircle(const uchar8 block[]) {

    uint32 a = A, b = B, c = C, d = D;
    uint32 temp, swap;
    uint32 M[16];
    int index;

    ucharToUint(block, M);

    for (int i = 0; i < 64; i++) {
        if (i >= 0 && i <= 15) {
            temp = F(b, c, d);
            index = i;
        } else if (i >= 16 && i <= 31) {
            temp = G(b, c, d);
            index = (5 * i + 1) % 16;
        } else if (i >= 32 && i <= 47) {
            temp = H(b, c, d);
            index = (3 * i +5) % 16;
        } else if (i >= 48 && i <= 63) {
            temp = I(b, c, d);
            index = (7 * i) % 16;
        }
        
        swap = d;
        d = c;
        c = b;
        b = leftRotate((a + temp + constant[i] + M[index]), rotateBits[i]) + b;
        a = swap;
    }

    A += a;
    B += b;
    C += c;
    D += d;

}

void MD5::ucharToUint(const uchar8 input[], uint32 output[]) {

    for (int i = 0, j = 0; j < blockSize; i++, j += 4) {
        output[i] = ((uint32)input[j]) | (((uint32)input[j+1]) << 8)
                    | (((uint32)input[j+2]) << 16) | (((uint32)input[j+3]) << 24);
    }

}

inline MD5::uint32 MD5::leftRotate(const uint32 x, int n) {

    return ((x << n) | (x >> (32-n)));  //向左环移n位。

}

inline MD5::uint32 MD5::F(const uint32 x, const uint32 y, const uint32 z) {

    return (x & y) | ((~x) & z);

}

inline MD5::uint32 MD5::G(const uint32 x, const uint32 y, const uint32 z) {

    return (x & z) | (y & (~z));

}

inline MD5::uint32 MD5::H(const uint32 x, const uint32 y, const uint32 z) {

    return x ^ y ^ z;

}

inline MD5::uint32 MD5::I(const uint32 x, const uint32 y, const uint32 z) {

    return y ^ (x | (~z));

}

int main () {

    cout << "Please enter your message:" << endl;
    string str;
    cin >> str;
    MD5 instance = MD5(str);
    cout << "The MD5 code of your message is: ";
    instance.printMD5();
    
    return 0;
 
}
