#include "headerFiles/densityEstimation.h"
#include "headerFiles/error.h"
#include "headerFiles/baseline.h"
#include "headerFiles/method1.h"
#include "headerFiles/method2.h"
#include "headerFiles/method3.h"
#include "headerFiles/method4.h"
#include "headerFiles/method5.h"

//____________________________________________________________________________________________

string vid_path = "trafficvideo.mp4";
//string vid_path = "trafficvideo.mp4";

bool testbase = false; int method_number = -1;
int runTime_avgOver = 1, sleep_time = 10;

//method parameters
vector<int> p1 = {}, p2 = {}, p5 = {}, p6 = {};
vector<vector<int>> p3p4 = {};

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

bool isInteger(string str) {
    int len = str.length();
    for (int i = 0; i < len; i++) {
        if (str.at(i) < '0' || str.at(i) > '9')
            return false;
    }
    return true;
}

void invalidargs(){
    cout << "ERROR: Incorrect arguments provided. \n";
    cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> <METHOD_NUMBER> <METHOD_PARAMETERS> [AVG]\n";
    cout << "Refer to README.md for more details. \n\n";
}

// main function
int main(int argc, char** argv) {
    cout<<"Starting Execution\n\n";
    if (argc > 6 || argc < 3) {
        invalidargs();
        return 0;
    }
    
    vid_path = argv[1];
    VideoCapture video(vid_path);
    
    if (!video.isOpened()) {
        // unable to load video
        cout << "ERROR: Unable to load video. Program terminating!\n";
        cout << "Please check if the video path provided is valid or not.\n";
        cout << "Refer to README.md for details. \n\n";
        // unsuccessful loading
        return 0;
    }
    
    if(isInteger(argv[2]) && stoi(argv[2])>=0 && stoi(argv[2])<=5){
        method_number = stoi(argv[2]);
    }
    else{
        cout << "ERROR: Invalid Method number.\n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> <METHOD_NUMBER> <METHOD_PARAMETERS> [AVG]\n";
        cout << "Refer to README.md for more details. \n\n";
        return 0;
    }
    
    if(method_number==0){
        if(argc!=3 && argc != 4) { invalidargs(); return 0;}
        testbase = true;
        if(argc == 4){
            if(isInteger(argv[3])) {
                runTime_avgOver = stoi(argv[3]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
    if(method_number==1){
        if( (argc!=4 && argc!=5) || !isInteger(argv[3]) || stoi(argv[3])<=0) { invalidargs(); return 0;}
        if(stoi(argv[3]) > 5736) { 
            cout << "CONSTRAINT: Parameter is constrained to be less than or equal to 5736.\n";
            cout << "Enter a suitable number. Program terminating!\n\n";
            return 0;    
        }
        p1.push_back(stoi(argv[3]));
        if(argc == 5){
            if(isInteger(argv[4])) {
                runTime_avgOver = stoi(argv[4]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
    if(method_number==2){
        if(argc!=3 && argc != 4) { invalidargs(); return 0;}
        p2.push_back(0);
        if(argc == 4){
            if(isInteger(argv[3])) {
                runTime_avgOver = stoi(argv[3]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
    if(method_number==3){
        if( (argc!=5 && argc != 6) || !isInteger(argv[3]) || stoi(argv[3])<=0 || !isInteger(argv[4]) || stoi(argv[4])<=0) { invalidargs(); return 0;}
        vector<int> temp = {stoi(argv[3]), stoi(argv[4])};
        p3p4.push_back(temp);
        if(argc == 6){
            if(isInteger(argv[5])) {
                runTime_avgOver = stoi(argv[5]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
    if(method_number==4){
        if((argc!=4 && argc != 5)|| !isInteger(argv[3]) || stoi(argv[3])<=0) { invalidargs(); return 0;}
        if(stoi(argv[3]) > 16) { 
            cout << "CONSTRAINT: Number of pthreads is constrained to be less than or equal to 16.\n";
            cout << "Enter a suitable number. Program terminating\n\n";
            return 0;    
        }
        p5.push_back(stoi(argv[3]));
        if(argc == 5){
            if(isInteger(argv[4])) {
                runTime_avgOver = stoi(argv[4]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
    if(method_number==5){
        if((argc!=4 && argc != 5) || !isInteger(argv[3]) || stoi(argv[3])<=0) { invalidargs(); return 0;}
        if(stoi(argv[3]) > 16) { 
            cout << "CONSTRAINT: Number of pthreads is constrained to be less than or equal to 16.\n";
            cout << "Enter a suitable number. Program terminating!\n\n";
            return 0;    
        }
        p6.push_back(stoi(argv[3]));
        if(argc == 5){
            if(isInteger(argv[4])) {
                runTime_avgOver = stoi(argv[4]);
            }    
            else {
                cout << "WARNING: Optional argument not a number. Ignored.\n";
                cout << "Refer to README.md for more details. \n\n";
            }
        }
    }
                
    
    cout<<"Video Path: "<<vid_path<<"\n\n";
    
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
    
    ifstream filecheck1, filecheck;
    filecheck1.open("Baseline.csv");
    if(filecheck1) {} else {
        testbase = true;
    }

    //Write in text-file time | denseQ | denseM | frame number(optional). useful for graphing.
    if (testbase) {
        cout<<"Running Baseline Tests\n";
        for(int i = 0; i < runTime_avgOver; i++){
            cout<<"Testing Number: "<<i+1<<"\n";
            ofstream fbase("Baseline.csv");
            auto start = high_resolution_clock::now();
            baseline(video, background.clone(), matrix, fbase);
            auto end = high_resolution_clock::now();
            rtbase += duration_cast<milliseconds>(end-start).count()/1000.0;
            fbase.close();
            sleep(sleep_time+i);
        }
        rtbase = (float) rtbase / runTime_avgOver;
        futil << "Baseline RunTime = " << rtbase << " secs\n" << "Baseline Resolution = " << szbase << "\n\n";
        cout<<"Finished Baseline Testing\n\n";
    }

    if (method_number == 1) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        filecheck.open("UtilityVsRuntime.csv");
        if(filecheck) {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
        }
        else {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
            fgraph<<"Parameter,Queue Utility,Moving Utility,Runtime\n";
        }
        for(int j = 0; j < p1.size(); j++){
            ifstream fbase_in("Baseline.csv"); rt1 = 0;
            cout<<"Running Method 1 - Subsampling with parameter "<<p1[j]<<"\n";
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Testing Number: "<<i+1<<"\n";
                ofstream f1("M1_subSampleParam"+to_string(p1[j])+".csv");
                auto start = high_resolution_clock::now();
                M1_subSample(video, background.clone(), matrix, p1[j], f1);
                auto end = high_resolution_clock::now();
                rt1 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f1.close();
                sleep(sleep_time+i+j);
            }
            rt1 = (float) rt1 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f1_in("M1_subSampleParam"+to_string(p1[j])+".csv");
            e1 = error(fbase_in, f1_in, "M1vsBase_moveParam"+to_string(p1[j])+".csv", "M1vsBase_queueParam"+to_string(p1[j])+".csv", p1[j]);
            f1_in.close();     fbase_in.close();
            u11 = 10 / (0.1 + e1[0]);
            u12 = 10 / (0.1 + e1[1]);
            futil << "Method 1: Sub-Sample - No. of frames to drop = " << p1[j] << ".\n\tQueue Utility = " << u11 << ". Moving Utility = " << u12 << ". RunTime = " << rt1 << " secs\n\n";
            fgraph<<p1[j]<<","<<u11<<","<<u12<<","<<rt1<<"\n";
            cout<<"Finished Task "<<j+1<<"/"<<p1.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 2) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        filecheck.open("UtilityVsRuntime.csv");
        if(filecheck) {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
        }
        else {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
            fgraph<<"Parameter,Queue Utility,Moving Utility,Runtime\n";
        }
        for(int j = 0; j < p2.size(); j++){
            ifstream fbase_in("Baseline.csv"); rt2 = 0;
            cout<<"Running Method 2 - Spare v/s Dense optical flow with parameter "<<p2[j]<<"\n";
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Testing Number: "<<i+1<<"\n";
                ofstream f2("M2_sparseDenseParam"+to_string(p2[j])+".csv");
                auto start = high_resolution_clock::now();
                M2_sparseDense(video, background.clone(), matrix, p2[j], f2);
                auto end = high_resolution_clock::now();
                rt2 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f2.close();
                sleep(sleep_time+i+j);
            }
            rt2 = (float) rt2 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f2_in("M2_sparseDenseParam"+to_string(p2[j])+".csv");
            e2 = error(fbase_in, f2_in, "M2vsBase_moveParam"+to_string(p2[j])+".csv", "M2vsBase_queueParam"+to_string(p2[j])+".csv");
            f2_in.close();     fbase_in.close();
            u21 = 10 / (0.1 + e2[0]);
            u22 = 10 / (0.1 + e2[1]);
            futil << "Method 2: Sparse/Dense Flow - Type = " << p2[j] << ".\n\tQueue Utility = " << u21 << ". Moving Utility = " << u22 << ". RunTime = " << rt2 << " secs\n\n";
            fgraph<<p2[j]<<","<<u21<<","<<u22<<","<<rt2<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p2.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 3) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        filecheck.open("UtilityVsRuntime.csv");
        if(filecheck) {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
        }
        else {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
            fgraph<<"ParameterX,ParameterY,Queue Utility,Moving Utility,Runtime\n";
        }
        for(int j = 0; j < p3p4.size(); j++){
            ifstream fbase_in("Baseline.csv"); rt3 = 0;
            cout<<"Running Method 3 - Reduced Resolution with parameters "<<p3p4[j][0]<<"x"<<p3p4[j][1]<<"\n";
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Testing Number: "<<i+1<<"\n";
                ofstream f3("M3_reduceResolParam"+to_string(p3p4[j][0])+"x"+to_string(p3p4[j][1])+".csv");
                auto start = high_resolution_clock::now();
                M3_reduceResol(video, background.clone(), matrix, p3p4[j][0], p3p4[j][1], f3);
                auto end = high_resolution_clock::now();
                rt3 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f3.close();
                sleep(sleep_time+i+j);
            }
            rt3 = (float) rt3 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f3_in("M3_reduceResolParam"+to_string(p3p4[j][0])+"x"+to_string(p3p4[j][1])+".csv");
            e3 = error(fbase_in, f3_in, "M3vsBase_moveParam"+to_string(p3p4[j][0])+"x"+to_string(p3p4[j][1])+".csv", "M3vsBase_queueParam"+to_string(p3p4[j][0])+"x"+to_string(p3p4[j][1])+".csv");
            f3_in.close();     fbase_in.close();
            u31 = 10 / (0.1 + e3[0]);
            u32 = 10 / (0.1 + e3[1]);
            futil << "Method 3: Reduce Resolution - Resolution = " << p3p4[j][0] << "x" << p3p4[j][1] << ".\n\tQueue Utility = " << u31 << ". Moving Utility = " << u32 << ". RunTime = " << rt3 << " secs\n\n";
            fgraph << p3p4[j][0] << "," << p3p4[j][1] << "," << u31 << "," << u32 << "," << rt3 << "\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p3p4.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 4) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        filecheck.open("UtilityVsRuntime.csv");
        if(filecheck) {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
        }
        else {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
            fgraph<<"Parameter,Queue Utility,Moving Utility,Runtime\n";
        }
        for(int j = 0; j < p5.size(); j++){
            ifstream fbase_in("Baseline.csv"); rt4 = 0;
            cout<<"Running Method 4 - Spatial Split with parameter "<<p5[j]<<"\n";
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Testing Number: "<<i+1<<"\n";
                ofstream f4("M4_spatialSplitParam"+to_string(p5[j])+".csv");
                auto start = high_resolution_clock::now();
                M4_spatialSplit(video, background.clone(), matrix, p5[j], f4);
                auto end = high_resolution_clock::now();
                rt4 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f4.close();
                sleep(sleep_time+i+j);
            }
            rt4 = (float) rt4 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f4_in("M4_spatialSplitParam"+to_string(p5[j])+".csv");
            e4 = error(fbase_in, f4_in, "M4vsBase_moveParam"+to_string(p5[j])+".csv", "M4vsBase_queueParam"+to_string(p5[j])+".csv");
            f4_in.close();     fbase_in.close();
            u41 = 10 / (0.1 + e4[0]);
            u42 = 10 / (0.1 + e4[1]);
            futil << "Method 4: Spatial Split - No. of frame splits = " << p5[j] << ".\n\tQueue Utility = " << u41 << ". Moving Utility = " << u42 << ". RunTime = " << rt4 << " secs\n\n";
            fgraph<<p5[j]<<","<<u41<<","<<u42<<","<<rt4<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p5.size()<<"\n\n";
        }fgraph.close();
    }
    if (method_number == 5) {
        //ofstream fgraph("UtilityVsRuntime.csv");
        filecheck.open("UtilityVsRuntime.csv");
        if(filecheck) {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
        }
        else {
            fgraph.open("UtilityVsRuntime.csv", ios::app);
            fgraph<<"Parameter,Queue Utility,Moving Utility,Runtime\n";
        }
        for(int j = 0; j < p6.size(); j++){
            ifstream fbase_in("Baseline.csv"); rt5 = 0;
            cout<<"Running Method 5 - Temporal Split with parameter "<<p6[j]<<"\n";
            for(int i = 0; i < runTime_avgOver; i++){
                cout<<"Testing Number: "<<i+1<<"\n";
                ofstream f5("M5_temporalSplitParam"+to_string(p6[j])+".csv");
                auto start = high_resolution_clock::now();
                M5_temporalSplit(vid_path, background.clone(), matrix, p6[j], f5);
                auto end = high_resolution_clock::now();
                rt5 += duration_cast<milliseconds>(end-start).count()/1000.0;
                f5.close();
                sleep(sleep_time+i+j);
            }
            rt5 = (float) rt5 / runTime_avgOver;
            cout<<"Evaluating Utility\n";
            ifstream f5_in("M5_temporalSplitParam"+to_string(p6[j])+".csv");
            e5 = error(fbase_in, f5_in, "M5vsBase_moveParam"+to_string(p6[j])+".csv", "M5vsBase_queueParam"+to_string(p6[j])+".csv");
            f5_in.close();     fbase_in.close();
            u51 = 10 / (0.1 + e5[0]);
            u52 = 10 / (0.1 + e5[1]);
            futil << "Method 5: Temporal Split - No. of threads = " << p6[j] << ".\n\tQueue Utility = " << u51 << ". Moving Utility = " << u52 << ". RunTime = " << rt5 << " secs\n\n";
            fgraph<<p6[j]<<","<<u51<<","<<u52<<","<<rt5<<"\n";
            cout<<"Finished Task: "<<j+1<<"/"<<p6.size()<<"\n\n";
        }fgraph.close();
    }

    //Close utility file
    futil.close();

    cout << "Program Executed Successfully\n";
    return 0;
}
