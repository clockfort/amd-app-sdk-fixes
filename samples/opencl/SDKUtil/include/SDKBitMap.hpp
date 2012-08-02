#ifndef SDKBITMAP_H_
#define SDKBITMAP_H_

/**
 * Headers
 */
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <SDKCommon.hpp>

/**
 * Namespace streamsdk
 */
namespace streamsdk
{

/**
 * fixme this needs to be moved to common types header?
 */
#pragma pack(push,1)

/**
 * uchar4
 * struct implements a vector of chars
 */
typedef struct
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char w;
} uchar4;

/**
 * ColorPelette of type uchar4
 */
typedef uchar4 ColorPalette;

/**
 * struct Bitmap header
 */
typedef struct {
    short id;
    int size;
    short reserved1;
    short reserved2;
    int offset;
} BitMapHeader;


/**
 * struct Bitmap info header
 */
typedef struct {
    int sizeInfo;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    unsigned compression;
    unsigned imageSize;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;
} BitMapInfoHeader;

/**
 *class Bitmap used to load a bitmap image from a file.
 */
class SDKBitMap : public BitMapHeader, public BitMapInfoHeader
{
private:
    uchar4 * pixels_;				/**< Pixel Data */
    int numColors_;					/**< Number of colors */
    ColorPalette * colors_;			/**< Color Data */
    bool isLoaded_;					/**< If Bitmap loaded */
    void releaseResources(void);	/**< Release Resources */
    int colorIndex(uchar4 color);	/**< get a color index */
public:

    /**
	 * brief Default constructor
	 */
    SDKBitMap()
        : pixels_(NULL),
          numColors_(0),
          colors_(NULL),
          isLoaded_(false)
    {}

    /**
	 * brief Constructor
     * Tries to load bitmap image from filename provided.
     *
     * @param filename pointer to null terminated string that is the path and
     * filename to the bitmap image to be loaded.
     *
     * In the case of an error, e.g. the bitmap file could not be loaded for
     * some reason, then a following call to isLoaded will return false.
     */
    SDKBitMap(const char * filename)
        : pixels_(NULL),
          numColors_(0),
          colors_(NULL),
          isLoaded_(false)
    {
        load(filename);
    }

    /**
	 * Copy constructor
     *
     * @param rhs is the bitmap to be copied (cloned).
     */
    SDKBitMap(const SDKBitMap& rhs)
    {
        *this = rhs;
    }

    /** 
	 * Destructor
	 */
    ~SDKBitMap()
    {
        releaseResources();
    }

    /**
	 * Assignment operator
     * @param rhs is the bitmap to be assigned (cloned).
     */
    SDKBitMap& operator=(const SDKBitMap& rhs);

    /**
	 * Load Bitmap image
     *
     * @param filename is a pointer to a null terminated string that is the
     * path and filename name to the the bitmap file to be loaded.
     *
     * @return In the case of an error, e.g. the bitmap file could not be loaded for
     * some reason, then a following call to isLoaded will return false.
     */
    void
    load(const char * filename);

    /**
	 * Write Bitmap image
     *
     * @param filename is a pointer to a null terminated string that is the
     * path and filename name to the the bitmap file to be written.
     *
     * @return In the case that the bitmap is written true is returned. In
     * the case that a bitmap image is not already loaded or the write fails
     * for some reason false is returned.
     */
    bool
    write(const char * filename);

    bool 
    write(const char * filename, int width, int height, unsigned int *ptr);

    /**
	 * Get image width
     *
     * @return If a bitmap image has been successfully loaded, then the width
     * image is returned, otherwise -1;
     */
    int
    getWidth(void) const
    {
        if (isLoaded_) {
            return width;
        }
        else {
            return -1;
        }
    }

    
	 /**
	  * Get Number of Channels
      *
      * @return the number of channels used in image, otherwise -1.
      */
	int getNumChannels();

    /**
	 * Get image height
     *
     * @return If a bitmap image has been successfully loaded, then the height
     * image is returned, otherwise -1.
     */
    int
    getHeight(void) const
    {
        if (isLoaded_) {
            return height;
        }
        else {
            return -1;
        }
    }

    /**
	 * Get image width
     *
     * @return If a bitmap image has been successfully loaded, then returns
     * a pointer to image's pixels, otherwise NULL.
     */
    uchar4 * getPixels(void) const { return pixels_; }

    /**
	 * Is an image currently loaded
     *
     * @return If a bitmap image has been successfully loaded, then returns
     * true, otherwise if an image could not be loaded or an image has yet
     * to be loaded false is returned.
     */
    bool
    isLoaded(void) const { return isLoaded_; }

};
}
#pragma pack(pop)
#endif //CL_BITMAP
