/*
 * tinyexr.h placeholder
 * 
 * Download the real tinyexr.h from:
 * https://github.com/syoyo/tinyexr
 *
 * Place it in this directory.
 *
 * License: MIT License
 * https://github.com/syoyo/tinyexr/blob/master/LICENSE
 */
#ifndef TINYEXR_H_
#define TINYEXR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int num_channels;
    const char **channel_names;
    const float **images; /* RGBARFP image data */
    int width;
    int height;
} EXRImage;

typedef struct {
    const char *name;
    int pixel_type;         /* 0=UINT, 1=HALF, 2=FLOAT */
    int x_sampling;
    int y_sampling;
    unsigned char pad[2];
} EXRChannelInfo;

typedef struct {
    int num_channels;
    EXRChannelInfo *channels;
    int data_window[4];     /* min_x, min_y, max_x, max_y */
    int display_window[4];
    float pixel_aspect_ratio;
    int line_order;         /* 0=increasing, 1=decreasing, 2=random */
    float screen_window_center[2];
    float screen_window_width;
    int compression;        /* 0=none, 1=RLE, 2=ZIP, 3=PIZ, etc. */
} EXRHeader;

typedef struct {
    const char *name;
    int pixel_type;
    int requested_pixel_type;
    int x_sampling;
    int y_sampling;
    unsigned char pad[2];
} EXRChannelInfoWritable;

typedef struct {
    int num_channels;
    EXRChannelInfoWritable *channels;
    int data_window[4];
    int display_window[4];
    float pixel_aspect_ratio;
    int line_order;
    float screen_window_center[2];
    float screen_window_width;
    int compression;
} EXRHeaderWritable;

/* Placeholder stubs - replace with real tinyexr.h */
const char* EXRErrorMessage(int errcode);
int LoadEXR(float **out_rgba, int *width, int *height, const char *filename, const char **err);
int LoadEXRFromMemory(float **out_rgba, int *width, int *height, const unsigned char *memory, size_t size, const char **err);
int SaveEXR(const float *in_rgba, int width, int height, int components, int compression, const char *filename, const char **err);
int SaveEXRToMemory(const float *in_rgba, int width, int height, int components, int compression, unsigned char **out_memory, size_t *out_size, const char **err);
void InitEXRHeader(EXRHeader *exr_header);
void InitEXRImage(EXRImage *exr_image);
void FreeEXRHeader(EXRHeader *exr_header);
void FreeEXRImage(EXRImage *exr_image);
void FreeEXRMappedMemory(unsigned char *map);

#ifdef __cplusplus
}
#endif

#endif /* TINYEXR_H_ */
