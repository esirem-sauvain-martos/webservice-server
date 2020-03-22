#include <boost/algorithm/string.hpp>
#include <chrono>
#include <thread>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>
#include <memory>
#include <experimental/filesystem>

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "soapH.h"
#include "agent.nsmap"
#include "soapagentService.h"

using namespace std;
namespace fs = std::experimental::filesystem;

string exec(const char* cmd)
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) 
        throw runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
        result += buffer.data();
    return result;
}

bool is_digit(char& c)
{
    return (c >= 48 && c <= 57);
}

bool get_cpu_value(int& work_value, int& total_value)
{
    ifstream cpuinfo;
    cpuinfo.open("/proc/stat", ios::in);

    if (cpuinfo.is_open())
    {
        string line;
        bool read = true;

        while (read && getline(cpuinfo, line))
        {
            if(line.rfind("cpu ", 0) == 0)
                read = false;
        }
        cpuinfo.close();

        int i = 0;
        while (i < line.size() && !is_digit(line.at(i)))
            i++;

        line = line.substr(i);

        vector<int> figures;
        vector<string> figures_str;
        boost::split(figures_str, line, boost::is_any_of(" "));

        for(int j = 0; j < figures_str.size(); j++)
            figures.push_back(stoi(figures_str[j]));

        work_value = std::accumulate(figures.begin(), figures.begin() + 3, 0);
        total_value = std::accumulate(figures.begin(), figures.end(), 0);

        return true;
    }

    return false;
}

bool get_unix_memory_info(const string& field, int& value)
{
    ifstream memory_info;
    memory_info.open("/proc/meminfo", ios::in);

    if(memory_info.is_open())
    {
        string line;
        bool read = true;

        while(read && getline(memory_info, line))
        {
            if(line.rfind(field, 0) == 0)
                read = false;
        }
        memory_info.close();

        int start(0), end(0), i(0);
        bool was_digit = false;
        while(i < line.size())
        {
            if(is_digit(line.at(i)) && !was_digit)
                start = i;
            else if(!is_digit(line.at(i)) && was_digit)
                end = i;
            
            was_digit = is_digit(line.at(i++));
        }

        value = stoi(line.substr(start, end - start));

        return true;
    }
    else
        return false;
}

int main()
{
    int port = 8080;
    
    agentService agent;
    agent.soap->accept_timeout = 24 * 60 * 60;               // quit after 24h of inactivity (optional)
    agent.soap->send_timeout = agent.soap->recv_timeout = 5; // max send and receive socket inactivity time (sec)
    agent.soap->transfer_timeout = 10;                       // max time for send or receive of messages (sec)
    while (agent.run(port))                                  // bind, then loop to accept and serve requests
        agent.soap_stream_fault(std::cerr);
    agent.destroy();

    return EXIT_SUCCESS;
}

int agentService::get_cpu_info(string& info)
{
    ifstream cpuinfo;
    cpuinfo.open("/proc/cpuinfo", ios::in);

    if (cpuinfo.is_open())
    {
        string line;
        bool read = true;

        while (getline(cpuinfo, line) && read)
        {
            if(line.rfind("model name", 0) == 0)
            {
                info = line.substr(line.find(":") + 2);
                read = false;
            }
        }
        cpuinfo.close();

        return SOAP_OK;
    }
    else
        return -1;
}

int agentService::get_cpu_usage(float& usage)
{
    int total_value_1(0), total_value_2(0), work_value_1(0), work_value_2(0);

    if (get_cpu_value(work_value_1, total_value_1))
    {
        std::this_thread::sleep_for(250ms);
        if (get_cpu_value(work_value_2, total_value_2))
        {
            usage = float(work_value_2 - work_value_1) / float(total_value_2 - total_value_1) ;
            
            return SOAP_OK;
        }
    }

    return -1;
}

int agentService::get_memory_info(string& info)
{
    info = "";

    return SOAP_OK;
}

int agentService::get_memory_available(int& available)
{
    if(get_unix_memory_info("MemAvailable", available))
        return SOAP_OK;
    else
        return -1;
}

int agentService::get_memory_free(int& free)
{
    if(get_unix_memory_info("MemFree", free))
        return SOAP_OK;
    else
        return -1;
}

int agentService::get_drive_number(unsigned int& number)
{
    return SOAP_OK;
}

int agentService::get_drive_names(string *names)
{
    return SOAP_OK;
}

int agentService::get_drive_info(const string& name, string& info)
{
    return SOAP_OK;
}

int agentService::get_drive_total_capacity(const string& name, float& total)
{
    return SOAP_OK;
}

int agentService::get_drive_used_capatity(const string& name, float& usage)
{
    return SOAP_OK;
}

int agentService::get_interface_number(unsigned int& number)
{
    number = stoi(exec("ls /sys/class/net | wc -w"));
    
    return SOAP_OK;
}

int agentService::get_interface_names(string* names)
{
    string int_names[7];

    struct ifaddrs * ifAddrStruct = nullptr;
    struct ifaddrs * ifa = nullptr;
    void * tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
        string ip_address(ifa->ifa_name);
        if (!ifa->ifa_addr)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET)
            string ip_address(ifa->ifa_name);
    }
    if (ifAddrStruct != nullptr)
        freeifaddrs(ifAddrStruct);

    return SOAP_OK;
}

int agentService::get_interface_info(const string& name, string& info)
{
    struct ifaddrs * ifAddrStruct = nullptr;
    struct ifaddrs * ifa = nullptr;
    void * tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) 
        {
            tmpAddrPtr =& ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } 
        else if (ifa->ifa_addr->sa_family == AF_INET6) 
        {
            tmpAddrPtr =& ((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } 
    }
    if (ifAddrStruct != nullptr) freeifaddrs(ifAddrStruct);

    return SOAP_OK;
}

int agentService::get_interface_ip_address(const string& name, string& ip_address)
{
    return SOAP_OK;
}

int agentService::get_interface_speed(const string& name, int& speed)
{
    return SOAP_OK;
}

int agentService::set_hostname(const string& hostname)
{
    string command = "hostname " + hostname;
    exec(command.c_str());

    return SOAP_OK;
}

int agentService::set_interface_ip_address(const string& name, const string& ip_address, const string& subnet_mask)
{
    return SOAP_OK;
}