/**
********************************************************************************
*
*   @file       edgeDetection3.cxx
*
*   @brief      Use a Canny operator.
*
*   @version    1.0
*
*   @date       04/02/2019
*
*   @author     Franck Vidal
*
*
********************************************************************************
*/


//******************************************************************************
//    Includes
//******************************************************************************
#include <exception> // Header for catching exceptions
#include <iostream>  // Header to display text in the console
#include <string>    // Header to manipulate strings
#include <opencv2/opencv.hpp> // Main OpenCV header
#include <algorithm>

//******************************************************************************
//    Namespaces
//******************************************************************************
using namespace std;


//******************************************************************************
//    Global variables
//******************************************************************************
cv::Mat g_display_image;
cv::Mat g_scharr_image;
cv::Mat g_edge_image;
cv::Mat g_gaussian_image;
std::string g_image_window_title("Edge detection");

int g_slider_count(256);
int g_low_slider_position(g_slider_count / 4);
int g_high_slider_position(g_slider_count / 2);
int N = 3;
int k = 10;


//******************************************************************************
//    Function declaration
//******************************************************************************
void callback(int, void*);


//******************************************************************************
//    Implementation
//******************************************************************************


//-----------------------------
int main(int argc, char** argv)
//-----------------------------
{
	try
	{
		/**********************************************************************/
		/* Declare some local variables                                       */
		/**********************************************************************/

		// File names
		std::string input_file_name;
		std::string output_file_name;

		// Image structures
		cv::Mat rgb_image;
		cv::Mat grey_image;




		/**********************************************************************/
		/* Process the command line arguments                                 */
		/**********************************************************************/

		// No file to display
		if (argc != 3)
		{
			// Create an error message
			std::string error_message;
			error_message = "usage: ";
			error_message += argv[0];
			error_message += " <input_image>";
			error_message += " <output_image>";

			// Throw an error
			throw error_message;
		}


		// Get the file names
		input_file_name = argv[1];
		output_file_name = argv[2];


		/**********************************************************************/
		/* Read the input                                                     */
		/**********************************************************************/

		// Open and read the image
		rgb_image = cv::imread(input_file_name, CV_LOAD_IMAGE_COLOR);

		// The image has not been loaded
		if (!rgb_image.data)
		{
			// Create an error message
			std::string error_message;
			error_message = "Could not open or find the image \"";
			error_message += input_file_name;
			error_message += "\".";

			// Throw an error
			throw error_message;
		}

		// Write your own code here to
		// Create the displayed image
		g_display_image = cv::Mat(rgb_image.rows, rgb_image.cols * N + (N - 1) * k, CV_32FC1, cv::Scalar(0.5, 0.5, 0.5));

		// Create the window
		cv::namedWindow(g_image_window_title, cv::WINDOW_AUTOSIZE);


		/**********************************************************************/
		/* Convert the RGB data to greyscale                                  */
		/**********************************************************************/
		cv::cvtColor(rgb_image, grey_image, cv::COLOR_RGB2GRAY);
		grey_image.convertTo(grey_image, CV_32FC1);
		cv::normalize(grey_image, grey_image, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC1);
		// Create the ROI in the target image
		cv::Mat targetROI = g_display_image(cv::Rect(0, 0, grey_image.cols, grey_image.rows));

		// Copy the source in the target
		grey_image.copyTo(targetROI);


		/**********************************************************************/
		/* Apply a 3x3 Gaussian filter with sigma 0.5 to reduce noise         */
		/**********************************************************************/
		cv::Size kernel(3, 3);
		cv::GaussianBlur(grey_image, g_gaussian_image, kernel, 0.5);




		/**********************************************************************/
		/* Gradient filter                                                    */
		/**********************************************************************/
		cv::Mat scharr_x;
		cv::Scharr(g_gaussian_image, scharr_x, -1, 1, 0);
		// 2) Compute the absolute value of the gradient along the X-axis
		scharr_x = cv::abs(scharr_x);

		// 3) Apply the Scharr filter on gaussain_image along the Y-axis
		cv::Mat scharr_y;
		cv::Scharr(g_gaussian_image, scharr_y, -1, 0, 1);
		// 4) Compute the absolute value of the gradient along the Y-axis
		scharr_y = cv::abs(scharr_y);
		// 5) Combined scharr_x and scharr_y together so that
		//scharr_image = 0.5 * scharr_x + 0.5 * scharr_y;

		cv::addWeighted(scharr_x, 0.5, scharr_y, 0.5, 0, g_scharr_image);


		// Copy the result
		targetROI = g_display_image(cv::Rect(g_scharr_image.cols * 1 + 1 * k, 0, g_scharr_image.cols, g_scharr_image.rows));
		g_scharr_image.copyTo(targetROI);


		/**********************************************************************/
		/* Create the slider                                                  */
		/**********************************************************************/

		// Write your own code here to
		// Create the slider
		
		cv::createTrackbar("low", g_image_window_title, &g_low_slider_position, g_slider_count, callback);
		cv::createTrackbar("high", g_image_window_title, &g_high_slider_position, g_slider_count, callback);

		/**********************************************************************/
		/* Threshold filter                                                   */
		/**********************************************************************/
		callback(0, 0);


		/**********************************************************************/
		/* Event loop                                                         */
		/**********************************************************************/

		// Wait for the user to press 'q' or 'Escape' (27 in ASCII code
		int key;
		do
		{
			key = cv::waitKey(0);
		} while (key != 'q' && key != 27);


		/**********************************************************************/
		/* Write the output                                                   */
		/**********************************************************************/

		// Write your own code here to
		// Write the image
		cv::normalize(g_edge_image, g_edge_image, 0, 255, cv::NORM_MINMAX, CV_32FC1);

		cv::imwrite(output_file_name, g_edge_image);


	}
	// An error occured
	catch (const std::exception& error)
	{
		// Display an error message in the console
		cerr << error.what() << endl;
	}
	catch (const std::string& error)
	{
		// Display an error message in the console
		cerr << error << endl;
	}
	catch (const char* error)
	{
		// Display an error message in the console
		cerr << error << endl;
	}

	// Do not close the prompt window
#if defined(WIN32) && defined(_DEBUG)
	system("pause");
#endif

	// Exit the program
	return 0;
}


//-----------------------
void callback(int, void*)
//-----------------------
{
	/**********************************************************************/
	/* Threshold                                                          */
	/**********************************************************************/
	double min, max;
	cv::minMaxLoc(g_scharr_image, &min, &max);

	//canny edge detector

	double low_thresh(255 * (double(std::min(g_low_slider_position, g_high_slider_position) / double(g_slider_count))));
	double high_thresh(255 * (double(std::max(g_low_slider_position, g_high_slider_position) / double(g_slider_count))));

	cv::Mat grey_image;
	g_gaussian_image.convertTo(grey_image, CV_8UC1, 255);
	cv::Canny(grey_image, g_edge_image, low_thresh, high_thresh);
	g_edge_image.convertTo(g_edge_image, CV_32FC1, 1.0 / 255.0);


	// Write your own code here to
	// Copy the result
	cv::Mat targetROI = g_display_image(cv::Rect(g_edge_image.cols * 2 + 2 * k, 0, g_edge_image.cols, g_edge_image.rows));
	g_edge_image.copyTo(targetROI);
	// Write your own code here to
	// Display the window
	cv::imshow(g_image_window_title, g_display_image);
}

