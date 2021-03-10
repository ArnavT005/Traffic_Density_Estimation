#include <opencv2/opencv.hpp>
//#include<opencv2/bgsegm.hpp>
//#include<opencv2/optflow.hpp>

using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs


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


// main function
int main(int argc, char** argv) {

    // argc is the number of command line arguments including the executable file
    // argv is the array of character pointers (storing strings)
    // argv[0] is the executable file
    // argv[1] should contain the relative/absolute path of the video (preferably "trafficvideo.mp4")
    // argv[2] is the optional parameter that represents FPS at which the image must be processed
    
    if (argc > 3) {

        // extra arguments provided, just a warning
        cout << "WARNING: Extra arguments provided. \n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> [<FPS_VID>{1-15}]\n";
        cout << "Refer to README.md for more details. \n\n";

    }

    if (argc >= 2) {

        VideoCapture video(argv[1]);

        if (!video.isOpened()) {

            // unable to load video
            cout << "ERROR: Unable to load video. Program terminating!\n";
            cout << "Please check if the video path provided is valid or not.\n";
            cout << "Refer to README.md for details. \n\n";

            // unsuccessful loading
            return 0;
        }

        // else video is initialized and ready for analysis

        Mat background, frame1, frame2, diff, thresh;

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

        // used to store homography matrix
        Mat matrix = findHomography(source_points, trnsfrm_points);

        //choose the frame at 173000ms as background(empty road)
        video.set(CAP_PROP_POS_MSEC, 173000);

        video.read(background);
        background = warpAndCrop(background, matrix);

        // total image area
        float AREA = background.size().area();
        float denseQ = 0, denseM = 0;

        int see_every_n_frame = 3, frame = 1;

        // set optional FPS
        if(argc > 2 && stoi(argv[2]) >=1 && stoi(argv[2]) <= 15) see_every_n_frame = int(15 / stoi(argv[2]));
        else if(argc > 2) {
            // not a valid FPS rate
            cout << "WARNING: Invalid FPS rate. Using default (5 FPS). \n";
            cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> [<FPS_VID>{1-15}]\n";
            cout << "Refer to README.md for more details. \n\n";
        }    

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

            cout << frame << "," << denseQ << "," << denseM << "\n";
            
            //update frame1 to frame2 and loop back
            frame1 = frame2;
        }
    }
    else {
        // argc < 2, so insufficient number of arguments
        cout << "ERROR: Insufficient number of arguments provided. Program terminating! \n";
        cout << "Expected number of arguments: 2, " << "Present: " << argc << ".\n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> [<FPS_VID>{1-15}]\n";
        cout << "Refer to README.md for details. \n\n";
    }

    return 0;
}
