#include <cstdio>
#include <cstring>

#include <opencv2/core.hpp>
#include "cma/libxlnk_cma.h"

#include "DmaDriver.hpp"

using namespace std;
using namespace cv;

/*
     
*/

// DMA regs base address (set in vivado address editor)
const unsigned int FAST_BASE_PADDR = 0x40400000;
const unsigned int GAUS_BASE_PADDR = 0x40410000;
const unsigned int DESC_BASE_PADDR = 0x40420000;

const unsigned int cntLayers = 4;

// data struct received from PL
typedef struct KeypointAndDesc {
    uint32_t desc[8];   // 256-bit descriptor(already been rotated to main direction)
    uint32_t posX;      // horizontal coordinate
    uint32_t posY;      // vertical coordinate
    uint32_t response;  // response value
} KeypointAndDesc;

class FPGAORBExtractor {
public:

    FPGAORBExtractor(int imgSizeInBytes, int maxCntKeypoints) {
        printf("+++++FPGAORBExtractor Constructor+++++\n");

        this->imgSizeInBytes = imgSizeInBytes;
        this->dstBufSizeInBytes = maxCntKeypoints * sizeof(KeypointAndDesc);

        fast = DMAChannel(FAST_BASE_PADDR);
        gaus = DMAChannel(GAUS_BASE_PADDR);
        desc = DMAChannel(DESC_BASE_PADDR);

        srcBuf = (uchar*)cma_alloc(imgSizeInBytes, 0);
        srcBufPAddr = cma_get_phy_addr((void*)srcBuf);
        dstBuf = (KeypointAndDesc*)cma_alloc(dstBufSizeInBytes, 0);
        dstBufPAddr = cma_get_phy_addr((void*)dstBuf);

        fast.startSendChannel();
        gaus.startSendChannel();
        desc.startRecvChannel();

        printf("-----FPGAORBExtractor Constructor-----\n");
    }

    void extract(const Mat &img, vector< vector<KeyPoint> > &keypointsInPyramid, vector< vector<> > &descriptorsInPyramid) {
        memcpy(srcBuf, (void*)img.data, imgSizeInBytes);

        desc.recv(dstBufPAddr, dstBufSizeInBytes);
        gaus.send(srcBufPAddr, imgSizeInBytes);
        fast.send(srcBufPAddr, imgSizeInBytes);

        fast.waitforSendDone();
        gaus.waitforSendDone();
        desc.waitforRecvDone();

        int packetsRecvd = (desc.getBytesRecvd() / sizeof(KeypointAndDesc)) - 1;
        uint32_t *cntKeypointsPerLayer = (uint32_t*)(dstBuf + packetsRecvd);
        KeypointAndDesc *curKeypointAndDesc = dstBuf;

        keypointsInPyramid.resize(cntLayers);
        for(int i = 0; i < cntLayers; i++) {
            keypointsInPyramid[i].resize(cntKeypointsPerLayer[i]);

            KeyPoint curKeypoint;
            curKeypoint.pt.x = curKeypointAndDesc->posX;
            curKeypoint.pt.y = curKeypointAndDesc->posY;
            curKeypoint.response = curKeypointAndDesc->response;
        }
    }

private:
    int imgSizeInBytes, dstBufSizeInBytes;

    DMAChannel fast, gaus, desc;

    uchar *srcBuf;
    KeypointAndDesc *dstBuf;
    unsigned long srcBufPAddr, dstBufPAddr;

};