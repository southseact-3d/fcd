/*
 * miniz.h placeholder
 * 
 * Download the real miniz.h from:
 * https://github.com/richgel999/miniz/blob/master/miniz.h
 *
 * Place it in this directory. Required by tinyexr for ZIP compression.
 *
 * License: MIT
 */
#ifndef MINIZ_H
#define MINIZ_H

#ifdef __cplusplus
extern "C" {
#endif

/* Placeholder stubs - replace with real miniz.h */

/* Compression/Decompression API */
void *mz_compress(void *pDest, size_t *pDest_len, const void *pSource, size_t source_len);
void *mz_uncompress(void *pDest, size_t *pDest_len, const void *pSource, size_t source_len);

/* ZIP archive handling */
typedef void *mz_zip_archive;

int mz_zip_reader_init(mz_zip_archive *pZip, mz_uint64 zip_size, mz_uint flags);
int mz_zip_reader_init_file(mz_zip_archive *pZip, const char *pZip_filename, mz_uint flags);
int mz_zip_reader_init_cfile(mz_zip_archive *pZip, FILE *pFile, mz_uint64 zip_size, mz_uint flags);
mz_uint mz_zip_reader_get_num_files(mz_zip_archive *pZip);
int mz_zip_reader_file_stat(mz_zip_archive *pZip, mz_uint file_index, void *pStat);
void *mz_zip_reader_extract_to_heap(mz_zip_archive *pZip, mz_uint file_index, size_t *pSize, mz_uint flags);
int mz_zip_reader_end(mz_zip_archive *pZip);

#endif /* MINIZ_H */
