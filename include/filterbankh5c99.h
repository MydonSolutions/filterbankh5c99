/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * filterbankh5_defs.h                                                                 *
 * -----------                                                                 *
 * Global Definitions       .                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _FILTERBANKH5_C99_H
#define _FILTERBANKH5_C99_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "filterbankc99/filterbank_header.h"
#include "filterbankc99/filterbank_utils.h"

/*
 * HDF5 library definitions
 */
#include "hdf5/serial/hdf5.h"
#include "hdf5/serial/H5pubconf.h"
#include "hdf5/serial/H5DSpublic.h"
#ifndef H5_HAVE_THREADSAFE
#error The installed HDF5 run-time is not thread-safe!
#endif

// This stringification trick is from "info cpp"
// See https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define STRINGIFY1(s) #s
#define STRINGIFY(s) STRINGIFY1(s)

typedef struct {
    int active;                 // Still active? 1=yes, 0=no
    hid_t file_id;              // File-level handle (similar to an fd)
    hid_t dataset_id;           // Dataset "data" handle
    hid_t dataspace_id;         // Dataspace handle for dataset "data"
    unsigned int elem_size;     // Byte size of one spectra element (E.g. 4 if nbits=32)
    hid_t elem_type;            // HDF5 type for all elements (derived from nbits in filterbankh5_open)
    size_t tint_size;           // Size of a time integration (computed in filterbankh5_open)
    hsize_t offset_dims[3];     // Next offset dimensions for the filterbankh5_write function
                                // (offset_dims[0] : time integration count)
    hsize_t filesz_dims[3];     // Accumulated file size in dimensions
    unsigned long byte_count;   // Number of bytes output so far
    unsigned long dump_count;   // Number of dumps processed so far
} filterbankh5_context_t;

/*
 * Global definitions
 */
#define DATASETNAME         "data"  // FBH5 dataset name to hold the data matrix
#define NDIMS               3       // # of data matrix dimensions (rank)
#define FILTERBANK_CLASS    "FILTERBANK"    // File-level attribute "CLASS"
#define FILTERBANK_VERSION  "2.0"   // File-level attribute "VERSION"
#define ENABLER_FD_FOR_FBH5 42      // Fake fd value to enable dump_file_thread_func()

/*
 * fbh5 API functions
 */
int     filterbankh5_open(filterbankh5_context_t * p_fbh5_ctx, filterbank_header_t * p_fb_hdr, unsigned int Nds, char * output_path, int debug_callback);
int     filterbankh5_write(filterbankh5_context_t * p_fbh5_ctx, filterbank_header_t * p_fb_hdr, void * buffer, size_t bufsize, int debug_callback);
int     filterbankh5_close(filterbankh5_context_t * p_fbh5_ctx, int debug_callback);

/*
 * fbh5_util.c functions
 */
void    filterbankh5_info(const char * format, ...);
void    filterbankh5_warning(char * srcfile, int linenum, char * msg);
void    filterbankh5_error(char * srcfile, int linenum, char * msg);
void    filterbankh5_set_str_attr(hid_t file_or_dataset_id, char * tag, char * value, int debug_callback);
void    filterbankh5_set_dataset_double_attr(hid_t dataset_id, char * tag, double * p_value, int debug_callback);
void    filterbankh5_set_dataset_int_attr(hid_t dataset_id, char * tag, int * p_value, int debug_callback);
void    filterbankh5_write_metadata(hid_t dataset_id, filterbank_header_t * p_metadata, int debug_callback);
void    filterbankh5_set_ds_label(filterbankh5_context_t * p_fbh5_ctx, char * label, int dims_index, int debug_callback);
void    filterbankh5_show_context(char * caller, filterbankh5_context_t * p_fbh5_ctx);
void    filterbankh5_blimpy_chunking(filterbank_header_t * p_fb_hdr, hsize_t * p_cdims);

/*
 * HDF5 library ID of the Bitshuffle filter.
 */
#define FILTER_ID_BITSHUFFLE 32008

#endif
