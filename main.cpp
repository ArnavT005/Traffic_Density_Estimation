//???Add feature to remove last clicked point.
//???Add circle to chosen point.
//???Print point choose order in img
//??? case : argc > 4 should be handled.

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
        if (data->points.size() < 4) {
            data->points.push_back(Point2f(x, y));
            circle((data->image),Point(x,y),1000,Scalar(0,0,255));
        }
    }
}

int main(int argc, char** argv) {
    
    // argc is the number of command line arguments including the executable file
    // argv is the array of character pointers (storing strings)
    // argv[0] is the executable file
    // argv[1] should contain the relative/absolute path of the "empty" image
    // argv[2] should contain the relative/absolute path of the "traffic" image
    // argv[3] (optional) should be either "0" or "1" depending on whether or
    // not the user wants to use the same set of coordinates for both the images

    if (argc < 3) {

        cout << "Sufficient arguments not provided. Program terminating! \n";
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

        Size size;                                         // size of first-image

        if (src_img.data == NULL) {
            // unable to load image
            cout << "Unable to load first image. Program terminating!\n";
            cout << "Please check if the image path provided is valid or not.\n";
            
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
                cout << "Less than 4 points were selected. Angle correction not possible";
            }

            // find a perspective transformation (transformation matrix)
            Mat matrix = findHomography(data.points, trnsfrm_points);

            //transform perspective
            warpPerspective(src_img, transformed_image, matrix, size);

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
                
                if (src_img.data == NULL) {
                    // unable to load image
                    cout << "Unable to load second image. Program terminating!\n";
                    cout << "Please check if the image path provided is valid or not.\n";

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
                    warpPerspective(src_img, transformed_image, matrix, size);
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

        // argv[3] != 1 or no optional argument provided
        // so take user inputs (mouse clicks) for second image as well

        // read second image and store it in src_img
        src_img = imread(argv[2], 0);

        if (src_img.data == NULL) {
            // unable to load image
            cout << "Unable to load second image. Program terminating!\n";
            cout << "Please check if the image path provided is valid or not.\n";

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

            // finds a perspective transformation (transformation matrix)
            Mat matrix = findHomography(data.points, trnsfrm_points);

            warpPerspective(src_img, transformed_image, matrix, size);

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
