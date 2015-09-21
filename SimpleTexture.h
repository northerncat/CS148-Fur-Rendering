// SimpleTexture.h
#ifndef __SIMPLE_TEXTURE_H__
#define __SIMPLE_TEXTURE_H__

#include "stgl.h"
#include "SimpleImage.h"

/**
* The SimpleTexture class allows use of an SimpleImage as an OpenGL texture.
* In the simplest case, just construct a new SimpleTexture with an image:
*
*   SimpleTexture* texture = new SimpleTexture(image);
*
* You can also change the image used by an SimpleTexture at any time by
* using the LoadImageData() method:
*
*   texture->LoadImageData(someOtherImage);
*
* To use an SimpleTexture for OpenGL rendering, you should call Bind()
* before rendering with the texture, and UnBind() after you are done.
*
*   texture->Bind();
*   // do OpenGL rendering
*   texture->UnBind();
*
* You must remember not to create any SimpleTextures until you have
* initialized OpenGL.
*/
class SimpleTexture
{
public:
    //
    // Options when loading an image to an SimpleTexture. Use the
    // kGenerateMipmaps option to use OpenGL to generate mipmaps -
    // downsampled images used to improve the quality of texture
    // filtering.
    //
    enum ImageOptions {
        kNone = 0,
        kGenerateMipmaps = 0x1,
    };

    //
    // Create a new SimpleTexture using the pixel data from the given
    // image (which will be copied in). Use the options to specify
    // whether mipmaps should be generated.
    //
    SimpleTexture(const SimpleImage* image,
              ImageOptions options = kGenerateMipmaps);
	SimpleTexture(const float* pixels, int width, int height,
		ImageOptions options = kGenerateMipmaps);

    //
    // Create an "empty" SimpleTexture with no image data. You will need
    // to load an image before you can use this texture for
    // rendering.
    //
    SimpleTexture();

    //
    // Delete an existing SimpleTexture.
    //
    ~SimpleTexture();

    //
    // Load image data into the SimpleTexture. The texture will be
    // resized to match the image as needed. Use the options
    // to specify whether mipmaps should be generated.
    //
    void LoadImageData(const SimpleImage* image,
                       ImageOptions options = kGenerateMipmaps);
	void LoadImageData(const float* pixels, int width, int height,
		ImageOptions options = kGenerateMipmaps);

    //
    // Bind this texture for use in subsequent OpenGL drawing.
    //
    void Bind();

    //
    // Un-bind this texture and return to untextured drawing.
    //
    void UnBind();

    //
    // Set the OpenGL texture-filtering mode to use for texture
    // magnification and minification respectively.
    // For example:
    //      SetFilter(GL_LINEAR,
    //                GL_LINEAR_MIPMAP_LINEAR);
    //
    void SetFilter(GLint magFilter, GLint minFilter);

    //
    // Set the OpenGL mode to use for texture addressing in
    // the S and T dimensions respectively.
    // For example:
    //      SetWrap(GL_REPEAT, GL_REPEAT);
    //
    void SetWrap(GLint wrapS, GLint wrapT);

    //
    // Get the width (in pixels) of the image.
    //
    int GetWidth() const { return mWidth; }

    //
    // Get the height (in pixels) of the image.
    //
	int GetHeight() const { return mHeight; }

	// Common initialization code, used by all constructors.
	void Initialize();

private:

    // The OpenGL texture id.
    GLuint mTexId;

    // The width and height of the image data.
    int mWidth;
    int mHeight;
};

#endif // __SIMPLE_TEXTURE_H__
