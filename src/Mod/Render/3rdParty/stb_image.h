/*
 * stb_image.h placeholder
 * 
 * Download the real stb_image.h from:
 * https://github.com/nothings/stb/blob/master/stb_image.h
 *
 * Place it in this directory.
 *
 * License: Public domain (MIT OR Unlicense)
 * https://github.com/nothings/stb/blob/master/LICENSE
 */
#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Placeholder stub - replace with real stb_image.h */
#ifndef STB_IMAGE_IMPLEMENTATION

typedef unsigned char stbi_uc;
typedef float stbi__float;

stbi_uc *stbi_load(const char *filename, int *x, int *y, int *channels_in_file, int desired_channels);
stbi_uc *stbi_load_from_memory(const stbi_uc *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
void stbi_image_free(void *retval_from_stbi_load);
int stbi_info(const char *filename, int *x, int *y, int *comp);
int stbi_is_hdr(const char *filename);

/* HDR-specific loading */
float *stbi_loadf(const char *filename, int *x, int *y, int *channels_in_file, int desired_channels);
float *stbi_loadf_from_memory(const stbi_uc *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);

#endif /* STB_IMAGE_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* STB_IMAGE_H */
