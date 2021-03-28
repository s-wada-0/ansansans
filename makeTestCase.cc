#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

int main(){

  std::ofstream outputfile("test_case.txt");

  for(int i=0;i<20;i++){
    outputfile << "2020101913" << 31+i << "24,10.20.30.1/16," << rand()%10+1 << std::endl;
    outputfile << "2020101913" << 31+i << "25,10.20.30.2/16," << rand()%10+1 << std::endl;
    outputfile << "2020101913" << 31+i << "34,192.168.1.1/24," << rand()%10+1 << std::endl;
    outputfile << "2020101913" << 31+i << "35,192.168.1.2/24," << rand()%10+1 << std::endl;
  }

  outputfile.close();

  return 0;
}