#ifndef TFT_UTILS
#define TFT_UTILS

#include "Arduino.h"
#include <vector>
#include <serial_utils.h>
#include <TFT_22_ILI9225.h>
#include <gfxfont.h>
#include <../fonts/FreeMono9pt7b.h>

using namespace std;

template<class T> using vct = vector<T>;
#ifndef rep
#define rep(i,a,b) for(int i = (a); i <= (b); ++i)
#endif
#ifndef rev
#define rev(i,a,b) for(int i = (b); i >= (a); --i)
#endif
#ifndef revt
#define rept(type, i,a,b) for(type i = (a); i <= (b); ++i)
#endif
#ifndef revt
#define revt(type, i,a,b) for(type i = (b); i >= (a); --i)
#endif

const uint8_t     *FontBitmap = FreeMono9pt7bBitmaps;
const GFXglyph    *FontGlyph  = FreeMono9pt7bGlyphs;




/// >>>>>>>>>>>>>> initializing tft object >>>>>>>>>>>>>>
#define RST_PIN (int8_t) 2
#define RS_PIN  (int8_t) 4
#define CS_PIN  (int8_t) 15
/// @brief The tft object for comunication with tft screen
/// @note  Plase do NOT change <tft> object's name!
/// @note  This object only work if you run <tft.begin()>. 
TFT_22_ILI9225 tft(RST_PIN, RS_PIN, CS_PIN, 0);








/// >>>>>>>>>>>>>>> definition of POINT >>>>>>>>>>>>>>>>

/// @brief The point class, with add, minus method.
template<class Tpoint = uint16_t>
class POINT{
    Tpoint x, y;
public:
    /// @brief Initializing a point with X and Y (default value is 0)
    /// @param X X value
    /// @param Y Y value
    POINT(uint16_t X = 0, uint16_t Y = 0){ x=X, y=Y;}

    /// @return X value of the point.
    uint16_t& X(){return x;}

    /// @return Y value of the point.
    uint16_t& Y(){return y;}

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '=' operation
    POINT operator= (POINT another){
        this->X() = another.X();
        this->Y() = another.Y();
    }

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '+' operation
    POINT operator+ (POINT another){
        this->X() += another.X();
        this->Y() += another.Y();
    }

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '-' operation
    POINT operator+ (POINT another){
        this->X() -= another.X();
        this->Y() -= another.Y();
    }


};


/// >>>>>>>>>>>>>>> definition of IMAGE >>>>>>>>>>>>>>>>
template<class Timage>
class IMAGE{
private:
    /// @brief The image is stored in 1d-vector.
    vct<Timage> _img;
    uint16_t _w, _h;
public:
    
    /// @brief Check the given size is valid or not?
    template<class Tsize>
    bool invalid_size(Tsize h, Tsize w){
        return (h<1 || w<1 );
    }

    /// @brief Check the position is valid or not?
    /// @param X X value in image
    /// @param Y Y value in image
    /// @note X: 0...(h-1) and Y: 0...(w-1)
    template<class Tsize>
    bool invalid_position(Tsize x, Tsize y){
        return (x >= _h || y >= _w || x < 0 || y < 0);
    }

    /// @brief Check the position is valid or not?
    /// @param p The point in the image.
    /// @note X: 0...(h-1) and Y: 0...(w-1)
    template<class Tpoint>
    bool invalid_position(Tpoint p){
        return (p.X() >= _h || p.Y() >= _w);
    }

    /// @brief Resize to the single color image
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    void resize(uint16_t h = 220, uint16_t w = 172, Timage filled_value = 0){
        if(invalid_size(h, w)) return;
        _h = h, _w = w;
        _img.resize(_h * _w, filled_value);
    }

    /// @brief Resize the image to an image construct from 1d-array
    /// @param color_array The pixel-color-array of the image (has been flatten)
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    void resize(Timage* color_array, uint16_t h = 172, uint16_t w = 220){
        _h = h, _w = w;
        _img.clear(); 
        rep(i, 0, h * w - 1) _img.push_back( *(color_array + i) );
    };

    /// @brief Initializing the non-existed image 
    IMAGE(){
        _w = _h = 0,
        _img.clear();
    }

    /// @brief Initialize single color image
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    IMAGE(uint16_t h = 220, uint16_t w = 172, Timage filled_value = 0){
        this->resize(h, w, filled_value);
    };

    /// @brief Initialize image from 1d-array
    /// @param color_array The pixel-color-array of the image (has been flatten)
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    IMAGE(Timage* color_array, uint16_t h = 172, uint16_t w = 220){
        this->resize(color_array, h, w);
    };

    /// @brief Initialize image from 1d-vector
    /// @param color_vector The pixel-color-array of the image (has been flatten)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    IMAGE(vct<Timage> color_vector, uint16_t h = 172, uint16_t w = 220){
        _h = h, _w = w;
        _img = color_vector;
    };

    /// @return The height of image
    uint16_t& H(){return _h;}

    /// @return The width of image
    uint16_t& W(){return _w;}

    /// @return The pixel-color at (x,y) (x: 0...<h-1> and y: 0...<w-1>). 
    Timage& pixel(uint16_t x, uint16_t y){
        return _img[x * _w + y];
    }
    
    /// @return The pixel-color at (x,y) (x: 0...<h-1> and y: 0...<w-1>). 
    template<class Tpoint>
    Timage& pixel(Tpoint p){
        return _img[p.X() * _w + p.Y()];
    }

    /// @return The vector-image
    vct<Timage>& vector_image(){
        return _img;
    }

    /// @brief Return the cropped image
    /// @param top_left The point for top-left point of original image
    /// @param bottom_right The point for bottom-right point of original image
    template<class Tpoint>
    IMAGE crop(Tpoint top_left, Tpoint bottom_right){
        /// 1st row
        vct<Timage> res;
        rep(r, top_left.X(), bottom_right.X()){
            rep(c, top_left.Y(), bottom_right.Y()){
                res.push_back(_img(r * _w + c));
            }
        }
        uint16_t new_h = bottom_right.X() - top_left.X(),
                 new_w = bottom_right.Y() - top_left.Y();
        return IMAGE(res, new_h, new_w);
    }

    /// @brief Add the other image to this image from top-left point.
    /// @param o The other image.
    template<class Tpoint>
    void add(IMAGE o, Tpoint top_left){
    	if(invalid_position(top_left)) return;
        rep(r, 0, o.H() - 1){
            rep(c, 0, o.W() - 1){
                if(invalid_position(top_left.X() + r, top_left.Y() + c)){
                    break;
                }else{
                    pixel(top_left.X() + r, top_left.Y() + c) += o.pixel(r, c);
                }
            }
        }
    }

    /// @brief Set pixel at <pos> on the image
    /// @param pos The position on the image (unit: pixel)
    /// @param color The color will be drawed on the image (16bit-color) 
    template<class Tpoint>
    void set_pixel(POINT<Tpoint> pos, Timage color){
        this->pixel(pos) = color;
    }

    /// @brief Assign an image to this image
    IMAGE operator= (IMAGE o){
        this->vector_image() = o.vector_image();
        this->H() = o.H();
        this->W() = o.W();
        return o;        
    }

};








/// >>>>>>>>>>>>>>> definition of canvas >>>>>>>>>>>>>>>>

/// @brief The default orientation of the screen
#define ORIENTATION 1

/// @brief A canvas and some methods
/// @tparam Tcanvas typef of canvasm default is uint16_t
template<class Tcanvas = uint16_t>
class CANVAS{
private:
    bool tft_initialized;
    POINT<uint16_t> point;
    IMAGE<Tcanvas> _canvas_old;
    IMAGE<Tcanvas> _canvas;
public:



    /// @brief Initializing a CANVAS object
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    CAMVAS(uint16_t h = 220, uint16_t w = 172, Timage filled_value = 0){
        /// Start tft screen comunication
        tft.begin();    tft_initialized = true;
        

        _canvas_old.resize(h, w);
        _canvas.resize(h, w);
    }

    /// @brief Show in screen
    void show(){
        if(!tft_initialized){
            tft.begin();    tft_initialized = true;
        }
        for(int8_t r = 0; r < _canvas.H(); ++r){
            for(int8_t c = 0; c < _canas.W(); ++r){
                if(_canvas.pixel(r, c) == _canvas_old(r, c)) continue;
                tft.drawPixel(_canas.pixel(r, c));
            }
        }
        _canvas_old = _canvas;
    }

    /// @brief Set pixel at <pos> on screen
    /// @param pos The position on the screen(unit: pixel) 
    /// @param color The color of the text (16bit-color) 
    void set_pixel(POINT pos, uint16_t color){
        _canvas.
    }

    /// @brief Add text
    /// @param pos The position on the screen(unit: pixel) 
    /// @param text The text to be inserted. 
    /// @param color The color of the text (16bit-color) 
    void insert_text(POINT pos, String text, uint16_t color = 0xFFFF){
        
        uint16_t char_index = text.charAt(i) - ' ';
        uint8_t bit_count  = 0;

        rept(uint8_t, r, 0, FontGlyph[char_index].height-1){
            rept(uint8_t, r, 0, FontGlyph[char_index].width-1){
                _canvas.set_pixel(pos + POINT<uint16_t>({r, c}), color);
            }
        }

        rept(uint8_t, i, 0, text.size()-1){
            FontGlyphs[text.charAt(i)-' ']
        }
    }

};





/// >>>>>>>>>>>>>>> dmake a canvas object >>>>>>>>>>>>>>>>

/// @brief The canvas object
CANVAS<uint16_t> canvas;






#endif