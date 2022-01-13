#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//typedef structure of pixel colors
typedef struct
{
    unsigned char red, green, blue;
} pixelColor;

//typedef structure of ppm image informations
typedef struct
{
    unsigned int xImage, yImage;
    pixelColor *dataImage;
} ppmImage;

//write final result of sharpening image
void writePPM(const char *filename, ppmImage *img)
{
    FILE *ppmFile;
    //open file for output
    ppmFile = fopen(filename, "wb");
    if (!ppmFile)
    {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //image format info-s
    fprintf(ppmFile, "P6\n%d\n%d\n%d\n",img->xImage, img->yImage, 255);

    // pixel dataImage
    fwrite(img->dataImage, 3 * img->xImage, img->yImage, ppmFile);
    fclose(ppmFile);
}

//function of sharpeing pixels
void sharpeningPPM(ppmImage *img, ppmImage *imageSharpen)
{
    if (img)
    {

        for (unsigned i = 0; i < img->yImage; i++)
        {
            for (unsigned j = 0; j < img->xImage; j++)
            {
                if (i == 0 || i == img->yImage - 1 || j == 0 || j == img->xImage - 1)
                {
                    int currPixel = (img->xImage * i + j);
                    imageSharpen->dataImage[currPixel].red = img->dataImage[currPixel].red;
                    imageSharpen->dataImage[currPixel].green = img->dataImage[currPixel].green;
                    imageSharpen->dataImage[currPixel].blue = img->dataImage[currPixel].blue;
                }
                else
                {
                    //position of pixels
                    int currPixel = (img->xImage * i + j);
                    int upPixel = (img->xImage * (i - 1) + j);
                    int downPixel = (img->xImage * (i + 1) + j);
                    int leftPixel = (img->xImage * i + (j - 1));
                    int rightPixel = (img->xImage * i + (j + 1));
                    //data from pixels red
                    int dataCur = img->dataImage[currPixel].red;
                    int dataUp = img->dataImage[upPixel].red;
                    int dataDown = img->dataImage[downPixel].red;
                    int dataLeft = img->dataImage[leftPixel].red;
                    int dataRight = img->dataImage[rightPixel].red;
                    //calculate sharpening
                    int sharpy = (5 * dataCur) - (dataUp + dataRight + dataDown + dataLeft);
                    if (sharpy < 0)
                        sharpy = 0;
                    else if (sharpy > 255)
                        sharpy = 255;

                    //sharepening image
                    imageSharpen->dataImage[currPixel].red = sharpy;

                    //data from pixels green
                    dataCur = img->dataImage[currPixel].green;
                    dataUp = img->dataImage[upPixel].green;
                    dataDown = img->dataImage[downPixel].green;
                    dataLeft = img->dataImage[leftPixel].green;
                    dataRight = img->dataImage[rightPixel].green;
                    //calculate sharpening
                    sharpy = (5 * dataCur) - (dataUp + dataRight + dataDown + dataLeft);
                    if (sharpy < 0)
                        sharpy = 0;
                    else if (sharpy > 255)
                        sharpy = 255;

                    //sharepening image
                    imageSharpen->dataImage[currPixel].green = sharpy;

                    //data from pixels green
                    dataCur = img->dataImage[currPixel].blue;
                    dataUp = img->dataImage[upPixel].blue;
                    dataDown = img->dataImage[downPixel].blue;
                    dataLeft = img->dataImage[leftPixel].blue;
                    dataRight = img->dataImage[rightPixel].blue;
                    //calculate sharpening
                    sharpy = (5 * dataCur) - (dataUp + dataRight + dataDown + dataLeft);
                    if (sharpy < 0)
                        sharpy = 0;
                    else if (sharpy > 255)
                        sharpy = 255;

                    //sharepening image
                    imageSharpen->dataImage[currPixel].blue = sharpy;
                }
            }
        }
    }
}
int my_round (double n)
{
    int n_round;
    if(((int)(n*10))%10>=5)
        n_round=(int)n+1;
    else
        n_round=(int)n;
    
    return n_round; 
}
//histogram of sharpening images
void histogram(const char *filename, ppmImage *imageSharpen, int *histo)
{
    FILE *ppmFile;
    //open file for output
    ppmFile = fopen(filename, "wb");
    if (!ppmFile)
    {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }
    for (int i = 0; i < 5; i++)
        histo[i] = 0;
    for (unsigned i = 0; i < imageSharpen->yImage; i++)
    {
        for (unsigned j = 0; j < imageSharpen->xImage; j++)
        {
            int currPixel = (imageSharpen->xImage * i + j);
            //printf("%d ",imageSharpen->dataImage[currPixel].red);
            int y = my_round(0.2126 * imageSharpen->dataImage[currPixel].red + 0.7152 * imageSharpen->dataImage[currPixel].green + 0.0722 * imageSharpen->dataImage[currPixel].blue);
            if (y >= 0 && y <= 50)
                histo[0]++;
            else if (y >= 51 && y <= 101)
                histo[1]++;
            else if (y >= 102 && y <= 152)
                histo[2]++;
            else if (y >= 153 && y <= 203)
                histo[3]++;
            else if (y >= 204 && y <= 255)
                histo[4]++;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        fprintf(ppmFile, "%d ", histo[i]);
    }
    fclose(ppmFile);
}
//args ubuntu 
int main(int argc, char *argv[])
{
    //file argument
    if (argc != 2)
    {
        printf("ERROR\nPlease insert .ppm file\n");
        exit(1);
    }

    //read from argv file
    //char bufferFile[16]; //buffer reading from file
    FILE *ppmFile;       //file pointer
    int rgb;             //255
    char bufferFile[16];
    //open PPM file for reading
    ppmFile = fopen(argv[1], "rb");
    if (!ppmFile)
    {
        fprintf(stderr, "unable to open file '%s'\n", argv[1]);
        exit(1);
    }
    
    //read image format
    if (!fgets(bufferFile, sizeof(bufferFile), ppmFile))
    {
        perror(argv[1]);
        exit(1);
    }

    //check the image format
    if (bufferFile[0] != 'P' || bufferFile[1] != '6')
    {
        fprintf(stderr, "invalid image format \n");
        exit(1);
    }

    /*
        DINAMIC ALLOCATION 1
    */

    //alloc memory for original image
    ppmImage *img = (ppmImage *)malloc(sizeof(ppmImage));
    //creating sharpening image (same as dimensions with original image)
    ppmImage *imageSharpen = (ppmImage *)malloc(sizeof(ppmImage)); //sharpen neighbors

    if (!img)
    {
        fprintf(stderr, "can't allocate memory\n");
        exit(1);
    }
    if (!imageSharpen)
    {
        fprintf(stderr, "can't allocate memory\n");
        exit(1);
    }

    //read image size information
    if (fscanf(ppmFile, "%d %d %d", &img->xImage, &img->yImage, &rgb) != 3)
    {
        fprintf(stderr, "invalid size '%s'\n", argv[1]);
        exit(1);
    }

    //x, y originals for sharpen image
    imageSharpen->xImage = img->xImage;
    imageSharpen->yImage = img->yImage;
    
    //check rgb component depth
    if (rgb != 255)
    {
        fprintf(stderr, "'%s' invalid\n", argv[1]);
        exit(1);
    }

    while (fgetc(ppmFile) != '\n');
    
    /*
        DINAMIC ALLOCATION 2
    */
    
    //memory allocation for pixel dataImage
    img->dataImage = (pixelColor *)malloc(img->xImage * img->yImage * sizeof(pixelColor));
    //alocation of memory for sharpen pixel image
    imageSharpen->dataImage = (pixelColor *)malloc(imageSharpen->xImage * imageSharpen->yImage * sizeof(pixelColor));

    if (!img)
    {
        fprintf(stderr, "can't allocate memory\n");
        exit(1);
    }

    if (!imageSharpen)
    {
        fprintf(stderr, "can't allocate memory\n");
        exit(1);
    }

    //read pixel dataImage from file
    if (fread(img->dataImage, 3 * (unsigned int)(img->xImage), (unsigned int)img->yImage, ppmFile) != img->yImage)
    {
        fprintf(stderr, "error image '%s'\n", argv[1]);
        exit(1);
    }

    fclose(ppmFile);
	int *histo = (int*)malloc(5*sizeof(int));
    //shapering the image
    sharpeningPPM(img, imageSharpen);

    //write the image into output file
    writePPM("output.ppm", imageSharpen);
    histogram("output.txt", imageSharpen,histo);

    free((void*)imageSharpen->dataImage);
    free(imageSharpen);
    free((void*)img->dataImage);
    free(img);
    free(histo);

    exit(0);
}