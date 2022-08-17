/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * simon.c                                                                     *
 * -------                                                                     *
 * Sample fbh5 application.                                .                   *
 * See Makefile for the integration with librawspec.so.                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "filterbankh5c99.h"


#define DEBUG_CALLBACK  0
#define NBITS           32
#define NCHANS          (1048576/16)
#define NFPC            1
#define NIFS            1
#define NTINTS          16
#define PATH_H5         "./simon.h5"


/***
	Initialize metadata to Voyager 1 values.
***/
void make_voyager_1_metadata(filterbankc99_header_t *fb_hdr) {
    memset(fb_hdr, 0, sizeof(filterbankc99_header_t));
    fb_hdr->az_start = 0.0;
    fb_hdr->data_type = 1;
    fb_hdr->fch1 = 8421.386717353016;       // MHz
    fb_hdr->foff = -2.7939677238464355e-06; // MHz
    fb_hdr->ibeam = 1;
    fb_hdr->machine_id = 42;
    fb_hdr->nbeams = 1;
    fb_hdr->nchans = NCHANS;            // # of fine channels
    fb_hdr->nfpc = NFPC;                // # of fine channels per coarse channel
    fb_hdr->nifs = NIFS;                // # of feeds (E.g. polarisations)
    fb_hdr->nbits = 32;                 // 4 bytes i.e. float32
    fb_hdr->src_raj = 171003.984;       // 17:12:40.481
    fb_hdr->src_dej = 121058.8;         // 12:24:13.614
    fb_hdr->telescope_id = 6;           // GBT
    fb_hdr->tsamp = 18.253611008;       // seconds
    fb_hdr->tstart = 57650.78209490741; // 2020-07-16T22:13:56.000
    fb_hdr->za_start = 0.0;

    strcpy(fb_hdr->source_name, "Voyager1");
    strcpy(fb_hdr->rawdatafile, "guppi_57650_67573_Voyager1_0002.0000.raw");
}


void fatal_error(int linenum, char * msg) {
    fprintf(stderr, "\n*** simon: FATAL ERROR at line %d :: %s.\n", linenum, msg);
    exit(86);
}


/***
	Main entry point.
***/
int main(int argc, char **argv) {

    long            itime, jfreq;   // Loop controls for dummy spectra creation
    size_t          sz_alloc = 0;   // size of data matrix to allocate from the heap
    char            wstr[256];      // sprintf target
    float           *p_data;        // pointer to allocated heap
    float           *wfptr;         // working float pointer
    time_t          time1, time2;   // elapsed time calculation (seconds)

    // Data generation variables.
    float           low = 4.0e9, high = 9.0e9; // Element value boundaries
    float           freq = 8000.0e6;
    float           float_elem;         // Current float_element value
    unsigned long   count_elems;        // Elemount count
    float           amplitude;

    /*
     * Allocate enough heap for the entire data matrix.
     */
    sz_alloc = NTINTS * NIFS * NCHANS * NBITS / 8;
    p_data = malloc(sz_alloc);
    if(p_data == NULL) {
        sprintf(wstr, "main malloc(%ld) FAILED", (long)sz_alloc);
        fatal_error(__LINE__, wstr);
        exit(86);
    }
    printf("simon: Data matrix allocated, size  = %ld\n", (long) sz_alloc);

    /*
     * Make dummy spectra matrix.
     */
    wfptr = (float *) p_data;
    count_elems = 0;
	for(itime = 0; itime < NTINTS; itime++)
        for(jfreq = 0; jfreq < NCHANS; jfreq++) {
            amplitude = rand() * (high - low);
            float_elem = amplitude * sin(freq);
            *wfptr++ = float_elem;
            count_elems++;
        }
    printf("simon: Matrix element count = %ld\n", count_elems);

    /*
     * Create the data.
     */
    int debug_callback = DEBUG_CALLBACK;
    float* h_pwrbuf = p_data;
    size_t h_pwrbuf_size = sz_alloc;
    filterbankc99_header_t fb_hdr;
    fbh5_context_t fbh5_ctx;

    make_voyager_1_metadata(&fb_hdr);
    printf("simon: Callback data ready.\n");

    /*
     * Create/recreate the file and store the metadata.
     */
    time(&time1);
    if(fbh5_open(&fbh5_ctx, &fb_hdr, NTINTS, PATH_H5, debug_callback) != 0) {
        fatal_error(__LINE__, "fbh5_open failed");
        exit(86);
    }

    /*
     * Write data.
     */
    for(int ii = 0; ii < NTINTS; ++ii)
        if(fbh5_write(&fbh5_ctx, &fb_hdr, h_pwrbuf, h_pwrbuf_size, debug_callback) != 0) {
            fatal_error(__LINE__, "fbh5_write failed");
            exit(86);
        }

    /*
     * Close FBH5 session.
     */
    if(fbh5_close(&fbh5_ctx, debug_callback) != 0) {
        fatal_error(__LINE__, "fbh5_close failed");
        exit(86);
    }

    /*
     * Compute elapsed time.
     */
    time(&time2);
    free(p_data);
    printf("simon: End, e.t. = %.2f seconds.\n", difftime(time2, time1));

    /*
     * Bye-bye.
     */
    return 0;
}

