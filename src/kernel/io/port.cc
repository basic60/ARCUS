#include"io/port.h"
namespace arcus::io
{
    extern "C" uint8 read_io_port(uint16 port_num);
    extern "C" void write_io_port (uint16 portNum,uint8 val);

    uint8 read_port(uint16 port_num) {
        return read_io_port(port_num);
    }
     
    void write_port(uint16 port_num,uint8 val) {
        write_io_port(port_num, val);
    }
}