#include "img_omp.h"

int main()
{
    // Read image data

    BmpHeader *header = get_BmpHeader("test_img.bmp");
    unsigned char *image = get_BmpImage("test_img.bmp", header);

    // Read desired filter type

    char filter_type[15] = {0};
    printf("Enter the filter to be applied on the test image (brightness/negative/grayscale)\n> ");
    fgets(filter_type, sizeof(filter_type), stdin);

    // Apply desired filter

    unsigned char *result = NULL;

    if (strcmp(filter_type, "brightness\n") == 0)
    {
        printf("Enter the value of brightness to be applied: ");
        int value;
        scanf("%d", &value);

	    result = set_brightness(value, header, image);
    }

    else if (strcmp(filter_type, "negative\n") == 0)
        result = negative(header, image);

    else if (strcmp(filter_type, "grayscale\n") == 0)
        result = grayscale(header, image);

    else
        printf("Please enter a valid filter type!\n");

    // Write filtered image result to file

    if (result)
    {
        write_bmp_image("result_img.bmp", header, result);
	    printf("Filtered image written successfully!\n");
    }

    // Free allocated resources

    if (result)
        free(result);
    free(image);
    free(header);

    return 0;
}
