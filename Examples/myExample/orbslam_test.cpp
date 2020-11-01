//
// Created by xiang on 11/29/17.
//

// ���ļ����򿪸�������Ƶ�ļ�������ͼ�񴫵ݸ�ORB-SLAM2���ж�λ

// ��Ҫopencv
#include <opencv2/opencv.hpp>

// ORB-SLAM��ϵͳ�ӿ�
#include "System.h"

#include <string>
#include <chrono>   // for time stamp
#include <iostream>

using namespace std;

// �����ļ����ֵ��ļ�
// �����ϵͳ�ϵ�·����ͬ�����޸���
string parameterFile = "../myvideo.yaml";
string vocFile = "../ORBvoc.txt";

// ��Ƶ�ļ�
string videoFile = "../../myvideo_640x480.mp4";

int main(int argc, char **argv) {

    // ���� ORB-SLAM2 ϵͳ
    ORB_SLAM2::System SLAM(vocFile, parameterFile, ORB_SLAM2::System::MONOCULAR, true);

    cv::VideoCapture cap;
    cap.open(videoFile);    // change to 1 if you want to use USB camera.
    // cv::VideoCapture cap(0);
    // ��¼ϵͳʱ��
    cout << "slam started" << endl;
    auto start = chrono::system_clock::now();

    for(int i = 0; i < 20; i++) {
        cv::Mat frame;
        cap >> frame;   // ��ȡ�������
        if ( frame.data == nullptr ) {
	    cout << "failed to read video." << endl;
            break;
	}

        // rescale because image is too large
        cv::Mat frame_resized;
        cv::resize(frame, frame_resized, cv::Size(640,360));

        auto now = chrono::system_clock::now();
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(now - start);
        SLAM.TrackMonocular(frame_resized, double(timestamp.count())/1000.0);
        cout << "one frame is done." << endl;
    }

    SLAM.Shutdown();
    return 0;
}
