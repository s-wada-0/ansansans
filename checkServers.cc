#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <bitset>

const int N = 3;
const int m = 3;
const int t = 12;

typedef struct{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int ip[4];
  int add_pre;
  int ping;
} server_info;

typedef struct{
  int time[6];
  int address[4];
  bool isBreak;
  int breakCount;
  std::vector<int> ping_log;
  bool isOverLoaded;
  int time_ol[6]; //time over loaded
  int subnetId[2];
} server;

typedef struct{
  std::bitset<32> subnetMask;
  std::vector<bool> isBreak;
  int time_sub[6];
  bool allBreak;
} subnet;

int readLine(std::string line, server_info *info);
std::vector<std::string> split(std::string str, char del);

int main(int argc, char *argv[]){

  if(argc != 2){
    std::cerr << "usage: ./checkServers [input file]" << std::endl;
    return -1;
  }

  std::ifstream ifs(argv[1]);
  if(ifs.fail()){
    std::cerr << "Failed to open file." << std::endl;
    return -1;
  }

  std::string line;
  server_info info;
  std::vector< std::bitset<32> > address_list;
  std::vector< server > server_list;
  std::vector< subnet > subnet_list;
  while(getline(ifs, line)){
    //std::cout << line << std::endl;
    if(readLine(line, &info)){
      /*
      std::cout << info.year << "/" << info.month << "/" << info.day << " "
                << info.hour << "'" << info.minute << '"' << info.second << " "
                << info.ip[0] << "." << info.ip[1] << "." << info.ip[2] << "." << info.ip[3] << "/" << info.add_pre << " "
                << info.ping
                << std::endl;
      */

      // 32bit ip
      std::bitset<32> bs1(info.ip[0]);
      std::bitset<32> bs2(info.ip[1]);
      std::bitset<32> bs3(info.ip[2]);
      std::bitset<32> bs4(info.ip[3]);
      std::bitset<32> ip_bit = (bs1<<24) | (bs2<<16) | (bs3<<8) | bs4;

      bool flag = false;
      for(int i=0;i<address_list.size();i++){
        std::bitset<32> xor_bits = ip_bit ^ address_list[i];
        if(xor_bits.none()){ //the ip is already exists
          //std::cout << "already exists   i:"  << i << std::endl;
          flag = true;

          if(info.ping==-1){ // ping == "-"

            if(server_list[i].breakCount==0){ //start stopping

              server_list[i].breakCount++;
              server_list[i].time[0]=info.year;
              server_list[i].time[1]=info.month;
              server_list[i].time[2]=info.day;
              server_list[i].time[3]=info.hour;
              server_list[i].time[4]=info.minute;
              server_list[i].time[5]=info.second;
              //server_list[i].isBreak = true;
              server_list[i].ping_log.clear();

              if(server_list[i].isOverLoaded){ //due to the over loaded
                server_list[i].isOverLoaded = false;
                  std::cout << "server " 
                          << info.ip[0] << "." << info.ip[1] << "." << info.ip[2] << "." << info.ip[3]
                          << " over loaded (" 
                          << server_list[i].time_ol[0] << "/" << server_list[i].time_ol[1] << "/" << server_list[i].time_ol[2] << " "
                          << server_list[i].time_ol[3] << "'" << server_list[i].time_ol[4] << '"' << server_list[i].time_ol[5]
                          << " - "
                          << info.year << "/" << info.month << "/" << info.day << " "
                          << info.hour << "'" << info.minute << '"' << info.second << " "
                          << ")" << std::endl;
              }

            }else if(server_list[i].breakCount>0){ //continue stopping
            
              server_list[i].breakCount++;
              if(server_list[i].breakCount>=N){ //stop
                server_list[i].isBreak = true;
                int subnetId1 = server_list[i].subnetId[0];
                int subnetId2 = server_list[i].subnetId[1];
                subnet_list[subnetId1].isBreak[subnetId2] = true;

              }

            }
          }else if(server_list[i].breakCount>0&&info.ping!=-1){ //recover from server stop

            server_list[i].breakCount = 0;
            server_list[i].ping_log.push_back(info.ping);

            if(server_list[i].isBreak){

              server_list[i].isBreak = false;

              int subnetId1 = server_list[i].subnetId[0];
              int subnetId2 = server_list[i].subnetId[1];
              subnet_list[subnetId1].isBreak[subnetId2] = false;

              std::cout << "server " 
                        << info.ip[0] << "." << info.ip[1] << "." << info.ip[2] << "." << info.ip[3]
                        << " stopped (" 
                        << server_list[i].time[0] << "/" << server_list[i].time[1] << "/" << server_list[i].time[2] << " "
                        << server_list[i].time[3] << "'" << server_list[i].time[4] << '"' << server_list[i].time[5]
                        << " - "
                        << info.year << "/" << info.month << "/" << info.day << " "
                        << info.hour << "'" << info.minute << '"' << info.second << " "
                        << ")" << std::endl;
            }

          }else{ // usual

            server_list[i].ping_log.push_back(info.ping);

            if(server_list[i].ping_log.size()>m){
              server_list[i].ping_log.erase(server_list[i].ping_log.begin());
            }

            if(server_list[i].ping_log.size()==m){

              int sum = 0;
              for(int j=0;j<server_list[i].ping_log.size();j++){
                sum += server_list[i].ping_log[j];
              }

              if(sum/m>=t && !server_list[i].isOverLoaded){ //start overloaded
                server_list[i].isOverLoaded = true;
                server_list[i].time_ol[0]=info.year;
                server_list[i].time_ol[1]=info.month;
                server_list[i].time_ol[2]=info.day;
                server_list[i].time_ol[3]=info.hour;
                server_list[i].time_ol[4]=info.minute;
                server_list[i].time_ol[5]=info.second;
              }else if(sum/m<t && server_list[i].isOverLoaded){ //recover from over loaded
                server_list[i].isOverLoaded = false;
                std::cout << "server " 
                        << info.ip[0] << "." << info.ip[1] << "." << info.ip[2] << "." << info.ip[3]
                        << " over loaded (" 
                        << server_list[i].time_ol[0] << "/" << server_list[i].time_ol[1] << "/" << server_list[i].time_ol[2] << " "
                        << server_list[i].time_ol[3] << "'" << server_list[i].time_ol[4] << '"' << server_list[i].time_ol[5]
                        << " - "
                        << info.year << "/" << info.month << "/" << info.day << " "
                        << info.hour << "'" << info.minute << '"' << info.second << " "
                        << ")" << std::endl;
              }
            }

          }
        }
      }
      if(!flag){ // new address
        //std::cout << "new address" << std::endl;
        server server_tmp;
        server_tmp.breakCount = 0;
        server_tmp.time[0]=info.year;
        server_tmp.time[1]=info.month;
        server_tmp.time[2]=info.day;
        server_tmp.time[3]=info.hour;
        server_tmp.time[4]=info.minute;
        server_tmp.time[5]=info.second;
        server_tmp.address[0]=info.ip[0];
        server_tmp.address[1]=info.ip[1];
        server_tmp.address[2]=info.ip[2];
        server_tmp.address[3]=info.ip[3];
        if(info.ping==-1){
          server_tmp.breakCount = 1;
        }else{
          server_tmp.ping_log.push_back(info.ping);
        }
        server_tmp.isOverLoaded = false;

        ////subnet
        std::bitset<32> subnet_bit = ip_bit>>(32-info.add_pre);
        bool flag2 = false;

        for(int i=0;i<subnet_list.size();i++){
          std::bitset<32> xor_bits = subnet_bit ^ subnet_list[i].subnetMask;
          if(xor_bits.none()){ //the subnet is already exists
            flag2 = true;
            server_tmp.subnetId[0] = i;
            server_tmp.subnetId[1] = subnet_list[i].isBreak.size();
            subnet_list[i].isBreak.push_back(false);
          }
        }
        if(!flag2){ // new subnet mask
          server_tmp.subnetId[0] = subnet_list.size();
          server_tmp.subnetId[1] = 0;
          subnet subnet_tmp;
          subnet_tmp.isBreak.push_back(false);
          subnet_tmp.subnetMask = subnet_bit;
          subnet_tmp.allBreak = false;
          subnet_list.push_back(subnet_tmp);
        }
        ////

        server_list.push_back(server_tmp);
        address_list.push_back(ip_bit);

      }

      for(int i=0;i<subnet_list.size();i++){
        bool flag3 = true;
        for(int j=0;j<subnet_list[i].isBreak.size();j++){
          if(!subnet_list[i].isBreak[j]) flag3 = false;
        }

        if(!subnet_list[i].allBreak && flag3){
          subnet_list[i].time_sub[0] = info.year;
          subnet_list[i].time_sub[1] = info.month;
          subnet_list[i].time_sub[2] = info.day;
          subnet_list[i].time_sub[3] = info.hour;
          subnet_list[i].time_sub[4] = info.minute;
          subnet_list[i].time_sub[5] = info.second;
        }
        if(subnet_list[i].allBreak && !flag3){
          std::cout << "subnet " 
                        << subnet_list[i].subnetMask
                        << " was broken (" 
                        << subnet_list[i].time_sub[0] << "/" << subnet_list[i].time_sub[1] << "/" << subnet_list[i].time_sub[2] << " "
                        << subnet_list[i].time_sub[3] << "'" << subnet_list[i].time_sub[4] << '"' << subnet_list[i].time_sub[5]
                        << " - "
                        << info.year << "/" << info.month << "/" << info.day << " "
                        << info.hour << "'" << info.minute << '"' << info.second << " "
                        << ")" << std::endl;
        }
        if(flag3){
          subnet_list[i].allBreak = true;
        }else{
          subnet_list[i].allBreak = false;
        }
      }
    }
  }
  // server dosen't recover
  for(int i=0;i<server_list.size();i++){
    if(server_list[i].isBreak){
      std::cout << "server " 
                      << server_list[i].address[0] << "." << server_list[i].address[1] << "." << server_list[i].address[2] << "." << server_list[i].address[3]
                      << " stopped (" 
                      << server_list[i].time[0] << "/" << server_list[i].time[1] << "/" << server_list[i].time[2] << " "
                      << server_list[i].time[3] << "'" << server_list[i].time[4] << '"' << server_list[i].time[5]
                      << " - "
                      << "now )" << std::endl;
    }
  }
  // over loaded dosen't recover
  for(int i=0;i<server_list.size();i++){
    if(server_list[i].isOverLoaded){
      std::cout << "server " 
                      << server_list[i].address[0] << "." << server_list[i].address[1] << "." << server_list[i].address[2] << "." << server_list[i].address[3]
                      << " over loaded (" 
                      << server_list[i].time_ol[0] << "/" << server_list[i].time_ol[1] << "/" << server_list[i].time_ol[2] << " "
                      << server_list[i].time_ol[3] << "'" << server_list[i].time_ol[4] << '"' << server_list[i].time_ol[5]
                      << " - "
                      << "now )" << std::endl;
    }
  }
  //subnet dosen't recover
  for(int i=0;i<subnet_list.size();i++){
    if(subnet_list[i].allBreak){
      std::cout << "subnet " 
                      << subnet_list[i].subnetMask
                      << " is broken (" 
                      << subnet_list[i].time_sub[0] << "/" << subnet_list[i].time_sub[1] << "/" << subnet_list[i].time_sub[2] << " "
                      << subnet_list[i].time_sub[3] << "'" << subnet_list[i].time_sub[4] << '"' << subnet_list[i].time_sub[5]
                      << " - "
                      << "now )" << std::endl;
    }
  }

  return 0;
}

std::vector<std::string> split(std::string str, char del[10]) {
    int first = 0;
    int last = str.find_first_of(del);

    std::vector<std::string> result;

    while (first < str.size()) {
        std::string subStr(str, first, last - first);

        result.push_back(subStr);

        first = last + 1;
        last = str.find_first_of(del, first);

        if (last == std::string::npos) {
            last = str.size();
        }
    }

    return result;
}

int readLine(std::string line, server_info *info){

  char del[10] = ",";
  std::vector<std::string> splitted_line = split(line, del);
  /*
  for(int i=0;i<splitted_line.size();i++){
    std::cout << splitted_line[i] << std::endl;
  }
  */

  if(splitted_line.size()!=3){
    std::cerr << "invalid line: " << line << std::endl;
    return 0;
  }

  //time
  if(splitted_line[0].length()!=14){
    std::cerr << "invalid time: " << line << std::endl;
    return 0;
  }
  try{
    info->year = stoi(splitted_line[0].substr(0,4));
    info->month = stoi(splitted_line[0].substr(4,2));
    info->day = stoi(splitted_line[0].substr(6,2));
    info->hour = stoi(splitted_line[0].substr(8,2));
    info->minute = stoi(splitted_line[0].substr(10,2));
    info->second = stoi(splitted_line[0].substr(12,2));
  }catch(std::exception &e){
    std::cerr << "invalid time: " << line << std::endl;
    return 0;
  }

  //address
  char del2[10] = "/";
  std::vector<std::string> address = split(splitted_line[1], del2);
  if(address.size()!=2){
    std::cerr << "invalid address: " << splitted_line[1] << std::endl;
    return 0;
  }
  char del3[10] = ".";
  std::vector<std::string> splitted_address = split(address[0], del3);
  if(splitted_address.size()!=4){
    std::cerr << "invalid address: " << splitted_line[1] << std::endl;
    return 0;
  }
  /*
  std::cout << splitted_address[0] << "." 
            << splitted_address[1] << "." 
            << splitted_address[2] << "." 
            << splitted_address[3] << "/"
            << address[1] << std::endl;
            */
  try{
    for(int i=0;i<4;i++){
      int ip = stoi(splitted_address[i]);
      if(!(ip>=0&&ip<256)){
        std::cerr << "invalid address: " << splitted_line[1] << std::endl;
        return 0;
      }
      info->ip[i] = ip;
    }
    info->add_pre = stoi(address[1]);
  }catch(std::exception &e){
    std::cerr << "invalid address: " << splitted_line[1] << std::endl;
    return 0;
  }

  //ping
  try{
    if(splitted_line[2] == "-"){
      info->ping = -1;
    }else{
      info->ping = stoi(splitted_line[2]);
    }
  }catch(std::exception &e){
    std::cerr << "invalid ping value: " << splitted_line[2] << std::endl;
    return 0;
  }

  return 1;
}