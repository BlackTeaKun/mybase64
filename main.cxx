#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

constexpr char capitalStart = 0x00;
constexpr char smallStart   = 0x1a;
constexpr char numberStart  = 0x34;

union int32char{
  uint32_t dataInt;
  char dataChar[4];
};

char charTable(char in){
  if(in >= numberStart)  return '0' + (in - numberStart);
  if(in >= smallStart)   return 'a' + (in - smallStart);
  return 'A' + (in - capitalStart);
}

void encode(const char *in, char *out){
  int32char tmp;
  tmp.dataInt = 0;
  for(int i = 0; i < 3; ++i) tmp.dataChar[2-i] = in[i];
  for(int i = 0; i < 4; ++i){
    out[3-i] = charTable(tmp.dataInt&0x3F);
    tmp.dataInt >>= 6;
  }
}

void encodePadded(const char *in, char *out, int sz){
  if(sz == 0) return;
  int32char tmp;
  tmp.dataInt = 0;
  for(int i = 0; i < sz; ++i) tmp.dataChar[2-i] = in[i];
  int nout = (sz*8+5)/6;
  for(int i = 0; i < 4; ++i){
    out[3-i] = charTable(tmp.dataInt&0x3F);
    tmp.dataInt >>= 6;
  }
  for(int i = nout; i < 4; ++i){
    out[i] = '=';
  }
}

int main(int argc, char **argv){
  std::ifstream in;
  if(argc == 1){
    return 0;
  }
  in.open(argv[1], std::ios::binary | std::ios::in | std::ios::ate);
  size_t size = in.tellg();

  auto indata = new char[size];
  in.seekg(0);
  in.read(indata, size);

  char *outData = nullptr;
  size_t nGroup, nLeft, outSize;
  {
    nGroup = (size+2) / 3;
    outSize = nGroup*4;
    outData = new char[nGroup*4];
    nLeft = size % 3;
  }
  for(int i = 0; i < nGroup - 1; ++i){
    encode(indata + 3*i, outData + 4*i);
  }
  encodePadded(indata + 3*(nGroup-1), outData+4*(nGroup-1), nLeft);
  for(int i = 0; i < outSize; ++i){
    std::cout.put(outData[i]);
    if((i+1) % 76 == 0){
      std::cout.put(0x0D);
      std::cout.put(0x0A);
    }
  }
  std::cout<<std::endl;
  return 0;
}
