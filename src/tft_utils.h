#ifndef TFT_UTILS
#define TFT_UTILS

#include "Arduino.h"
#include <vector>
#include <serial_utils.h>
#include <mod/TFT_22_ILI9225_MOD.h>
#include <gfxfont.h>
#include <../fonts/FreeMono9pt7b.h>

using namespace std;

#define TFT_SCREEN

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

/// >>>>>>>>>>> initializing font properties >>>>>>>>>>>>
const uint8_t     *FontBitmap = FreeMono9pt7bBitmaps;
const GFXglyph    *FontGlyph  = FreeMono9pt7bGlyphs;

#ifdef TFT_SCREEN
    /// >>>>>>>>>>>>>> initializing tft object >>>>>>>>>>>>>>
    #define RST_PIN (int8_t) 2
    #define RS_PIN  (int8_t) 4
    /// @brief The tft object for comunication with tft screen
    /// @note  Plase do NOT change <tft> object's name!
    /// @note  This object only work if you run <tft.begin()>. 
    TFT_22_ILI9225_MOD tft(RST_PIN, RS_PIN);

#endif

/// >>>>>>>>>>>>>>> definition of POINT >>>>>>>>>>>>>>>>

/// @brief The point class, with add, minus method.
template<class Tpoint = uint16_t>
class POINT{
    Tpoint x, y;
public:
    /// @brief Initializing a point with X and Y (default value is 0)
    /// @param X X value
    /// @param Y Y value
    POINT<Tpoint>(Tpoint X = 0, Tpoint Y = 0){ x=X, y=Y;}

    /// @return X value of the point.
    Tpoint& X(){return x;}

    /// @return Y value of the point.
    Tpoint& Y(){return y;}

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '=' operation
    POINT<Tpoint> operator= (POINT<Tpoint> another){
        this->X() = another.X();
        this->Y() = another.Y();
        return another;
    }

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '=+' operation
    POINT<Tpoint> operator+= (POINT<Tpoint> another){
        this->X() += another.X();
        this->Y() += another.Y();
        return *this;
    }

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '=-' operation
    POINT<Tpoint> operator-= (POINT<Tpoint> another){
        this->X() -= another.X();
        this->Y() -= another.Y();
        return *this;
    }


    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '+' operation
    POINT<Tpoint> operator+ (POINT<Tpoint> another){
        return POINT<Tpoint>(
            this->X() + another.X(),
            this->Y() + another.Y()
        );
    }

    /// @param POINT's object to be operated
    /// @return POINT's object - the result of '-' operation
    POINT<Tpoint> operator- (POINT<Tpoint> another){
        return POINT<Tpoint>(
        this->X() - another.X(),
        this->Y() - another.Y()
        );
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
    IMAGE(uint16_t h, uint16_t w, Timage filled_value = 0){
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
    IMAGE<Timage> crop(POINT<uint16_t> top_left, POINT<uint16_t> bottom_right){
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
    void add(IMAGE<Timage> o, POINT<uint16_t> top_left){
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
    void set_pixel(POINT<uint16_t> pos, Timage color){
        this->pixel(pos) = color;
    }

    /// @brief Assign an image to this image
    IMAGE<Timage> operator= (IMAGE o){
        this->vector_image() = o.vector_image();
        this->H() = o.H();
        this->W() = o.W();
        return o;        
    }

    /// @brief Assign an image to this image
    bool operator== (IMAGE o){
        return bool(
            this->H() == o.H() &&
            this->W() == o.W()
        );
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
    /// @brief Initializing a empty CANVAS object
    CANVAS(){
        
    };

    /// @brief Resize a CANVAS object
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    void resize(uint16_t h, uint16_t w, Tcanvas filled_value = 0)
    {
        _canvas_old.resize(h, w, filled_value);
        _canvas.resize(h, w, filled_value);
    }

    /// @brief Show in screen
    void show(){
        // msg2ser("_show()");
        for(uint16_t r = 0; r < _canvas.H(); ++r){
            for(uint16_t c = 0; c < _canvas.W(); ++c){
                // msg2ser("r: ", r, " c: ", c);
                if(_canvas.pixel(r, c) == _canvas_old.pixel(r, c)) 
                    continue;
                else{
                    tft.drawPixel(r, c, _canvas.pixel(r, c));
                    msg2ser("?", _canvas.pixel(r, c));
                }
            }
        }
        // _canvas_old = _canvas;
    }

    /// @brief Set pixel at <pos> on screen
    /// @param pos The position on the screen(unit: pixel) 
    /// @param color The color of the text (16bit-color) 
    void set_pixel(POINT<uint16_t> pos, uint16_t color){
        _canvas.set_pixel(point, color);
    }

    /// @return the height of the canvas
    uint16_t H(){
        return _canvas.H();
    }

    /// @return the widht of the canvas
    uint16_t W(){
        return _canvas.W();
    }

    /// @brief Add text
    /// @param pos The position on the screen(unit: pixel) 
    /// @param text The text to be inserted. 
    /// @param color The color of the text (16bit-color) 
    void insert_text( POINT<uint16_t> pos, String text, uint16_t color = 0xFFFF){
        /// drawed bits
        uint8_t     dbits;
        /// the bitmap byte of the character in FontBitmap
        uint8_t     bm_byte;
        /// byte-bitmap position in FontBitmap
        uint16_t    bm_byte_pos;
        /// the oder of character in FontGlyph 
        uint16_t    char_index;

        /// draw pointer (the index of draw-point in canvas, 
        /// it starts from UL corner of the canvas)
        POINT<uint16_t> draw_pos;
        /// process each character in ``text``
        rept(uint8_t, i, 0, text.length()-1){
            /// get the oder of character in FontGlyph 
            char_index = text.charAt(i) - ' ';
            /// reset dbits to ZERO
            dbits  = 0;
            /// get bm_byte_pos 
            bm_byte_pos = FontGlyph[char_index].bitmapOffset;
            /// get bitmap_byte in bitmap
            bm_byte = FontBitmap[bm_byte_pos];

            /// Place character in canvas
            rept(uint16_t, r, 0, FontGlyph[char_index].height){
                rept(uint16_t, c, 0, FontGlyph[char_index].width){
                    /// update draw-pos
                    draw_pos.X() = pos.X() + FontGlyph[char_index].xOffset + r;  
                    draw_pos.Y() = pos.Y() + FontGlyph[char_index].xOffset + c;  
                    /// if the pixel is not 
                    if( (bm_byte & 0x80) != 0 ){
                        _canvas.pixel(draw_pos) = color;
                        ++dbits;
                    }

                    if(dbits > 0 && dbits == 8){
                        // after shifted-left 8bits, move to next byte!
                        ++bm_byte_pos;
                    }else{
                        /// shift-left to removed drawed bit
                        (bm_byte) <<= 1;
                    }
                }
            }

            /// move pointer to next position (for new character)
            pos.X() += FontGlyph[char_index].xAdvance;
            /// out of canvas :v
            if(_canvas.invalid_position(pos)) break;
        }
    }

};


/// >>>>>>>>>>>>>>> dmake a canvas object >>>>>>>>>>>>>>>>






#endif