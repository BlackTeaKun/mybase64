#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

constexpr char capitalStart = 0x00;
constexpr char smallStart   = 0x1a;
constexpr char numberStart  = 0x34;

char charTable(char in){
  if(in >= numberStart)  return '0' + (in - numberStart);
  if(in >= smallStart)   return 'a' + (in - smallStart);
  return 'A' + (in - capitalStart);
}

void encode(const char *in, char *out){
  char mask6 = 0x3F;
  char mask4 = 0x0F;
  char mask2 = 0x03;
  out[0] = charTable((in[0]&(mask6<<2))>>2);
  out[1] = charTable(((in[0]&mask2)<<4) + ((in[1]&(mask4<<4))>>4));
  out[2] = charTable(((in[1]&mask4)<<2) + (in[2]&((mask2<<6)>>6)));
  out[3] = charTable(in[2]&(mask6));
}

int main(int argc, char **argv){
  std::ifstream in;
  if(argc == 1){
    return 0;
  }
  in.open(argv[1], std::ios::binary | std::ios::in | std::ios::ate);
  auto size = in.tellg();

  auto indata = new char[size];
  in.seekg(0);
  in.read(indata, size);

  char *outdata = nullptr;
  int ngroup;
  {
    ngroup = size / 3 + 1;
    outdata = new char[ngroup*4];
  }
  for(int i = 0; i < ngroup - 1; ++i){
    encode(indata + 3*i, outdata + 4*i);
  }
  std::cout << outdata << std::endl;
  return 0;
}
