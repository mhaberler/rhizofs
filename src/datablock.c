#include "datablock.h"

#include "dbg.h"
#include "lz4.h"

// prototypes
static int get_lz4_compressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, int do_alloc);
static int set_lz4_compressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, const size_t len);
static int get_uncompressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, int do_alloc);



Rhizofs__DataBlock *
DataBlock_create()
{
    Rhizofs__DataBlock * datablock = NULL;

    datablock = calloc(sizeof(Rhizofs__DataBlock), 1);
    check_mem(datablock);
    rhizofs__data_block__init(datablock);
    datablock->data.data = NULL;
    datablock->data.len = 0;

    return datablock;

error:
    free(datablock);
    return NULL;
}


void
DataBlock_destroy(Rhizofs__DataBlock * dblk)
{
    if (dblk != NULL) {
        if (dblk->data.data != NULL) {
            free(dblk->data.data);
            dblk->data.data = NULL;
        }
        free(dblk);
    }
    dblk = NULL;
}

int
DataBlock_set_data(Rhizofs__DataBlock * dblk, uint8_t * data, 
        size_t len, Rhizofs__CompressionType compression)
{
    check((dblk != NULL), "passed datablock is null");
    check((len != 0), "passed length of datablock is 0");

    switch (compression) {
        case RHIZOFS__COMPRESSION_TYPE__COMPR_NONE:
            dblk->data.len = len;
            dblk->data.data = data;
            dblk->compression = compression;
            break;

        case RHIZOFS__COMPRESSION_TYPE__COMPR_LZ4:
            {
                if (set_lz4_compressed_data(dblk, data, len) != -1) {
                    dblk->compression = compression;
                }
                else {
                    debug("could not lz4 compress - fallback to use no compression");
                    dblk->data.len = len;
                    dblk->data.data = data;
                    dblk->compression = RHIZOFS__COMPRESSION_TYPE__COMPR_NONE;
                }
            }
            break;

        default:
            log_and_error("Unsupported compression type %d", compression);
    }

    dblk->size = len;
    return 0;

error:

    if (data) {
        free(data);
        data=NULL;
    }

    if (dblk) {
        dblk->data.data = NULL;
        dblk->data.len = 0;
    }
    return -1;
}


int
DataBlock_get_data(Rhizofs__DataBlock * dblk, uint8_t * data)
{
    int len = 0;

    check((dblk != NULL), "passed datablock is null");

    switch (dblk->compression) {
        case RHIZOFS__COMPRESSION_TYPE__COMPR_NONE:
            {
                len = get_uncompressed_data(dblk, data, 1);
            }
            break;

        case RHIZOFS__COMPRESSION_TYPE__COMPR_LZ4:
            {
                len = get_lz4_compressed_data(dblk, data, 1);
            }
            break;

        default:
            log_and_error("Unsupported compression type %d", dblk->compression);
    }

    check((len > -1), "could not copy uncompressed data");

    return len;

error:
    if (data != NULL) {
        free(data);
        data = NULL;
    }
    return -1;
}


int
DataBlock_get_data_noalloc(Rhizofs__DataBlock * dblk, uint8_t * data, size_t data_len)
{
    int len = 0;

    check((dblk != NULL), "passed datablock is null");
    check((data != NULL), "passed data buffer is null");
    check((data_len >= (size_t)dblk->size), "passed data buffer is too small "
        "for contents of datablock"
        "(length of data=%ld; buffer size=%ld)", dblk->size, data_len);

    switch (dblk->compression) {
        case RHIZOFS__COMPRESSION_TYPE__COMPR_NONE:
            {
                len = get_uncompressed_data(dblk, data, 0);
            }
            break;

        case RHIZOFS__COMPRESSION_TYPE__COMPR_LZ4:
            {
                len = get_lz4_compressed_data(dblk, data, 0);
            }
            break;

        default:
            log_and_error("Unsupported compression type %d", dblk->compression);
    }

    check((len > -1), "could not copy uncompressed data");

    return len;

error:
    return -1;
}


/*** uncompressed  *************************************/

/**
 * get data from a datablock
 *
 * do_alloc indicates if the buffer "data" should be allocated by this function
 * or already comes preallocated. if it is preallocated a size of at least dblk->size
 * is asumed.
 *
 * returns length of data or -1 on failure
 */
static int
get_uncompressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, int do_alloc)
{
    size_t len = dblk->data.len;

    check((dblk != NULL), "passed datablock is null");

    if (do_alloc) {
        data = calloc(sizeof(uint8_t), len);
    }
    check((data != NULL), "data buffer is null");
    memcpy(data, dblk->data.data, len);

    return len;

error:
    return -1;
}



/*** LZ4 Compression *************************************/

/**
 * compress the given data into the datablock
 *
 * returns 0 on success and -1 on failure
 **/
int
set_lz4_compressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, const size_t len)
{
    size_t bytes_compressed = 0;

    check((dblk != NULL), "passed datablock is null");
    check((data != NULL), "passed data is null");

    dblk->data.data = calloc(sizeof(uint8_t), len); /// TODO: GETS LOST -- already allocated???
    check_mem(dblk->data.data);

    bytes_compressed = LZ4_compress((const char*)data, (char*)dblk->data.data, len);
    check_debug((bytes_compressed != 0), "LZ4_compress failed");

    dblk->data.len = bytes_compressed;

    free(data);
    data = NULL;

    return bytes_compressed;

error:
    if (dblk != NULL) {
        free(dblk->data.data);
        dblk->data.data = NULL;
        dblk->data.len = 0;
    }
    return -1;
}

/**
 * get the uncompressed data from a lz4 compressed datablock
 *
 * see  get_uncompressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, int do_alloc)
 */
static int
get_lz4_compressed_data(Rhizofs__DataBlock * dblk, uint8_t * data, int do_alloc)
{
    size_t len = dblk->size;
    int bytes_uncompressed = 0;

    check((dblk != NULL), "passed datablock is null");

    if (do_alloc) {
        data = calloc(sizeof(uint8_t), len);
    }
    check((data != NULL), "data buffer is null");

    bytes_uncompressed = LZ4_uncompress((const char*)dblk->data.data,
                (char*)data, len);
    check((bytes_uncompressed >= 0), "LZ4_uncompress failed");
    check((dblk->data.len == bytes_uncompressed), "could not decompress the whole block "
                "(only %d bytes of %ld bytes)", bytes_uncompressed, dblk->data.len);

    return len;

error:
    return -1;
}
