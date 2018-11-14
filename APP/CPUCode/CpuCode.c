#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

#include "ppmIO.h"

int check(int size, int32_t *vector1, int32_t *vector2)
{
	int status = 0;
	for (int i = 0; i < size; i++) {
		if (vector1[i] != vector2[i]) {
			fprintf(stderr, "Error detected outVector[%f] != %f\n",
				vector1[i], vector2[i]);
			status = 1;
		}
	}
	return status;
}

int prepareExpected(int width, int height, int32_t *inVector, int32_t *expectedVector)
{
	for (int x = 0; x < width; x++) {
	    for (int y = 0; y < height; y++) {
	        if((x > 0 && x < width-1) && (y > 0 && y < height-1)) {
	            int max = 0;
	            int tmp;
	            for(int movex = -1; movex <= 1; movex++) {
                    for(int movey = -1; movey <= 1; movey++) {
                        if(movex == 0 && movey == 0) {
                            continue;
                        }
                        tmp = fabs(inVector[x*width+y] - inVector[(x+movex)*width+(y+movey)]);

                        if(tmp > max) {
                            max = tmp;
                        }
                    }
	            }
	            expectedVector[x*width+y] = max;
	        }else {
	            expectedVector[x*width+y] = 0;
	        }
	    }
	}
}

int main(void)
{
	printf("Loading image.\n");
	int32_t *inImage;
	int32_t *expectedImage;
	int width = 0, height = 0;

	loadImage("lena.ppm", &inImage, &width, &height, 1);
	loadImage("lena.ppm", &expectedImage, &width, &height, 1);


	int dataSize = width * height * sizeof(int32_t);
	// Allocate a buffer for the output image
	int32_t *outImage = malloc(dataSize);

    printf("Preparing expected output.\n");
    prepareExpected(width, height, inImage, expectedImage);

	printf("Running Kernel.\n");


	MeanSolution(width * height, inImage, outImage);

	printf("Saving image.\n");
	writeImage("lena_mean1.ppm", outImage, width, height, 1);
	int status = check(width*height, outImage, expectedImage);

	if (status)
		printf("Test failed.\n");
	else
		printf("Test passed OK!\n");

	printf("Exiting\n");
	return status;
}
