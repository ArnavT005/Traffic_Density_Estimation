#include <opencv2/opencv.hpp>

using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs

//clickData struct stores all info to be updated incase of any event(click)
struct clickData {

    int clickCnt;
    Mat image;
    vector<Point2f> points;

};

// MouseCallback function for setMouseCallback
void onMouse(int event, int x, int y, int flags, void* dataPointer) {

    // retrieving clickData pointer from void pointer
    clickData* data = (clickData*)dataPointer;

    // user has selected a point (left-click)
    if (event == EVENT_LBUTTONDOWN) {

        // only accept 4 points from user
        // ignores additional points
        if (data->clickCnt < 4) {

            data->points.push_back(Point2f(x, y));
            data->clickCnt++;

            // draw a white circle at the selected point
            circle((data->image), Point(x, y), 8, Scalar(255, 255, 255), FILLED);
            // update image window
            imshow("Original Frame", data->image);

        }

    }

    // right click to remove last point selected
    if (event == EVENT_RBUTTONDOWN) {

        if (data->clickCnt > 0) {

            // retrieve and delete last point
            Point2f xy = (data->points)[data->clickCnt - 1];
            data->clickCnt--;
            data->points.pop_back();

            // draw a black circle at that point to indicate deletion
            circle((data->image), Point(xy.x, xy.y), 5, Scalar(0, 0, 0), FILLED);
            // update image window
            imshow("Original Frame", data->image);

        }

    }

}


// helper function used to warp, crop and save the image in the same directory
Mat warpAndCrop(Mat path, vector<Point2f> trnsfrm_points, Mat& matrix, bool optional) {

    // read image and store it in src_img
    Mat src_img = path;
    // create a copy, as the original image is going to be modified (drawing circles etc.)
    Mat clone = src_img.clone();

    Mat transformed_image, cropped_image;

    // used to store the size of an image
    Size size;

    if (src_img.data == NULL) {

        // unable to load image
        cout << "ERROR: Unable to load image"<< ". Program terminating!\n";
        cout << "Please check if the image path provided is valid or not.\n";
        cout << "Refer to README.md for details. \n\n";

        // unsuccessful job
        return Mat();

    }

    else {

        // first image loaded

        // store the size of the image
        size = src_img.size();

        // for storing user inputs during events (mouse click)
        clickData data;
        data.clickCnt = 0;
        data.image = src_img;

        // check if selection of points is optional or not
        // select points only if it not optional
        if (!optional) {

            //display image
            imshow("Original Frame", data.image);

            //user input via mouse
            setMouseCallback("Original Frame", onMouse, &data);

            //wait until any key is pressed
            waitKey(0);

            //close all windows on key press
            destroyAllWindows();

            if (data.points.size() < 4) {

                //less than 4 point were selected before key press
                cout << "ERROR: Less than 4 points were selected. Homography cannot be determined.\n";
                cout << "Atleast four points are required. Re-run the code and select 4 points. (additional points will be ignored)\n";
                cout << "Refer to README.md for details. \n\n";

                return Mat();

            }

            // find a perspective transformation (transformation matrix)
            matrix = findHomography(data.points, trnsfrm_points);

        }

        //transform perspective, and store it in transformed_image
        warpPerspective(clone, transformed_image, matrix, size);

        //show and save cropped image
        cropped_image = transformed_image(Range(52, 831), Range(472, 801));
        
        // successful completion
        return cropped_image;

    }

}

Mat subImg(Mat frame1, Mat frame2){
    Mat diff, thresh;
    //grayscale
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    GaussianBlur(frame1, frame1, Size(5,5), 0);

    //grayscale
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);
    GaussianBlur(frame2, frame2, Size(5,5), 0);

    //frame diff
    absdiff(frame1, frame2, diff);
    //threshold. 30 = double threshold
    threshold(diff, thresh, 35, 255.0, THRESH_BINARY);
    
    // DILATE & ERODE
    Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    dilate(thresh, thresh, structuringElement3x3);
    //erode(thresh, thresh, structuringElement3x3);
    return thresh;
}
float contourAndArea(Mat thresh){
    vector<vector<Point>> contour;
    findContours(thresh, contour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contour.size());
    for( int i = 0; i < contour.size(); i++){
        convexHull(contour[i], hull[i]);
    }
    float area = 0;
    for(int i = 0; i < hull.size(); i++){
        area+=contourArea(hull[i]);
    }
    drawContours(thresh, hull, -1, Scalar(255,0,0));
    return area;
}
// main function
int main(int argc, char** argv) {
    VideoCapture video("/Users/aparahuja/Desktop/trafficvideo.mp4");
    Mat background, frame1, frame2, diff, thresh;
    background = imread("/Users/aparahuja/Desktop/emptyRoad.jpg");
    // coordinates in transformed image
    vector<Point2f> trnsfrm_points;

    trnsfrm_points.push_back(Point2f(472, 52));        // top left corner
    trnsfrm_points.push_back(Point2f(472, 830));       // bottom left corner
    trnsfrm_points.push_back(Point2f(800, 830));       // bottom right corner
    trnsfrm_points.push_back(Point2f(800, 52));        // top right corner

    // used to store homography matrix
    Mat matrix;
    background = warpAndCrop(background , trnsfrm_points, matrix, false);

    if(!video.isOpened()){
        cout<<"ERROR";
        return 0;
    }
    int see_every_n_frame = 3;
    
    //Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, trnsfrm_points, matrix, true);
    while(true){
        //Frame 2
        for(int i = 0; i < see_every_n_frame - 1; i++){
            video.read(frame2);
        }
        //Video End
        if(frame2.empty()){
            break;
        }
        frame2 = warpAndCrop(frame2, trnsfrm_points, matrix, true);
        
        //Dynamic DEnsity
        thresh = subImg(frame1,frame2);
        float area = contourAndArea(thresh);
        cout<<(float)area/255184<<", ";
        imshow("Dynamic Image", thresh);
        
        //Queue Density
        thresh = subImg(background,frame2);
        area = contourAndArea(thresh);
        cout<<(float)area/255184<<"\n";
        imshow("Queue Image", thresh);
        waitKey(10);
        frame1 = frame2;
        
        //AUTOMATIC BG SUBTRACT
        //Ptr<BackgroundSubtractor> bgfg = createBackgroundSubtractorMOG2();
        //Mat fgm;
        //bgfg->apply(frame2, fgm);
        //imshow("Mask Image", fgm);
        //imshow("Frame2 Image", frame2);
    }
    return 0;

}
