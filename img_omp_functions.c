#include "img_omp.h"

BmpHeader *get_BmpHeader(char *fname)
{
    // Allocate memory to story header information

    BmpHeader *header = malloc(sizeof(BmpHeader));
    if (!header)
    {
        printf("Header memory allocation failed.\n");
        return NULL;
    }

    // Open file in read binary mode

    FILE *fptr = fopen(fname, "rb");
    if (!fptr)
    {
        printf("File read failed.\n");
        return NULL;
    }

    // Read the file header

    fread(header, sizeof(BmpHeader), 1, fptr);

    // Close file pointer and return header

    fclose(fptr);
    return header;
}

unsigned char *get_BmpImage(char *fname)
{
    // Open file in read binary mode

    FILE *fptr = fopen(fname, "rb");
    if (!fptr)
    {
        printf("File read failed.\n");
        return NULL;
    }

    // Get header information

    BmpHeader *header = get_BmpHeader(fname);

    // Verify that this is a .BMP file by checking bitmap id

    if (header->bfType != 0x4D42)
    {
        printf("The read file is not a .bmp file.\n");
        fclose(fptr);
        return NULL;
    }

    // Prepare to extract image bitmap data

    unsigned char *image;  // store image data

    // Move file pointer to the beginning of bitmap data

    fseek(fptr, header->bfOffBits, SEEK_SET);

    // Allocate enough memory for the bitmap image data

    image = malloc(header->biSizeImage);

    // Verify memory allocation

    if (!image)
    {
        printf("BitmapImage memory allocation failed.\n");
        fclose(fptr);
        return NULL;
    }

    // Read in the bitmap image data

    fread(image, header->biSizeImage, 1, fptr);

    // Make sure bitmap image data was read

    if (!image)
    {
        printf("BitmapImage data read failed.\n");
        fclose(fptr);
        return NULL;
    }

    // Close file, free header pointers and return bitmap image data

    fclose(fptr);
    free(header);
    return image;
}

int write_bmp_image(const char *fname, const BmpHeader *header, const unsigned char *image)
{
    // Open file in write binary mode

    FILE *fptr = fopen(fname, "wb");
    if (!fptr)
    {
        printf("File write failed.\n");
        return -1;
    }

    // Write header information

    fwrite(header, sizeof(BmpHeader), 1, fptr);

    // Move file pointer to beginning of bitmap data

    fseek(fptr, header->bfOffBits, SEEK_SET);
    fwrite(image, header->biSizeImage, 1, fptr);

    // Close file and return

    fclose(fptr);
    return 0;
}

unsigned char *set_brightness(const int value, const BmpHeader *header, const unsigned char *image)
{

    /*
      Sets brightness by modifying R, G and B values according to the "value" parameter.
      Positive values will increase the brightness and vice versa.
      Values range = [-100, 100]
    */

    // Check value range

    if (value < -100 || value > 100)
    {
        printf("Invalid brightness value provided.\n");
        return NULL;
    }

    // Get relevant image details

    int numBytes = header->biSizeImage;
    unsigned char *imageOut = malloc(numBytes);
    if (!imageOut)
    {
        printf("Output image memory allocation failed.\n");
        return NULL;
    }

    // Set brightness by modifying each pixel

    int temp, i;
#pragma omp parallel for shared(image, imageOut, numBytes, value) private(i, temp)
    for (i = 0; i < numBytes; i++)
    {
        temp = image[i];
        if (temp + value > 255)
            imageOut[i] = 255;
        else if (temp + value < 0)
            imageOut[i] = 0;
        else
            imageOut[i] = temp + value;
    }

    return imageOut;
}

unsigned char *negative(const BmpHeader *header, const unsigned char *image)
{

    /*
      Adds negative filter by modifying R, G and B values.
    */

    // Get relevant image details

    int numBytes = header->biSizeImage;

    unsigned char *imageOut = malloc(numBytes);
    if (!imageOut)
    {
        printf("Output image memory allocation failed.\n");
        return NULL;
    }

    // Invert values of each pixel in imageOut
    
    int i;
#pragma omp parallel for shared(image, imageOut, numBytes) private(i)
    for (i = 0; i < numBytes; i++)
        imageOut[i] = 255 - image[i];

    return imageOut;
}

unsigned char *grayscale(const BmpHeader *header, const unsigned char *image)
{

    /*
      Creates grayscale by modifying R, G and B values.
    */

    // Get relevant image details

    int numBytes = header->biSizeImage;
    unsigned char *imageOut = malloc(numBytes);
    if (!imageOut)
    {
        printf("Output image memory allocation failed.\n");
        return NULL;
    }

    // Invert values of each pixel in imageOut

    int i, avg;
#pragma omp parallel for shared(image, imageOut, numBytes) private(i, avg)
    for (i = 0; i < numBytes; i += 3)
    {
        avg = (int)(image[i] + image[i + 1] + image[i + 2]) / 3;
        imageOut[i] = avg;
        imageOut[i + 1] = avg;
        imageOut[i + 2] = avg;
    }

    return imageOut;
}
