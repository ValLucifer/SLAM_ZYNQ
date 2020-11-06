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

#include "fpga/FPGAORBextractor.hpp"


using namespace cv;
using namespace std;

namespace ORB_SLAM2
{

const int PATCH_SIZE = 31;
const int HALF_PATCH_SIZE = 15;
const int EDGE_THRESHOLD = 19;

FPGAORBextractor::FPGAORBextractor(int _nfeatures, float _scaleFactor, int _nlevels,
         int _iniThFAST, int _minThFAST):
    nfeatures(_nfeatures), scaleFactor(_scaleFactor), nlevels(_nlevels),
    iniThFAST(_iniThFAST), minThFAST(_minThFAST)
{
    _fpgaORBextractor = _FPGAORBextractor(640 * 480, 8192);

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

vector<KeypointAndDesc> FPGAORBextractor::DistributeOctTree(const vector<KeypointAndDesc>& vToDistributeKeyAndDescs, const int &minX,
                                       const int &maxX, const int &minY, const int &maxY, const int &N, const int &level)
{
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

    //Associate points to childs
    for(size_t i=0;i<vToDistributeKeyAndDescs.size();i++)
    {
        const KeypointAndDesc &kpAndDesc = vToDistributeKeyAndDescs[i];
        vpIniNodes[kpAndDesc.posX/hX]->vKeys.push_back(kpAndDesc);
    }

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

    // Retain the best point in each node
    vector<KeypointAndDesc> vResultKeys;
    vResultKeys.reserve(nfeatures);
    for(list<ExtractorNode>::iterator lit=lNodes.begin(); lit!=lNodes.end(); lit++)
    {
        vector<KeypointAndDesc> &vNodeKeys = lit->vKeys;
        KeypointAndDesc* pKP = &vNodeKeys[0];
        float maxResponse = pKP->response;

        for(size_t k=1;k<vNodeKeys.size();k++)
        {
            if(vNodeKeys[k].response>maxResponse)
            {
                pKP = &vNodeKeys[k];
                maxResponse = vNodeKeys[k].response;
            }
        }

        vResultKeys.push_back(*pKP);
    }

    return vResultKeys;
}

void FPGAORBextractor::ComputeKeyPointsOctTree(vector<vector<KeypointAndDesc> >& allKeypointAndDescs,
                                               vector< vector<KeypointAndDesc> >& distributedKeypointAndDescs)
{

    const float W = 30;

    distributedKeypointAndDescs.reserve(nlevels);
    for (int level = 0; level < nlevels; ++level)
    {
        const int minBorderX = EDGE_THRESHOLD-3;
        const int minBorderY = minBorderX;
        const int maxBorderX = mvImagePyramid[level].cols-EDGE_THRESHOLD+3;
        const int maxBorderY = mvImagePyramid[level].rows-EDGE_THRESHOLD+3;

        vector<KeypointAndDesc> &vToDistributeKeypointAndDescs = allKeypointAndDescs[level];

        vector<KeypointAndDesc> & layerKeypointAndDescs = distributedKeypointAndDescs[level];
        layerKeypointAndDescs.reserve(nfeatures);

        layerKeypointAndDescs = DistributeOctTree(vToDistributeKeypointAndDescs, minBorderX, maxBorderX,
                                                  minBorderY, maxBorderY,mnFeaturesPerLevel[level], level);

        const int scaledPatchSize = PATCH_SIZE*mvScaleFactor[level];

        // Add border to coordinates and scale information
        const int nkps = layerKeypointAndDescs.size();
        for(int i=0; i<nkps ; i++)
        {
            layerKeypointAndDescs[i].posX+=minBorderX;
            layerKeypointAndDescs[i].posY+=minBorderY;
            // layerKeypointAndDescs[i].octave=level;
            // layerKeypointAndDescs[i].size = scaledPatchSize;
        }
    }
}

void FPGAORBextractor::operator()( InputArray _image, InputArray _mask, vector<KeyPoint>& _keypoints,
                      OutputArray _descriptors)
{ 
    if(_image.empty())
        return;

    Mat image = _image.getMat();
    assert(image.type() == CV_8UC1 );

    vector< vector<KeypointAndDesc> > allKeypointAndDescs;
    _fpgaORBextractor.extract(image, allKeypointAndDescs);

    vector < vector<KeypointAndDesc> > distributedKeypointAndDescs;
    ComputeKeyPointsOctTree(allKeypointAndDescs, distributedKeypointAndDescs);
    //ComputeKeyPointsOld(allKeypoints);

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

    int offset = 0;
    for (int level = 0; level < nlevels; ++level)
    {
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

        offset += nkeypointsLevel;

        // Scale keypoint coordinates
        if (level != 0)
        {
            float scale = mvScaleFactor[level]; //getScale(level, firstLevel, scaleFactor);
            for (vector<KeypointAndDesc>::iterator keypoint = keypoints.begin(),
                 keypointEnd = keypoints.end(); keypoint != keypointEnd; ++keypoint) {
                // keypoint->posX *= scale;
                // keypoint->posY *= scale;

                _keypoints.push_back(cv::KeyPoint(keypoint->posX*scale, keypoint->posY*scale, 
                                                  PATCH_SIZE*scale, keypoint->response, level));
            }
        }
        // And add the keypoints to the output
        // _keypoints.insert(_keypoints.end(), keypoints.begin(), keypoints.end());
    }
}

} //namespace ORB_SLAM
