#include <opencv2/opencv.hpp>

using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs

struct Points {

	vector<Point2f> points;

};

void onMouse(int event, int x, int y, int flags, void* data) {

	Points* img_point = (Points*)data;  // retrieving Points pointer from void pointer

	int clicks = 0;

	if (event == EVENT_LBUTTONDOWN) {
		// user has picked a point
		clicks++;
		if (img_point->points.size() < 4) {
			// only need to take 4 points from user
			img_point->points.push_back(Point2f(x, y));
		}
		if (clicks == 4)
			return;
	}

}

int main(int argc, char** argv) {
	
	// argc is the number of command line arguments including the executable file
	// argv is the array of character pointers (storing strings), argv[0] is the executable file
	// argv[1] should contain the relative/absolute path of the "empty" image
	// argv[2] (optional) should contain the relative/absolute path of the "traffic" image
	// argv[3] (optional) should be either "0" or "1" depending on whether or 
	// not the user wants to use the same set of coordinates for both the images 

	if (argc < 3) {

		cout << "Sufficient arguments not provided. Program terminating!\n";
		cout << "Please pass in arguments as <EXECUTABLE_FILE> <IMG_PATH1> <IMG_PATH2> <SAME_POINTS>{0,1}(optional)*\n";
		cout << "Refer to README.md for details.\n";

		return 0;
	}
	else {

		vector<Point2f> trnsfrm_points;			// coordinates in transformed image

		trnsfrm_points.push_back(Point2f(472, 52));		// upper left corner
		trnsfrm_points.push_back(Point2f(472, 830));	// lower left corner
		trnsfrm_points.push_back(Point2f(800, 830));	// lower right corner
		trnsfrm_points.push_back(Point2f(800, 52));		// upper right corner

/*		namedWindow("Source Image"); // WINDOW_NORMAL enables resizing of window
		namedWindow("Transformed Image");
		namedWindow("Cropped Image");
		*/
		Mat src_img = imread(argv[1], 0);  // read first image and store it in src_img
		Mat transformed_image;
		Mat cropped_image;

		Size size;

		if (src_img.data == NULL) {
			// unable to load image
			cout << "Unable to load first image. Program terminating!\n";
			cout << "Please check if the image path provided is valid or not.\n";
			
			return 0;
		}
		else {
			// first image loaded
			// hard-coded set of points for camera angle correction and frame cropping

			size = src_img.size();	// stores the size of the image

			// display image, and accept user input through mouse clicks
			// first create a window to act as a placeholder

			imshow("Original Frame", src_img);

			Points source_points;			// used for storing user inputs (mouse click)

			setMouseCallback("Original Frame", onMouse, &source_points);

			waitKey(0);
			destroyAllWindows();

			//if (source_points.points.size() < 4) {
			//	cout << "Less than 4 points were given. Angle correction not possible".
			//}

			Mat matrix = findHomography(source_points.points, trnsfrm_points);  // finds a perspective transformation (transformation matrix) 
																				// using all the points provided
			// matrix.size = (3, 3)

			warpPerspective(src_img, transformed_image, matrix, size);

			imshow("Projected Frame", transformed_image);
			imwrite("Transformed_Image_1.jpg", transformed_image);

			waitKey(0);
			destroyAllWindows();

			cropped_image = transformed_image(Range(52, 830), Range(472, 800));

			imshow("Cropped Frame", cropped_image);
			imwrite("Cropped_Image_1.jpg", cropped_image);

			waitKey(0);
			destroyAllWindows();

			if (argc == 4 && stoi(argv[3]) == 1) {
				// use same set of points for image 2 as well
				src_img = imread(argv[2], 0);
				
				if (src_img.data == NULL) {
					// unable to load image
					cout << "Unable to load second image. Program terminating!\n";
					cout << "Please check if the image path provided is valid or not.\n";

					return 0;
				}
				else {
					size = src_img.size();

					imshow("Original Frame", src_img);

					waitKey(0);
					destroyAllWindows();

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

					return 0;
				}
			}
		}

		// argv[3] != 1 or no optional argument provided
		// so take user inputs (mouse clicks) for second image as well

		src_img = imread(argv[2], 0);  // read second image and store it in src_img

		if (src_img.data == NULL) {
			// unable to load image
			cout << "Unable to load second image. Program terminating!\n";
			cout << "Please check if the image path provided is valid or not.\n";

			return 0;
		}
		else {
			// first image loaded
			// hard-coded set of points for camera angle correction and frame cropping

			size = src_img.size();	// stores the size of the image

			// display image, and accept user input through mouse clicks
			// first create a window to act as a placeholder

			imshow("Original Frame", src_img);

			Points source_points;			// used for storing user inputs (mouse click)

			setMouseCallback("Original Frame", onMouse, &source_points);

			waitKey(0);

			destroyAllWindows();

			//if (source_points.points.size() < 4) {
			//	cout << "Less than 4 points were given. Angle correction not possible".
			//}

			Mat matrix = findHomography(source_points.points, trnsfrm_points);  // finds a perspective transformation (transformation matrix) 
																				// using all the points provided
			// matrix.size = (3, 3)

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