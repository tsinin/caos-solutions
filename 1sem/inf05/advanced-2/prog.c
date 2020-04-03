#include <emmintrin.h>
#include <immintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BMP_HEADER_SIZE 56

static unsigned char prologue[14];

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint16_t type;      // Magic identifier: 0x4d42
    uint32_t size;      // File size in bytes
    uint16_t reserved1; // Not used
    uint16_t reserved2; // Not used
    uint32_t offset;    // Offset to image data in bytes from beginning of file
    uint32_t dib_header_size;  // DIB Header size in bytes
    int32_t width_px;          // Width of the image
    int32_t height_px;         // Height of image
    uint16_t num_planes;       // Number of color planes
    uint16_t bits_per_pixel;   // Bits per pixel
    uint32_t compression;      // Compression type
    uint32_t image_size_bytes; // Image size in bytes
    int32_t x_resolution_ppm;  // Pixels per meter
    int32_t y_resolution_ppm;  // Pixels per meter
    uint32_t num_colors;       // Number of colors
    uint32_t important_colors; // Important colors
    uint16_t unimportant_smth;
} BMPHeader;

#pragma pack(pop)

typedef struct {
    BMPHeader header;
    unsigned char* data;
} BMPImage;

BMPImage* read_image(FILE* fp)
{
    BMPImage* image = malloc(sizeof(BMPImage));
    fread(&image->header, BMP_HEADER_SIZE, 1, fp);
    image->data = malloc(image->header.image_size_bytes);

    fread(prologue, image->header.offset - BMP_HEADER_SIZE, 1, fp);
    fread(image->data, image->header.image_size_bytes, 1, fp);
    return image;
}

void write_image(FILE* fp, BMPImage* image)
{
    fwrite(&image->header, BMP_HEADER_SIZE, 1, fp);
    fwrite(prologue, image->header.offset - BMP_HEADER_SIZE, 1, fp);
    fwrite(image->data, image->header.image_size_bytes, 1, fp);
}

void free_image(BMPImage* image)
{
    free(image->data);
    free(image);
}

int main(int argc, char* argv[])
{
    FILE* pic1 = fopen(argv[1], "rb");
    FILE* pic2 = fopen(argv[2], "rb");
    FILE* pic3 = fopen(argv[3], "wb");
    BMPImage* image1 = read_image(pic1);
    BMPImage* image2 = read_image(pic2);
    BMPImage* image3 = malloc(sizeof(BMPImage));
    image3->header = image1->header;
    image3->data = malloc(
        sizeof(unsigned char) * 4 * image3->header.height_px *
        image3->header.width_px);
    for (int i = 0; i < image3->header.width_px * image3->header.height_px;
         i += 2) {
        // Update image3->data[4 * i, 4 * i + 1, 4 * i + 2, 4 * i + 3]
        // and image3->data[4 * i + 4, 4 * i + 5, 4 * i + 6, 4 * i + 7]
        if (i + 1 == image3->header.width_px * image3->header.height_px)
            break;
        float alpha_b = image1->data[4 * i];
        float col_b_1 = image1->data[4 * i + 1];
        float col_b_2 = image1->data[4 * i + 2];
        float col_b_3 = image1->data[4 * i + 3];

        float alpha_a = image2->data[4 * i];
        float col_a_1 = image2->data[4 * i + 1];
        float col_a_2 = image2->data[4 * i + 2];
        float col_a_3 = image2->data[4 * i + 3];

        float alpha_d = image1->data[4 * i + 4];
        float col_d_1 = image1->data[4 * i + 5];
        float col_d_2 = image1->data[4 * i + 6];
        float col_d_3 = image1->data[4 * i + 7];

        float alpha_c = image2->data[4 * i + 4];
        float col_c_1 = image2->data[4 * i + 5];
        float col_c_2 = image2->data[4 * i + 6];
        float col_c_3 = image2->data[4 * i + 7];
        __m256 ans1 = _mm256_set_ps(
            col_c_3, col_c_2, col_c_1, 1, col_a_3, col_a_2, col_a_1, 1);
        __m256 ans2 = _mm256_set_ps(
            col_d_3, col_d_2, col_d_1, 1, col_b_3, col_b_2, col_b_1, 1);
        __m256 mult = _mm256_set1_ps(255);
        ans1 = _mm256_mul_ps(ans1, mult);
        mult = _mm256_set_ps(
            alpha_c,
            alpha_c,
            alpha_c,
            alpha_c,
            alpha_a,
            alpha_a,
            alpha_a,
            alpha_a);
        ans1 = _mm256_mul_ps(ans1, mult);
        mult = _mm256_set_ps(
            255 - alpha_c,
            255 - alpha_c,
            255 - alpha_c,
            255 - alpha_c,
            255 - alpha_a,
            255 - alpha_a,
            255 - alpha_a,
            255 - alpha_a);
        ans2 = _mm256_mul_ps(ans2, mult);
        mult = _mm256_set_ps(
            alpha_d,
            alpha_d,
            alpha_d,
            alpha_d,
            alpha_b,
            alpha_b,
            alpha_b,
            alpha_b);
        ans2 = _mm256_mul_ps(ans2, mult);
        ans1 = _mm256_add_ps(ans1, ans2);

        float new_alpha1 = ans1[0];
        float new_alpha2 = ans1[4];

        image3->data[4 * i] = (unsigned char)(new_alpha1 / 255);
        image3->data[4 * i + 4] = (unsigned char)(new_alpha2 / 255);

        __m256 divider = _mm256_set_ps(
            new_alpha2,
            new_alpha2,
            new_alpha2,
            new_alpha2,
            new_alpha1,
            new_alpha1,
            new_alpha1,
            new_alpha1);
        ans1 = _mm256_div_ps(ans1, divider);

        image3->data[4 * i + 1] = (unsigned char)ans1[1];
        image3->data[4 * i + 2] = (unsigned char)ans1[2];
        image3->data[4 * i + 3] = (unsigned char)ans1[3];
        image3->data[4 * i + 5] = (unsigned char)ans1[5];
        image3->data[4 * i + 6] = (unsigned char)ans1[6];
        image3->data[4 * i + 7] = (unsigned char)ans1[7];
    }
    if (image3->header.width_px * image3->header.height_px % 2 ==
        1) { // Last pixel
        int pix_num = image3->header.width_px * image3->header.height_px - 1;
        int alpha1 = image1->data[4 * pix_num];
        int alpha2 = image2->data[4 * pix_num];
        for (int j = 1; j < 4; ++j) {
            int c1 = image1->data[4 * pix_num + j];
            int c2 = image2->data[4 * pix_num + j];
            image3->data[4 * pix_num + j] =
                (unsigned char)((255 * c2 * alpha2 + c1 * alpha1 * (255 - alpha2)) / (255 * alpha2 + alpha1 * (255 - alpha2)));
        }
        image3->data[4 * pix_num] =
            (unsigned char)(alpha2 + (255 * alpha1 - alpha1 * alpha2) / 255);
    }
    write_image(pic3, image3);
    fclose(pic1);
    fclose(pic2);
    fclose(pic3);
    free_image(image1);
    free_image(image2);
    free_image(image3);
    return 0;
}
