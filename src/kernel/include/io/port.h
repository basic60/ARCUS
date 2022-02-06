#ifndef __ARCUS_INCLUDE_IO_PORT
#define __ARCUS_INCLUDE_IO_PORT
#include"types.h"

namespace arcus::io
{
    uint8 read_port(uint16 port_num);
     
    void write_port(uint16 portNum,uint8 val);
}
#endif