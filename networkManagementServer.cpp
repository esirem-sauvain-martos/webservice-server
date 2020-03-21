#include <boost/algorithm/string.hpp>
#include <chrono>
#include <thread>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "soapH.h"
#include "ns.nsmap"

using namespace std;

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

int main()
{

    // return soap_serve(soap_new());
    return EXIT_SUCCESS;
}

int ns__get_cpu_info(struct soap *soap, string& info)
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

int ns__get_cpu_usage(struct soap *soap, float& usage)
{
    int total_value_1(0), total_value_2(0), work_value_1(0), work_value_2(0);

    if (get_cpu_value(work_value_1, total_value_1))
    {
        std::this_thread::sleep_for(250ms);
        if (get_cpu_value(work_value_2, total_value_2))
        {
            cout << total_value_2 - total_value_1 << " " << work_value_2 - work_value_1 << endl;
            usage = float(work_value_2 - work_value_1) / float(total_value_2 - total_value_1) ;
            return SOAP_OK;
        }
    }

    return -1;
}

int ns__get_memory_info(struct soap *soap, string& info)
{
    info = "";

    return SOAP_OK;
}

int ns__get_memory_available(struct soap *soap, float& available)
{
    ifstream memory_info;
    memory_info.open("/proc/meminfo", ios::in);

    if(memory_info.is_open())
    {
        string line;
        bool read = true;

        while(read && getline(memory_info, line))
        {
            if(line.rfind("MemAvailable:", 0) == 0)
                read = false;
        }
        memory_info.close();

        cout << line << endl;

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

        available = stof(line.substr(start, end - start));
    }

    return SOAP_OK;
}

int ns__get_memory_free(struct soap *soap, float& free)
{
    ifstream memory_info;
    memory_info.open("/proc/meminfo", ios::in);

    if(memory_info.is_open())
    {
        string line;
        bool read = true;

        while(read && getline(memory_info, line))
        {
            if(line.rfind("MemFree:", 0) == 0)
                read = false;
        }
        memory_info.close();

        cout << line << endl;

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

        free = stof(line.substr(start, end - start));
    }

    return SOAP_OK;
}

int ns__get_drive_number(struct soap *soap, unsigned int& number)
{
    return SOAP_OK;
}

int ns__get_drive_names(struct soap *soap, string names[])
{
    return SOAP_OK;
}

int ns__get_drive_info(struct soap *soap, string name, string& info)
{
    return SOAP_OK;
}

int ns__get_drive_total_capacity(struct soap *soap, string name, float& total)
{
    return SOAP_OK;
}

int ns__get_drive_used_capatity(struct soap *soap, string name, float& usage)
{
    return SOAP_OK;
}

int ns__get_interface_number(struct soap *soap, unsigned int& number)
{
    return SOAP_OK;
}

int ns__get_interface_names(struct soap *soap, string names[])
{
    return SOAP_OK;
}

int ns__get_interface_info(struct soap *soap, string name, string& info)
{
    return SOAP_OK;
}

int ns__get_interface_ip_address(struct soap *soap, string name, string& ip_address)
{
    return SOAP_OK;
}

int ns__get_interface_speed(struct soap *soap, string name, int& speed)
{
    return SOAP_OK;
}

int ns__set_hostname(struct soap *soap, string hostname)
{
    return SOAP_OK;
}

int ns__set_interface_ip_address(struct soap *soap, string name, string ip_address, string subnet_mask)
{
    return SOAP_OK;
}