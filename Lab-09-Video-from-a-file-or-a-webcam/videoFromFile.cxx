/**
********************************************************************************
*
*    @file      videoFromFile.cxx
*
*    @brief     A simple program using OpenCV to display a video and
*               perform some image processing tasks.
*
*    @version   1.0
*
*    @date      05/02/2020
*
*    @author    Franck Vidal
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
#include <cmath>     // Header use round()

#include <opencv2/opencv.hpp> // Main OpenCV header


//******************************************************************************
//    Namespaces
//******************************************************************************
using namespace std;


//******************************************************************************
//    Global variables
//******************************************************************************
const double EPSILON(1.0e-5);

const int g_edge = 5; // Edge around the images in the window

cv::Mat g_current_frame; // Store the current frame
cv::Mat g_edge_frame;    // Store the edges detected in the current frame
cv::Mat g_displayed_image; // The image displayed in the window

// The title of every window
std::string g_window_title("Video");


//******************************************************************************
//    Function declaration
//******************************************************************************
void cartoonise(int, void*);


//******************************************************************************
//    Implementation
//******************************************************************************


//-----------------------------
int main(int argc, char** argv)
//-----------------------------
{
	try{
		std::cout << "1" << endl;
    
		/**********************************************************************/
		/* Declare some local variables                                       */
		/**********************************************************************/

		// File names
		std::string input_file_name;
		std::string output_file_name;


		/**********************************************************************/
		/* Process the command line arguments                                 */
		/**********************************************************************/

        // No file to display
        if (argc != 3 && argc != 4)
        {
            // Create an error message
            std::string error_message;
            error_message  = "Usage: ";
            error_message += argv[0];
            error_message += " <input_video>";
            error_message += " <scaling_factor>";
            error_message += " [output_video]";

            error_message += "\n\tExample: ";
            error_message += argv[0];
            error_message += " SAMPLING.AVI";
            error_message += " 0.25";
            error_message += " test.avi";

            // Throw an error
			std::cout << error_message << endl;
			//throw error_message;
        }

		std::cout << "2" << endl;
		// Get the file names
		input_file_name  = argv[1];
        double scaling_factor = atof(argv[2]);

        // An output file name has been specified
        if (argc == 4)
        {
            output_file_name = argv[3];
			std::cout << "3" << endl;
        }
		
		cv::VideoCapture video_capture(input_file_name);//, cv::CAP_FFMPEG);

		if (!video_capture.isOpened())
		{
			//error
			std::string error_message;
			error_message = "Could not open or find the video \"";
			error_message += input_file_name;
			error_message += "\".";
			throw error_message;

		}

		// Read the frame rate of the video (does not always work)
		double fps = video_capture.get(CV_CAP_PROP_FPS);
		cout << "Frame per seconds : " << fps << endl;

		// Convert in seconds per frame (default 30 ms / frame)
		double seconds_per_frame(1.0 / fps);

		// Convert in milliseconds
		int milliseconds_per_frame(round(seconds_per_frame * 1000.0));

		// Get the video size
		cv::Size input_video_size(video_capture.get(CV_CAP_PROP_FRAME_WIDTH), video_capture.get(CV_CAP_PROP_FRAME_HEIGHT));

		// Apply the scaling factor
		cv::Size scaled_video_size(input_video_size.width * scaling_factor, input_video_size.height * scaling_factor);

		// Set the size of the image displayed in the window
		cv::Size target_video_size(g_edge * 3 + 2 * scaled_video_size.width, g_edge * 2 + scaled_video_size.height);


		// Create the output image
		g_displayed_image = cv::Mat(target_video_size.height, target_video_size.width, CV_8UC3, cv::Scalar(128, 128, 128));

		/**********************************************************************/
		/* File writer                                                        */
		/**********************************************************************/
		int input_codec = video_capture.get(CV_CAP_PROP_FOURCC); //get files codec
		cv::VideoWriter video_writer;


		// Open the output
		if (output_file_name.size()){

			video_writer.open(output_file_name, input_codec, fps, target_video_size, true);

			if (!video_writer.isOpened()) {
				//open file writer with opencv codec
				video_writer.open(output_file_name, CV_FOURCC('M', 'J', 'P', 'G'), fps, target_video_size, true);

				//if file is stile not open
				if (!video_writer.isOpened()) {
					//error
					std::cerr << "WARNING: Cannot Create Output video." << std::endl;
				}
			}
		}
		/*video_capture >> g_current_frame;
		cartoonise(0, 0);
		cv::imshow("frame", g_displayed_image);*/
		int key;
		bool input_stream_status(true);
		do
		{
		
			video_capture >> g_current_frame;

			if (g_current_frame.empty()) {
				break;
			}
			// Resize the input if needed
			if (input_video_size != scaled_video_size)
			{
				cv::resize(g_current_frame, g_current_frame, scaled_video_size);
			}
			cartoonise(0, 0);
			// The file writer is working
			if (video_writer.isOpened())
			{
				// Add the current frame
				video_writer.write(g_displayed_image);

			}

			cv::imshow(g_window_title, g_displayed_image);
			key = cv::waitKey(milliseconds_per_frame);
		} while (key != 'q' && key != 27 && input_stream_status);

		
		
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

	// Exit the program
	return 0;
}
  


//-------------------------
void cartoonise(int, void*)
//-------------------------
{
	// Copy the current frame into the large image (g_displayed_image).
   // Add an edge of g_edge pixels around g_current_frame.
	cv::Mat targetROI = g_displayed_image(cv::Rect(g_edge, g_edge, g_current_frame.cols, g_current_frame.rows));
	g_current_frame.copyTo(targetROI);

	// convert the image (g_current_frame) to greyscale.
	// save the resulting image in greyscale_frame.
	cv::Mat greyscale_frame;
	cv::cvtColor(g_current_frame, greyscale_frame, cv::COLOR_BGR2GRAY);
	//cv::normalize(greyscale_frame, greyscale_frame, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC1);
	
//	// Apply a median filter on greyscale_frame with a size of 7 pixels.
//	// Save the resulting image in median_frame.
	cv::Mat median_frame;
	cv::medianBlur(greyscale_frame, median_frame, 7);
	
//	// Perform a Laplacian filter on median_frame
//	// You must use unsigned char for the output, i.e. the depth is CV_8U
//	// The kernel size is 5x5
//	// Save the resulting image in edge_frame.
	cv::Mat edge_frame;
	cv::Laplacian(median_frame, edge_frame, CV_8U, 5);
	
//	// Perform an edge detection using edge_frame and the threshold function.
//	// The threshold is 100, the maximum value is 255, and the thresholding type is THRES_BINARY_INV (or 1).
//	// Save the resulting image in mask_frame.
	cv::Mat mask_frame;
	cv::threshold(edge_frame, mask_frame, 100, 255, CV_THRESH_BINARY_INV);
	//targetROI = g_displayed_image(cv::Rect(g_edge * 2 + g_current_frame.cols, g_edge, mask_frame.cols, mask_frame.rows));
	//mask_frame.copyTo(targetROI);

//	// Reduce the input image (g_current_frame) size by a factor 10 and resample using pixel area relation.
	// Save the resulting image in small_frame.
	float ds_factor = 4;
	cv::Mat small_frame;
#if CV_MAJOR_VERSION <= 3
	cv::resize(g_current_frame, small_frame, cv::Size(0, 0), 1.0 / ds_factor, 1.0 / ds_factor, CV_INTER_AREA);
#else
	cv::resize(g_current_frame, small_frame, cv::Size(0, 0), 1.0 / ds_factor, 1.0 / ds_factor, cv::INTER_AREA);
#endif
	//targetROI = g_displayed_image(cv::Rect(g_edge * 2 + g_current_frame.cols, g_edge, small_frame.cols, small_frame.rows));
	//small_frame.copyTo(targetROI);


	// Apply a bilateral filter 10 times. The kernel size is 5, sigma colour is 5, and sigma space is 7.
	// Save the resulting image in small_frame.
	for (int i = 0; i < 10; ++i) {
		cv::Mat temp;
		cv::bilateralFilter(small_frame, temp, 5, 5, 7);
		small_frame = temp;
	}
	//targetROI = g_displayed_image(cv::Rect(g_edge * 2 + g_current_frame.cols, g_edge, small_frame.cols, small_frame.rows));
	//small_frame.copyTo(targetROI);
	 // Restore the of the image (small_frame) so that it is the same as the input image (g_current_frame) and resample using bi-linear interpolation.
	// Save the resulting image in output_frame.

	cv::Mat output_frame;
#if CV_MAJOR_VERSION <= 3
	cv::resize(small_frame, output_frame, cv::Size(0, 0), ds_factor ,ds_factor, CV_INTER_LINEAR);
#else
	cv::resize(small_frame, output_frame, cv::Size(0, 0), ds_factor, ds_factor, cv::INTER_LINEAR);
#endif
 
	//targetROI = g_displayed_image(cv::Rect(g_edge * 2 + g_current_frame.cols, g_edge, output_frame.cols, output_frame.rows));
	//output_frame.copyTo(targetROI);
	

	 //add a thick boundary using a boolean operator (and).
	// save the resulting image in cartoon_frame.
	cv::Mat cartoon_frame;
	cv::bitwise_and(output_frame, output_frame, cartoon_frame, mask_frame);


//	// copy the result
	targetROI = g_displayed_image(cv::Rect(g_edge * 2 + g_current_frame.cols, g_edge, cartoon_frame.cols, cartoon_frame.rows));
	cartoon_frame.copyTo(targetROI);
//
	
}
