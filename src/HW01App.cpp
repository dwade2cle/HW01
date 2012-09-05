/**
@file HW01App.cpp
CSE 274 09/05/2012 - HW01

@author Eddie Obissi

@note This project satisfies A.1, A.2, A.3, A.4, B.1, E.1 and E.6

*/

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Resources.h"
#include <cmath>
#include <windows.h>
#include <stdio.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class HW01App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);


private:
	Surface* mySurface_; //Our surface

	// For animation
	int frame_number_;
	boost::posix_time::ptime app_start_time_;

	// Dimensions of window
	static const int appWidth = 800;
	static const int appHeight = 600;
	static const int surfaceSize = 1024;
	/**
	Draws a rectangle with a border between (x1,y1) and (x2,y2) the logic assumes that the 
	rectangle is declared within the bounds of the window
	This satisfies the A.1 requirement to create a rectangle.
	*/
	void rectangle(uint8_t* pixels, int x1, int y1, int x2, int y2, Color8u fillColor, Color8u borderColor); 
	
	/**
	Draws a circle with center at (centerX, centerY).  It takes in a radius and a color to further 
	control it's appearance.
	This satisfies the A.2 requirment to create a circle.
	*/
	void circle(uint8_t* surface, int centerX, int centerY, int radius, Color8u fillColor);
	/**
	Uses a simple convolution filter to blur the image
	This satisfies the B.1 requirment to create blur
	*/
	void blur(uint8_t* surface);
	/**
	Takes in two colors and surface to create a gradient from left to right.
	This satisfies the A.4 requirement.
	*/
	void gradientFill(uint8_t* surface, Color8u color1, Color8u color2);
	/**
	This draws a line segment and satisfies the A.3 requirement.
	*/
	void line(uint8_t* surface, int x1, int y1, int x2, int y2, Color8u color);
};

void HW01App::prepareSettings(Settings* settings)	{
	(*settings).setWindowSize(appWidth, appHeight);
	(*settings).setResizable(false);
}

void HW01App::rectangle(uint8_t* pixels, int x1, int y1, int x2, int y2, Color8u fillColor, Color8u borderColor)	{
	
	for (int y = y1; y < y2; y++)	{
		for (int x = x1; x < x2; x++)	{
			
			//Border
			if (std::abs(y - y1) < 4 || std::abs(x - x1) < 4|| std::abs(y - y2) < 4 || std::abs(x - x2) < 4)	{ 
				pixels[3*(y*surfaceSize + x)] = borderColor.r;
				pixels[3*(y*surfaceSize + x)+1] = borderColor.g;
				pixels[3*(y*surfaceSize + x)+2] = borderColor.b;
			}
			//Fill
			else	{
				pixels[3*(y*surfaceSize + x)] = fillColor.r;
				pixels[3*(y*surfaceSize + x)+1] = fillColor.g;
				pixels[3*(y*surfaceSize + x)+2] = fillColor.b;
			}
		}
	}
}

void HW01App::circle(uint8_t* surface, int centerX, int centerY, int radius, Color8u fillColor)	{
	// Square around circle
	for (int j = centerY - radius; j < (centerY + radius); j++)	{
		for (int i = centerX - radius; i < (centerX + radius); i++)	{
			// If inside circle radius, change pixel value
			int distX = std::abs(i - centerX);
			int distY = std::abs(j - centerY);
			if (std::sqrt((double)(distX*distX + distY*distY)) < radius)	{
				surface[3*(j*surfaceSize + i) + 0] = fillColor.r;
				surface[3*(j*surfaceSize + i) + 1] = fillColor.g;
				surface[3*(j*surfaceSize + i) + 2] = fillColor.b;
			}
		}
	}
}
void HW01App::blur(uint8_t* surface)	{
	static uint8_t buffer[3*surfaceSize*surfaceSize];
	memcpy(buffer, surface, 3*surfaceSize*surfaceSize);

	// Kernel
	/*float k[9] = 
	{1/9.0, 1/9.0, 1/9.0,
	1/9.0, 1/9.0, 1/9.0,
	1/9.0, 1/9.0, 1/9.0};*/

	int y, x, shift;
	double sumRed = 0;
	double sumGreen = 0;
	double sumBlue = 0;

	for (y = 1; y < appHeight - 1; y++)	{
		for (x = 1; x < appWidth - 1; x++)	{
			for (int j = - 1; j < 2; j++)	{
				for (int i = -1; i < 2; i++)	{
					shift = 3*((y + j)*surfaceSize + (x + i));
					// Average of the 9 values
					sumRed += 1/9.0*buffer[shift];
					sumGreen += 1/9.0*buffer[shift + 1];
					sumBlue += 1/9.0*buffer[shift + 2];
				}
			}
			surface[3*(y*surfaceSize + x) + 0] = (int) sumRed;
			surface[3*(y*surfaceSize + x) + 1] = (int) sumGreen;
			surface[3*(y*surfaceSize + x) + 2] = (int) sumBlue;
			
			sumRed = 0; sumGreen = 0; sumBlue = 0;
		}
	}

}

void HW01App::gradientFill(uint8_t* surface, Color8u color1, Color8u color2)	{
	int diffRed = color1.r - color2.r;
	int diffGreen = color1.g - color2.g;
	int diffBlue = color1.b - color2.b;
	double diffPerPixelRed = diffRed/(appWidth + 0.0);
	double diffPerPixelGreen = diffGreen/(appWidth + 0.0);
	double diffPerPixelBlue = diffBlue/(appWidth + 0.0);
	double cNewRed = 0; double cNewGreen = 0; double cNewBlue = 0;
	// Gradually changes the color value
	for (int y = 0; y < appHeight; y++)	{
		for (int x = 0; x < appWidth; x++)	{
			cNewRed += diffPerPixelRed;
			cNewGreen += diffPerPixelGreen;
			cNewBlue += diffPerPixelBlue;
			surface[3*(y*surfaceSize + x) + 0] = (int) cNewRed;
			surface[3*(y*surfaceSize + x) + 1] = (int) cNewGreen;
			surface[3*(y*surfaceSize + x) + 2] = (int) cNewBlue;
		}
		cNewRed = 0;
		cNewGreen = 0;
		cNewBlue = 0;
	}
}

void HW01App::line(uint8_t* surface, int x1, int y1, int x2, int y2, Color8u color)	{
	double ratioXY = (double) std::abs(x1 - x2)/std::abs(y1 - y2);
	double yUsed = y1 + 0.0; double xUsed = x1 + 0.0;
	//Logic if x1 and x2 are further apart than y1 and y2
	if (ratioXY >= 1)	{
		for (int x = x1; x < x2; x++)	{
			yUsed += (double)(y2 - y1)/(x2 - x1);
			surface[3*((int) yUsed*surfaceSize + x) + 0] = color.r;
			surface[3*((int) yUsed*surfaceSize + x) + 1] = color.g;
			surface[3*((int) yUsed*surfaceSize + x) + 2] = color.b;
		}
	}
	//...if y1 and y2 are larger
	else {
		for (int y = y1; y < y2; y++)	{
			xUsed += (double)(x2 - x1)/(y2 - y1);
			surface[3*(y*surfaceSize + (int) xUsed) + 0] = color.r;
			surface[3*(y*surfaceSize + (int) xUsed) + 1] = color.g;
			surface[3*(y*surfaceSize + (int) xUsed) + 2] = color.b;
			
		}
}
}

void HW01App::setup()
{
	frame_number_ = 0;
	mySurface_ = new Surface(surfaceSize, surfaceSize, false);

}

void HW01App::mouseDown( MouseEvent event )
{
}

void HW01App::update()
{
	// We want the update to simulate a ping pong game against the computer.  
	// The logic is not yet complete

	// Tracks mouse location
	POINT mousePos;
	GetCursorPos(&mousePos);
	uint8_t* dataArray = (*mySurface_).getData();
	
	Color8u fillColor = Color8u(0, 125, 50);
	Color8u borderColor = Color8u(200, 125, 0);
	if (frame_number_ == 0)
	gradientFill(dataArray, Color8u(0,100,200), Color8u(150, 50, 0));

	int sinRectY1 = 150 + 145*std::sin(2*3.14*.02*frame_number_);
	rectangle(dataArray, 25, sinRectY1, 50, sinRectY1 + 300, fillColor, borderColor);
	
	int y1try = mousePos.y; int y2try = y1try + 150;
	rectangle(dataArray, appWidth-50, y1try, appWidth-25, y2try, Color8u(200, 0, 25), Color8u(250, 250, 210));

	bool up, right;
	int centerX = appWidth/2; int centerY = appHeight/2;

	// Unsuccessful logic for the circle to bounce around screen
	/*if (frame_number_ == 0)	{
		int centerX = appWidth/2;
		int centerY = appHeight/2;
		up = false; right = true; 
	}
	else if (centerX > appWidth - 50)	{
		centerX  -= 4; right = false;
			if (up == true)
				centerY += 4;
			else
				centerY -= 4;
	}
	else if (centerX  < 50)	{
		centerX  += 4; right = false;
			if (up == true)
				centerY += 4;
			else
				centerY -= 4;
	}
	else if (centerY > appHeight - 50)	{
		centerY  -= 4; up = false;
			if (right == true)
				centerX += 4;
			else
				centerX -= 4;
	}
	else if (centerY  < 50)	{
		centerY  += 4; up = true;
			if (right == true)
				centerX += 4;
			else
				centerX -= 4;
	}
	else	{
		centerX +=4; centerY +=4;}*/

	// Move the circle around
	centerX = appWidth/2 + (appWidth/2-50)*std::sin(2*3.14*.02*frame_number_);
	circle(dataArray, centerX, centerY, 15, borderColor);

	// These lines will serve as bumpers to push the ball around
	int y1Bumper1 = 100 + 50*std::sin(2*3.14*.005*frame_number_);
	int y2Bumper1 = 100 - 50*std::sin(2*3.14*.005*frame_number_);
	line(dataArray, 25, y1Bumper1, appWidth - 25, y2Bumper1, Color8u(255,255,255));	
	line(dataArray, 25, y1Bumper1 + 400, appWidth - 25, y2Bumper1 + 400, Color8u(255,255,255)); 
 
	
	blur(dataArray);
	if (frame_number_ == 0)	{
		writeImage("HW01.png", *mySurface_);
		app_start_time_ = boost::posix_time::microsec_clock::local_time();
	}

	frame_number_++;
}

void HW01App::draw()
{
	gl::draw(*mySurface_);
}

CINDER_APP_BASIC( HW01App, RendererGl )
