#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "util.h"
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:
string ProcessParser::getCmd(string pid) {
  string line;
  ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
  std::getline(stream, line);
  return line;
}

string ProcessParser::getVmSize(string pid) {
  string line;
  //Declaring search attribute for file
  string name = "VmData";
  string value;
  float result;
  //Opening stream for specific result
  ifstream stream = Util::getStream((Path::basePath() + pid + Path.statusPath));
  while(std::getline(stream, line)) {
    //Searching line by line
    if(line.compare(0, name.size.name(), name) == 0) {
      //slicing string line on ws for values using sstream
      istringstream buff(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      //conversion from kb to GB
      result = (stof(values[1])/float(1024));
      break;
    }
  };
  return to_string(result);
}

string ProcessParser::getCpuPercent(string pid) {
  string line;
  string value;
  float result;
  //opening stream for specific result
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  //acquiring relevant time for calculation of active occupation of CPU
  float utime = stof(ProcessParser::getProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float totaltime = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime/freq);
  result = 100.0*((totaltime/freq)/seconds);
  return to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
  string line;
  string value;
  float result;
  //opening stream for result
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  vector<string> values(beg, end);
  //using sysconf to get clockticks of the host machine
  float freq = sysconf(_SC_CLK_TCK);
  return to_string(stof(values[13])/freq);
}


long int ProcessParser::getSysUpTime() {
  string line;
  ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath());
  getStream(stream, line);
  istringstream buf(line);
  istream_iterator<string> buf(beg), end;
  vector<string> values(beg, end);
  return stoi(values[0]);
}


string ProcessParser::getProcUser() {
  string line;
  string name="Uid:";
  string result;
  ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
  //Getting UID for user
  while(std::getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> buf(beg), end;
      vector<string> values(beg, end);
      result = values[1];
      break;
    }
  }
  stream = Util::getStream("/etc/passwd");
  name = ("x:" + result);
  //searching name of the user with selected ID
  while (std::getline(stream, line)) {
    if(line.find(name) != std::string::npos) {
      result = line.substr(0, line.find(":"));
      return result;
    }
  }
  return "";
}

vector<string> ProcessParser::getPidList() {
  DIR *dir;
  //return a PID list from procs
  vector<string> container;
  if(!(dir = opendir("/proc")))
      throw std::runtime_error(std::strerror(errno));

  while (dirent* dirp = readdir(dir)) {
    //is this a directory
    if(dirp->d_type != DT_DIR)
        continue;
    //is every character of the name a digit
    if(all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){return std::isdigit(c); })) {
      container.push_back(dirp->d_name);
    }
  }

  //Validating process of directory closing
  if(closeddir(dir))
      throw std::runtime_error(std::strerror(errno));
  return container;
}
