#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <pthread.h>
using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs
using namespace chrono;


//fbase = file baseline, f1 = file of method 1 ...
ofstream futil("UtilityReport.txt", ios::app);


//rtbase = runtime baseline, rt1 = runtime of method 1 ...
//runtime calculation should EXCLUDE file input output!
float rtbase, rt1, rt2, rt3, rt4, rt5;

//method parameters
int p1 = 5, p2, p3 = 100, p4 = 200, p5 = 4, p6 = 8;

//method utilities
float u11, u21, u31, u41, u51;
float u12, u22, u32, u42, u52;

vector<float> e1, e2, e3, e4, e5;

//Set testmi to true to test method i
bool testbase = true, testm1 = false, testm2 = false, testm3 = false, testm4 = false, testm5 = true;

struct subArgs{
    Mat frame1;
    Mat frame2;
    Mat background;
    float areaM, areaQ;
};

struct m5{
    VideoCapture video;
    Mat background;
    Mat matrix;
    string file;
    int start;
    int end;
};

//FUNCTIONS
void* findDiffArea(void* arg) {
    Mat diff, thresh, frame1, frame2, background;
    frame1 = ((struct subArgs*)arg)-> frame1.clone();
    frame2 = ((struct subArgs*)arg)-> frame2.clone();
    background = ((struct subArgs*)arg) -> background.clone();

    //grayscale
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    GaussianBlur(frame1, frame1, Size(5, 5), 0);

    //grayscale
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);
    GaussianBlur(frame2, frame2, Size(5, 5), 0);

    cvtColor(background, background, COLOR_BGR2GRAY);
    GaussianBlur(background, background, Size(5, 5), 0);

    //absolute frame diff
    absdiff(frame1, frame2, diff);

    //threshold
    threshold(diff, thresh, 20, 255, THRESH_BINARY);

    // DILATE & ERODE
    Mat kernel5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel21x21 = getStructuringElement(MORPH_RECT, Size(21, 21));

    erode(thresh, thresh, kernel5x5);
    dilate(thresh, thresh, kernel21x21);
    
    float area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    
    ((struct subArgs*)arg) -> areaM = area;

    //absolute frame diff
    absdiff(frame2, background, diff);

    //threshold
    threshold(diff, thresh, 40, 255, THRESH_BINARY);
    erode(thresh, thresh, kernel5x5);
    dilate(thresh, thresh, kernel21x21);
    
    area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    
    ((struct subArgs*)arg) -> areaQ = area;
    
    return (void*) arg;
}

Mat subImg(Mat frame1, Mat frame2, int thr = 20) {

    Mat diff, thresh;

    //grayscale
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    GaussianBlur(frame1, frame1, Size(5, 5), 0);

    //grayscale
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);
    GaussianBlur(frame2, frame2, Size(5, 5), 0);

    //absolute frame diff
    absdiff(frame1, frame2, diff);

    //threshold
    threshold(diff, thresh, thr, 255, THRESH_BINARY);

    // DILATE & ERODE
    Mat kernel5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel21x21 = getStructuringElement(MORPH_RECT, Size(21, 21));

    erode(thresh, thresh, kernel5x5);
    dilate(thresh, thresh, kernel21x21);

    return thresh;

}

float findArea(Mat thresh) {
    float area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    return area;
}

Mat warpAndCrop(Mat image, Mat matrix) {
    Size size = image.size();
    Mat transformed_image, cropped_image;
    //warp image using matrix
    warpPerspective(image, transformed_image, matrix, size);
    //crop using preselected points
    cropped_image = transformed_image(Range(52, 831), Range(472, 801));
    return cropped_image;
}

vector<float> error(ifstream& base, ifstream& file, string move, string queue, int x = 3) {
    vector<float> error = {0, 0};
    ofstream fmove(move), fqueue(queue);
    int baseLineNumber = 1, fileLineNumber, currentLine = 1;
    string buffer, baseQ, baseM, fileQ, fileM;
    bool flag = false;
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
    error[0] /= 2 * (baseLineNumber - 1);
    error[1] /= 2 * (baseLineNumber - 1);
    fmove.close();
    fqueue.close();
    return error;
}

void baseline(VideoCapture video, Mat background, Mat matrix, ofstream &file) {
    video.set(CAP_PROP_POS_MSEC, 0);

    Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);

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

void M1_subSample(VideoCapture video, Mat background, Mat matrix, int x, ofstream &file) {
    video.set(CAP_PROP_POS_MSEC, 0);

    Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = x, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);

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

void M2_sparseDense(int x) {

}

void M3_reduceResol(VideoCapture video, Mat background, Mat matrix, int x, int y, ofstream &file) {
    video.set(CAP_PROP_POS_MSEC, 0);

    Mat frame1, frame2, thresh;
    
    // total image area
    resize(background, background, Size(x, y));
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    resize(frame1, frame1, Size(x, y));

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
        resize(frame2, frame2, Size(x, y));

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

void M4_spatialSplit(VideoCapture video, Mat background, Mat matrix, int x, ofstream &file) {
    Mat frame1, frame2, thresh;
    video.set(CAP_PROP_POS_MSEC, 0);

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    pthread_t t[8];
    subArgs arg[8];
    subArgs* args[8];
    
    //divide background
    for ( int i = 0; i < x; i++ ){
        arg[i].background = background(Range((i*background.rows)/x, ((i+1)*background.rows)/x), Range(0, background.cols)).clone();
    }
        
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

void* M5(void* arg) {
    VideoCapture video = ((struct m5*)arg)-> video;
    Mat background = ((struct m5*)arg)-> background;
    Mat matrix = ((struct m5*)arg)-> matrix;
    ofstream file(((struct m5*)arg)-> file);
    int start = ((struct m5*)arg)-> start;
    
    //start = start + (3 - start%3)%3;
    // MAGIC
    //if (start!=0) { start-=1; }
    int end = ((struct m5*)arg)-> end;
    
    video.set(CAP_PROP_POS_FRAMES, start);
    //cout<<start<<" "<<end<<" ";

    Mat frame1, frame2, thresh;

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
        //if (frame2.empty() || frame > end + 1) {
        if (frame2.empty() || frame > end) {
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
        //if(start!=0){time = (float)(frame+1)/15; }
        //else{ time = (float)frame / 15;}
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
    file.close();
    return NULL;
}

void M5_temporalSplit(string path, Mat background, Mat matrix, int x, ofstream &file) {
    pthread_t t[8];
    m5 arg[8];
    ifstream f[8];
    VideoCapture temp(path);
    int frame_cnt = temp.get(CAP_PROP_FRAME_COUNT);
    int start = 0, end = frame_cnt/x;
    end = end + (3-end%3)%3;
    int diff = end;
    for(int i = 0; i < x; i++)
    {
        arg[i].video.open(path);
        arg[i].background = background;
        arg[i].matrix = matrix;
        arg[i].file = to_string(i+1) + ".txt";
        arg[i].start = start;
        arg[i].end = end;
        pthread_create(&t[i], NULL, &M5, (void*)(&arg[i]));
        start = end;
        end += diff;
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
}

// main function
int main() {
    cout<<"Starting Execution\n";
    string vid_path = "/Users/aparahuja/Desktop/trafficvideo.mp4";
    //string vid_path = "trafficvideo.mp4";
    cout<<"Video Path: "<<vid_path<<"\n";
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

    //Write in text-file time | denseQ | denseM | frame number(optional). useful for graphing.
    if (testbase) {
        cout<<"Running Baseline Tests\n";
        ofstream fbase("Baseline.txt");
        auto start = high_resolution_clock::now();
        baseline(video, background.clone(), matrix, fbase);
        auto end = high_resolution_clock::now();
        rtbase = duration_cast<milliseconds>(end-start).count()/1000.0;
        fbase.close();
    }
    if (testm1) {
        cout<<"Running Method 1 - Subsampling with parameter "<<p1<<"\n";
        ofstream f1("M1_subSample.txt");
        auto start = high_resolution_clock::now();
        M1_subSample(video, background.clone(), matrix, p1, f1);
        auto end = high_resolution_clock::now();
        rt1 = duration_cast<milliseconds>(end-start).count()/1000.0;
        f1.close();
    }
    if (testm2) {
        cout<<"Running Method 2 - Spare v/s Dense optical flow with parameter "<<p2<<"\n";
        ofstream f2("M2_sparseDense.txt");
        auto start = high_resolution_clock::now();
        M2_sparseDense(p2);
        auto end = high_resolution_clock::now();
        rt2 = duration_cast<milliseconds>(end-start).count()/1000.0;
        f2.close();
    }
    if (testm3) {
        cout<<"Running Method 3 - Reduced Resolution with parameters "<<p3<<"x"<<p4<<"\n";
        ofstream f3("M3_reduceResol.txt");
        auto start = high_resolution_clock::now();
        M3_reduceResol(video, background.clone(), matrix, p3, p4, f3);
        auto end = high_resolution_clock::now();
        rt3 = duration_cast<milliseconds>(end-start).count()/1000.0;
        f3.close();
    }
    if (testm4) {
        cout<<"Running Method 4 - Spatial Split with parameter "<<p5<<"\n";
        ofstream f4("M4_spatialSplit.txt");
        auto start = high_resolution_clock::now();
        M4_spatialSplit(video, background.clone(), matrix, p5, f4);
        auto end = high_resolution_clock::now();
        rt4 = duration_cast<milliseconds>(end-start).count()/1000.0;
        f4.close();
    }
    if (testm5) {
        cout<<"Running Method 4 - Temporal Split with parameter "<<p6<<"\n";
        ofstream f5("M5_temporalSplit.txt");
        auto start = high_resolution_clock::now();
        M5_temporalSplit(vid_path, background.clone(), matrix, p6, f5);
        auto end = high_resolution_clock::now();
        rt5 = duration_cast<milliseconds>(end-start).count()/1000.0;
        f5.close();
    }

    //Print utility report in text file. Print method name | parameter value | utility | time consumed in one line. used for debugging and changes.
    //Then after the above report print comma seperated utility, runtime for final graphing.
    cout<<"Evaluating Utility\n";
    if(testbase){
        futil << "Baseline RunTime = " << rtbase << " secs\n" << "Baseline Resolution = " << szbase << "\n\n";
    }
    ifstream fbase_in("Baseline.txt");
    if (testm1) {
        ifstream f1_in("M1_subSample.txt");
        e1 = error(fbase_in, f1_in, "M1_moveVsBase", "M1_queueVsBase", p1);
        u11 = 1 / (0.01 + e1[0]);
        u12 = 1 / (0.01 + e1[0]);
        futil << "Method 1: Sub-Sample - No. of frames to drop = " << p1 << ".\n\tQueue Utility = " << u11 << ". Moving Utility = " << u12 << ". RunTime = " << rt1 << " secs\n\n";
    }
    if (testm2) {
        ifstream f2_in("M2_sparseDense.txt");
        e2 = error(fbase_in, f2_in, "M2_moveVsBase", "M2_queueVsBase");
        u21 = 1 / (0.01 + e2[0]);
        u22 = 1 / (0.01 + e2[1]);
        futil << "Method 2: Sparse/Dense Flow - Type = " << p2 << ".\n\tQueue Utility = " << u21 << ". Moving Utility = " << u22 << ". RunTime = " << rt2 << " secs\n\n";
    }
    if (testm3) {
        ifstream f3_in("M3_reduceResol.txt");
        e3 = error(fbase_in, f3_in, "M3_moveVsBase", "M3_queueVsBase");
        u31 = 1 / (0.01 + e3[0]);
        u32 = 1 / (0.01 + e3[1]);

        futil << "Method 3: Reduce Resolution - Resolution = " << p3 << "x" << p4 << ".\n\tQueue Utility = " << u31 << ". Moving Utility = " << u32 << ". RunTime = " << rt3 << " secs\n\n";
    }
    if (testm4) {
        ifstream f4_in("M4_spatialSplit.txt");
        e4 = error(fbase_in, f4_in, "M4_moveVsBase", "M4_queueVsBase");
        u41 = 1 / (0.01 + e4[0]);
        u42 = 1 / (0.01 + e4[1]);

        futil << "Method 4: Spatial Split - No. of frame splits = " << p5 << ".\n\tQueue Utility = " << u41 << ". Moving Utility = " << u42 << ". RunTime = " << rt4 << " secs\n\n";
    }
    if (testm5) {
        ifstream f5_in("M5_temporalSplit.txt");
        e5 = error(fbase_in, f5_in, "M5_moveVsBase", "M5_queueVsBase");
        u51 = 1 / (0.01 + e5[0]);
        u52 = 1 / (0.01 + e5[1]);
        futil << "Method 5: Temporal Split - No. of threads = " << p6 << ".\n\tQueue Utility = " << u51 << ". Moving Utility = " << u52 << ". RunTime = " << rt5 << " secs\n\n";
    }

    //Close all files
    futil.close();

    cout << "Program Executed Successfully\n";
    return 0;
}

