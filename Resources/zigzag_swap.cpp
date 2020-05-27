#include <iostream>
using namespace std;
#define POS(y, x) (y)*8+(x)
void SWAP(int &a, int &b) {
  int buf = a;
  a = b;
  b = buf;
}
int main() {
  int zigzagTable[8][8] = {
          {POS(0, 0), POS(0, 1),  POS(1, 0),  POS(2, 0),  POS(1, 1), POS(0, 2), POS(0, 3), POS(1, 2)},
          {POS(2, 1),  POS(3, 0),  POS(4, 0),  POS(3, 1), POS(2, 2), POS(1, 3), POS(0, 4), POS(0, 5)},
          {POS(1, 4),  POS(2, 3),  POS(3, 2), POS(4, 1), POS(5, 0), POS(6, 0), POS(5, 1), POS(4, 2)},
          {POS(3, 3),  POS(2, 4), POS(1, 5), POS(0, 6),  POS(0, 7), POS(1, 6), POS(2, 5),  POS(3, 4)},
          {POS(4, 3),  POS(5, 2), POS(6, 1), POS(7, 0),  POS(7, 1), POS(6, 2), POS(5, 3),  POS(4, 4)},
          {POS(3, 5),  POS(2, 6), POS(1, 7), POS(2, 7),  POS(3, 6), POS(4, 5), POS(5, 4),  POS(6, 3)},
          {POS(7, 2),  POS(7, 3), POS(6, 4), POS(5, 5),  POS(4, 6), POS(3, 7), POS(4, 7),  POS(5, 6)},
          {POS(6, 5),  POS(7, 4), POS(7, 5), POS(6, 6),  POS(5, 7), POS(6, 7), POS(7, 6),  POS(7, 7)}
  };
  int result[8][8];
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j) {
      if(i*8+j != zigzagTable[i][j]) {
        for(int k = 0;k < 8;++k) {
          for(int l = 0;l < 8;++l) {
            if(zigzagTable[k][l] == i*8+j) {
              SWAP(zigzagTable[i][j], zigzagTable[k][l]);
              result[i][j] = k*8+l;
              goto OUT;
            }
          }
        }
        OUT:;
      } else {
        result[i][j] = i*8+j;
      }
    }
  }

  int testZigzagTable[8][8] = {
    {POS(0, 0), POS(0, 1),  POS(1, 0),  POS(2, 0),  POS(1, 1), POS(0, 2), POS(0, 3), POS(1, 2)},
    {POS(2, 1),  POS(3, 0),  POS(4, 0),  POS(3, 1), POS(2, 2), POS(1, 3), POS(0, 4), POS(0, 5)},
    {POS(1, 4),  POS(2, 3),  POS(3, 2), POS(4, 1), POS(5, 0), POS(6, 0), POS(5, 1), POS(4, 2)},
    {POS(3, 3),  POS(2, 4), POS(1, 5), POS(0, 6),  POS(0, 7), POS(1, 6), POS(2, 5),  POS(3, 4)},
    {POS(4, 3),  POS(5, 2), POS(6, 1), POS(7, 0),  POS(7, 1), POS(6, 2), POS(5, 3),  POS(4, 4)},
    {POS(3, 5),  POS(2, 6), POS(1, 7), POS(2, 7),  POS(3, 6), POS(4, 5), POS(5, 4),  POS(6, 3)},
    {POS(7, 2),  POS(7, 3), POS(6, 4), POS(5, 5),  POS(4, 6), POS(3, 7), POS(4, 7),  POS(5, 6)},
    {POS(6, 5),  POS(7, 4), POS(7, 5), POS(6, 6),  POS(5, 7), POS(6, 7), POS(7, 6),  POS(7, 7)}
  };
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j) {
      SWAP(testZigzagTable[i][j], testZigzagTable[result[i][j]/8][result[i][j]%8]);
    }
  }
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j) {
      cout << testZigzagTable[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j) {
      cout << result[i][j] << " ";
    }
    cout << endl;
  }
}
