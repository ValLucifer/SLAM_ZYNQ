// works with /lib/firmware/ORB_FPGA.bin

#include <cstdio>
#include <string>

#include <opencv2/opencv.hpp>
#include <fpga/DmaDriver.h>

extern "C" {
    #include <cma/libxlnk_cma.h>
}

using namespace std;
using namespace cv;

#define FAST_BASE 0x40400000
#define GAUS_BASE 0x40410000
#define DESC_BASE 0x40420000

typedef struct KeyPointAndDesc {
    uint32_t desc[8];
    uint32_t posX, posY;
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
        printf("img size in bytes: %d\n", imgSizeInBytes);
    }

    DMAChannel fast(FAST_BASE);
    DMAChannel gaus(GAUS_BASE);
    DMAChannel desc(DESC_BASE);

    void *srcBuf = cma_alloc(imgSizeInBytes, 0);
    unsigned long srcBufPAddr = cma_get_phy_addr(srcBuf);
    void *dstBuf = cma_alloc(8192 * 10, 0);
    unsigned long dstBufPAddr = cma_get_phy_addr(dstBuf);

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

    fast.dispMM2SStatus();
    gaus.dispMM2SStatus();
    desc.dispS2MMStatus();

    printf("reseting DMA channels\n");
    fast.reset();
    gaus.reset();
    desc.reset();

    fast.dispMM2SStatus();
    gaus.dispMM2SStatus();
    desc.dispS2MMStatus();

    printf("starting DMA channels.\n");
    fast.startSendChannel();
    gaus.startSendChannel();
    desc.startRecvChannel();

    fast.dispMM2SStatus();
    gaus.dispMM2SStatus();
    desc.dispS2MMStatus();

    printf("starting DMA transfers\n");
    desc.recv(dstBufPAddr, 8192 * 10);
    fast.send(srcBufPAddr, imgSizeInBytes);
    gaus.send(srcBufPAddr, imgSizeInBytes);

    printf("waiting for DMA transfers done\n");
    fast.waitforSendDone();
    printf("fast is done\n");
    gaus.waitforSendDone();
    printf("gaus is done\n");
    desc.waitforRecvDone();
    printf("desc is done\n");

    printf("halting DMA channels\n");
    fast.halt();
    gaus.halt();
    desc.halt();

    fast.dispMM2SStatus();
    gaus.dispMM2SStatus();
    desc.dispS2MMStatus();

    int bytesRecvd = desc.getBytesRecvd();
    int keypointsRecvd = (bytesRecvd / 40) + 3;

    KeyPointAndDesc *keyPointAndDescs = (KeyPointAndDesc*)dstBuf;

    printf("received %d bytes in total.\n", bytesRecvd);
    printf("extracted %d key points in total.\n", keypointsRecvd);

    for(int i = 1; i <= 10; i++)
        printf("X: %d, Y: %d\n", ((KeyPointAndDesc*)dstBuf)[keypointsRecvd - i].posX, ((KeyPointAndDesc*)dstBuf)[keypointsRecvd - i].posY);

    cma_free(srcBuf);
    cma_free(dstBuf);

    fast.destroy();
    gaus.destroy();
    desc.destroy();

    return 0;
}