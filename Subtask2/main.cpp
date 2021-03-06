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
    
    VideoCapture video("trafficvideo.mp4");
    
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
    Mat temp;

    video.set(CAP_PROP_POS_MSEC, 173000);
    video.read(background);

    background = warpAndCrop(background, matrix);

    float AREA = 1.1 * background.size().area();

    if (!video.isOpened()) {
        cout << "ERROR";
        return 0;
    }

    int see_every_n_frame = 3;

    int frame = 0;

    float denseQ = 0, denseM = 0;

    video.set(CAP_PROP_POS_MSEC, 0);

    //Frame 1
    video.read(frame1);
    
    frame1 = warpAndCrop(frame1, matrix);
   

    while (true) {
        
        //Frame 2
        for (int i = 0; i < see_every_n_frame ; i++) {
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

        denseQ = denseQ > denseM ? denseQ : denseM;

        //Dynamic DEnsity
        thresh = subImg(frame1, frame2);
        denseM = contourAndArea(thresh) / AREA;
        //imshow("Dynamic Image", thresh);

        cout << frame << "," << denseQ << "," << denseM << "\n";
        
        //waitKey(30);
        frame1 = frame2;
    }

    return 0;

}

//cvtColor(frame1, frame1, COLOR_BGR2GRAY);
//Ptr<BackgroundSubtractor> bg;
//bg = bgsegm::createBackgroundSubtractorMOG();

/*cvtColor(frame2, frame2, COLOR_BGR2GRAY);
        Mat flow(frame1.size(), CV_32FC2);
        optflow::calcOpticalFlowSparseToDense(frame1, frame2, flow, 8, 128, 0.05f, true, 500.0f, 1.5f);
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        cvtColor(bgr, bgr, COLOR_BGR2GRAY);
        threshold(bgr, bgr, 50, 255, THRESH_BINARY);
        dilate(bgr, bgr, getStructuringElement(MORPH_RECT, Size(5, 5)));
        imshow("flow", bgr);
        imshow("frame", frame2);*/

        //cvtColor(frame1, frame1, COLOR_BGR2GRAY);
        //GaussianBlur(frame1, frame1, Size(5, 5), 0);
        //bg->apply(frame1, thresh);
        //threshold(thresh, thresh, 50, 255, THRESH_BINARY);
        //dilate(thresh, thresh, getStructuringElement(MORPH_RECT, Size(3, 3)));
        //erode(thresh, thresh, getStructuringElement(MORPH_RECT, Size(5, 5)));