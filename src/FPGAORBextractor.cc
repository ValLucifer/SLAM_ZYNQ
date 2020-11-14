/**
* This file is part of ORB-SLAM2.
* This file is based on the file orb.cpp from the OpenCV library (see BSD license below).
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/
/**
* Software License Agreement (BSD License)
*
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*/


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include <chrono>

#include "ORBextractor.h"

using namespace cv;
using namespace std;

namespace ORB_SLAM2
{

const int PATCH_SIZE = 31;
const int HALF_PATCH_SIZE = 15;
const int EDGE_THRESHOLD = 19;


KeypointAndDesc::KeypointAndDesc() {}
KeypointAndDesc::KeypointAndDesc(const KeypointAndDesc & kp) {
    // printf("copy constructor called\n");
    for(int i = 0; i < 8; i++)
        this->desc[i] = kp.desc[i];
    this->posX = kp.posX;
    this->posY = kp.posY;
    this->response = kp.response;
}
KeypointAndDesc::~KeypointAndDesc() {
    // printf("destructor called\n");
}

_FPGAORBextractor::_FPGAORBextractor(int imgSizeInBytes, int maxCntKeypoints, int nlevels, float scaleFactor) {
    printf("+++++FPGAORBExtractor Constructor+++++\n");
    printf("nLevels: %d\n", nlevels);

    this->imgSizeInBytes = imgSizeInBytes;
    this->dstBufSizeInBytes = maxCntKeypoints * sizeof(KeypointAndDesc);
    this->nlevels = nlevels;
    this->scaleFactor = scaleFactor;

    fast = DMAChannel(FAST_BASE_PADDR);
    gaus = DMAChannel(GAUS_BASE_PADDR);
    desc = DMAChannel(DESC_BASE_PADDR);

    // extra 1 byte for pyramid level
    srcBuf = (uchar*)cma_alloc(imgSizeInBytes + 1, 0);
    srcBufPAddr = cma_get_phy_addr((void*)srcBuf);
    dstBuf = (KeypointAndDesc*)cma_alloc(dstBufSizeInBytes, 0);
    dstBufPAddr = cma_get_phy_addr((void*)dstBuf);
    printf("srcBuf: %x, srcBufPAddr: %x\n", srcBuf, srcBufPAddr);
    printf("dstBuf: %x, dstBufPAddr: %x\n", dstBuf, dstBufPAddr);

    fast.reset();
    gaus.reset();
    desc.reset();

    fast.startSendChannel();
    gaus.startSendChannel();
    desc.startRecvChannel();

    timeCopyImgToCmaBuffer.reserve(2000);
    timeOnFpga.reserve(2000);
    timeCopyResFromCmaBuffer.reserve(2000);

    printf("-----FPGAORBExtractor Constructor-----\n");
}

void _FPGAORBextractor::extract(const Mat &img, vector< vector<KeypointAndDesc> > &allKpAndDesc) {
    double count;
    
    auto t1 = chrono::steady_clock::now();
    memcpy(srcBuf + 1, (void*)img.data, imgSizeInBytes);
    auto t2 = chrono::steady_clock::now();
    count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
    // printf("copy img to cma buffer spent %lf s\n", count);
    timeCopyImgToCmaBuffer.push_back(count);

    int bytesRecvd;
    int totalBytesRecvd = 0;
    vector<int> cntKeypointsPerLayer(nlevels);
    t1 = chrono::steady_clock::now();
    for(int i = 0; i < nlevels; i++) {
        // auto t3 = chrono::steady_clock::now();
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

        // auto t4 = chrono::steady_clock::now();
        // count = chrono::duration_cast<std::chrono::duration<double> >(t4 - t3).count();
        // printf("layer %d, on fpga spent %lf s\n", i, count);

        // printf("received %d bytes in layer %d\n", bytesRecvd, i);
    }
    t2 = chrono::steady_clock::now();
    count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
    timeOnFpga.push_back(count);

    int offsetInKpAndDesc = 0;
    allKpAndDesc.resize(nlevels);
    t1 = chrono::steady_clock::now();
    for(int i = 0; i < nlevels; i++) {
        allKpAndDesc[i] = vector<KeypointAndDesc>(dstBuf + offsetInKpAndDesc, dstBuf + offsetInKpAndDesc + cntKeypointsPerLayer[i]);
        offsetInKpAndDesc += cntKeypointsPerLayer[i] + 1;
    }
    t2 = chrono::steady_clock::now();
    count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
    // printf("copy result from cma buffer spent %lf s\n", count);
    timeCopyResFromCmaBuffer.push_back(count);
}

void _FPGAORBextractor::printProfileInfo() {
    double avgTimeCopyImgToCmaBuffer = 0, avgTimeOnFpga = 0, avgTimeCopyResFromCmaBuffer = 0;
    for(int i = 0; i < timeCopyImgToCmaBuffer.size(); i++) {
        avgTimeCopyImgToCmaBuffer += timeCopyImgToCmaBuffer[i];
        avgTimeOnFpga += timeOnFpga[i];
        avgTimeCopyResFromCmaBuffer += timeCopyResFromCmaBuffer[i];
    }
    
    avgTimeCopyImgToCmaBuffer /= timeCopyImgToCmaBuffer.size();
    avgTimeOnFpga /= timeOnFpga.size();
    avgTimeCopyResFromCmaBuffer /= timeCopyResFromCmaBuffer.size();

    printf("average time spent on copying img to cma buffer: %lf s.\n", avgTimeCopyImgToCmaBuffer);
    printf("average time spent on fpga: %lf\n", avgTimeOnFpga);
    printf("average time spent on copying result from cma buffer: %lf\n", avgTimeCopyResFromCmaBuffer);
}

ORBextractor::ORBextractor(int _nfeatures, float _scaleFactor, int _nlevels,
         int _iniThFAST, int _minThFAST):
    nfeatures(_nfeatures), scaleFactor(_scaleFactor), nlevels(_nlevels),
    iniThFAST(_iniThFAST), minThFAST(_minThFAST)
{
    _fpgaORBextractor = _FPGAORBextractor(640 * 480, 8192, nlevels, scaleFactor);
    printf("+++++ORBextractor constructor+++++\n");
    printf("nfeatures: %d, scaleFactor: %.1f, nlevels: %d\n", nfeatures, scaleFactor, nlevels);

    mvScaleFactor.resize(nlevels);
    mvLevelSigma2.resize(nlevels);
    mvScaleFactor[0]=1.0f;
    mvLevelSigma2[0]=1.0f;
    for(int i=1; i<nlevels; i++)
    {
        mvScaleFactor[i]=mvScaleFactor[i-1]*scaleFactor;
        mvLevelSigma2[i]=mvScaleFactor[i]*mvScaleFactor[i];
    }

    mvInvScaleFactor.resize(nlevels);
    mvInvLevelSigma2.resize(nlevels);
    for(int i=0; i<nlevels; i++)
    {
        mvInvScaleFactor[i]=1.0f/mvScaleFactor[i];
        mvInvLevelSigma2[i]=1.0f/mvLevelSigma2[i];
    }

    mvImagePyramid.resize(nlevels);

    mnFeaturesPerLevel.resize(nlevels);
    float factor = 1.0f / scaleFactor;
    float nDesiredFeaturesPerScale = nfeatures*(1 - factor)/(1 - (float)pow((double)factor, (double)nlevels));

    int sumFeatures = 0;
    for( int level = 0; level < nlevels-1; level++ )
    {
        mnFeaturesPerLevel[level] = cvRound(nDesiredFeaturesPerScale);
        sumFeatures += mnFeaturesPerLevel[level];
        nDesiredFeaturesPerScale *= factor;
    }
    mnFeaturesPerLevel[nlevels-1] = std::max(nfeatures - sumFeatures, 0);

    timeTotal.reserve(2000);
    timeConvertRes.reserve(2000);

    printf("-----ORBextractor constructor-----\n");
}

void ExtractorNode::DivideNode(ExtractorNode &n1, ExtractorNode &n2, ExtractorNode &n3, ExtractorNode &n4)
{
    const int halfX = ceil(static_cast<float>(UR.x-UL.x)/2);
    const int halfY = ceil(static_cast<float>(BR.y-UL.y)/2);

    //Define boundaries of childs
    n1.UL = UL;
    n1.UR = cv::Point2i(UL.x+halfX,UL.y);
    n1.BL = cv::Point2i(UL.x,UL.y+halfY);
    n1.BR = cv::Point2i(UL.x+halfX,UL.y+halfY);
    n1.vKeys.reserve(vKeys.size());

    n2.UL = n1.UR;
    n2.UR = UR;
    n2.BL = n1.BR;
    n2.BR = cv::Point2i(UR.x,UL.y+halfY);
    n2.vKeys.reserve(vKeys.size());

    n3.UL = n1.BL;
    n3.UR = n1.BR;
    n3.BL = BL;
    n3.BR = cv::Point2i(n1.BR.x,BL.y);
    n3.vKeys.reserve(vKeys.size());

    n4.UL = n3.UR;
    n4.UR = n2.BR;
    n4.BL = n3.BR;
    n4.BR = BR;
    n4.vKeys.reserve(vKeys.size());

    //Associate points to childs
    for(size_t i=0;i<vKeys.size();i++)
    {
        const KeypointAndDesc &kp = vKeys[i];
        if(kp.posX<n1.UR.x)
        {
            if(kp.posY<n1.BR.y)
                n1.vKeys.push_back(kp);
            else
                n3.vKeys.push_back(kp);
        }
        else if(kp.posY<n1.BR.y)
            n2.vKeys.push_back(kp);
        else
            n4.vKeys.push_back(kp);
    }

    if(n1.vKeys.size()==1)
        n1.bNoMore = true;
    if(n2.vKeys.size()==1)
        n2.bNoMore = true;
    if(n3.vKeys.size()==1)
        n3.bNoMore = true;
    if(n4.vKeys.size()==1)
        n4.bNoMore = true;

}

void ORBextractor::DistributeOctTree(const vector<KeypointAndDesc>& vToDistributeKeyAndDescs, const int &minX,
                                     const int &maxX, const int &minY, const int &maxY, const int &N, const int &level,
                                     vector<KeypointAndDesc> & vResultKeys)
{
    // printf("layer: %d, toDistKp size: %d\n", level, vToDistributeKeyAndDescs.size());
    // printf("target kp cnt: %d\n", N);
    // printf("minX: %d, maxX: %d, minY: %d, maxY: %d\n", minX, maxX, minY, maxY);

    // Compute how many initial nodes   
    const int nIni = round(static_cast<float>(maxX-minX)/(maxY-minY));

    const float hX = static_cast<float>(maxX-minX)/nIni;

    list<ExtractorNode> lNodes;

    vector<ExtractorNode*> vpIniNodes;
    vpIniNodes.resize(nIni);

    for(int i=0; i<nIni; i++)
    {
        ExtractorNode ni;
        ni.UL = cv::Point2i(hX*static_cast<float>(i),0);
        ni.UR = cv::Point2i(hX*static_cast<float>(i+1),0);
        ni.BL = cv::Point2i(ni.UL.x,maxY-minY);
        ni.BR = cv::Point2i(ni.UR.x,maxY-minY);
        ni.vKeys.reserve(vToDistributeKeyAndDescs.size());

        lNodes.push_back(ni);
        vpIniNodes[i] = &lNodes.back();
    }

    // printf("1\n");
    // printf("\t%d\n", vToDistributeKeyAndDescs.size());
    // printf("\t%d\n", vpIniNodes.size());
    // printf("\t%f\n", hX);

    //Associate points to childs
    for(size_t i=0;i<vToDistributeKeyAndDescs.size();i++)
    {
        const KeypointAndDesc &kpAndDesc = vToDistributeKeyAndDescs[i];
        // printf("%d %d %f %f\n", i, kpAndDesc.posX, hX, kpAndDesc.posX/hX);
        if(kpAndDesc.posX/hX < vpIniNodes.size())
            vpIniNodes[kpAndDesc.posX/hX]->vKeys.push_back(kpAndDesc);
    }

    // printf("2\n");

    list<ExtractorNode>::iterator lit = lNodes.begin();

    while(lit!=lNodes.end())
    {
        if(lit->vKeys.size()==1)
        {
            lit->bNoMore=true;
            lit++;
        }
        else if(lit->vKeys.empty())
            lit = lNodes.erase(lit);
        else
            lit++;
    }

    // printf("3\n");

    bool bFinish = false;

    int iteration = 0;

    vector<pair<int,ExtractorNode*> > vSizeAndPointerToNode;
    vSizeAndPointerToNode.reserve(lNodes.size()*4);

    while(!bFinish)
    {
        iteration++;

        int prevSize = lNodes.size();

        lit = lNodes.begin();

        int nToExpand = 0;

        vSizeAndPointerToNode.clear();

        while(lit!=lNodes.end())
        {
            if(lit->bNoMore)
            {
                // If node only contains one point do not subdivide and continue
                lit++;
                continue;
            }
            else
            {
                // If more than one point, subdivide
                ExtractorNode n1,n2,n3,n4;
                lit->DivideNode(n1,n2,n3,n4);

                // Add childs if they contain points
                if(n1.vKeys.size()>0)
                {
                    lNodes.push_front(n1);                    
                    if(n1.vKeys.size()>1)
                    {
                        nToExpand++;
                        vSizeAndPointerToNode.push_back(make_pair(n1.vKeys.size(),&lNodes.front()));
                        lNodes.front().lit = lNodes.begin();
                    }
                }
                if(n2.vKeys.size()>0)
                {
                    lNodes.push_front(n2);
                    if(n2.vKeys.size()>1)
                    {
                        nToExpand++;
                        vSizeAndPointerToNode.push_back(make_pair(n2.vKeys.size(),&lNodes.front()));
                        lNodes.front().lit = lNodes.begin();
                    }
                }
                if(n3.vKeys.size()>0)
                {
                    lNodes.push_front(n3);
                    if(n3.vKeys.size()>1)
                    {
                        nToExpand++;
                        vSizeAndPointerToNode.push_back(make_pair(n3.vKeys.size(),&lNodes.front()));
                        lNodes.front().lit = lNodes.begin();
                    }
                }
                if(n4.vKeys.size()>0)
                {
                    lNodes.push_front(n4);
                    if(n4.vKeys.size()>1)
                    {
                        nToExpand++;
                        vSizeAndPointerToNode.push_back(make_pair(n4.vKeys.size(),&lNodes.front()));
                        lNodes.front().lit = lNodes.begin();
                    }
                }

                lit=lNodes.erase(lit);
                continue;
            }
        }       

        // Finish if there are more nodes than required features
        // or all nodes contain just one point
        if((int)lNodes.size()>=N || (int)lNodes.size()==prevSize)
        {
            bFinish = true;
        }
        else if(((int)lNodes.size()+nToExpand*3)>N)
        {

            while(!bFinish)
            {

                prevSize = lNodes.size();

                vector<pair<int,ExtractorNode*> > vPrevSizeAndPointerToNode = vSizeAndPointerToNode;
                vSizeAndPointerToNode.clear();

                sort(vPrevSizeAndPointerToNode.begin(),vPrevSizeAndPointerToNode.end());
                for(int j=vPrevSizeAndPointerToNode.size()-1;j>=0;j--)
                {
                    ExtractorNode n1,n2,n3,n4;
                    vPrevSizeAndPointerToNode[j].second->DivideNode(n1,n2,n3,n4);

                    // Add childs if they contain points
                    if(n1.vKeys.size()>0)
                    {
                        lNodes.push_front(n1);
                        if(n1.vKeys.size()>1)
                        {
                            vSizeAndPointerToNode.push_back(make_pair(n1.vKeys.size(),&lNodes.front()));
                            lNodes.front().lit = lNodes.begin();
                        }
                    }
                    if(n2.vKeys.size()>0)
                    {
                        lNodes.push_front(n2);
                        if(n2.vKeys.size()>1)
                        {
                            vSizeAndPointerToNode.push_back(make_pair(n2.vKeys.size(),&lNodes.front()));
                            lNodes.front().lit = lNodes.begin();
                        }
                    }
                    if(n3.vKeys.size()>0)
                    {
                        lNodes.push_front(n3);
                        if(n3.vKeys.size()>1)
                        {
                            vSizeAndPointerToNode.push_back(make_pair(n3.vKeys.size(),&lNodes.front()));
                            lNodes.front().lit = lNodes.begin();
                        }
                    }
                    if(n4.vKeys.size()>0)
                    {
                        lNodes.push_front(n4);
                        if(n4.vKeys.size()>1)
                        {
                            vSizeAndPointerToNode.push_back(make_pair(n4.vKeys.size(),&lNodes.front()));
                            lNodes.front().lit = lNodes.begin();
                        }
                    }

                    lNodes.erase(vPrevSizeAndPointerToNode[j].second->lit);

                    if((int)lNodes.size()>=N)
                        break;
                }

                if((int)lNodes.size()>=N || (int)lNodes.size()==prevSize)
                    bFinish = true;

            }
        }
    }

    // printf("4\n");

    // Retain the best point in each node
    // vResultKeys.clear();
    vResultKeys.reserve(nfeatures);

    // printf("5\n");
    for(list<ExtractorNode>::iterator lit=lNodes.begin(); lit!=lNodes.end(); lit++)
    {
        vector<KeypointAndDesc> &vNodeKeys = lit->vKeys;
        // printf("\t%d\n", vNodeKeys.size());
        KeypointAndDesc* pKP = &vNodeKeys[0];
        float maxResponse = pKP->response;

        int bestIdx = 0;
        for(size_t k=1;k<vNodeKeys.size();k++)
        {
            // printf("%d\n", k);
            if(vNodeKeys[k].response>maxResponse)
            {
                bestIdx = k;
                maxResponse = vNodeKeys[k].response;
            }
        }

        // printf("\t%d\n", bestIdx);
        // printf("\t%d %d %d\n", vNodeKeys[bestIdx].posX, vNodeKeys[bestIdx].posY, vNodeKeys[bestIdx].response);
        // printf("\t%d\n", vResultKeys.size());
        vResultKeys.push_back(vNodeKeys[bestIdx]);
    }

    // printf("6\n");
    // printf("%d kps in vResultKeys\n", vResultKeys.size());
}

void ORBextractor::ComputeKeyPointsOctTree(vector<vector<KeypointAndDesc> >& allKeypointAndDescs,
                                               vector< vector<KeypointAndDesc> >& distributedKeypointAndDescs)
{

    const float W = 30;

    const int imagePyramidCols[4] = {640, 320, 160, 80};
    const int imagePyramidRows[4] = {480, 240, 120, 60}; 

    double count;

    distributedKeypointAndDescs.resize(nlevels);
    for (int level = 0; level < nlevels; ++level)
    {
        const int minBorderX = EDGE_THRESHOLD-3;
        const int minBorderY = minBorderX;
        const int maxBorderX = mvImagePyramid[level].cols-EDGE_THRESHOLD+3;
        const int maxBorderY = mvImagePyramid[level].rows-EDGE_THRESHOLD+3;

        vector<KeypointAndDesc> &vToDistributeKeypointAndDescs = allKeypointAndDescs[level];

        vector<KeypointAndDesc> & layerKeypointAndDescs = distributedKeypointAndDescs[level];

        // printf("layer %d distribute starts\n", level);
        // auto t1 = chrono::steady_clock::now();
        DistributeOctTree(vToDistributeKeypointAndDescs, minBorderX, maxBorderX,
                          minBorderY, maxBorderY, mnFeaturesPerLevel[level], level, distributedKeypointAndDescs[level]);
        // auto t2 = chrono::steady_clock::now();
        // count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
        // printf("layer %d octTree distribute done, spent %lf s.\n", level, count);

        const int scaledPatchSize = PATCH_SIZE*mvScaleFactor[level];
    }
}

void ORBextractor::operator()( InputArray _image, InputArray _mask, vector<KeyPoint>& _keypoints,
                      OutputArray _descriptors)
{
    double count;
    auto t0 = chrono::steady_clock::now();

    if(_image.empty())
        return;

    Mat image = _image.getMat();
    assert(image.type() == CV_8UC1 );

    // Pre-compute the scale pyramid
    ComputePyramid(image);

    vector< vector<KeypointAndDesc> > allKeypointAndDescs;

    // auto t1 = chrono::steady_clock::now();
    _fpgaORBextractor.extract(image, allKeypointAndDescs);
    // auto t2 = chrono::steady_clock::now();
    // count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
    // printf("fpga extract and compute desc spent %lf s.\n", count);

    vector < vector<KeypointAndDesc> > distributedKeypointAndDescs;
    ComputeKeyPointsOctTree(allKeypointAndDescs, distributedKeypointAndDescs);
    // printf("octtree distribute done\n");

    Mat descriptors;

    int nkeypoints = 0;
    for (int level = 0; level < nlevels; ++level)
        nkeypoints += (int)distributedKeypointAndDescs[level].size();
    if( nkeypoints == 0 )
        _descriptors.release();
    else
    {
        _descriptors.create(nkeypoints, 32, CV_8U);
        descriptors = _descriptors.getMat();
    }

    _keypoints.clear();
    _keypoints.reserve(nkeypoints);

    auto t1 = chrono::steady_clock::now();
    int offset = 0;
    for (int level = 0; level < nlevels; ++level)
    {
        // printf("layer: %d\n", level);

        vector<KeypointAndDesc>& keypoints = distributedKeypointAndDescs[level];
        int nkeypointsLevel = (int)keypoints.size();

        if(nkeypointsLevel==0)
            continue;

        // Compute the descriptors
        Mat desc = descriptors.rowRange(offset, offset + nkeypointsLevel);
        for(int i = 0; i < nkeypointsLevel; i++) {
            uchar* pCurDescriptor = desc.ptr<uchar>(i);
            for(int j = 0; j < 32; j++)
                // haven't converted the byte order
                // so the bytes in one uint32 will be reversed
                // but theoreticaly this won't affect the matching of keypoints
                pCurDescriptor[j] = ((uchar*)keypoints[i].desc)[j];
        }

        // printf("layer descriptor done\n");

        offset += nkeypointsLevel;

        // Scale keypoint coordinates
        float scale = mvScaleFactor[level]; //getScale(level, firstLevel, scaleFactor);
        for (vector<KeypointAndDesc>::iterator keypoint = keypoints.begin(),
                keypointEnd = keypoints.end(); keypoint != keypointEnd; ++keypoint) {
            int oriPosX = keypoint->posX*scale;
            int oriPosY = keypoint->posY*scale;
            if(0 <= oriPosX && oriPosX < 640 && 0 <= oriPosY && oriPosY < 480)
                _keypoints.push_back(cv::KeyPoint(oriPosX, oriPosY, PATCH_SIZE*scale, keypoint->response, level));
        }
        // And add the keypoints to the output
        // _keypoints.insert(_keypoints.end(), keypoints.begin(), keypoints.end());
    }
    auto t2 = chrono::steady_clock::now();
    count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
    // printf("convert kp and desc spent %lf s.\n", count);
    timeConvertRes.push_back(count);
    
    count = chrono::duration_cast<std::chrono::duration<double> >(t2 - t0).count();
    timeTotal.push_back(count);

    // printf("ORBextractor operator () done\n");
    // printf("\t%d\n", _keypoints.size());
    // printf("\t%d %d\n", _descriptors.getMat().rows, _descriptors.getMat().cols);
}

void ORBextractor::ComputePyramid(cv::Mat image)
{
    for (int level = 0; level < nlevels; ++level)
    {
        float scale = mvInvScaleFactor[level];
        Size sz(cvRound((float)image.cols*scale), cvRound((float)image.rows*scale));
        Size wholeSize(sz.width + EDGE_THRESHOLD*2, sz.height + EDGE_THRESHOLD*2);
        Mat temp(wholeSize, image.type()), masktemp;
        mvImagePyramid[level] = temp(Rect(EDGE_THRESHOLD, EDGE_THRESHOLD, sz.width, sz.height));

        // Compute the resized image
        if( level != 0 )
        {
            resize(mvImagePyramid[level-1], mvImagePyramid[level], sz, 0, 0, INTER_LINEAR);

            copyMakeBorder(mvImagePyramid[level], temp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                           BORDER_REFLECT_101+BORDER_ISOLATED);            
        }
        else
        {
            copyMakeBorder(image, temp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                           BORDER_REFLECT_101);            
        }
    }

}

void ORBextractor::printProfileInfo() {
    double avgTimeTotal = 0, avgTimeConvertRes = 0;
    for(int i = 0; i < timeTotal.size(); i++) {
        avgTimeTotal += timeTotal[i];
        avgTimeConvertRes += timeConvertRes[i];
    }
    
    avgTimeTotal /= timeTotal.size();
    avgTimeConvertRes /= timeConvertRes.size();

    printf("-----ORBextractor timing info-----\n");
    _fpgaORBextractor.printProfileInfo();
    printf("average time spent on converting result format: %lf s\n", avgTimeConvertRes);
    printf("average time spent on extract kp and compute desc: %lf\n", avgTimeTotal);
    printf("-----------------------------------\n");
}

} //namespace ORB_SLAM
