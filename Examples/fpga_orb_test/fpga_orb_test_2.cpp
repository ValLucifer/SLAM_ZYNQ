// works with /lib/firmware/ORB_FPGA_2.bin

#include <cstdio>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <fpga/DmaDriver.h>

extern "C" {
    #include <cma/libxlnk_cma.h>
}

using namespace std;
using namespace cv;

#define GAUS_BASE 0x40400000
#define FAST_BASE 0x40410000
#define DESC_BASE 0x40420000

typedef struct KeypointAndDesc {
    uint32_t desc[8];   // 256-bit descriptor(already been rotated to main direction)
    uint32_t posX;      // horizontal coordinate
    uint32_t posY;      // vertical coordinate
    uint32_t response;  // response value
} KeypointAndDesc;

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

    int dstBufSizeInBytes = 8192 * sizeof(KeypointAndDesc);
    uchar *srcBuf = (uchar*)cma_alloc(imgSizeInBytes + 1, 0);
    unsigned long srcBufPAddr = cma_get_phy_addr(srcBuf);
    KeypointAndDesc *dstBuf = (KeypointAndDesc*)cma_alloc(dstBufSizeInBytes, 0);
    unsigned long dstBufPAddr = cma_get_phy_addr(dstBuf);

    for(int i = 0; i < 10; i++)
        printf("%d\t", srcBuf[i]);
    printf("\n");
    memcpy(srcBuf + 1, (void*)img.data, imgSizeInBytes);
    for(int i = 0; i < 10; i++)
        printf("%d\t", img.data[i]);
    printf("\n");
    for(int i = 0; i < 10; i++)
        printf("%d\t", srcBuf[i]);
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

    vector< vector<KeypointAndDesc> > allKpAndDesc;
    int cntLayers = 4;

    printf("----- extract start -----\n");

    int bytesRecvd;
    int totalBytesRecvd = 0;
    int cntKeypointsPerLayer[4];
    for(int i = 0; i < 4; i++) {
        *srcBuf = i;

        desc.recv(dstBufPAddr + totalBytesRecvd, dstBufSizeInBytes);
        gaus.send(srcBufPAddr, imgSizeInBytes + 1);
        fast.send(srcBufPAddr, imgSizeInBytes + 1);

        gaus.waitforSendDone();
        fast.waitforSendDone();
        desc.waitforRecvDone();

        bytesRecvd = desc.getBytesRecvd();
        cntKeypointsPerLayer[i] = (bytesRecvd / sizeof(KeypointAndDesc)) - 1;
        totalBytesRecvd += bytesRecvd;
        
        printf("received %d bytes in layer %d\n", bytesRecvd, i);
    }

    int offsetInBytes = 0, offsetInKpAndDesc = 0;
    allKpAndDesc.resize(cntLayers);
    for(int i = 0; i < cntLayers; i++) {
        allKpAndDesc[i].resize(cntKeypointsPerLayer[i]);
        allKpAndDesc[i] = vector<KeypointAndDesc>(dstBuf + offsetInKpAndDesc, dstBuf + offsetInKpAndDesc + cntKeypointsPerLayer[i]);
        // memcpy(&allKpAndDesc[i], dstBuf + offsetInKpAndDesc, cntKeypointsPerLayer[i] * sizeof(KeypointAndDesc));
        // extra 1 for terminating sign
        // offsetInBytes += (cntKeypointsPerLayer[i] + 1) * sizeof(KeypointAndDesc);
        offsetInKpAndDesc += cntKeypointsPerLayer[i] + 1;
    }

    printf("-----  extract end  -----\n");

    for(int i = 0; i < cntLayers; i++) {
        printf("Layer %d:\n", i);
        printf("%d keypoints\n", cntKeypointsPerLayer[i]);
        for(int j = 0; j < 10; j++) {
            KeypointAndDesc &curKeypointAndDesc = allKpAndDesc[i][allKpAndDesc[i].size()-1-j];
            printf("\tKP %d:\n", j);
            printf("\t\tDescriptor: ");
            for(int k = 0; k < 8; k++)
                printf("%d ", curKeypointAndDesc.desc[k]);
            printf("\n");
            printf("\t\tposX: %d, posY: %d\n", curKeypointAndDesc.posX, curKeypointAndDesc.posY);
            printf("\t\tresponse: %d\n", curKeypointAndDesc.response);
        }
    }

    printf("halting DMA channels\n");
    fast.halt();
    gaus.halt();
    desc.halt();

    fast.dispMM2SStatus();
    gaus.dispMM2SStatus();
    desc.dispS2MMStatus();

    cma_free(srcBuf);
    cma_free(dstBuf);

    fast.destroy();
    gaus.destroy();
    desc.destroy();

    return 0;
}