/**
********************************************************************************
*
*	@file		logScale.cxx
*
*	@brief		A program to display an image in the log scale.
*	            Care is given to convert the RGB image to greyscale,
*	            to float and to avoid log(0) which is undefined.
*
*	@version	1.0
*
*	@date		20/01/2020
*
*	@author		Franck Vidal
*
*
********************************************************************************
*/


//******************************************************************************
//	Includes
//******************************************************************************
#include <exception> // Header for catching exceptions
#include <iostream>  // Header to display text in the console
#include <opencv2/opencv.hpp> // Main OpenCV header


//******************************************************************************
//	Namespaces
//******************************************************************************
using namespace std;


//-----------------------------
int main(int argc, char** argv)
//-----------------------------
{
    try
    {
        // No file to display
        // No file to save
        if (argc != 3)
        {
            // Create an error message
            std::string error_message;
            error_message  = "usage: ";
            error_message += argv[0];
            error_message += " <input_image>  <output_image>";

            // Throw an error
            throw error_message;
        }

        // Write your own code here
		string input_filename(argv[1]);
		string output_filename(argv[2]);

		// Create an image instance


		cv::Mat image = cv::imread(input_filename, CV_LOAD_IMAGE_COLOR);

		// The image has not been loaded
		if (!image.data)
		{
			// Create an error message
			std::string error_message;
			error_message = "Could_not_open_or_find_the_image\"";
			error_message += input_filename;
			error_message += "\".";
			// Throw an error
			throw error_message;
		}

		//convert rgb image to grayscale
		cv::Mat grey_image;
		cv::cvtColor(image, grey_image, CV_RGB2GRAY);

		//Convert to float
		cv::Mat float_image;
		grey_image.convertTo(float_image, CV_32FC1);

		//Log transformation
		cv::Mat log_image;
		cv::log(float_image + 1.0, log_image);

		//normalise image
		double min, max;
		cv::minMaxLoc(log_image, &min, &max);
		cv::Mat normalised_image = 255.0 * (log_image - min) / (max - min);
		normalised_image.convertTo(normalised_image, CV_8UC1);



		string window_title;
		window_title = "Display_\"";
		window_title += input_filename;
		window_title += "\"";

		//create window
		cv::namedWindow(window_title, cv::WINDOW_AUTOSIZE);
		//Show image in window
		cv::imshow(window_title, normalised_image);





		string window_title_grey;
		window_title_grey = "Display_\"";
		window_title_grey += output_filename;
		window_title_grey += "\"";

		//create window
		cv::namedWindow(window_title, cv::WINDOW_AUTOSIZE);
		//Show image in window
		cv::imshow(window_title_grey, grey_image);

		//Save grayscale image
		if (!cv::imwrite(output_filename, normalised_image)) {
			//image has not been writen
			string error_message;
			error_message = "Could not write the image \"";
			error_message += argv[2];
			error_message += "\".";

			throw error_message;

		}


		cv::waitKey(0);





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

