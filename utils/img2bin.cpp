#include <bits/stdc++.h>
using namespace std;
#include <jpeglib.h>

#include <cstdint> 

void raise_exception(string msg = "unknown", int exit_code = 1){
    cout << "Err: " << msg << '\n';
    exit(exit_code);
}

inline uint16_t RGB_to_RGB565(uint8_t red, uint8_t green, uint8_t blue) {
        return ((red & 0xF8) << 8) |   // Take the top 5 bits of Red
               ((green & 0xFC) << 3) | // Take the top 6 bits of Green
               ((blue & 0xF8) >> 3);   // Take the top 5 bits of Blue
    }
 
class RGB_PIXEL {
private:
    uint16_t red, green, blue;

    // Convert RGB888 to RGB565
    inline uint16_t RGB_to_RGB565(uint8_t red, uint8_t green, uint8_t blue) const {
        return ((red & 0xF8) << 8) |   // Take the top 5 bits of Red
               ((green & 0xFC) << 3) | // Take the top 6 bits of Green
               ((blue & 0xF8) >> 3);   // Take the top 5 bits of Blue
    }

    // Normalize values to 8-bit range
    template <class T>
    T norm_1608(T in) const {
        return (in > 255) ? 255 : in;
    }

public:
    // Constructor
    RGB_PIXEL(uint16_t red = 0, uint16_t green = 0, uint16_t blue = 0)
        : red(red), green(green), blue(blue) {}

    // Getters
    uint16_t RED() const { return red; }
    uint16_t GREEN() const { return green; }
    uint16_t BLUE() const { return blue; }

    // Normalize function
    void NORMALIZE() {
        red   = norm_1608(red);
        green = norm_1608(green);
        blue  = norm_1608(blue);
    }

    // Assignment operator
    RGB_PIXEL& operator=(const RGB_PIXEL& o) {
        if (this != &o) {
            red = o.RED();
            green = o.GREEN();
            blue = o.BLUE();
        }
        return *this;
    }

    // Addition operators
    RGB_PIXEL operator+(const RGB_PIXEL& o) const {
        return RGB_PIXEL(RED() + o.RED(), GREEN() + o.GREEN(), BLUE() + o.BLUE());
    }

    template<class Tw>
    RGB_PIXEL operator+(Tw white) const {
        return RGB_PIXEL(RED() + white, GREEN() + white, BLUE() + white);
    }

    // Self-addition
    RGB_PIXEL& operator += (const RGB_PIXEL& o) {
        this->red   = (this->red   + o.red   > 65535U) ? 65535 : this->red   + o.red;
        this->green = (this->green + o.green > 65535U) ? 65535 : this->green + o.green;
        this->blue  = (this->blue  + o.blue  > 65535U) ? 65535 : this->blue  + o.blue;
        return *this;
    }

    // Subtraction operators
    RGB_PIXEL operator-(const RGB_PIXEL& o) const {
        return RGB_PIXEL(RED() - o.RED(), GREEN() - o.GREEN(), BLUE() - o.BLUE());
    }

    template<class Tw>
    RGB_PIXEL operator-(Tw white) const {
        return RGB_PIXEL(RED() - white, GREEN() - white, BLUE() - white);
    }

    // Multiplication operators
    RGB_PIXEL operator*(const RGB_PIXEL& o) const {
        return RGB_PIXEL(RED() * o.RED(), GREEN() * o.GREEN(), BLUE() * o.BLUE());
    }

    template<class Tw>
    RGB_PIXEL operator*(Tw white) const {
        return RGB_PIXEL(RED() * white, GREEN() * white, BLUE() * white);
    }

    // Division operators (avoid division by zero)
    RGB_PIXEL operator/(const RGB_PIXEL& o) const {
        return RGB_PIXEL(o.RED() ? RED() / o.RED() : 0,
                         o.GREEN() ? GREEN() / o.GREEN() : 0,
                         o.BLUE() ? BLUE() / o.BLUE() : 0);
    }

    template<class Tw>
    RGB_PIXEL operator/(Tw white) const {
        return RGB_PIXEL(white ? RED() / white : 0,
                         white ? GREEN() / white : 0,
                         white ? BLUE() / white : 0);
    }

    // Convert to 16-bit RGB565 format
    uint16_t to_565_format() const {
        return RGB_to_RGB565(
            static_cast<uint8_t>(norm_1608(RED())),
            static_cast<uint8_t>(norm_1608(GREEN())),
            static_cast<uint8_t>(norm_1608(BLUE()))
        );
    }

    bool operator == (const RGB_PIXEL& o){
        return (RED() == o.RED()) 
                && (GREEN() == o.GREEN()) 
                && (BLUE()  == o.BLUE());
    }

    bool operator != (const RGB_PIXEL& o){
        return (RED() != o.RED()) 
                || (GREEN() != o.GREEN()) 
                || (BLUE()  != o.BLUE());
    }


    friend std::ostream& operator<<(std::ostream& os, const RGB_PIXEL& pixel){
        os << "(" << pixel.RED() << ", " << pixel.GREEN() << ", " << pixel.BLUE() << ")";
        return os;
    }

};


class RGB_IMG{
private:
    uint32_t id;
    RGB_PIXEL* pixels;
    uint16_t max_height, max_width;

protected:
    uint32_t xy_to_index(uint16_t x, uint16_t y, uint16_t max_x, uint16_t max_y) const {
        if( x >= max_x || y >= max_y ) return 0U;
        return uint32_t(x) * uint32_t(max_y) + uint32_t(y);
    }

    template<class Tmsg>
    void actually_print_log(Tmsg msg) const {\
        // will be called at last of param list :v 
        cout << msg << '\n';
    }

    template<class Tmsg, class... Tmsgs>
    void actually_print_log(Tmsg msg, Tmsgs... msgs) const {
        cout << msg;
        actually_print_log(msgs...);
    }

    template<class... Tmsgs>
    void write_log(Tmsgs... msgs) const {
        cout << "<RGB_IMG id=" << id << "> ";
        actually_print_log(msgs...);
    }


public:
    RGB_IMG(uint16_t max_height = 0, uint16_t max_width = 0, RGB_PIXEL preset = {0, 0, 0})
    {

        this->id = uintptr_t(&id);
        if( !max_height  || !max_width ){
            this->pixels        = NULL;
            this->max_height    = 0;
            this->max_width     = 0;
            return;
        }else{
            this->pixels        = new RGB_PIXEL[ uint32_t(max_height) * uint32_t(max_width) ];
            this->max_height    = max_height;
            this->max_width     = max_width;
        }
        if( preset != RGB_PIXEL({0, 0, 0}) ){
            for(uint32_t i = 0; i < max_height*max_width; ++i){
                *(pixels + i) = preset;
            }
        }
        write_log("initialized ", "h= ", max_height, " w= ", max_width);
    }

    RGB_IMG(string jpg_path){
        FILE* infile = fopen(jpg_path.c_str(), "rb");

        if (!infile) {
            write_log("failed to initialize!");
            exit(1);
        }

        // Decompress the JPEG file
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);
        jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);

        // Get image dimensions
        this->max_width = cinfo.output_width;
        this->max_height = cinfo.output_height;
        this->pixels    = new RGB_PIXEL[ max_height * max_width ];
        int channels = cinfo.output_components; // 3 for RGB

        // Allocate memory for a row of pixels
        unsigned char* row_buffer = new unsigned char[max_width * channels];

        // Read each row
        for (int x = 0; x < max_height; ++x) {
            jpeg_read_scanlines(&cinfo, &row_buffer, 1);
            for (int y = 0; y < max_width; ++y) {
                int index = y * channels;
                unsigned char red = row_buffer[index];
                unsigned char green = row_buffer[index + 1];
                unsigned char blue = row_buffer[index + 2];
                this->pixels[xy_to_index(x, y, max_height, max_width)]
                = {red, green, blue};
            }
        }

        write_log("initialized", " h=", max_height ," w=", max_width, " c=", channels);

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
    }

    uint32_t ID() const {return this->id;}

    void RESIZE(uint16_t new_height, uint16_t new_width){
        delete[] pixels;
        pixels = NULL;
        this->max_height = new_height;
        this->max_width = new_width;
        if( this->max_height && this->max_width ){
            pixels = new RGB_PIXEL[ uint32_t(max_height) * uint32_t(max_width) ];
        }
    }

    void SET_HEIGHT(uint16_t new_height){this->max_height = new_height;}
    void SET_WIDTH(uint16_t new_width){this->max_width = new_width;}
    void SET_PIXEL(uint32_t index, RGB_PIXEL new_pixel){
        if(index >= uint32_t(this->HEIGHT()) * uint32_t(this->WIDTH())){
            write_log("ERROR: ", "RGB_PIXEL.PIXEL: out-of-range");
            exit(1);
        }
        *(this->pixels + index) = new_pixel;
    }
    void SET_PIXEL(uint16_t x, uint16_t y, RGB_PIXEL new_pixel){
        if(x >= this->HEIGHT() || y >= this->WIDTH()){
            write_log("ERROR: ", "RGB_PIXEL.PIXEL: out-of-range");
            exit(1);
        }
        *(pixels + uint32_t(x) * uint32_t(this->WIDTH()) + uint32_t(y)) = new_pixel;
    }

    uint16_t WIDTH() const {return this->max_width;}
    uint16_t HEIGHT()const {return this->max_height;}

    RGB_PIXEL& PIXEL(uint32_t index) const {
        if(index >= uint32_t(this->HEIGHT()) * uint32_t(this->WIDTH())){
            this->write_log("ERROR: ", "RGB_PIXEL.PIXEL: out-of-range");
            exit(1);
        }
        return *( pixels + index);    
    }

    RGB_PIXEL& PIXEL(uint16_t x, uint16_t y) const {
        if(x >= this->HEIGHT() || y >= this->WIDTH()){
            write_log("ERROR: ", "RGB_PIXEL.PIXEL: out-of-range");
            exit(1);
        }
        return *(pixels + uint32_t(x) * uint32_t(this->WIDTH()) + uint32_t(y));
    }

    friend std::ostream& operator<<(std::ostream& os, const RGB_IMG& img){
        os << "<RGB_IMG id=" << img.ID() << ">\n";
        for(uint16_t x = 0; x < img.HEIGHT(); ++x){
            for(uint16_t y = 0; y < img.WIDTH(); ++y){
                os << img.PIXEL(x, y) << '\t';
            }
            os << '\n';
        }
        return os;
    }

    RGB_IMG operator = (RGB_IMG o){
        this->RESIZE(o.HEIGHT(), o.WIDTH());
        for(uint32_t i = 0; i < uint32_t(o.HEIGHT()) * uint32_t(o.WIDTH()); ++i){
            this->SET_PIXEL(i, o.PIXEL(i));
        }
        return *this;
    }

    RGB_IMG operator + (RGB_IMG o){
        this->RESIZE(o.HEIGHT(), o.WIDTH());
        for(uint32_t i = 0; i < uint32_t(o.HEIGHT()) * uint32_t(o.WIDTH()); ++i){
            this->SET_PIXEL(i, o.PIXEL(i) + this->PIXEL(i));
        }
        return o;
    }

    RGB_IMG operator - (RGB_IMG o){
        this->RESIZE(o.HEIGHT(), o.WIDTH());
        for(uint32_t i = 0; i < uint32_t(o.HEIGHT()) * uint32_t(o.WIDTH()); ++i){
            this->SET_PIXEL(i, this->PIXEL(i) - o.PIXEL(i));
        }
        return o;
    }

    RGB_IMG& conv_2d(RGB_IMG* filter) {
        if(!pixels) return *this;

        auto const res_h = HEIGHT() - filter->HEIGHT() + 1;
        auto const res_w = WIDTH() - filter->WIDTH() + 1;
        
        RGB_PIXEL* new_pixels = new RGB_PIXEL[ res_h * res_w ];

        for(uint16_t res_x = 0; res_x < res_h; ++res_x){
            for(uint16_t res_y = 0; res_y < res_w; ++res_y){
                for(uint16_t filter_x = 0; filter_x < filter->HEIGHT(); ++filter_x){
                    for(uint16_t filter_y = 0; filter_y < filter->WIDTH(); ++filter_y){
                        new_pixels[xy_to_index(res_x, res_y, res_h, res_w)] 
                        += this->PIXEL(res_x+filter_x,res_y+filter_y) * filter->PIXEL(filter_x, filter_y);
                    }
                }
            }
        }

        delete[] this->pixels;
        
        this->max_height = res_h;
        this->max_width  = res_w;
        this->pixels = new_pixels;
        
        write_log("conv_2d"," res_h=", res_h,  ",res_w=", res_w);

        return *this;
    }

    ~RGB_IMG(){
        write_log("delete[] pixels=", pixels);
        if(pixels == NULL) return;
        delete[] pixels;
        pixels = NULL;
        write_log("done!");
    }
};

void readJPEG(const char* filename, const char* out) {
    // Open the file
    FILE* infile = fopen(filename, "rb");
    std::ofstream fs(out, std::ios::out | std::ios::binary | std::ios::app);
    if (!infile) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // Decompress the JPEG file
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // Get image dimensions
    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components; // 3 for RGB
    std::cout << "Image dimensions: " << width << "x" << height << ", Channels: " << channels << '\n';

    // Allocate memory for a row of pixels
    unsigned char* row_buffer = new unsigned char[width * channels];

    // Read each row
    for (int y = 0; y < height; y++) {
        jpeg_read_scanlines(&cinfo, &row_buffer, 1);
        for (int x = 0; x < width; x++) {
            int index = x * channels;
            unsigned char red = row_buffer[index];
            unsigned char green = row_buffer[index + 1];
            unsigned char blue = row_buffer[index + 2];
            /// RGB -> 565
            uint16_t _565f = RGB_to_RGB565(red, green, blue);
            /// split into byteH byteL
            const char byte_h = (_565f&0xFF00)>>8;
            const char byte_l = _565f & 0x00FF;
            /// write to bin file
            fs.ostream::write(&byte_h, 1);
            fs.ostream::write(&byte_l, 1);

            // std::cout << "Pixel (" << y << ", " << x << "): " \
            //           << "R=" << (int)red << ", G=" << (int)green << ", B=" << (int)blue \
            //           << "\t--> 565f: " << hex << (uint16_t) (byte_h&0xFF) <<(uint16_t) (byte_l&0xFF) \
            //           << dec << std::'\n';
        }
    }

    // Cleanup
    delete[] row_buffer;
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    fs.close();
}

int main(int argc, char* argv[]){
    cout << "+ params list:\n";
    for(int i = 0; i < argc; i++)
        cout << "\targ[" << i << "] : " << argv[i] << '\n';
    cout << '\n';
    // if(argc != 3) return 0;
    // readJPEG( argv[1], argv[2]);
    srand(time(0));

    RGB_IMG img("./imgs/img0.jpg");
    RGB_IMG filter(100, 100, {1, 1, 1});
    // cout << filter;
    img.conv_2d(&filter);


    // RGB_IMG img0(4, 4), img1(2, 2);    
    // img0.SET_PIXEL(0, 0, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(0, 1, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(0, 2, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(0, 3, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(1, 0, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(1, 1, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(1,2, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(1, 3, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(2, 0, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(2, 1, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(2,2, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(2, 3, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(3, 0, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(3, 1, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(3,2, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});
    // img0.SET_PIXEL(3, 3, {uint16_t(rand()%10), uint16_t(rand()%10), uint16_t(rand()%10)});

    // img1.SET_PIXEL(0, 0, {uint16_t(rand()%2), uint16_t(rand()%2), uint16_t(rand()%2)});
    // img1.SET_PIXEL(0, 1, {uint16_t(rand()%2), uint16_t(rand()%2), uint16_t(rand()%2)});
    // img1.SET_PIXEL(1, 0, {uint16_t(rand()%2), uint16_t(rand()%2), uint16_t(rand()%2)});
    // img1.SET_PIXEL(1, 1, {uint16_t(rand()%2), uint16_t(rand()%2), uint16_t(rand()%2)});

    // RGB_IMG_print(img0);
    // cout << "\nprint <img0>:\n" << img0 << '\n';
    // RGB_IMG_print(img1, "\nimg1:\n");
    // cout << "\nprint <img1>:\n" << img1 << '\n';
    // cout << "\nprint <img2>:\n" << (img0.conv_2d(&img1));
    
    cout << "\n>>>> done >>>>\n";
    
    return 0;
}