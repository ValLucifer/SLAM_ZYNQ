#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>

// DMA register address offsets (in bytes)
#define MM2S_CONTROL_REGISTER       0x00
#define MM2S_STATUS_REGISTER        0x04
#define MM2S_START_ADDRESS          0x18
#define MM2S_LENGTH                 0x28

#define S2MM_CONTROL_REGISTER       0x30
#define S2MM_STATUS_REGISTER        0x34
#define S2MM_DESTINATION_ADDRESS    0x48
#define S2MM_LENGTH                 0x58

class DMAChannel {
public:
    DMAChannel();
    DMAChannel(unsigned int dmaCtrlBasePAddr);

    void reset();

    void halt();
    
    void startSendChannel();
    void startRecvChannel();

    // send: MM2S
    void send(unsigned int dataStartPAddr, unsigned int dataLength);
    // recv: S2MM
    void recv(unsigned int bufStartPAddr, unsigned int bufSize);

    void waitforSendDone();
    void waitforRecvDone();

    unsigned int getBytesRecvd();

    void dispS2MMStatus();
    void dispMM2SStatus();

    void destroy();

private:
    unsigned int *dmaCtrlBaseVAddr;

    // The offset is in bytes
    void setReg(unsigned int regAddrOffset, unsigned int value);
    // The offset is in bytes
    unsigned int getReg(unsigned int regAddrOffset);
};