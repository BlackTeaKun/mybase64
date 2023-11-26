#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

constexpr char capitalStart = 0x00;
constexpr char smallStart   = 0x1a;
constexpr char numberStart  = 0x34;

union int32char{
  uint32_t      dataInt;
  unsigned char dataChar[4];
};

char charTable(char in){
  if(in >= numberStart)  return '0' + (in - numberStart);
  if(in >= smallStart)   return 'a' + (in - smallStart);
  return 'A' + (in - capitalStart);
}

uint32_t binTable(char in){
  if(std::isupper(in)) return capitalStart + in - 'A';
  if(std::islower(in)) return smallStart + in - 'a';
  return numberStart + in - '0';
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
  int nOut = (sz*8+5)/6;
  for(int i = 0; i < 4; ++i){
    out[3-i] = charTable(tmp.dataInt&0x3F);
    tmp.dataInt >>= 6;
  }
  for(int i = nOut; i < 4; ++i){
    out[i] = '=';
  }
}

void decode(const char *in, char *out){
  int32char tmp;
  tmp.dataInt = 0;
  for(int i = 0; i < 4; ++i){
    tmp.dataInt += binTable(in[i]) << ((3-i)*6);
  }
  out[0] = tmp.dataChar[2];
  out[1] = tmp.dataChar[1];
  out[2] = tmp.dataChar[0];
}

std::vector<char> readData(const std::string &path, bool doEncode){
  std::ifstream in;
  in.open(path, std::ios::binary | std::ios::in | std::ios::ate);

  size_t size = in.tellg();
  std::vector<char> data(size);

  in.seekg(0);
  if(doEncode){
    in.read(data.data(), size);
  }
  else{
    size_t i = 0;
    while(true){
      char chr = in.get();
      if(in.eof()) break;
      if(chr == '\n') continue;

      data[i] = chr;
      ++i;
    }
    data.resize(i);
  }
  return data;
}

void writeData(std::ostream &out, const std::vector<char> &outData, bool doEncode){
  for(int i = 0; i < outData.size(); ++i){
    out.put(outData[i]);
    if(doEncode){
      if((i+1) % 76 == 0){
        out.put('\n');
      }
    }
  }
  out << std::endl;
}

int main(int argc, char **argv){
  if(argc == 1){
    return 0;
  }
  bool doEncode = false;

  auto data = readData(argv[1], doEncode);

  if (data.size() == 0){
    return 0;
  }

  std::vector<char> outData;
  size_t nGroup, nLeft, outSize;
  if(doEncode) {
    nGroup = (data.size()+2) / 3;
    outSize = nGroup*4;
    outData = std::vector<char>(outSize);
    nLeft = data.size() % 3;

    for(int i = 0; 1 < nGroup - i; ++i){
      encode(data.data() + 3*i, outData.data() + 4*i);
    }
    encodePadded(data.data() + 3*(nGroup-1), outData.data()+4*(nGroup-1), nLeft);
  }
  else {
    nGroup = data.size() / 4; 
    outSize = nGroup * 3;
    outData = std::vector<char>(outSize);
    for(int i = 0; i < nGroup - 1; ++i){
      decode(data.data() + 4*i, outData.data() + 3*i);
    }
  }
  writeData(std::cout, outData, doEncode);
  return 0;
}
