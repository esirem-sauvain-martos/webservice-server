//gsoap ns service name:            agent
//gsoap ns service protocol:        SOAP
//gsoap ns service style:           rpc
//gsoap ns service encoding:        encoded

#include <string>

/* CPU management */
int ns__get_cpu_info(std::string& info);
int ns__get_cpu_usage(float& usage);

/* Memory management */
int ns__get_memory_info(std::string& info);
int ns__get_memory_available(int& available);
int ns__get_memory_free(int& free);

/* Storage management */
int ns__get_drive_number(unsigned int& number);
int ns__get_drive_names(std::string names[]);
int ns__get_drive_info(std::string name, std::string& info);
int ns__get_drive_total_capacity(std::string name, float& total);
int ns__get_drive_used_capatity(std::string name, float& usage);

/* Interfaces management */
int ns__get_interface_number(unsigned int& number);
int ns__get_interface_names(std::string names[]);
int ns__get_interface_info(std::string name, std::string& info);
int ns__get_interface_ip_address(std::string name, std::string& ip_address);
int ns__get_interface_speed(std::string name, int& speed);

/* Device management */
int ns__set_hostname(std::string hostname, void);
int ns__set_interface_ip_address(std::string name, std::string ip_address, std::string subnet_mask, void);
