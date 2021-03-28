#include <pthread.h>
#include <opencv2/opencv.hpp>
#include "densityEstimation.h"

struct m4{
    Mat frame1;
    Mat frame2;
    Mat background;
    float areaM, areaQ;
};

void* findDiffArea(void* arg) {
    m4* args = (m4*) arg;
    Mat diff, thresh, frame1, frame2, background;
    frame1 = args-> frame1.clone();
    frame2 = args-> frame2.clone();
    background = args -> background.clone();

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
    
    args -> areaM = area;

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
    
    args -> areaQ = area;
    
    return (void*) args;
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