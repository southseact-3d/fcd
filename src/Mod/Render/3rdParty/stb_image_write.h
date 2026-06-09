/*
 * stb_image_write.h placeholder
 * 
 * Download the real stb_image_write.h from:
 * https://github.com/nothings/stb/blob/master/stb_image_write.h
 *
 * Place it in this directory.
 *
 * License: Public domain (MIT OR Unlicense)
 */
#ifndef STB_IMAGE_WRITE_H
#define STB_IMAGE_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION

int stbi_write_png(const char *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_jpg(const char *filename, int w, int h, int comp, const void *data, int quality);
int stbi_write_bmp(const char *filename, int w, int h, int comp, const void *data);
int stbi_write_tga(const char *filename, int w, int h, int comp, const void *data);

#endif /* STB_IMAGE_WRITE_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* STB_IMAGE_WRITE_H */
