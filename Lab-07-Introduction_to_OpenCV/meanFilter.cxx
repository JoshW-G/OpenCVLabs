/**
********************************************************************************
*
*	@file		meanFilter.cpp
*
*	@brief		A program to perform the mean filter using OpenCV
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
#include <cstdlib>   // Header for atoi and atof
#include <exception> // Header for catching exceptions
#include <iostream>  // Header to display text in the console
#include <opencv2/opencv.hpp> // Main OpenCV header


//******************************************************************************
//	Namespaces
//******************************************************************************
using namespace std;


//******************************************************************************
//	Global variables
//******************************************************************************


//******************************************************************************
//	Function declaration
//******************************************************************************


//******************************************************************************
//	Implementation
//******************************************************************************


//-----------------------------
int main(int argc, char** argv)
//-----------------------------
{
    try
    {
        // No file to display
        // No file to save
        if (argc != 3 && argc != 4)
        {
            // Create an error message
            std::string error_message;
            error_message  = "usage: ";
            error_message += argv[0];
            error_message += " <input_image>  <output_image> <kernel_radius>";

            // Throw an error
            throw error_message;
        }

        // Filter radius
        unsigned int radius(1);

        if (argc == 4)
        {
            radius = atoi(argv[3]);
        }
    
        // Write your own code here
		unsigned int kernel_dim = (radius * 2) + 1; //calculate kernel dimensions 
		//Filter Size
		cv::Size filter_size(kernel_dim, kernel_dim);

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

		cv::Mat filterImage;
		cv::blur(image, filterImage, filter_size);

		string window_title;
		window_title = "Display_\"";
		window_title += output_filename;
		window_title += "\"";

		//create window
		cv::namedWindow(window_title, cv::WINDOW_AUTOSIZE);
		//Show image in window
		cv::imshow(window_title, filterImage);

		//Save  image
		if (!cv::imwrite(output_filename, filterImage)) {
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

