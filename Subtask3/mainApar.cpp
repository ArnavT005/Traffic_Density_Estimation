#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <pthread.h>
//#include "densityEstimation.h"
//#include "error.h"
//#include "method1.h"
//#include "method3.h"
//#include "method4.h"
//#include "method5.h"
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

cv::Mat subImg(cv::Mat frame1, cv::Mat frame2, int thr = 20) {

    cv::Mat diff, thresh;

    //grayscale
    cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame1, frame1, cv::Size(5, 5), 0);

    //grayscale
    cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame2, frame2, cv::Size(5, 5), 0);

    //absolute frame diff
    cv::absdiff(frame1, frame2, diff);

    //threshold
    cv::threshold(diff, thresh, thr, 255, cv::THRESH_BINARY);

    // DILATE & ERODE
    cv::Mat kernel5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat kernel21x21 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));

    cv::erode(thresh, thresh, kernel5x5);
    cv::dilate(thresh, thresh, kernel21x21);

    return thresh;

}

float findArea(cv::Mat thresh) {
    float area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    return area;
}

cv::Mat warpAndCrop(cv::Mat image, cv::Mat matrix) {
    cv::Size size = image.size();
    cv::Mat transformed_image, cropped_image;
    //warp image using matrix
    cv::warpPerspective(image, transformed_image, matrix, size);
    //crop using preselected points
    cropped_image = transformed_image(cv::Range(52, 831), cv::Range(472, 801));
    return cropped_image;
}

void baseline(cv::VideoCapture video, cv::Mat background, cv::Mat matrix, ofstream &file) {
    video.set(cv::CAP_PROP_POS_MSEC, 0);

    cv::Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    file<<"Time,DenseQueue,DenseMove\n";

    while (true) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        if (frame2.empty()) {
            break;
        }

        frame2 = warpAndCrop(frame2, matrix);

        //Queue Density - subtract background
        thresh = subImg(background.clone(), frame2.clone(), 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1.clone(), frame2.clone());
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

vector<float> error(ifstream& base, ifstream& file, string move, string queue, int x = 3) {
    vector<float> error = {0, 0};
    ofstream fmove(move), fqueue(queue);
    int baseLineNumber = 1, fileLineNumber, currentLine = 1;
    string buffer, baseQ, baseM, fileQ, fileM;
    bool flag = false;
    getline(base, buffer);
    getline(file, buffer);
    fmove<<"Time, Output, Baseline\n";
    fqueue<<"Time, Output, Baseline\n";

    getline(file, buffer, ',');
    getline(file, fileQ, ',');
    getline(file, fileM);
    
    while (getline(base, buffer, ',')) {
        getline(base, baseQ, ',');
        getline(base, baseM);
        fileLineNumber = (3 * baseLineNumber) / x;
        if (fileLineNumber == 0) {
            error[0] += (stof(baseQ)) * (stof(baseQ));
            error[1] += (stof(baseM)) * (stof(baseM));
            fmove<<buffer<<",0,"<<baseM<<"\n";
            fqueue<<buffer<<",0,"<<baseQ<<"\n";
            baseLineNumber++;
            continue;
        }
        while (!flag && currentLine < fileLineNumber) {
            if (!getline(file, buffer, ',')) {
                flag = true;
                break;
            }
            getline(file, fileQ, ',');
            getline(file, fileM);
            currentLine++;
        }
        //cout<<fileQ;
        error[0] += (stof(baseQ) - stof(fileQ)) * (stof(baseQ) - stof(fileQ));
        error[1] += (stof(baseM) - stof(fileM)) * (stof(baseM) - stof(fileM));
        fmove<<buffer<<","<<fileM<<","<<baseM<<"\n";
        fqueue<<buffer<<","<<fileQ<<","<<baseQ<<"\n";
        baseLineNumber++;
    }
    error[0] /= (baseLineNumber - 1);
    error[0] = sqrt(error[0]);
    error[1] /= (baseLineNumber - 1);
    error[1] = sqrt(error[1]);
    fmove.close();
    fqueue.close();
    return error;
}

void M1_subSample(cv::VideoCapture video, cv::Mat background, cv::Mat matrix, int x, ofstream &file) {
    video.set(cv::CAP_PROP_POS_MSEC, 0);

    cv::Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = x, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    file<<"Time,DenseQueue,DenseMove\n";

    while (true) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        if (frame2.empty()) {
            break;
        }

        frame2 = warpAndCrop(frame2, matrix);

        //Queue Density - subtract background
        thresh = subImg(background.clone(), frame2.clone(), 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1.clone(), frame2.clone());
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

void M3_reduceResol(cv::VideoCapture video, cv::Mat background, cv::Mat matrix, int x, int y, ofstream &file) {
    video.set(cv::CAP_PROP_POS_MSEC, 0);

    cv::Mat frame1, frame2, thresh;
    
    // total image area
    cv::resize(background, background, cv::Size(x, y));
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    cv::resize(frame1, frame1, cv::Size(x, y));
    file<<"Time,DenseQueue,DenseMove\n";

    while (true) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        if (frame2.empty()) {
            break;
        }

        frame2 = warpAndCrop(frame2, matrix);
        cv::resize(frame2, frame2, cv::Size(x, y));

        //Queue Density - subtract background
        thresh = subImg(background.clone(), frame2.clone(), 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1.clone(), frame2.clone());
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

struct m4{
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat background;
    float areaM, areaQ;
};

void* findDiffArea(void* arg) {
    m4* args = (m4*) arg;
    cv::Mat diff, thresh, frame1, frame2, background;
    frame1 = args-> frame1.clone();
    frame2 = args-> frame2.clone();
    background = args -> background.clone();

    //grayscale
    cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame1, frame1, cv::Size(5, 5), 0);

    //grayscale
    cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame2, frame2, cv::Size(5, 5), 0);

    cv::cvtColor(background, background, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(background, background, cv::Size(5, 5), 0);

    //absolute frame diff
    cv::absdiff(frame1, frame2, diff);

    //threshold
    cv::threshold(diff, thresh, 20, 255, cv::THRESH_BINARY);

    // DILATE & ERODE
    cv::Mat kernel5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat kernel21x21 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));

    cv::erode(thresh, thresh, kernel5x5);
    cv::dilate(thresh, thresh, kernel21x21);
    
    float area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    
    args -> areaM = area;

    //absolute frame diff
    cv::absdiff(frame2, background, diff);

    //threshold
    cv::threshold(diff, thresh, 40, 255, cv::THRESH_BINARY);
    cv::erode(thresh, thresh, kernel5x5);
    cv::dilate(thresh, thresh, kernel21x21);
    
    area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    
    args -> areaQ = area;
    
    return (void*) args;
}

void M4_spatialSplit(cv::VideoCapture video, cv::Mat background, cv::Mat matrix, int x, ofstream &file) {
    cv::Mat frame1, frame2, thresh;
    video.set(cv::CAP_PROP_POS_MSEC, 0);

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    pthread_t t[8];
    m4 arg[8];
    m4* args[8];
    
    //divide background
    for ( int i = 0; i < x; i++ ){
        arg[i].background = background(Range((i*background.rows)/x, ((i+1)*background.rows)/x), Range(0, background.cols)).clone();
    }
    file<<"Time,DenseQueue,DenseMove\n";
    while (true) {
        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        if (frame2.empty()) {
            break;
        }
        
        frame2 = warpAndCrop(frame2, matrix);
        
        //Queue Density - subtract background
        //topleft = frame2(Range(0, frame2.rows/2), Range(0, frame2.cols/2));
        for ( int i = 0; i < x; i++ ){
            arg[i].frame1 = frame1(Range((i*frame1.rows)/x, ((i+1)*frame1.rows)/x), Range(0, frame1.cols));
            arg[i].frame2 = frame2(Range((i*frame2.rows)/x, ((i+1)*frame2.rows)/x), Range(0, frame2.cols));
        }
        
        for(int i = 0; i < x; i++)
        {
           pthread_create(&t[i], NULL, &findDiffArea, (void*)(&arg[i]));
        }
        for(int i = 0; i < x; i++)
        {
            pthread_join(t[i], (void**)&args[i]);
            arg[i] = *args[i];
        }
        
        //findDiffArea(&arg[0]);
        float tempM = 0, tempQ = 0;
        for( int i = 0 ; i < x; i++){
            tempM += arg[i].areaM;
        }
        for( int i = 0 ; i < x; i++){
            tempQ += arg[i].areaQ;
        }
        denseQ = tempQ / AREA;

        //Dynamic Density - subtract frame1
        //thresh = subImg(frame1.clone(), frame2.clone());
        denseM = tempM / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
        
    }
}

struct m5{
    cv::VideoCapture video;
    cv::Mat background;
    cv::Mat matrix;
    string file;
    int start;
    int end;
    int magic_number;
};

void* M5(void* arg) {
    m5* args = (m5*)arg;
    cv::VideoCapture video = args-> video;
    cv::Mat background = args-> background;
    cv::Mat matrix = args-> matrix;
    ofstream file(args-> file);
    int start = args-> start;
    int magic = args -> magic_number;
    int end = args -> end;
    //start = start + (3 - start%3)%3;
    // MAGIC
    //if (start!=0) { start-=2; }
    start -= magic;
    
    video.set(cv::CAP_PROP_POS_FRAMES, start);
    cout<<start<<" "<<end<<"\n";

    cv::Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = start + 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);

    while (frame <= end) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        //MAGIC
        if (frame2.empty() || frame > end + 1) {
        //if (frame2.empty() || frame > end) {
            break;
        }

        frame2 = warpAndCrop(frame2, matrix);

        //Queue Density - subtract background
        thresh = subImg(background.clone(), frame2.clone(), 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1.clone(), frame2.clone());
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        //MAGIC
        //if(start!=0){time = (float)(frame+2)/15; }
        //else{ time = (float)frame / 15;}
        time = (float)(frame+magic) / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
    file.close();
    return NULL;
}

void M5_temporalSplit(string path, cv::Mat background, cv::Mat matrix, int x, ofstream &file) {
    pthread_t t[8];
    m5 arg[8];
    ifstream f[8];
    vector<vector<int>> magicVector = { {0}, {0,1}, {0,2,2}, {0,2,1,2}, {0,2,1,1,2}, {0,1,2,1,2,0}, {0,1,2,1,1,1,0}, {0,2,2,2,1,0,2,1} };
    cv::VideoCapture temp(path);
    int frame_cnt = temp.get(cv::CAP_PROP_FRAME_COUNT);
    int start = 0, end = frame_cnt/x;
    end = end + (3-end%3)%3;
    file<<"Time,DenseQueue,DenseMove\n";
    for(int i = 0; i < x; i++)
    {
        arg[i].video.open(path);
        arg[i].background = background;
        arg[i].matrix = matrix;
        arg[i].file = to_string(i+1) + ".txt";
        arg[i].start = start;
        arg[i].end = end;
        arg[i].magic_number = magicVector[x-1][i];
        pthread_create(&t[i], NULL, &M5, (void*)(&arg[i]));
        start = end;
        end = ((i + 2) * frame_cnt) / x;
        end = end + (3 - end % 3) % 3;
    }
    for(int i = 0; i < x; i++)
    {
        pthread_join(t[i], NULL);
        f[i].open(arg[i].file);
    }
    for(int i = 0; i < x; i++){
        string buffer;
        while(getline(f[i], buffer)){
            file<<buffer<<"\n";
        }
    }
    remove("1.txt");
    remove("2.txt");
    remove("3.txt");
    remove("4.txt");
    remove("5.txt");
    remove("6.txt");
    remove("7.txt");
    remove("8.txt");
}

//void M2_sparseDense(VideoCapture video, Mat background, Mat matrix, int x = 0) {
    // Mat frame1, frame2, thresh;
    // float AREA = background.size().area();
    // float denseQ = 0, denseM = 0, time;
    // int see_every_n_frame = 3, frame = 1;

    // //read Frame 1
    // video.read(frame1);
    // frame1 = warpAndCrop(frame1, matrix);
    // cvtColor(frame1, frame1, COLOR_BGR2GRAY);

    // if (x == 0) {
    //     while (true) {

    //         for (int i = 0; i < see_every_n_frame; i++) {
    //             video.read(frame2);
    //             frame++;
    //         }

    //         //Video End
    //         if (frame2.empty()) {
    //             break;
    //         }

    //         frame2 = warpAndCrop(frame2, matrix);

    //         thresh = subImg(background.clone(), frame2.clone(), 40);
    //         denseQ = findArea(thresh) / AREA;

    //         cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    //         Mat flow(frame1.size(), CV_32FC2);

    //         calcOpticalFlowFarneback(frame1, frame2, flow, 0.5, 3, 15, 3, 7, 1.5, 0);

    //         Mat flow_parts[2];
    //         split(flow, flow_parts);

    //         Mat magnitude, angle, magn_norm;
    //         cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
    //         normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    //         angle *= ((1.f / 360.f) * (180.f / 255.f));

    //         Mat _hsv[3], hsv, hsv8, bgr;
    //         _hsv[0] = angle;
    //         _hsv[1] = Mat::ones(angle.size(), CV_32F);
    //         _hsv[2] = magn_norm;
    //         merge(_hsv, 3, hsv);
    //         hsv.convertTo(hsv8, CV_8U, 255.0);

    //         cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    //         cvtColor(bgr, bgr, COLOR_BGR2GRAY);

    //         threshold(bgr, bgr, 15, 255, THRESH_BINARY);

    //         Mat kernel7x7 = getStructuringElement(MORPH_RECT, Size(7, 7));
    //         Mat kernel15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

    //         erode(bgr, bgr, kernel7x7);
    //         dilate(bgr, bgr, kernel15x15);

    //         denseM = findArea(bgr) / AREA;

    //         // error correction if Queue density < Dynamic density
    //         // slight error occurs due to different threshold values used
    //         // when only moving vehicles are present
    //         denseQ = denseQ > denseM ? denseQ : denseM;

    //         // video is 15 FPS
    //         time = (float)frame / 15;

    //         //f2 << time << "," << denseQ << "," << denseM << "\n";

    //         //update frame1 to frame2 and loop back
    //         frame1 = frame2;
    //     }
    // }
    // else {
    //     vector<Scalar> colors;
    //     RNG rng;
    //     for (int i = 0; i < 100; i++) {
    //         int r = rng.uniform(0, 256);
    //         int g = rng.uniform(0, 256);
    //         int b = rng.uniform(0, 256);
    //         colors.push_back(Scalar(r, g, b));
    //     }
    //     vector<Point2f> p0, p1;
    //     /*goodFeaturesToTrack(frame1, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    //     Mat mask = Mat::zeros(frame1.size(), frame1.type());*/
    //     while (true) {

    //         for (int i = 0; i < see_every_n_frame; i++) {
    //             video.read(frame2);
    //             frame++;
    //         }

    //         //Video End
    //         if (frame2.empty()) {
    //             break;
    //         }

    //         goodFeaturesToTrack(frame1, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    //         Mat mask = Mat::zeros(frame1.size(), frame1.type());

    //         frame2 = warpAndCrop(frame2, matrix);

    //         thresh = subImg(background.clone(), frame2.clone(), 40);
    //         denseQ = findArea(thresh) / AREA;

    //         cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    //         vector<uchar> status;
    //         vector<float> err;

    //         TermCriteria criteria = TermCriteria((TermCriteria::COUNT)+(TermCriteria::EPS), 10, 0.03);
    //         calcOpticalFlowPyrLK(frame1, frame2, p0, p1, status, err, Size(15, 15), 2, criteria);   

    //         vector<Point2f> good_new;

    //         for (uint i = 0; i < p0.size(); i++) {
    //             if (status[i] == 1) {
    //                 good_new.push_back(p1[i]);
    //                 line(mask, p1[i], p0[i], colors[i], 2);
    //                 circle(frame2, p1[i], 5, colors[i], -1);
    //             }
    //         }

    //         Mat img;
    //         add(frame2, mask, img);

    //         imshow("Flow", img);
    //         waitKey(30);

    //         //denseM = findArea(bgr) / AREA;

    //         // error correction if Queue density < Dynamic density
    //         // slight error occurs due to different threshold values used
    //         // when only moving vehicles are present
    //         denseQ = denseQ > denseM ? denseQ : denseM;

    //         // video is 15 FPS
    //         time = (float)frame / 15;

    //         //f2 << time << "," << denseQ << "," << denseM << "\n";

    //         //update frame1 to frame2 and loop back
    //         frame1 = frame2.clone();
    //         p0 = good_new;

    //     }


    // }
//}


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
                //M2_sparseDense(p2[j]);
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

