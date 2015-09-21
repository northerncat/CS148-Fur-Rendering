#ifndef _SIMPLEIMAGE_H_
#define _SIMPLEIMAGE_H_ 

#include <string>
#include <iostream>
#include <assert.h>

/***
 * An image class, adapted from libST's STImage 
 * The class stores colors in rgba form and can load and write PNG files
 * Colors are stored as rgba floats with values between 0 and 1
 *
 * Individual pixel values can be accessed by calling I(x,y)
 */

using namespace std;

class RGBColor
{
public:
    float r, g, b;

    RGBColor(float red, float green, float blue):
        r(red),
        g(green),
        b(blue)
    {}

    RGBColor():
        r(0),
        g(0),
        b(0)
    {}

    RGBColor operator+(RGBColor const o) { return RGBColor(r+o.r, g+o.g, b+o.b); }
    RGBColor operator-(RGBColor const o) { return RGBColor(r-o.r,g-o.g, b-o.b); }
    RGBColor operator*(float s) { return RGBColor(r*s, g*s, b*s); }
};


class SimpleImage{
public:
	SimpleImage(void);
	~SimpleImage(void);

	SimpleImage(const SimpleImage& img);
    SimpleImage(int width, int height, RGBColor &bg);
    SimpleImage(int width, int height, unsigned char* data);
	SimpleImage(string filename);

	void Initialize(int width, int height);

	//Pixel accessors
	RGBColor& operator() (int x, int y) {
        assert(in_bounds(x,y));
        return _data[_width*y+x];
	}

	RGBColor operator() (int x, int y) const{
        assert(in_bounds(x,y));
        return _data[_width*y+x];
	}

    // Pixel setter
    void setPixel(int x, int y, RGBColor c){
        if (! in_bounds (x,y) ) {
            std::cerr<<"warning, attempting to set pixel beyond image bounds: ("
                    << x << ", " << y <<"). Ignoring." << std::endl;
            return;
        }
        _data[_width*y + x] = c;
    }

	//file loading and saving
	void LoadPNG(string filename);
	bool WritePNG(string filename);


	//property accessors
	bool Empty() const;

	int GetWidth() const{
		return _width;
    }

	int GetHeight() const{
		return _height;
	}

    // be careful with  pointers, kids!
    RGBColor* GetData() const{
        return _data;
    }


private:
	RGBColor* _data;
	int _width;
	int _height;

    bool in_bounds(int x, int y) const { return x>=0 && x<_width && y>=0 &&y<_height; }
};


#endif
