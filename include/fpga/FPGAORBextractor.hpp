#include <cstdio>
#include <cstring>
#include <vector>
#include <list>

#include <opencv/cv.h>
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

namespace ORB_SLAM2
{
    // data struct received from PL
typedef struct KeypointAndDesc {
    uint32_t desc[8];   // 256-bit descriptor(already been rotated to main direction)
    uint32_t posX;      // horizontal coordinate
    uint32_t posY;      // vertical coordinate
    uint32_t response;  // response value
} KeypointAndDesc;

class _FPGAORBextractor {
public:

    _FPGAORBextractor() {}

    _FPGAORBextractor(int imgSizeInBytes, int maxCntKeypoints) {
        printf("+++++FPGAORBExtractor Constructor+++++\n");

        this->imgSizeInBytes = imgSizeInBytes;
        this->dstBufSizeInBytes = maxCntKeypoints * sizeof(KeypointAndDesc);

        fast = DMAChannel(FAST_BASE_PADDR);
        gaus = DMAChannel(GAUS_BASE_PADDR);
        desc = DMAChannel(DESC_BASE_PADDR);

        srcBuf = (uchar*)cma_alloc(imgSizeInBytes + 1, 0);
        srcBufPAddr = cma_get_phy_addr((void*)srcBuf);
        dstBuf = (KeypointAndDesc*)cma_alloc(dstBufSizeInBytes, 0);
        dstBufPAddr = cma_get_phy_addr((void*)dstBuf);

        fast.startSendChannel();
        gaus.startSendChannel();
        desc.startRecvChannel();

        printf("-----FPGAORBExtractor Constructor-----\n");
    }

    void extract(const Mat &img, vector< vector<KeypointAndDesc> > &allKpAndDesc) {
        memcpy(srcBuf + 1, (void*)img.data, imgSizeInBytes);

        int bytesRecvd;
        int totalBytesRecvd = 0;
        int cntKeypointsPerLayer[4];
        for(int i = 0; i < 4; i++) {
            *srcBuf = i;

            desc.recv(dstBufPAddr + totalBytesRecvd, dstBufSizeInBytes);
            gaus.send(srcBufPAddr, imgSizeInBytes + 1);
            fast.send(srcBufPAddr, imgSizeInBytes + 1);

            fast.waitforSendDone();
            gaus.waitforSendDone();
            desc.waitforRecvDone();

            bytesRecvd = desc.getBytesRecvd();
            cntKeypointsPerLayer[i] = (bytesRecvd / sizeof(KeypointAndDesc)) - 1;
            totalBytesRecvd += bytesRecvd;
        }

        int offsetInBytes = 0;
        allKpAndDesc.resize(cntLayers);
        for(int i = 0; i < cntLayers; i++) {
            allKpAndDesc[i].resize(cntKeypointsPerLayer[i]);
            memcpy(&allKpAndDesc[i], dstBuf + offsetInBytes, cntKeypointsPerLayer[i] * sizeof(KeypointAndDesc));
            // extra 1 for terminating sign
            offsetInBytes += (cntKeypointsPerLayer[i] + 1) * sizeof(KeypointAndDesc);
        }
    }

private:
    int imgSizeInBytes, dstBufSizeInBytes;

    DMAChannel fast, gaus, desc;

    uchar *srcBuf;
    KeypointAndDesc *dstBuf;
    unsigned long srcBufPAddr, dstBufPAddr;
};

class ExtractorNode
{
public:
    ExtractorNode():bNoMore(false){}

    void DivideNode(ExtractorNode &n1, ExtractorNode &n2, ExtractorNode &n3, ExtractorNode &n4);

    std::vector<KeypointAndDesc> vKeys;
    cv::Point2i UL, UR, BL, BR;
    std::list<ExtractorNode>::iterator lit;
    bool bNoMore;
};

class FPGAORBextractor
{
public:
    
    enum { HARRIS_SCORE=0, FAST_SCORE=1 };

    FPGAORBextractor(int nfeatures, float scaleFactor, int nlevels,
                 int iniThFAST, int minThFAST);

    ~FPGAORBextractor(){}

    // Compute the ORB features and descriptors on an image.
    // ORB are dispersed on the image using an octree.
    // Mask is ignored in the current implementation.
    void operator()( cv::InputArray image, cv::InputArray mask,
      std::vector<cv::KeyPoint>& keypoints,
      cv::OutputArray descriptors);

    int inline GetLevels(){
        return nlevels;}

    float inline GetScaleFactor(){
        return scaleFactor;}

    std::vector<float> inline GetScaleFactors(){
        return mvScaleFactor;
    }

    std::vector<float> inline GetInverseScaleFactors(){
        return mvInvScaleFactor;
    }

    std::vector<float> inline GetScaleSigmaSquares(){
        return mvLevelSigma2;
    }

    std::vector<float> inline GetInverseScaleSigmaSquares(){
        return mvInvLevelSigma2;
    }

    std::vector<cv::Mat> mvImagePyramid;

protected:

    _FPGAORBextractor _fpgaORBextractor;

    void ComputeKeyPointsOctTree(vector<std::vector<KeypointAndDesc> >& allKeypointsAndDesc, 
                                 vector< vector<KeypointAndDesc> >& resultKeypointsAndDesc);    
    std::vector<KeypointAndDesc> DistributeOctTree(const std::vector<KeypointAndDesc>& vToDistributeKeypointAndDescs, 
                                                   const int &minX, const int &maxX, const int &minY, const int &maxY,
                                                   const int &nFeatures, const int &level);

    int nfeatures;
    double scaleFactor;
    int nlevels;
    int iniThFAST;
    int minThFAST;

    std::vector<int> mnFeaturesPerLevel;

    std::vector<int> umax;

    std::vector<float> mvScaleFactor;
    std::vector<float> mvInvScaleFactor;    
    std::vector<float> mvLevelSigma2;
    std::vector<float> mvInvLevelSigma2;
};

} //namespace ORB_SLAM