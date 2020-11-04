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
    DMAChannel(unsigned int dmaCtrlBasePAddr) {
        int devMem = open("/dev/mem", O_RDWR | O_SYNC);
        if(devMem < 0)
            printf("failed to open /dev/mem, errno: %d\n", errno);
        // else
        //     printf("/dev/mem opened, fd: %d\n", devMem);
        dmaCtrlBaseVAddr = (unsigned int*)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, devMem, dmaCtrlBasePAddr);
        if(close(devMem))
            printf("failed to close /dev/mem, errno: %d\n", errno);
    }

    void reset() {
        setReg(S2MM_CONTROL_REGISTER, 4);
        setReg(MM2S_CONTROL_REGISTER, 4);
    }

    void halt() {
        setReg(S2MM_CONTROL_REGISTER, 0);
        setReg(MM2S_CONTROL_REGISTER, 0);
    }
    
    void startSendChannel() {
        setReg(MM2S_CONTROL_REGISTER, 0xf001);  // start S2MM channel with all interrupts masked
    }

    void startRecvChannel() {
        setReg(S2MM_CONTROL_REGISTER, 0xf001);
    }

    // send: MM2S
    void send(unsigned int dataStartPAddr, unsigned int dataLength) {
        // printf("-----starting send trans-----\n");
        // dispMM2SStatus();
        setReg(MM2S_START_ADDRESS, dataStartPAddr);
        // printf("----------\n");
        // dispMM2SStatus();
        // setReg(MM2S_CONTROL_REGISTER, 0xf001);  // start S2MM channel with all interrupts masked
        // printf("----------\n");
        // dispMM2SStatus();
        setReg(MM2S_LENGTH, dataLength);  // transfer will actually start after the LENGTH is set
        // printf("----------\n");    
    }

    // recv: S2MM
    void recv(unsigned int bufStartPAddr, unsigned int bufSize) {
        // printf("-----starting recv trans-----\n");
        // dispS2MMStatus();
        setReg(S2MM_DESTINATION_ADDRESS, bufStartPAddr);
        // printf("----------\n");
        // dispS2MMStatus();
        // setReg(S2MM_CONTROL_REGISTER, 0xf001);
        // printf("----------\n");
        // dispS2MMStatus();
        setReg(S2MM_LENGTH, bufSize);  // just size of the buffer, not bytes actually received
        // printf("----------\n");    
    }

    void waitforSendDone() {
        unsigned int mm2sStatus = getReg(MM2S_STATUS_REGISTER);
        while(!(mm2sStatus & 1<<12) || !(mm2sStatus & 1<<1)){
            // printf("1\n");
            mm2sStatus = getReg(MM2S_STATUS_REGISTER);
        }
    }

    void waitforRecvDone() {
        unsigned int s2mmStatus = getReg(S2MM_STATUS_REGISTER);
        while(!(s2mmStatus & 1<<12) || !(s2mmStatus & 1<<1)){
            // printf("2\n");
            s2mmStatus = getReg(S2MM_STATUS_REGISTER);
        }
    }

    unsigned int getBytesRecvd() {
        return getReg(S2MM_LENGTH);
    }

    void destroy() {
        munmap(dmaCtrlBaseVAddr, 65535);
    }

    void dispS2MMStatus() {
        unsigned int status = getReg(S2MM_STATUS_REGISTER);
        printf("Stream to memory-mapped status (0x%08x@0x%02x):", status, S2MM_STATUS_REGISTER);
        if (status & 0x00000001) printf(" halted"); else printf(" running");
        if (status & 0x00000002) printf(" idle");
        if (status & 0x00000008) printf(" SGIncld");
        if (status & 0x00000010) printf(" DMAIntErr");
        if (status & 0x00000020) printf(" DMASlvErr");
        if (status & 0x00000040) printf(" DMADecErr");
        if (status & 0x00000100) printf(" SGIntErr");
        if (status & 0x00000200) printf(" SGSlvErr");
        if (status & 0x00000400) printf(" SGDecErr");
        if (status & 0x00001000) printf(" IOC_Irq");
        if (status & 0x00002000) printf(" Dly_Irq");
        if (status & 0x00004000) printf(" Err_Irq");
        printf("\n");
    }

    void dispMM2SStatus() {
        unsigned int status = getReg(MM2S_STATUS_REGISTER);
        printf("Memory-mapped to stream status (0x%08x@0x%02x):", status, MM2S_STATUS_REGISTER);
        if (status & 0x00000001) printf(" halted"); else printf(" running");
        if (status & 0x00000002) printf(" idle");
        if (status & 0x00000008) printf(" SGIncld");
        if (status & 0x00000010) printf(" DMAIntErr");
        if (status & 0x00000020) printf(" DMASlvErr");
        if (status & 0x00000040) printf(" DMADecErr");
        if (status & 0x00000100) printf(" SGIntErr");
        if (status & 0x00000200) printf(" SGSlvErr");
        if (status & 0x00000400) printf(" SGDecErr");
        if (status & 0x00001000) printf(" IOC_Irq");
        if (status & 0x00002000) printf(" Dly_Irq");
        if (status & 0x00004000) printf(" Err_Irq");
        printf("\n");
    }

private:
    unsigned int *dmaCtrlBaseVAddr;

    // The offset is in bytes
    void setReg(unsigned int regAddrOffset, unsigned int value) {
        // in bytes -> in 4 bytes(unsigned int)
        dmaCtrlBaseVAddr[regAddrOffset >> 2] = value;
    }

    // The offset is in bytes
    unsigned int getReg(unsigned int regAddrOffset) {
        // in bytes -> in 4 bytes(unsigned int)
        return dmaCtrlBaseVAddr[regAddrOffset >> 2];
    }
};