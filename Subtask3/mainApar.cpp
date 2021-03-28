#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <pthread.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <math.h>
using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs
using namespace chrono; // for time measurement constructs

//____________________________________________________________________________________________

string vid_path = "/Users/aparahuja/Desktop/trafficvideo.mp4";
//string vid_path = "trafficvideo.mp4";

bool testbase = true; int method_number = 4;
int runTime_avgOver = 1, sleep_time = 3;

//method parameters
vector<int> p1 = {5}, p2 = {}, p5 = {4}, p6 = {7};
vector<vector<int>> p3p4 = {{100,200}};

//____________________________________________________________________________________________

//rtbase = runtime baseline, rt1 = runtime of method 1 ...
float rtbase = 0, rt1 = 0, rt2 = 0, rt3 = 0, rt4 = 0, rt5 = 0;

//utility output file
ofstream futil("UtilityReport.txt", ios::app), fgraph;

//method utilities
float u11, u21, u31, u41, u51;
float u12, u22, u32, u42, u52;

vector<float> e1, e2, e3, e4, e5;

//____________________________________________________________________________________________


// main function
int main() {
    cout<<"Starting Execution\n\n";
    cout<<"Video Path: "<<vid_path<<"\n\n";
    VideoCapture video(vid_path);
    
    Mat background, matrix;
    vector<Point2f> source_points, trnsfrm_points;
    
    //preselected source points
    source_points.push_back(Point2f(980, 224));        //top left corner
    source_points.push_back(Point2f(418, 830));        //bottom left corner
    source_points.push_back(Point2f(1506, 833));       //bottom right corner
    source_points.push_back(Point2f(1290, 211));       //top right corner

    //preselected destination points for homography
    trnsfrm_points.push_back(Point2f(472, 52));        // top left corner
    trnsfrm_points.push_back(Point2f(472, 830));       // bottom left corner
    trnsfrm_points.push_back(Point2f(800, 830));       // bottom right corner
    trnsfrm_points.push_back(Point2f(800, 52));        // top right corner

    matrix = findHomography(source_points, trnsfrm_points);

    //choose the frame at 173000ms as background(empty road)
    video.set(CAP_PROP_POS_MSEC, 173000);

    video.read(background);
    // baseline resolution = 329 x 779
    background = warpAndCrop(background, matrix);
    Size szbase = background.size();
    ifstream fbase_in("Baseline.csv");

    //Write in text-file time | denseQ | denseM | frame number(optional). useful for graphing.
    if (testbase) {
        for(int i = 0; i < runTime_avgOver; i++){
            cout<<"Running Baseline Tests\n";
            ofstream fbase("Baseline.csv");
            auto start = high_resolution_clock::now();
            baseline(video, background.clone(), matrix, fbase);
            auto end = high_resolution_clock::now();
            rtbase += duration_cast<milliseconds>(end-start).count()/1000.0;
            fbase.close();
            sleep(sleep_time);
        }
        rtbase = (float) rtbase / runTime_avgOver;
        cout<<"Evaluating Utility\n";
        futil << "Baseline RunTime = " << rtbase << " secs\n" << "Baseline Resolution = " << szbase << "\n\n";
        cout<<"Finished Baseline Testing\n\n";
    }
    if (method_number == 1) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        fgraph.open("UtilityVsRuntime.csv", ios::app);
        fgraph<<"Parameter,QueueUtility,MovingUtility,RunTime\n";
        for(int j = 0; j < p1.size(); j++){
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Running Method 1 - Subsampling with parameter "<<p1[j]<<"\n";
                ofstream f1("M1_subSample.csv");
                auto start = high_resolution_clock::now();
                M1_subSample(video, background.clone(), matrix, p1[j], f1);
                auto end = high_resolution_clock::now();
                rt1 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f1.close();
                sleep(sleep_time);
            }
            rt1 = (float) rt1 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f1_in("M1_subSample.csv");
            e1 = error(fbase_in, f1_in, "M1_moveVsBase.csv", "M1_queueVsBase.csv", p1[j]);
            f1_in.close();
            u11 = 1 / (0.01 + e1[0]);
            u12 = 1 / (0.01 + e1[0]);
            futil << "Method 1: Sub-Sample - No. of frames to drop = " << p1[j] << ".\n\tQueue Utility = " << u11 << ". Moving Utility = " << u12 << ". RunTime = " << rt1 << " secs\n\n";
            fgraph<<p1[j]<<","<<u11<<","<<u12<<","<<rt1<<"\n";
            cout<<"Finished Task "<<j+1<<"/"<<p1.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 2) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        fgraph.open("UtilityVsRuntime.csv", ios::app);
        fgraph<<"Parameter,QueueUtility,MovingUtility,RunTime\n";
        for(int j = 0; j < p2.size(); j++){
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Running Method 2 - Spare v/s Dense optical flow with parameter "<<p2[j]<<"\n";
                ofstream f2("M2_sparseDense.csv");
                auto start = high_resolution_clock::now();
                M2_sparseDense(p2[j]);
                auto end = high_resolution_clock::now();
                rt2 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f2.close();
                sleep(sleep_time);
            }
            rt2 = (float) rt2 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f2_in("M2_sparseDense.csv");
            e2 = error(fbase_in, f2_in, "M2_moveVsBase.csv", "M2_queueVsBase.csv");
            f2_in.close();
            u21 = 1 / (0.01 + e2[0]);
            u22 = 1 / (0.01 + e2[1]);
            futil << "Method 2: Sparse/Dense Flow - Type = " << p2[j] << ".\n\tQueue Utility = " << u21 << ". Moving Utility = " << u22 << ". RunTime = " << rt2 << " secs\n\n";
            fgraph<<p2[j]<<","<<u21<<","<<u22<<","<<rt2<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p2.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 3) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        fgraph.open("UtilityVsRuntime.csv", ios::app);
        fgraph<<"ParameterX,ParamterY,QueueUtility,MovingUtility,RunTime\n";
        for(int j = 0; j < p3p4.size(); j++){
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Running Method 3 - Reduced Resolution with parameters "<<p3p4[j][0]<<"x"<<p3p4[j][1]<<"\n";
                ofstream f3("M3_reduceResol.csv");
                auto start = high_resolution_clock::now();
                M3_reduceResol(video, background.clone(), matrix, p3p4[j][0], p3p4[j][1], f3);
                auto end = high_resolution_clock::now();
                rt3 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f3.close();
                sleep(sleep_time);
            }
            rt3 = (float) rt3 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f3_in("M3_reduceResol.csv");
            e3 = error(fbase_in, f3_in, "M3_moveVsBase.csv", "M3_queueVsBase.csv");
            f3_in.close();
            u31 = 1 / (0.01 + e3[0]);
            u32 = 1 / (0.01 + e3[1]);
            futil << "Method 3: Reduce Resolution - Resolution = " << p3p4[j][0] << "x" << p3p4[j][1] << ".\n\tQueue Utility = " << u31 << ". Moving Utility = " << u32 << ". RunTime = " << rt3 << " secs\n\n";
            fgraph << p3p4[j][0] << "," << p3p4[j][1] << "," << u31 << "," << u32 << "," << rt3 << "\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p3p4.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 4) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        fgraph.open("UtilityVsRuntime.csv", ios::app);
        fgraph<<"Parameter,QueueUtility,MovingUtility,RunTime\n";
        for(int j = 0; j < p5.size(); j++){
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Running Method 4 - Spatial Split with parameter "<<p5[j]<<"\n";
                ofstream f4("M4_spatialSplit.csv");
                auto start = high_resolution_clock::now();
                M4_spatialSplit(video, background.clone(), matrix, p5[j], f4);
                auto end = high_resolution_clock::now();
                rt4 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f4.close();
                sleep(sleep_time);
            }
            rt4 = (float) rt4 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f4_in("M4_spatialSplit.csv");
            e4 = error(fbase_in, f4_in, "M4_moveVsBase.csv", "M4_queueVsBase.csv");
            f4_in.close();
            u41 = 1 / (0.01 + e4[0]);
            u42 = 1 / (0.01 + e4[1]);
            futil << "Method 4: Spatial Split - No. of frame splits = " << p5[j] << ".\n\tQueue Utility = " << u41 << ". Moving Utility = " << u42 << ". RunTime = " << rt4 << " secs\n\n";
            fgraph<<p5[j]<<","<<u41<<","<<u42<<","<<rt4<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p5.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 5) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        fgraph.open("UtilityVsRuntime.csv", ios::app);
        fgraph<<"Parameter,QueueUtility,MovingUtility,RunTime\n";
        for(int j = 0; j < p6.size(); j++){
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Running Method 4 - Temporal Split with parameter "<<p6[j]<<"\n";
                ofstream f5("M5_temporalSplit.csv");
                auto start = high_resolution_clock::now();
                M5_temporalSplit(vid_path, background.clone(), matrix, p6[j], f5);
                auto end = high_resolution_clock::now();
                rt5 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f5.close();
                sleep(sleep_time);
            }
            rt5 = (float) rt5 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f5_in("M5_temporalSplit.csv");
            e5 = error(fbase_in, f5_in, "M5_moveVsBase.csv", "M5_queueVsBase.csv");
            f5_in.close();
            u51 = 1 / (0.01 + e5[0]);
            u52 = 1 / (0.01 + e5[1]);
            futil << "Method 5: Temporal Split - No. of threads = " << p6[j] << ".\n\tQueue Utility = " << u51 << ". Moving Utility = " << u52 << ". RunTime = " << rt5 << " secs\n\n";
            fgraph<<p6[j]<<","<<u51<<","<<u52<<","<<rt5<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p6.size()<<"\n\n";
        }fgraph.close();
    }

    //Close utility file
    futil.close();
    fbase_in.close();

    cout << "Program Executed Successfully\n";
    return 0;
}

