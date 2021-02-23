#include <opencv2/opencv.hpp>

using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs

//clickData struct stores all info to be updated incase of any event(click)
struct clickData {
    vector<Point2f> points;
    int clickCnt = 0;
    Mat image;
};

void onMouse(int event, int x, int y, int flags, void* dataPointer) {

    // retrieving clickData pointer from void pointer
    clickData* data = (clickData*)dataPointer;
    
    // user has clicked a point
    if (event == EVENT_LBUTTONDOWN) {
        // only accept 4 points from user
        if (data->clickCnt < 4) {
            data->points.push_back(Point2f(x, y));
            data->clickCnt++;
            circle((data->image),Point(x,y),10,Scalar(0,0,0),FILLED);
            imshow("Original Frame", data->image);
        }
    }
    // right click to remove last point selected
    if (event == EVENT_RBUTTONDOWN) {
        if (data->clickCnt > 0) {
            Point2f xy = (data->points)[data->clickCnt-1];
            data->clickCnt--;
            data->points.pop_back();
            circle((data->image),Point(xy.x,xy.y),10,Scalar(255,0,0),FILLED);
            imshow("Original Frame", data->image);
        }
    }
}

void warpAndCrop(String path, vector<Point2f> trnsfrm_points){
    
    Mat src_img = imread(path, 0);                  // read first-image and store it in src_img
    Mat transformed_image, cropped_image;
    Mat clone = src_img.clone();
    
    Size size;                                         // size of first-image

    if (src_img.data == NULL) {
        // unable to load image
        cout << "ERROR: Unable to load first image. Program terminating!\n";
        cout << "Please check if the image path provided is valid or not.\n";
        cout << "Refer to README.md for details. \n";
        return;
    }
    else {
        // first image loaded

        // stores the size of the image
        size = src_img.size();

        // to storing user inputs during events (mouse click)
        clickData data;
        data.image = src_img;

        //dispaly image
        imshow("Original Frame", data.image);
        //user input via mouse
        setMouseCallback("Original Frame", onMouse, &data);

        //wait until any key is pressed
        waitKey(0);
        //close all windows on key press
        destroyAllWindows();

        if (data.points.size() < 4) {
            //less than 4 point clicks before key press
            cout << "ERROR: Less than 4 points were selected. Homography cannot be determined\n";
            cout << "4 points are required. Rerun the code and select 4 points\n";
            cout << "Refer to README.md for details. \n";
            return;
        }

        // find a perspective transformation (transformation matrix)
        Mat matrix = findHomography(data.points, trnsfrm_points);

        //transform perspective
        warpPerspective(clone, transformed_image, matrix, size);

        //show and save transformed image
        imshow("Projected Frame", transformed_image);
        imwrite("Transformed_Image_1.jpg", transformed_image);

        //wait until any key is pressed. //close all windows on key press
        waitKey(0);
        destroyAllWindows();

        //show and save cropped image
        cropped_image = transformed_image(Range(52, 830), Range(472, 800));
        imshow("Cropped Frame", cropped_image);
        imwrite("Cropped_Image_1.jpg", cropped_image);

        //wait until any key is pressed. //close all windows on key press
        waitKey(0);
        destroyAllWindows();
    }}

int main(int argc, char** argv) {
    
    // argc is the number of command line arguments including the executable file
    // argv is the array of character pointers (storing strings)
    // argv[0] is the executable file
    // argv[1] should contain the relative/absolute path of the "empty" image
    // argv[2] should contain the relative/absolute path of the "traffic" image
    // argv[3] (optional) should be either "0" or "1" depending on whether or
    // not the user wants to use the same set of coordinates for both the images

    if (argc > 4) {

        cout << "WARNING: extra arguments provided. \n";
        cout << "Please pass in arguments as <EXECUTABLE_FILE> <IMG_PATH1> <IMG_PATH2> <SAME_POINTS(0,1)[optional]> \n";
        cout << "Refer to README.md for details. \n";
    }
    
    if (argc < 3) {

        cout << "ERROR: Sufficient arguments not provided. Program terminating! \n";
        cout << "Please pass in arguments as <EXECUTABLE_FILE> <IMG_PATH1> <IMG_PATH2> <SAME_POINTS(0,1)[optional]> \n";
        cout << "Refer to README.md for details. \n";

        return 0;
    }

    else {
        
        vector<Point2f> trnsfrm_points;                    // coordinates in transformed image
        trnsfrm_points.push_back(Point2f(472, 52));        // upper left corner
        trnsfrm_points.push_back(Point2f(472, 830));       // lower left corner
        trnsfrm_points.push_back(Point2f(800, 830));       // lower right corner
        trnsfrm_points.push_back(Point2f(800, 52));        // upper right corner

        Mat src_img = imread(argv[1], 0);                  // read first-image and store it in src_img
        Mat transformed_image, cropped_image;
        Mat clone = src_img.clone();
        
        Size size;                                         // size of first-image

        if (src_img.data == NULL) {
            // unable to load image
            cout << "ERROR: Unable to load first image. Program terminating!\n";
            cout << "Please check if the image path provided is valid or not.\n";
            cout << "Refer to README.md for details. \n";
            return 0;
        }
        else {
            // first image loaded

            // stores the size of the image
            size = src_img.size();

            // to storing user inputs during events (mouse click)
            clickData data;
            data.image = src_img;

            //dispaly image
            imshow("Original Frame", data.image);
            //user input via mouse
            setMouseCallback("Original Frame", onMouse, &data);

            //wait until any key is pressed
            waitKey(0);
            //close all windows on key press
            destroyAllWindows();

            if (data.points.size() < 4) {
                //less than 4 point clicks before key press
                cout << "ERROR: Less than 4 points were selected. Homography cannot be determined\n";
                cout << "4 points are required. Rerun the code and select 4 points\n";
                cout << "Refer to README.md for details. \n";
                return 0;
            }

            // find a perspective transformation (transformation matrix)
            Mat matrix = findHomography(data.points, trnsfrm_points);

            //transform perspective
            warpPerspective(clone, transformed_image, matrix, size);

            //show and save transformed image
            imshow("Projected Frame", transformed_image);
            imwrite("Transformed_Image_1.jpg", transformed_image);

            //wait until any key is pressed. //close all windows on key press
            waitKey(0);
            destroyAllWindows();

            //show and save cropped image
            cropped_image = transformed_image(Range(52, 830), Range(472, 800));
            imshow("Cropped Frame", cropped_image);
            imwrite("Cropped_Image_1.jpg", cropped_image);

            //wait until any key is pressed. //close all windows on key press
            waitKey(0);
            destroyAllWindows();

            //if optional argument entered
            if (argc == 4 && stoi(argv[3]) == 1) {
                //load second image
                src_img = imread(argv[2], 0);
                Mat clone = src_img.clone();

                if (src_img.data == NULL) {
                    // unable to load image
                    cout << "Unable to load second image. Program terminating!\n";
                    cout << "Please check if the image path provided is valid or not.\n";
                    cout << "Refer to README.md for details. \n";
                    return 0;
                }
                else {
                    // second image loaded
                    size = src_img.size();

                    imshow("Original Frame", src_img); //show second image

                    //wait until any key is pressed. //close all windows on key press
                    waitKey(0);
                    destroyAllWindows();

                    // use same transform matrix
                    warpPerspective(clone, transformed_image, matrix, size);
                    //show and save transformed image
                    imshow("Projected Frame", transformed_image);
                    imwrite("Transformed_Image_2.jpg", transformed_image);

                    //wait until any key is pressed. //close all windows on key press
                    waitKey(0);
                    destroyAllWindows();

                    //show and save cropped image
                    cropped_image = transformed_image(Range(52, 830), Range(472, 800));
                    imshow("Cropped Frame", cropped_image);
                    imwrite("Cropped_Image_2.jpg", cropped_image);

                    //wait until any key is pressed. //close all windows on key press
                    waitKey(0);
                    destroyAllWindows();

                    return 0;
                }
            }
        }
        
        if (argc == 4 && stoi(argv[3]) != 0) {

            cout << "Warning: optional arguments provided incorrectly. \n";
            cout << "Please pass optional argument correctly <SAME_POINTS(0,1)[optional]> \n";
            cout << "Refer to README.md for details. \n";
        }

        // argv[3] != 1 or no optional argument provided
        // so take user inputs (mouse clicks) for second image as well

        // read second image and store it in src_img
        src_img = imread(argv[2], 0);
        clone = src_img.clone();

        if (src_img.data == NULL) {
            // unable to load image
            cout << "Unable to load second image. Program terminating!\n";
            cout << "Please check if the image path provided is valid or not.\n";
            cout << "Refer to README.md for details. \n";
            return 0;
        }
        else {
            // second image loaded

            // stores the size of the image
            size = src_img.size();
            
            // to storing user inputs during events (mouse click)
            clickData data;
            data.image = src_img;

            //dispaly image
            imshow("Original Frame", data.image);
            //user input via mouse
            setMouseCallback("Original Frame", onMouse, &data);
            
            //wait until any key is pressed. //close all windows on key press
            waitKey(0);
            destroyAllWindows();
            
            
            if (data.points.size() < 4) {
                //less than 4 point clicks before key press
                cout << "ERROR: Less than 4 points were selected. Homography cannot be determined\n";
                cout << "4 points are required. Rerun the code and select 4 points\n";
                cout << "Refer to README.md for details. \n";
                return 0;
            }

            // finds a perspective transformation (transformation matrix)
            Mat matrix = findHomography(data.points, trnsfrm_points);

            warpPerspective(clone, transformed_image, matrix, size);

            imshow("Projected Frame", transformed_image);
            imwrite("Transformed_Image_2.jpg", transformed_image);

            waitKey(0);
            destroyAllWindows();

            cropped_image = transformed_image(Range(52, 830), Range(472, 800));

            imshow("Cropped Frame", cropped_image);
            imwrite("Cropped_Image_2.jpg", cropped_image);

            waitKey(0);
            destroyAllWindows();
        }

    }
}
