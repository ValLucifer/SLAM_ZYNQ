#ifndef ORBEXTRACTOR_H
#define ORBEXTRACTOR_H

#include <cstdio>
#include <cstring>
#include <vector>
#include <list>

#include <opencv/cv.h>
#include <opencv2/core.hpp>

extern "C" {
#include "cma/libxlnk_cma.h"
}

#include "fpga/DmaDriver.h"

#include <chrono>
using namespace std;
using namespace cv;

// DMA regs base address (set in vivado address editor)
const unsigned int GAUS_BASE_PADDR = 0x40400000;
const unsigned int FAST_BASE_PADDR = 0x40410000;
const unsigned int DESC_BASE_PADDR = 0x40420000;

namespace ORB_SLAM2
{
    // data struct received from PL
class KeypointAndDesc {
public:
    uint32_t desc[8] = {0};   // 256-bit descriptor(already been rotated to main direction)
    uint32_t posX = 0;      // horizontal coordinate
    uint32_t posY = 0;      // vertical coordinate
    uint32_t response = 0;  // response value

    KeypointAndDesc();
    KeypointAndDesc(const KeypointAndDesc &kp);

    ~KeypointAndDesc();
};

class _FPGAORBextractor {
public:

    _FPGAORBextractor() {}
    _FPGAORBextractor(int imgSizeInBytes, int maxCntKeypoints, int nLevels, float factor);

    void extract(const Mat &img, vector< vector<KeypointAndDesc> > &allKpAndDesc);

    void printProfileInfo();
private:
    int imgSizeInBytes, dstBufSizeInBytes;
    int nlevels;
    float scaleFactor;

    DMAChannel fast, gaus, desc;

    uchar *srcBuf;
    KeypointAndDesc *dstBuf;
    unsigned long srcBufPAddr, dstBufPAddr;

    vector<double> timeCopyImgToCmaBuffer, timeOnFpga, timeCopyResFromCmaBuffer;
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

class ORBextractor
{
public:
    
    enum { HARRIS_SCORE=0, FAST_SCORE=1 };

    ORBextractor(int nfeatures, float scaleFactor, int nlevels,
                 int iniThFAST, int minThFAST);

    ORBextractor(){}

    ~ORBextractor(){}

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

    void printProfileInfo();

    std::vector<cv::Mat> mvImagePyramid;
    std::vector<double> timeTotal;

    void printProfileInfo();


protected:

    _FPGAORBextractor _fpgaORBextractor;

    void ComputeKeyPointsOctTree(vector<std::vector<KeypointAndDesc> >& allKeypointsAndDesc, 
                                 vector< vector<KeypointAndDesc> >& resultKeypointsAndDesc);    
    vector<KeypointAndDesc> DistributeOctTree(const std::vector<KeypointAndDesc>& vToDistributeKeypointAndDescs, 
                                                   const int &minX, const int &maxX, const int &minY, const int &maxY,
                                                   const int &nFeatures, const int &level);
    void DistributeOctTree(const std::vector<KeypointAndDesc>& vToDistributeKeypointAndDescs, 
                           const int &minX, const int &maxX, const int &minY, const int &maxY,
                           const int &nFeatures, const int &level, vector<KeypointAndDesc> &vResultKeys);
    
    void ComputePyramid(cv::Mat image);

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

    vector<double> timeTotal, timeConvertRes;
};

} //namespace ORB_SLAM

#endif