#include <cstdio>
#include <string>
#include <sys/mman.h>

#include <opencv2/opencv.hpp>
#include <fpga/DmaDriver.hpp>

extern "C" {
    #include <cma/libxlnk_cma.h>
}

using namespace std;
using namespace cv;

#define FAST_BASE 0x40400000
#define GAUS_BASE 0x40410000
#define DESC_BASE 0x40420000


unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value) {
    dma_virtual_address[offset>>2] = value;
}

unsigned int dma_get(unsigned int* dma_virtual_address, int offset) {
    return dma_virtual_address[offset>>2];
}

void dma_s2mm_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
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

void dma_mm2s_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
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

int dma_mm2s_sync(unsigned int* dma_virtual_address) {
    volatile unsigned int mm2s_status;
    mm2s_status =  dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    while(!(mm2s_status & 1<<12) || !(mm2s_status & 1<<1) ){
        // dma_s2mm_status(dma_virtual_address);
        // dma_mm2s_status(dma_virtual_address);
        // printf("1\n");

        mm2s_status = dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    }
}

int dma_s2mm_sync(unsigned int* dma_virtual_address) {
    volatile unsigned int s2mm_status;
    s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    while(!(s2mm_status & 1<<12) || !(s2mm_status & 1<<1)){
        // dma_s2mm_status(dma_virtual_address);
        // dma_mm2s_status(dma_virtual_address);
        // printf("2\n");

        s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    }
}

typedef struct KeyPointAndDesc {
    uint32_t posX, posY;
    uint32_t desc[8];
} KeyPointAndDesc;

int main(int argc, char **argv) {
    printf("start\n");

    Mat img = imread((argc == 2) ? argv[1] : "./test.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    int imgSizeInBytes = 0;
    if(img.data == nullptr) {
        printf("failed to open the img\n");
        return -1;
    } else{
        imgSizeInBytes = img.rows * img.cols;
        printf("%d\n", imgSizeInBytes);
    }

    int devMem = open("/dev/mem", O_RDWR | O_SYNC);
    unsigned int *fastBasePAddr = (unsigned int*)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, devMem, FAST_BASE);
    unsigned int *gausBasePAddr = (unsigned int*)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, devMem, GAUS_BASE);
    unsigned int *descBasePAddr = (unsigned int*)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, devMem, DESC_BASE);
    close(devMem);

    void *srcBuf = cma_alloc(imgSizeInBytes, 0);
    unsigned int srcBufPAddr = cma_get_phy_addr(srcBuf);
    printf("src buf paddr: 0x%x\n", srcBufPAddr);
    void *dstBuf = cma_alloc(8192 * 10, 0);
    unsigned int dstBufPAddr = cma_get_phy_addr(dstBuf);
    printf("dst buf paddr: 0x%x\n", dstBufPAddr);

    for(int i = 0; i < 10; i++)
        printf("%d\t", ((uchar*)srcBuf)[i]);
    printf("\n");
    memcpy(srcBuf, (void*)img.data, imgSizeInBytes);
    for(int i = 0; i < 10; i++)
        printf("%d\t", img.data[i]);
    printf("\n");
    for(int i = 0; i < 10; i++)
        printf("%d\t", ((uchar*)srcBuf)[i]);
    printf("\n");

    dma_mm2s_status(fastBasePAddr);
    dma_mm2s_status(gausBasePAddr);
    dma_s2mm_status(descBasePAddr);

    printf("reseting\n");
    dma_set(fastBasePAddr, S2MM_CONTROL_REGISTER, 4);
    dma_set(fastBasePAddr, MM2S_CONTROL_REGISTER, 4);
    dma_set(gausBasePAddr, S2MM_CONTROL_REGISTER, 4);
    dma_set(gausBasePAddr, MM2S_CONTROL_REGISTER, 4);
    dma_set(descBasePAddr, S2MM_CONTROL_REGISTER, 4);
    dma_set(descBasePAddr, MM2S_CONTROL_REGISTER, 4);
    dma_mm2s_status(fastBasePAddr);
    dma_mm2s_status(gausBasePAddr);
    dma_s2mm_status(descBasePAddr);

    printf("halting\n");
    dma_set(fastBasePAddr, MM2S_CONTROL_REGISTER, 0);
    dma_set(gausBasePAddr, MM2S_CONTROL_REGISTER, 0);
    dma_set(descBasePAddr, S2MM_CONTROL_REGISTER, 0);
    dma_mm2s_status(fastBasePAddr);
    dma_mm2s_status(gausBasePAddr);
    dma_s2mm_status(descBasePAddr);

    printf("starting desc\n");
    dma_set(descBasePAddr, S2MM_CONTROL_REGISTER, 0x1001);

    printf("starting fast\n");
    dma_set(fastBasePAddr, MM2S_CONTROL_REGISTER, 0x1001);

    printf("starting gaus\n");
    dma_set(gausBasePAddr, MM2S_CONTROL_REGISTER, 0x1001);
    
    dma_set(descBasePAddr, S2MM_DESTINATION_ADDRESS, dstBufPAddr);
    dma_set(descBasePAddr, S2MM_LENGTH, 8192 * 10);
    dma_set(fastBasePAddr, MM2S_START_ADDRESS, srcBufPAddr);
    dma_set(fastBasePAddr, MM2S_LENGTH, imgSizeInBytes);
    dma_set(gausBasePAddr, MM2S_START_ADDRESS, srcBufPAddr);
    dma_set(gausBasePAddr, MM2S_LENGTH, imgSizeInBytes);

    printf("waiting for fast done\n");
    dma_mm2s_sync(fastBasePAddr);
    printf("waiting for gaus done\n");
    dma_mm2s_sync(gausBasePAddr);

    printf("waiting ofr desc done\n");
    dma_s2mm_sync(descBasePAddr);

    dma_s2mm_status(descBasePAddr);
    dma_mm2s_status(fastBasePAddr);
    dma_mm2s_status(gausBasePAddr);

    int bytesRecvd = dma_get(descBasePAddr, S2MM_LENGTH);

    KeyPointAndDesc *keyPointAndDescs = (KeyPointAndDesc*)dstBuf;

    printf("received %d bytes in total.\n", bytesRecvd);
    printf("extracted %d key points in total.\n", int(bytesRecvd / 40) - 1);

    for(int i = 0; i < 10; i++)
        printf("X: %d, Y: %d\n", (int)((KeyPointAndDesc*)dstBuf)[i].posX, (int)((KeyPointAndDesc*)dstBuf)[i].posY);

    cma_free(srcBuf);
    cma_free(dstBuf);

    return 0;
}