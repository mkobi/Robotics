/*
 * pngUtil.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: colman
 */
#include "pngUtil.h"
#include "loadpng.h"
#include "../Mapping/Cell.h"
#include <math.h>

using namespace Mapping;
using namespace Common;

//Encode from raw pixels to disk with a single function call
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void pngUtil::encodeOneStep(const char* filename, std::vector<unsigned char> image,
		unsigned width, unsigned height) {
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": "
				<< lodepng_error_text(error) << std::endl;
}

void pngUtil::decodeOneStep(const char* filename) {
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	//if there's an error, display it
	if (error)
		std::cout << "decoder error " << error << ": "
				<< lodepng_error_text(error) << std::endl;

	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}
bool pngUtil::IsInDistance(int firstX,int firstY,int secondX,int secondY, unsigned paddingSize) {
	return (sqrt((double)((firstX - secondX)*(firstX - secondX) + (firstY - secondY)*(firstY - secondY))) < paddingSize);
}
char* pngUtil::PadMap(const char* filename, unsigned paddingSize) {
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height, paddedWidth, paddedHeight;
	unsigned x, y, innerX, innerY;
	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	//if there's an error, display it
	if (error)
		std::cout << "decoder error " << error << ": "
				<< lodepng_error_text(error) << std::endl;

	std::vector<unsigned char> navImage; //the raw pixels
	std::vector<unsigned char> paddedImage;

	paddedWidth = width + (paddingSize * 2);
	paddedHeight = height + (paddingSize * 2);
	paddedImage.resize(paddedWidth * paddedHeight * 4);
	unsigned char color;

	for (y = 0; y < paddedHeight; y++)
		for (x = 0; x < paddedWidth; x++)
		{
			paddedImage[y * (paddedWidth * 4) + (x * 4 + 0)] = 255;
			paddedImage[y * (paddedWidth * 4) + (x * 4 + 1)] = 255;
			paddedImage[y * (paddedWidth * 4) + (x * 4 + 2)] = 255;
			paddedImage[y * (paddedWidth * 4) + (x * 4 + 3)] = 255;
		}

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {
			if (image[y * width * 4 + x * 4 + 0] == 0 && image[y * width * 4 + x * 4 + 1] == 0 && image[y * width * 4 + x * 4 + 2] == 0)
			{
				for (innerX = x; innerX <= x +(paddingSize*2); innerX++)
				{
					for (innerY = y ; innerY <= y + (paddingSize*2); innerY++)
					{
						if (IsInDistance(x + paddingSize, y + paddingSize, innerX, innerY, paddingSize))
						{
							paddedImage[innerY * (paddedWidth * 4) + (innerX * 4 + 0)] = 0;
							paddedImage[innerY * (paddedWidth * 4) + (innerX * 4 + 1)] = 0;
							paddedImage[innerY * (paddedWidth * 4) + (innerX * 4 + 2)] = 0;
							paddedImage[innerY * (paddedWidth * 4) + (innerX * 4 + 3)] = 255;
						}
					}
				}
			}
		}

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {
			image[y * width * 4 + x * 4 + 0] = paddedImage[(y * paddedWidth + paddingSize) * 4 + (x + paddingSize) * 4 + 0];
			image[y * width * 4 + x * 4 + 1] = paddedImage[(y * paddedWidth + paddingSize) * 4 + (x + paddingSize) * 4 + 1];
			image[y * width * 4 + x * 4 + 2] = paddedImage[(y * paddedWidth + paddingSize) * 4 + (x + paddingSize) * 4 + 2];
			image[y * width * 4 + x * 4 + 3] = paddedImage[(y * paddedWidth + paddingSize) * 4 + (x + paddingSize) * 4 + 3];
		}

	string directory = ConfigurationManager::Instance()->GetMap()->Map_Directory;
	char* path = StringHelper::ConvertStringToCharArray(directory + "/newMap.png");
	encodeOneStep(path, image, width, height);

	char* pathToReturn = StringHelper::ConvertStringToCharArray(directory + "/newMap.png");
	return pathToReturn;
}



unsigned char pngUtil::getPixelColor(const std::vector<unsigned char>& rawImage, unsigned width, unsigned height, unsigned row, unsigned col) {
	unsigned basePos = row * width * 4 + col * 4;

	if (width == 0 || height == 0 || basePos + A_OFFSET > rawImage.size()) {
		return COLOR_INVALID;
	}

	if (rawImage[basePos + R_OFFSET] == COLOR_BLACK && rawImage[basePos + G_OFFSET] == COLOR_BLACK && rawImage[basePos + B_OFFSET] == COLOR_BLACK) {
		return COLOR_BLACK;
	}
	return COLOR_WHITE;
}
