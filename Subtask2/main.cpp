#include <opencv2/opencv.hpp>
//#include<opencv2/bgsegm.hpp>
//#include<opencv2/optflow.hpp>

using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs


Mat subImg(Mat frame1, Mat frame2, int thr = 30, int dil = 1) {

    Mat diff, thresh, img1, img2, img3;

    //grayscale
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    GaussianBlur(frame1, frame1, Size(5, 5), 0);

    //grayscale
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);
    GaussianBlur(frame2, frame2, Size(5, 5), 0);

    //frame diff
    absdiff(frame1, frame2, diff);

    //threshold
    threshold(diff, thresh, thr, 255, THRESH_BINARY);

    // DILATE & ERODE
    Mat kernel5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel9x9 = getStructuringElement(MORPH_RECT, Size(9, 9));
    Mat kernel11x11 = getStructuringElement(MORPH_RECT, Size(11, 11));

    if (dil == 1) {
        dilate(thresh, thresh, kernel11x11);
        erode(thresh, thresh, kernel5x5);
    }
    else {
        dilate(thresh, thresh, kernel9x9);
        erode(thresh, thresh, kernel5x5);
    }


    return thresh;

}


float contourAndArea(Mat thresh) {

    vector<vector<Point>> contour;
    float area = 0;

    findContours(thresh, contour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> hull(contour.size());

    for (int i = 0; i < contour.size(); i++) {
        convexHull(contour[i], hull[i]);
    }

    for (int i = 0; i < contour.size(); i++) {
        area += contourArea(hull[i]);
    }

    //drawContours(thresh, hull, -1, Scalar(255, 0, 0));

    return area;

}

Mat warpAndCrop(Mat image, Mat matrix) {
    Size size = image.size();
    Mat transformed_image, cropped_image;

    warpPerspective(image, transformed_image, matrix, size);
    cropped_image = transformed_image(Range(52, 831), Range(472, 801));

    return cropped_image;
}


// main function
int main(int argc, char** argv) {

    // argc is the number of command line arguments including the executable file
    // argv is the array of character pointers (storing strings)
    // argv[0] is the executable file
    // argv[1] should contain the relative/absolute path of the video (preferably "trafficvideo.mp4")

    if (argc > 2) {

        // extra arguments provided, just a warning
        cout << "WARNING: Extra arguments provided. \n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH> <IMG2_PATH>\n";
        cout << "Refer to README.md for more details. \n\n";

    }

    if (argc >= 2) {

        VideoCapture video(argv[1]);

        if (!video.isOpened()) {

            // unable to load video
            cout << "ERROR: Unable to load video. Program terminating!\n";
            cout << "Please check if the video path provided is valid or not.\n";
            cout << "Refer to README.md for details. \n\n";

            // unsuccessful job
            return 0;
        }

        // else video is initialized and ready for analysis

        Mat background, frame1, frame2, diff, thresh;

        vector<Point2f> source_points, trnsfrm_points;

        source_points.push_back(Point2f(980, 224));
        source_points.push_back(Point2f(418, 830));
        source_points.push_back(Point2f(1506, 833));
        source_points.push_back(Point2f(1290, 211));

        trnsfrm_points.push_back(Point2f(472, 52));        // top left corner
        trnsfrm_points.push_back(Point2f(472, 830));       // bottom left corner
        trnsfrm_points.push_back(Point2f(800, 830));       // bottom right corner
        trnsfrm_points.push_back(Point2f(800, 52));        // top right corner

        // used to store homography matrix
        Mat matrix = findHomography(source_points, trnsfrm_points);

        video.set(CAP_PROP_POS_MSEC, 173000);

        video.read(background);
        background = warpAndCrop(background, matrix);

        float AREA = 1.2 * background.size().area();
        float denseQ = 0, denseM = 0;;

        int see_every_n_frame = 3, frame = 1;

        video.set(CAP_PROP_POS_MSEC, 0);

        //Frame 1
        video.read(frame1);
        frame1 = warpAndCrop(frame1, matrix);


        while (true) {

            //Frame 2
            for (int i = 0; i < see_every_n_frame; i++) {
                video.read(frame2);
                frame++;
            }

            //Video End
            if (frame2.empty()) {
                break;
            }

            frame2 = warpAndCrop(frame2, matrix);


            //Queue Density
            thresh = subImg(background, frame2, 40, 0);
            denseQ = contourAndArea(thresh) / AREA;
            //imshow("Queue Image", thresh);

            //Dynamic DEnsity
            thresh = subImg(frame1, frame2);
            denseM = contourAndArea(thresh) / AREA;
            //imshow("Dynamic Image", thresh);

            denseQ = denseQ > denseM ? denseQ : denseM;

            cout << frame << "," << denseQ << "," << denseM << "\n";

            //waitKey(30);
            frame1 = frame2;
        }
    }
    else {
        // argc < 2, so insufficient number of arguments
        cout << "ERROR: Insufficient number of arguments provided. Program terminating! \n";
        cout << "Expected number of arguments: 2, " << "Present: " << argc << ".\n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <VID_PATH>\n";
        cout << "Refer to README.md for details. \n\n";
    }

    return 0;
}
