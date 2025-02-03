#ifndef TFT_UTILS
#define TFT_UTILS

#include <vector>
#include <algorithm>
#include <Arduino.h>
#include <serial_utils.h>
#include <mod_libs/TFT_22_ILI9225_MOD.h>

using namespace std;

#ifndef TFT_SCREEN
    #define TFT_SCREEN true
#endif

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
#define revt(type, i,b, a) for(type i = (b); i >= (a); --i)
#endif

/// >>>>>>>>>>> initializing font properties >>>>>>>>>>>>
/// @note CUSTOM_FONT is a macro defined in <mod/TFT_22_ILI9225_MOD.h>

const uint8_t     *FontBitmap = CUSTOM_FONT.bitmap;
const GFXglyph    *FontGlyph  = CUSTOM_FONT.glyph;

#ifdef TFT_SCREEN
    /// >>>>>>>>>>>>>> initializing tft object >>>>>>>>>>>>>>
    /// @brief The tft object for comunication with tft screen
    /// @note  Plase do NOT change <tft> object's name!
    /// @note  This object only work if you run <tft.begin()>. 
    TFT_22_ILI9225_MOD tft;

#endif

/// >>>>>>>>>>>>>>> definition of POINT >>>>>>>>>>>>>>>>

/// @brief The point class, with add, minus method.
/// @note  X and y in this header file is different with  x and y in FontGlyph and 
/// tft screen. While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
/// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
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

    void swap(POINT<Tpoint> &another){
        Tpoint tmpX, tmpY;
        
        tmpX = this->X();
        tmpY = this->Y();
        
        this->X() = another.X();
        this->Y() = another.Y();

        another.X() = tmpX;
        another.Y() = tmpY;
    }
};


/// >>>>>>>>>>>>>>> definition of IMAGE >>>>>>>>>>>>>>>>
/// @brief IMAEG class to provides some method to process the image
/// (non-parallel computing). The image in this class is stored in
/// a vector image.
/// @note  X and y in this header file is different with  x and y in FontGlyph and 
/// tft screen. While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
/// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
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
        _w = w, _h = h;
        _img.resize(_w * _h, filled_value);
    };

    /// @brief Initialize image from 1d-array
    /// @param color_array The pixel-color-array of the image (has been flatten)
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    /// default is black (0x0).
    IMAGE(  Timage* color_array, uint16_t h = 172, uint16_t w = 220, 
            uint16_t filled_value = 0x0
    ){
        _w = w, _h = h;
        this->resize(color_array, h, w, filled_value);
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

    void fill(Timage color){
        for(auto &p:_img) p = color;
    }

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
    /// @param o The other image.
    void assign(IMAGE o){
        this->vector_image() = o.vector_image();
        this->H() = o.H();
        this->W() = o.W();
    }

    /// @brief Assign an image to this image
    /// @param o The other image.
    IMAGE operator= (IMAGE o){
        this->vector_image() = o.vector_image();
        this->H() = o.H();
        this->W() = o.W();
        return *this;
    }



    /// @brief compare size of two image
    /// @param o The other image.
    bool size_cmp(IMAGE o){
        if(this->H() != o.H() || this->W() != o.W())
            return false;
        return true;
    }

    /// @brief compare two image
    /// @param o The other image
    bool operator== (IMAGE o){
        if(this->H() != o.H() || this->W() != o.W())
            return false;
        rept(uint16_t, i, 0, this->H()-1){
            rept(uint16_t, j, 0, this->W()-1){
                if(this->vector_image()[i*this->W()+j]!=o.vector_image()[i*this->W()+j])
                    return false;
            }
        }
        return true;
    }

};

/// >>>>>>>>> some utilities of image processing >>>>>>>>
uint16_t to_grayscale_565format(uint16_t _565color){
    return (
        ((_565color>>11) & 0x1F)*76 + 
        ((_565color>>5) & 0x3F)*160 + 
        (_565color & 0x1F)*30
    )/256.0;
}

/// >>>>>>>>>>>>>>> definition of canvas >>>>>>>>>>>>>>>>

/// @brief The default orientation of the screen
#define ORIENTATION 1

/// @brief A canvas and some methods
/// @tparam Tcanvas typef of canvasm default is uint16_t
/// @note  X and y in this header file is different with  x and y in FontGlyph
/// and tft-screen.
/// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
/// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
template<class Tcanvas = uint16_t>
class CANVAS{
private:
    uint16_t _background_color;
    IMAGE<Tcanvas> _canvas_prev, _canvas;
    
public:
    /// @brief Initializing a empty CANVAS object
    CANVAS(){
        _background_color = 0x0;
    };

    /// @brief Resize a CANVAS object
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    void resize(uint16_t h, uint16_t w, Tcanvas filled_value = 0)
    {
        _background_color = filled_value;
        _canvas.resize(h, w, filled_value);
        _canvas_prev.resize(h, w, filled_value);
    }

    /// @brief Resize a CANVAS object
    /// @param h The number of rows (or the height of the image)
    /// @param w The number of cols (or the width of the image)
    /// @param filled_value The color will be filled in the image 
    void initialize( 
        uint16_t h, uint16_t w, 
        uint8_t orientation = 0, Tcanvas filled_value = 0
    )
    {
        
        tft.begin(orientation, filled_value);
        _background_color = filled_value;
        resize(h, w, filled_value);
    }

    /// @brief Show in screen
    /// @param force_show force to show all pixel in canvas
    /// @note  X and y in this header file is different with  x and y in FontGlyph.
    /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    void show(bool force_show = false){
        if(force_show)
            for(uint16_t r = 0; r < _canvas.H(); ++r)
                for(uint16_t c = 0; c < _canvas.W(); ++c){
                    _canvas_prev.pixel(r, c) = _canvas.pixel(r, c);
                    tft.drawPixel(c, r, _canvas.pixel(r, c));
                }
        else
            for(uint16_t r = 0; r < _canvas.H(); ++r)
                for(uint16_t c = 0; c < _canvas.W(); ++c)
                    if((_canvas.pixel(r, c) == _canvas_prev.pixel(r, c))) 
                        continue;
                    else{
                        _canvas_prev.pixel(r, c) = _canvas.pixel(r, c);
                        tft.drawPixel(c, r, _canvas.pixel(r, c));
                    }
    }

    /// @brief Clear current canvas
    /// @param clear_all to clear current canvas and old_canvas
    void clear(bool clear_all = false){
        this->_canvas.fill(this->_background_color);
        if(clear_all)
            this->_canvas_prev.fill(this->_background_color);
    }

    /// @brief Refill the canvas
    /// @param filled_color The color to be filled
    void refill(uint16_t filled_color){
        for(auto &p:_canvas.vector_image()) p = filled_color;
    }

    // /// @brief Set pixel at <pos> on screen
    // /// @param pos The position on the screen(unit: pixel) 
    // /// @param color The color of the text (16bit-color) 
    // /// @note  X and y in this header file is different with  x and y in FontGlyph.
    // /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    // /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    // void set_pixel(POINT<uint16_t> pos, uint16_t color){
    //     _canvas.set_pixel(point, color);
    // }

    /// @return the height of the canvas
    uint16_t H(){
        return _canvas.H();
    }

    /// @return the widht of the canvas
    uint16_t W(){
        return _canvas.W();
    }

    /// @brief Sets the color of a pixel in the canvas.
    /// @param pos A `POINT` object that specifies the position of the pixel in the canvas.
    /// @param color The color to be set to the pixel. Default is `0xFFFF` (white).
    /// @note The `POINT` object should contain `x` and `y` values, representing the pixel's position in the canvas.
    /// If no color is provided, the default color will be white (`0xFFFF`).
    void set_pixel(POINT<uint16_t> pos, uint16_t color = 0xFFFF){
        this->_canvas.pixel(pos) = color;
    }

    /// @brief Add text
    /// @param pos The position on the screen (bottom-left, unit: pixel) 
    /// @param text The text to be inserted. 
    /// @param color The color of the text (16bit-color) 
    /// @note - The text is being inserted from the top-left to bottom-right;
    /// the current position (left-bottom).
    /// @note  - X and y in this header file is different with  x and y in FontGlyph.
    /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    void insert_text(POINT<uint16_t> pos, String text, uint16_t color = 0xFFFF){
        /// nothin' to insert
        if(text.isEmpty()) return;
        /// drawed bits
        uint8_t     dbits;
        /// the bitmap byte of the character in FontBitmap
        uint8_t     bm_byte;
        /// byte-bitmap position in FontBitmap
        uint16_t    bm_byte_pos;
        /// the oder of character in FontGlyph 
        uint16_t    char_index;

        /// draw pointer (the index of draw-point in canvas, 
        /// it starts from UL (current position) corner of the canvas)
        POINT<uint16_t> draw_pos;
        /// process each character in ``text``
        rept(uint8_t, i, 0, text.length()-1){
            /// get the oder of character in FontGlyph 
            if(text.charAt(i) > '~' || text.charAt(i) < ' ')
                char_index = '?' - ' ';
            else
                char_index = text.charAt(i) - ' ';
            /// reset dbits to ZERO
            dbits  = 0;
            /// get bm_byte_pos 
            bm_byte_pos = FontGlyph[char_index].bitmapOffset;
            /// get bitmap_byte in bitmap
            bm_byte = FontBitmap[bm_byte_pos];

            /// Place character in canvas
            rept(uint16_t, r, 0, FontGlyph[char_index].height-1){
                /// update draw-pos
                draw_pos.X() = pos.X() + FontGlyph[char_index].yOffset + r;
                if(_canvas.invalid_position(draw_pos)) break;
                rept(uint16_t, c, 0, FontGlyph[char_index].width-1){
                    /// update draw-pos
                    draw_pos.Y() = pos.Y() + FontGlyph[char_index].xOffset + c; 
                    if(_canvas.invalid_position(draw_pos)) break;
                    /// if the pixel is display (bit displayed is not 0-bit) 
                    if( (bm_byte & 0x80) != 0 ){
                        _canvas.pixel(draw_pos) = color;
                    }
                    /// increase the drawed bit
                    ++dbits;
                    /// next byte or bit?
                    if(dbits > 0 && dbits%8 == 0){
                        //// after shifted-left 8bits, move to next byte!
                        bm_byte = FontBitmap[++bm_byte_pos];
                        dbits = 0;
                    }else{
                        /// self shift-left to removed drawed bit
                        (bm_byte) <<= 1;
                    }
                }
            }
            /// move pointer to next position (for new character)
            pos.Y() += FontGlyph[char_index].xAdvance;
            /// out of canvas :v
            if(_canvas.invalid_position(pos)) break;
        }
    }

    /// @brief Add bitmap image
    /// @param pos The position on the screen (top-left, unit: pixel) 
    /// @param color The color of the text (16bit-color) 
    /// @param bitmap_img The binary image to be inserted to the canvas
    /// @param W The width of the binary image
    /// @param H The Height of the binary image
    /// @param _1bit_color The color will be filled in canvas with 1-bit in bitmap
    /// @param _0bit_color The color will be filled in canvas with 0-bit in bitmap.
    /// If _0bit_color, it means no-changed!.
    /// @note - The image is being inserted from the top-left to bottom-right;
    /// the current position (left-bottom).
    /// @note  - X and y in this header file is different with  x and y in FontGlyph.
    /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    void insert_bitmap_image(
        POINT<uint16_t> pos, 
        const uint8_t* bitmap_img, uint16_t w, uint16_t h, 
        uint16_t _1bit_color = 0xFFFF, uint16_t _0bit_color = 0x0
    ){
        /// Compute the # of bytes per line (row)
        /// The # of bytes per line = upper_bound(W/8)
        uint16_t const bytes_per_row = (w/8) + ((w%8)?1:0);
        /// drawed bits 
        uint8_t    ins_bits = 0;
        /// bitmap byte position
        uint16_t    bm_byte_pos = 0;
        /// bitmap byte
        uint8_t     bm_byte = bitmap_img[bm_byte_pos];
        /// insert position
        POINT<uint16_t> ins_pos = 0;
        /// process all pixel in bitmap image
        rept(uint16_t, r, 0, h-1){
            /// update X value
            ins_pos.X() = pos.X() + r;
            if(this->_canvas.invalid_position(ins_pos)) break;
            /// reset bit count at new row
            ins_bits = 0;
            /// reset byte bitmap position at new row
            bm_byte_pos = r * bytes_per_row;
            /// update bitmap byte value
            bm_byte = bitmap_img[ bm_byte_pos ];
            /// process the row
            rept(uint16_t, c, 0, w-1){
                /// update Y value
                ins_pos.Y() = pos.Y() + c;
                if(this->_canvas.invalid_position(ins_pos)) break;
                /// process each bit
                if(bm_byte & 0x80){
                    /// 1-bit
                    _canvas.pixel(ins_pos) = _1bit_color;
                }else{
                    /// 0-bit
                    if(_0bit_color != 0)
                        _canvas.pixel(ins_pos) = _0bit_color;
                }
                /// inscrease the # of inserted bits
                ++ins_bits;
                /// move next byte if has inserted 8 bits
                if(ins_bits > 0 && ins_bits%8 == 0)
                    ins_bits == 0,
                    bm_byte = bitmap_img[++bm_byte_pos];
                else
                    /// self shift-left to remove inserted bit
                    bm_byte <<= 1;
            }
        }
    }

    /// @brief Add 565-color image
    /// @param pos The position on the canvas (top-left, unit: pixel) 
    /// @param color The color of the text (16bit-color) 
    /// @param _565color_img The binary image to be inserted to the canvas
    /// @param W The width of the binary image
    /// @param H The Height of the binary image
    /// @param chromakey TRUE: Enable chromakey
    /// @param chromakey_color choose the color for chromakey
    /// @note - The image is being inserted from the top-left to bottom-right;
    /// the current position (left-bottom).
    /// @note  - X and y in this header file is different with  x and y in FontGlyph.
    /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    void insert_565color_image(
        POINT<uint16_t> pos, 
        const uint16_t* _565color_img, 
        uint16_t w, uint16_t h,
        bool chromakey = false,
        uint16_t chromakey_color = 0xFFFF
    ){
        /// insert pixel position (in 565-color array)
        uint16_t inspix_pos = 0;
        /// insert position
        POINT<uint16_t> ins_pos = 0;

        /// process all pixel in bitmap image
        rept(uint16_t, r, 0, h-1){
            /// update X value
            ins_pos.X() = pos.X() + r;
            if(this->_canvas.invalid_position(ins_pos)) break;
            rept(uint16_t, c, 0, w-1){
                /// update Y value
                ins_pos.Y() = pos.Y() + c;
                if(this->_canvas.invalid_position(ins_pos)) break;
                /// check chromakey and process
                if(
                    chromakey
                ){
                    /// skip this pixel
                    ++inspix_pos;
                    continue;
                }
                /// process each pixel
                _canvas.pixel(ins_pos) = _565color_img[inspix_pos++];
            }
        }
    }

    /// @brief Add a rectangle to canvas
    /// @param pos The position on the canvas (top-left, unit: pixel) 
    /// @param h The width of the rectangle
    /// @param w The Height of the rectangle
    /// @param border_color The color of the rectangle border  (16bit-color) 
    /// @param fill TRUE: filled_color will be filled into the rectangle 
    /// @param filled_color The color will be filled
    /// @param border_select Select the border will be drawed.
    /// NOTE: 0x1 - top, 0x2 - right, 0x4 - bottom, 0x8 - left.
    /// @note - The shape is being inserted from the top-left to bottom-right;
    /// the current position (left-bottom).
    /// @note  - X and y in this header file is different with  x and y in FontGlyph.
    /// While x in this header file is row-order-number (start from ZERO), x in FontGlyph 
    /// and tft_screen is col-order-number (start from ZERO). The similar ro y. 
    void insert_rectangle(
        POINT<uint16_t> pos, 
        uint16_t w, uint16_t h, 
        uint16_t border_color = 0xFFFF,
        bool fill = false, 
        uint16_t filled_color = 0x0,
        uint8_t border_select = 0xF
    ){
        if(fill == true){
            /// process all pixel in rectangle
            rept(uint16_t, r, 1, h-2){
                rept(uint16_t, c, 1, w-2){
                    /// process each pixel
                    _canvas.pixel(pos.X() + r, pos.Y() + c) = filled_color;
                }
            }
        }
        /// draw the left edge
        if(border_select & 0x8)
            rept(uint16_t, r, 0, h-1){
                _canvas.pixel(pos.X() + r, pos.Y() + 0) = border_color;
            }
        /// draw the right edge
        if(border_select & 0x2)
            rept(uint16_t, r, 0, h-1){
                _canvas.pixel(pos.X() + r, pos.Y() + w-1) = border_color;
            }
        /// draw the upper edge / top
        if(border_select & 0x1)
            rept(uint16_t, c, 0, w-1){
                _canvas.pixel(pos.X() + 0,      pos.Y() + c) = border_color;
            }
        /// draw the lower edge / bottom
        if(border_select & 0x4)
            rept(uint16_t, c, 0, w-1){
                _canvas.pixel(pos.X() + h-1,    pos.Y() + c) = border_color;
            }

    }

    /// @brief Add a line in canvas
    /// @param A The position on the canvas (unit: pixel) 
    /// @param B The position on the canvas (unit: pixel) 
    void insert_line(POINT<uint16_t> A, POINT<uint16_t> B, uint16_t color = 0x0){
        /// A === B
        if(A.X() == B.X() && A.Y() == B.Y() ){
            _canvas.pixel(A.X(), A.Y()) = color;
            return;
        }
        /// horizonal and vertical line
        if( A.X() == B.X() ){
            if(A.Y() > B.Y()) swap(A.Y(), B.Y());
            for(uint16_t c = A.Y(); c <= B.Y(); ++c)
                _canvas.pixel(A.X(), c) = color;
            return;
        }
        if( A.Y() == B.Y() ){
            if(A.X() > B.X()) swap(A.X(), B.X());
            for(uint16_t r = A.X(); r <= B.X(); ++r)
                _canvas.pixel(r, A.Y()) = color;
            return;
        }
        /// positive slope line
        if( 
            (A.X() < B.X() && A.Y() < B.Y()) || 
            (A.X() > B.X() && A.Y() > B.Y()) 
        ){
            if(A.X() > B.X() && A.Y() > B.Y()) { 
                swap(A.X(), B.X()); swap(A.Y(), B.Y());
            }
            uint16_t row, col, prev_col = this->W();
            for(row = A.X(); row <= B.X(); ++row){
                col = A.Y() + ((B.Y()-A.Y())*(row-A.X()))/(B.X()-A.X());
                while(prev_col < col)
                    _canvas.pixel(row, prev_col++) = color;
                _canvas.pixel(row, col) = color;
                prev_col = col + 1;
            }
            return;
        }
        /// nagative slope line
        if( 
            (A.X() < B.X() && A.Y() > B.Y()) || 
            (A.X() > B.X() && A.Y() < B.Y()) 
        ){
            if(A.X() > B.X() && A.Y() < B.Y()) { 
                swap(A.X(), B.X()); swap(A.Y(), B.Y());
            }
            uint16_t row, col, prev_col = 0;
            for(row = A.X(); row <= B.X(); ++row){
                col = A.Y() - (A.Y()-B.Y())*(row-A.X())/(B.X()-A.X());
                while(prev_col > col)
                    _canvas.pixel(row, prev_col++) = color;
                _canvas.pixel(row, col) = color;
                prev_col = (col) ? col - 1 : 0;
            }
            return;
        }
    }

};


/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
enum enum_SCREEN_MODE { 
    NORMAL_MODE = 0 , SETUP_WIFI_MODE  = 1, SHOW_ENVINFO_MODE  = 2,

    TEST_MODE_SCREEN      = 252, TEST_MODE_BLANK = 253,
    RESERVED_FEATURE_MODE = 254, ERROR_MODE      = 255
};
uint16_t screen_mode = NORMAL_MODE;

/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// @brief The canvas object
CANVAS<uint16_t> canvas;
void canvas_init(){
    #if LOG == true
        call("canvas_init:");
    #endif
    canvas.initialize(220, 172, 0, 0xFFFF);
    #if LOG == true
        log2ser("Resolution: " "W: ", canvas.W(), " H: ", canvas.H());
    #endif
    canvas.insert_text(POINT<>(90, 20),  "~Hello! #HAHA~", 0xAAAA);
    canvas.insert_text(POINT<>(125, 19), "This is a gift", 0xAAAA);
    canvas.insert_text(POINT<>(145, 19), "from ngxxfus :>", 0xAAAA);
    canvas.show(true);
    
    #if BASIC_IO == true
        basic_io::led1_blinky(3, 10, 40);
        basic_io::led1_blinky(2, 100, 100);
        basic_io::led1_blinky(1, 3000, 0);
    #endif
                                                                             
    canvas.clear(true);
    canvas.show(true);

}

/// >>>>>>>>>>>>>>>>> SOME UTIL FUNCS >>>>>>>>>>>>>>>>>>>>

/// @brief keyboard character map (provides most of common character)
const char keyboard[6][12] PROGMEM = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\"','\''},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', ';', '/','\\'},
    {'`', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(',')'},
    {'{', '}', '|', '?', '<', '>', '_', '=', ' ', ' ', ' ', ' '}
};

/// @brief ```pattern```: show keyboard
/// @param keyboard_row Where? (0->219)
/// @param keyboard_color The color will be set for the keyboard
/// @note position.unit: pixel
/// @note color.type: 16-bit color
inline void show_keyboard(uint8_t keyboard_row, uint16_t keyboard_color = 0x5aeb){
    uint8_t keyboard_row1 = keyboard_row + 18, keyboard_row2 = keyboard_row + 36,
            keyboard_row3 = keyboard_row + 54, keyboard_row4 = keyboard_row + 72, 
            keyboard_row5 = keyboard_row + 90;
    /// show keyboard
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row, 5 + c * 14), String(keyboard[0][c]), keyboard_color);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row1, 5 + c * 14), String(keyboard[1][c]), keyboard_color);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row2, 5 + c * 14), String(keyboard[2][c]), keyboard_color);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row3, 5 + c * 14), String(keyboard[3][c]), keyboard_color);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row4, 5 + c * 14), String(keyboard[4][c]), keyboard_color);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row5, 5 + c * 14), String(keyboard[5][c]), keyboard_color);
    canvas.insert_bitmap_image(POINT<>(keyboard_row5-10, 150), _10x13_capslock_icon, 10, 13, keyboard_color);
}

/// @brief ```pattern```: insert show inputbox
/// @param box_row Where? (0->219)
/// @param title_text Short text for title (top), (```size```: 1->8 characters)
/// @param content Short text for content inside the box, (```size```: 1->13 characters)
/// @param border_color The color of 4 border (top/bot/left/rigt)
/// @param del_icon_color The color of delete icon
/// @param title_color The color of the title
/// @param content_color The color of the text inside the box
/// @note position.unit: pixel
/// @note color.type: 16-bit color
inline void show_input_box(
    uint8_t box_row, 
    String title_text, String content, 
    uint16_t border_color = 0x8430, uint16_t del_icon_color = 0x5aeb,
    uint16_t title_color = 0x5aeb, uint16_t content_color = 0x0088
){
    if(title_text.length() > 8) title_text = title_text.substring(0, 8);

    /// upper edge
    canvas.insert_line(POINT<>(box_row,  2),  POINT<>(box_row,  10), border_color);
    canvas.insert_line(POINT<>(box_row, 15 + title_text.length() * 8),  POINT<>(box_row, 170), border_color);
    /// lower edge
    canvas.insert_line(POINT<>(box_row+31, 2), POINT<>(box_row+32, 170), border_color);
    /// left edge
    canvas.insert_line(POINT<>(box_row,    2), POINT<>(box_row+31,    2), border_color);
    /// right edge
    canvas.insert_line(POINT<>(box_row,  170), POINT<>(box_row+31,  170), border_color);
    /// show title text (max: 8 characters)
    canvas.insert_text(POINT<>(box_row+3, 13), title_text, title_color);
    /// show delete icon
    canvas.insert_bitmap_image(POINT<>(box_row+9, 140), _22x15_delete_icon, 22, 15, del_icon_color);
    /// show content on box (max: 10 characters)
    canvas.insert_text(POINT<>(box_row+18, 5), content, content_color);
}

/// @brief ```patern```: add two button on canvas 
/// @param btn_row Where? (0->219)
/// @param text0 Short_text for button 0 (left), (```size```: 1->6 characters)
/// @param col0  Start from the left-edge of the canvas, where? (0->171)
/// @param text1 Short_text for button 1 (right), (```size```: 1->6 characters)
/// @param col1  Start from the left-edge of the canvas, where? (0->171)
/// @param btn0_color The color for button 0
/// @param btn1_color The color for button 1
/// @param txt0_color The color for text button 0
/// @param txt1_color The color for text button 1
/// @note position.unit: pixel
/// @note color.type: 16-bit color
inline void show_2button_on_1line(
    uint8_t btn_row, 
    String text0, uint8_t col0, 
    String text1, uint8_t col1,
    uint16_t btn0_color = 0xf2b2, uint16_t btn1_color = 0x3d3b,
    uint16_t txt0_color = 0xFFFF, uint16_t txt1_color = 0xFFFF
){
    canvas.insert_bitmap_image(POINT<>(btn_row, 15), _64x27_rounded_rectangle, 64, 27, btn0_color);
    canvas.insert_text(POINT<>(btn_row+18, col0), text0, txt0_color);
    canvas.insert_bitmap_image(POINT<>(btn_row, 93), _64x27_rounded_rectangle, 64, 27, btn1_color);
    canvas.insert_text(POINT<>(btn_row+18, col1), text1, txt1_color);
}


/// @brief show error screen
void error_mode(){
    #if TFT_SCREEN == true
        canvas.refill(0xc0e5);
        uint16_t err_row = 18;
        canvas.insert_text(POINT<>(18, 55), "ERROR", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Hello!", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "An error occured!", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Please press RESET", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "button to restart", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "system!", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Contact info:", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "> fb.com/ngxxfus", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), "> msnp@outlook.com", 0xffff);
        canvas.insert_text(POINT<>(err_row+=18, 5), ".vn", 0xffff);
        canvas.show(true);
    #endif
    #if BASIC_IO == true
        basic_io::led0_val(1);
        basic_io::led0_blinky(10000, 19, 253);
    #endif
    #if CONTROLLER == true
        while(0x1)  controller::iled_blinky(1);
    #endif
}

/// @brief show notification screen for reserved feature
void reserved_feature_mode(){
    #if TFT_SCREEN == true
        canvas.refill(0x7fd4);
        uint16_t err_row = 18;
        canvas.insert_text(POINT<>(18, 48), "- DEV -", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Hello!", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "This is NOT an", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "ERROR. This is a", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "RESERVED feature!", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Press RESET to", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "restart :>", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "Contact info:", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "> fb.com/ngxxfus", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), "> msnp@outlook.com", 0x2124);
        canvas.insert_text(POINT<>(err_row+=18, 5), ".vn", 0x2124);
        canvas.show(true);
    #endif
    #if BASIC_IO == true
        basic_io::led0_val(1);
        basic_io::led0_blinky(10, 17, 329);
    #endif
    #if CONTROLLER == true
        controller::iled_blinky(10, 1000);
    #endif
}

/// @brief show humid and temp box (for slide show mode)
void show_humid_temp_box(POINT<> pos, float humid, float temp, uint16_t background_color = 0x0, uint16_t text_color = 0xFFFF){
    /// insert background
    canvas.insert_rectangle(pos, 120, 43, background_color, true, background_color);
    /// insert humid/
    canvas.insert_text(POINT<>(pos.X()+16, pos.Y()+5), "Humid: ", text_color);
    canvas.insert_text(POINT<>(pos.X()+16, pos.Y()+70), String(humid), text_color);
    /// insert temp
    canvas.insert_text(POINT<>(pos.X()+36, pos.Y()+5), "Temp: ", text_color);
    canvas.insert_text(POINT<>(pos.X()+36, pos.Y()+70), String(temp), text_color);
}

void single_text_line(uint16_t line, String text, uint16_t color = 0x0, uint16_t text_col = 5, uint16_t line_distance = 23, bool text_wrap = false){
    if( line * line_distance > 215 ) return;
    if(text_wrap == false)
        canvas.insert_text({uint16_t(line * line_distance), text_col}, text.substring(0, 22), color);
    else
        for(uint16_t i = 0; 22+i*22 <= text.length() || i == 0; ++i){
            canvas.insert_text({uint16_t((line+i) * line_distance), text_col}, text.substring(0+i*22, 22+i*22), color);
        }
}

void single_screen_color_and_text_line(
    uint16_t line = 1, String text = "loading...", 
    uint16_t background_color = 0xFFFF, uint16_t text_color = 0x0,
    bool immediately_show = true, bool draw_background = true
){
    if(draw_background) canvas.refill(background_color);
    single_text_line(line, text, text_color);
    if(immediately_show) canvas.show();
}

#endif