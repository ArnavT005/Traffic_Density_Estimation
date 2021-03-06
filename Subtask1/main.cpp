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
bool warpAndCrop(int count, String path, vector<Point2f> trnsfrm_points, Mat& matrix, bool optional) {

    // read image and store it in src_img
    Mat src_img = imread(path, 0);
    // create a copy, as the original image is going to be modified (drawing circles etc.)
    Mat clone = src_img.clone();

    Mat transformed_image, cropped_image;

    // used to store the size of an image
    Size size;

    if (src_img.data == NULL) {

        // unable to load image
        cout << "ERROR: Unable to load image " << count << ". Program terminating!\n";
        cout << "Please check if the image path provided is valid or not.\n";
        cout << "Refer to README.md for details. \n\n";

        // unsuccessful job
        return false;

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

                return false;

            }

            // find a perspective transformation (transformation matrix)
            matrix = findHomography(data.points, trnsfrm_points);

        }

        //transform perspective, and store it in transformed_image
        warpPerspective(clone, transformed_image, matrix, size);

        //show and save transformed image
        imshow("Projected Frame", transformed_image);
        imwrite("Transformed_Image_" + to_string(count) + ".jpg", transformed_image);

        //wait until any key is pressed 
        waitKey(0);

        //close all windows on key press
        destroyAllWindows();

        //show and save cropped image
        cropped_image = transformed_image(Range(52, 831), Range(472, 801));
        imshow("Cropped Frame", cropped_image);
        imwrite("Cropped_Image_" + to_string(count) + ".jpg", cropped_image);

        //wait until any key is pressed 
        waitKey(0);

        //close all windows on key press
        destroyAllWindows();

        // successful completion
        return true;

    }

}


// main function
int main(int argc, char** argv) {

    // argc is the number of command line arguments including the executable file
    // argv is the array of character pointers (storing strings)
    // argv[0] is the executable file
    // argv[1] should contain the relative/absolute path of image 1 (preferably "empty.jpg")
    // argv[2] should contain the relative/absolute path of image 2 (preferably "traffic.jpg")
    // argv[3] (optional) should be either "0" or "1" depending on whether or
    // not the user wants to use the same set of points for both the images

    if (argc > 4) {

        // extra arguments provided, just a warning
        cout << "WARNING: Extra arguments provided. \n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <IMG1_PATH> <IMG2_PATH> [<SAME_POINTS>{0,1}]\n";
        cout << "Refer to README.md for more details. \n\n";

    }

    if (argc >= 3) {

        // coordinates in transformed image
        vector<Point2f> trnsfrm_points;

        trnsfrm_points.push_back(Point2f(472, 52));        // top left corner
        trnsfrm_points.push_back(Point2f(472, 830));       // bottom left corner
        trnsfrm_points.push_back(Point2f(800, 830));       // bottom right corner
        trnsfrm_points.push_back(Point2f(800, 52));        // top right corner

        // used to store homography matrix
        Mat matrix;

        bool optional = false;

        // if argv[3] = 1, then set of points is to be selected only once
        if (argc > 3 && (stoi(argv[3]) == 1)) { optional = true; }

        if (warpAndCrop(1, argv[1], trnsfrm_points, matrix, false) == false) { return 0; };        // unsuccessful, so return
        if (warpAndCrop(2, argv[2], trnsfrm_points, matrix, optional) == false) { return 0; };     // unsuccessful, so return

        cout << "Program execution successful!\n\n";

    }

    else {

        // insufficient number of arguments provided
        cout << "ERROR: Insufficient number of arguments provided. Program terminating! \n";
        cout << "Expected number of arguments: 3 (or 4), " << "Present: " << argc << ".\n";
        cout << "Please pass in arguments as:- <EXECUTABLE_FILE> <IMG1_PATH> <IMG2_PATH> [<SAME_POINTS>{0,1}]\n";
        cout << "Refer to README.md for details. \n\n";

    }

    return 0;

}
