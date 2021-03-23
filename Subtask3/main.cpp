#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
//#include <pthread>
using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs
using namespace chrono;

//VARIABLES


string vid_path = "trafficvideo.mp4";
VideoCapture video(vid_path);

Mat background, frame1, frame2, diff, thresh, matrix;

vector<Point2f> source_points, trnsfrm_points;

//fbase = file baseline, f1 = file of method 1 ...
//ofstream fbase("Baseline.txt"); 
ofstream f1("M1_subSample.txt"), f2("M2_sparseDense.txt"), f3("M3_reduceResol.txt");
ofstream f4("M4_spatialSplit.txt"), f5("M5_temporalSplit.txt"), futil("UtilityReport.txt", ios::app);
ifstream fbase_in("Baseline.txt"), f1_in("M1_subSample.txt"), f2_in("M2_sparseDense.txt"), f3_in("M3_reduceResol.txt");
ifstream f4_in("M4_spatialSplit.txt"), f5_in("M5_temporalSplit.txt");

//rtbase = runtime baseline, rt1 = runtime of method 1 ...
//runtime calculation should EXCLUDE file input output!
float rtbase, rt1, rt2, rt3, rt4, rt5;


//FUNCTIONS


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

float utility(ifstream& base, ifstream& file, int x = 3) {
    int baseLineNumber = 1, fileLineNumber, currentLine = 1;
    float error = 0;
    string buffer, baseQ, baseM, fileQ, fileM;
    bool flag = false;
    while (getline(base, buffer, ',')) {
        getline(base, baseQ, ',');
        getline(base, baseM);
        fileLineNumber = (3 * baseLineNumber) / x;
        if (fileLineNumber == 0) {
            error += (stof(baseQ)) * (stof(baseQ)) + (stof(baseM)) * (stof(baseM));
            baseLineNumber++;
            continue;
        }
        while (currentLine < fileLineNumber) {
            getline(file, buffer);
            currentLine++;
        }
        
        getline(file, buffer, ',');
        getline(file, fileQ, ',');
        getline(file, fileM);
        
        error += (stof(baseQ) - stof(fileQ)) * (stof(baseQ) - stof(fileQ)) + (stof(baseM) - stof(fileM)) * (stof(baseM) - stof(fileM));
        cout << error << "\n";
        baseLineNumber++;
    }
    error /= 2 * (baseLineNumber - 1);
    return (1 / error);
}

void baseline() {
    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //set video time to 0
    video.set(CAP_PROP_POS_MSEC, 0);

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
        thresh = subImg(background, frame2, 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1, frame2);
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        //fbase << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

void M1_subSample(int x) {
    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = x, frame = 1;

    //set video time to 0
    video.set(CAP_PROP_POS_MSEC, 0);

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
        thresh = subImg(background, frame2, 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1, frame2);
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        f1 << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

void M2_sparseDense(int x) {

}

void M3_reduceResol(int x, int y) {
    // total image area
    resize(background, background, Size(x, y));
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //set video time to 0
    video.set(CAP_PROP_POS_MSEC, 0);

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
        thresh = subImg(background, frame2, 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1, frame2);
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        time = (float)frame / 15;

        f3 << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

void M4_spatialSplit(int x) {

}

void M5_temporalSplit(int x) {

}

//method parameters
int p1 = 15, p2, p3 = 200, p4 = 500, p5, p6;

//method utilities
float u1, u2, u3, u4, u5;

//Set testmi to true to test method i
bool testbase = false, testm1 = true, testm2 = false, testm3 = false, testm4 = false, testm5 = false;

// main function
int main() {
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
    Size szbase = background.size();
    // baseline resolution = 329 x 779
    background = warpAndCrop(background, matrix);
    clock_t start, end;
    //Write in text-file time | denseQ | denseM | frame number(optional). useful for graphing.
    if (testbase) { start = clock(); baseline(); end = clock(); rtbase = float(end - start) / CLOCKS_PER_SEC; }
    if (testm1) { start = clock(); M1_subSample(p1); end = clock(); rt1 = float(end - start) / CLOCKS_PER_SEC; }
    if (testm2) { start = clock(); M2_sparseDense(p2); end = clock(); rt2 = float(end - start) / CLOCKS_PER_SEC; }
    if (testm3) { start = clock(); M3_reduceResol(p3, p4); end = clock(); rt3 = float(end - start) / CLOCKS_PER_SEC; }
    if (testm4) { start = clock(); M4_spatialSplit(p5); end = clock(); rt4 = float(end - start) / CLOCKS_PER_SEC; }
    if (testm5) { start = clock(); M5_temporalSplit(p6); end = clock(); rt5 = float(end - start) / CLOCKS_PER_SEC; }

    //Print utility report in text file. Print method name | parameter value | utility | time consumed in one line. used for debugging and changes.
    //Then after the above report print comma seperated utility, runtime for final graphing.
    futil << "Baseline RunTime = " << rtbase << " secs\n" << "Baseline Resolution = " << szbase << "\n";
    if (testm1) {
        u1 = utility(fbase_in, f1_in, p1); futil << "Method 1: Sub-Sample - No. of frames to drop = " << p1 << ".\n\tUtility = " << u1 << ". RunTime = " << rt1 << " secs\n";
    }
    if (testm2) {
        u2 = utility(fbase_in, f2_in); futil << "Method 2: Sparse/Dense Flow - Type = " << p2 << ".\n\tUtility = " << u2 << ". RunTime = " << rt2 << " secs\n";
    }
    if (testm3) {
        u3 = utility(fbase_in, f3_in); futil << "Method 3: Reduce Resolution - Resolution = " << p3 << "x" << p4 << ".\n\tUtility = " << u3 << ". RunTime = " << rt3 << " secs\n";
    }
    if (testm4) {
        u4 = utility(fbase_in, f4_in); futil << "Method 4: Spatial Split - No. of frame splits = " << p5 << ".\n\tUtility = " << u4 << ". RunTime = " << rt4 << " secs\n";
    }
    if (testm5) {
        u5 = utility(fbase_in, f5_in); futil << "Method 5: Temporal Slpit - No. of threads = " << p6 << ".\n\tUtility = " << u5 << ". RunTime = " << rt5 << " secs\n\n";
    }

    //Close all files
    //fbase.close();
    f1.close();  f2.close();  f3.close();  f4.close();  f5.close();  futil.close();

    cout << "Program Executed Successfully\n";
    return 0;
}
