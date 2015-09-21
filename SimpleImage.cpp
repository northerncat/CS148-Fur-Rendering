#include "SimpleImage.h"
//!!#include <libpng/png.h>
#include <png.h>
#include <stdexcept>

SimpleImage::SimpleImage(void):
    _width(0),
    _height(0)
{
    _data = new RGBColor[1]; //just initialize the pointer to something
}

SimpleImage::SimpleImage(int width, int height, RGBColor &bg):
    _width(width),
    _height(height)
{
    _data = new RGBColor[_width*_height];

    for (int i = 0; i < _width; i++){
        for (int j = 0; j < _height; j++){           
            _data[j*_width + i] = bg;
        }
    }
}

SimpleImage::SimpleImage(const SimpleImage& img){
    _width = img.GetWidth();
    _height = img.GetHeight();
    _data = new RGBColor[_width*_height];

    for (int i=0; i<_width; i++){
        for (int j=0; j<_height; j++){
            _data[j*_width+i] = img(i,j);
        }
    }
}

SimpleImage::SimpleImage(string filename){
    //just set rgb color to something
    _data = new RGBColor[1];

    LoadPNG(filename);
}

SimpleImage::SimpleImage(int width, int height, unsigned char* data)
{
    if (width <= 0)
        throw std::runtime_error("SimpleImage width must be positive");
    if (height <= 0)
        throw std::runtime_error("SimpleImage height must be positive");

    _width = width;
    _height = height;
    _data = new RGBColor[width*height];
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            int idx = 4*(width*j+i);
            int idbx2 = width*j+i;
            _data[idbx2] = RGBColor(data[idx]/255.f,data[idx+1]/255.f, data[idx+2]/255.f);
        }
    }
}


SimpleImage::~SimpleImage(void)
{
    delete[] _data;
}

bool SimpleImage::Empty() const
{
    return _width==0 || _height == 0;
}

void SimpleImage::Initialize(int width, int height)
{
    if (width <= 0)
        throw std::runtime_error("SimpleImage width must be positive");
    if (height <= 0)
        throw std::runtime_error("SimpleImage height must be positive");

    _width = width;
    _height = height;

    int numPixels = _width * _height;

    //delete previous data
    delete[] _data;

    _data = new RGBColor[numPixels];

}

void SimpleImage::LoadPNG(string filename)
{
    //modified from STImage in libST (cs148 depr. codebase)
    FILE* imgFile = fopen(filename.c_str(), "rb");
    if (!imgFile) {
        fprintf(stderr, "SimpleImage::LoadPNG() - Could not open '%s'.\n",
                filename.c_str());
        throw std::runtime_error("Error in LoadPNG");
    }

    // Read the first 8 bytes from the file and validate that the
    // file is a valid png file
    char pngHeader[8];
    fread(pngHeader, 1, 8, imgFile);

    if (png_sig_cmp( (png_byte*)pngHeader, 0, 8)) {
        fprintf(stderr, "SimpleImage::LoadPNG() - Could not open '%s'. "
                "Unexpected format for png file.\n", filename.c_str());
        fclose(imgFile);
        throw std::runtime_error("Error in LoadPNG");
    }

    // The following code initializes the png struct, which serves as an opaque handle
    // to the png image, and the png info struct, which stores meta-information about the
    // image file that can be queried once the data is read

    png_structp pngPtr;
    png_infop infoPtr;
    
    // main png struct (opaque handle)
    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    
    if (!pngPtr) {
        fprintf(stderr, "SimpleImage::LoadPNG() - Error reading '%s'.\n",
                filename.c_str());
        fclose(imgFile);
        throw std::runtime_error("Error in LoadPNG");
    }

    // info struct (user accessible)
    infoPtr = png_create_info_struct(pngPtr);

    if (!infoPtr) {
        fprintf(stderr, "SimpleImage::LoadPNG() - Error reading '%s'.\n",
                filename.c_str());
        png_destroy_read_struct(&pngPtr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(imgFile);
        throw std::runtime_error("Error in LoadPNG");
    }

    // jump pointer.  Failure during read inside libpng will cause 
    // control to jump here (simple fail out)
    if (setjmp(png_jmpbuf(pngPtr))) {
        fprintf(stderr, "SimpleImage::LoadPNG() - Error reading '%s'.\n",
                filename.c_str());
        png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)NULL);
        fclose(imgFile);
        throw std::runtime_error("Error in LoadPNG");
    }

    png_init_io(pngPtr, imgFile);
    png_set_sig_bytes(pngPtr, 8);

    // The following code used the libpng high level interface
    // to read data from the file into the array of RGBColor structs
    // representing an image in this class.  Format conversion into 8-bit
    // RGBA as currently required by this class, requires an additional
    // buffer copy.  Using the libpng **low-level interface** this buffer
    // copy and the associated buffer double allocation could be avoided.
    // The code would be complicated slightly with additional libpng
    // transform calls, so I will leave this for later if performance or
    // memory footprint becomes an issue.

    // These transforms get us very close to 8 bit per channel RGBA, but
    // not quite (won't extend from 1 or 3 components to 4.  Must use low
    // level interface to do this.
    int transforms =            
            PNG_TRANSFORM_PACKING |
            PNG_TRANSFORM_PACKSWAP |
            PNG_TRANSFORM_EXPAND |
            PNG_TRANSFORM_SHIFT;

    
    // Tell libpng to load the image into memory
    png_read_png(pngPtr, infoPtr, transforms, NULL);

    
    // libpng exposes the data as an array of arrays of bytes
    png_bytepp pngRowPointers = png_get_rows(pngPtr, infoPtr);

    int width = png_get_image_width(pngPtr, infoPtr);
    int height = png_get_image_height(pngPtr, infoPtr);
    Initialize(width, height);
    RGBColor* pixels = _data;

    int numChannels = png_get_channels(pngPtr, infoPtr);

    // Now copy the data read by libpng into the SimpleImage class'
    // contiguous allocation of pixel data.  In doing so, explode
    // single channel images or images without an alpha channle
    // into full RGBColor pixels.  
    //
    // We also need to flip the order of the rows of data.  Data in
    // the png file begins with the topmost row of the image.  The
    // SimpleImage class stores data bottom row first to be consistent
    // with OpenGL pixel formats
    for (int i = 0; i < height; ++i) {

        png_bytep srcPtr = pngRowPointers[height-i-1]; 
        RGBColor* curPixel = &pixels[ i*width ];

        for (int j = 0; j < width; ++j) {

            switch (numChannels) {

            // monochrome, no alpha
            case 1:
                curPixel->r = curPixel->g = curPixel->b = (*srcPtr++)/255.f;
                break;

            // monochrome, with alpha
            case 2:
                curPixel->r = curPixel->g = curPixel->b = (*srcPtr++)/255.f;
                srcPtr++; //ignore alpha
                break;

            // RGBColor, no alpha
            case 3:
                curPixel->r = (*srcPtr++)/255.f;
                curPixel->g = (*srcPtr++)/255.f;
                curPixel->b = (*srcPtr++)/255.f;
                break;
                
            // RGBColor
            case 4:
                curPixel->r = (*srcPtr++)/255.f;
                curPixel->g = (*srcPtr++)/255.f;
                curPixel->b = (*srcPtr++)/255.f;
                
                //ignore alpha
                srcPtr++;
                break;
            }

            curPixel++;
        }
    }


    // Clean up libpng.
    png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)NULL);
    fclose(imgFile);

}

bool SimpleImage::WritePNG(string filename)
{
    FILE* imgFile = fopen(filename.c_str(), "wb");
    
    if (!imgFile) {
        fprintf(stderr, "SimpleImage::WritePNG() - Could not open '%s'.\n",
                filename.c_str());
        return false;
    }
    
    png_structp pngPtr;
    png_infop infoPtr;

    // main png struct (opaque handle)
    pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    
    if (!pngPtr) {
        fclose(imgFile);
        return false;
    }

    // info struct (user accessible)
    infoPtr = png_create_info_struct(pngPtr);

    if (!infoPtr) {
        png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
        fclose(imgFile);
        return false;
    }

    // jump pointer.  Failure during read inside libpng will cause 
    // control to jump here (simple fail out)
    if (setjmp(png_jmpbuf(pngPtr))) {
        fprintf(stderr, "Could not write '%s'.  Internal error in libpng.\n", filename.c_str());
        png_destroy_write_struct(&pngPtr, &infoPtr);
        fclose(imgFile);
        return false;
    }

    //convert array of float RGBColor into unsigned bytes
    unsigned char* bytedata = new unsigned char[4*_width*_height];
    for (int i=0; i<_width; i++)
    {
        for (int j=0; j<_height; j++)
        {
            int idx = (j*_width)+i;
            int dest = 4*(j*_width + i);
            RGBColor temp = _data[idx]*255.f;
            bytedata[dest] = temp.r;
            bytedata[dest+1] = temp.g;
            bytedata[dest+2] = temp.b;
            bytedata[dest+3] = 255;
        }
    }

    png_init_io(pngPtr, imgFile);

    png_set_IHDR(pngPtr, infoPtr, _width, _height, 8,
        PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    // allocate a set of row pointers that points directly into the existing
    // array of RGBColor
    png_bytepp rowPointers = (png_bytepp)png_malloc( pngPtr, sizeof(png_bytep) * _height );

    for (int i=0; i<_height; i++)
        rowPointers[i] = (png_bytep)(bytedata + (_height-i-1)*_width*4);

    png_set_rows(pngPtr, infoPtr, rowPointers);

    // write the image data to the specified file
    png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

    png_free( pngPtr, rowPointers ) ;

    // cleanup
    png_write_end(pngPtr, NULL);
    png_destroy_write_struct( &pngPtr, &infoPtr) ;
    fclose(imgFile);

    delete[] bytedata;

    return true;

}
