#include <bits/stdc++.h>
using namespace std;
// #include <jpeglib.h>

void raise_exception(string msg = "unknown"){
    cout << "Err: " << msg << endl;
}

template<class Tvalue>
class _2d_vct{

private:
    vector< vector<Tvalue> > data;

public:

    template<class Tsize>
    _2d_vct(Tsize h = 220U, Tsize w = 172U, Tvalue val = 0U){
        if(h == 0 || w == 0) return;
        data.resize(h, vector<Tvalue>(w, val));
    }

    template<class Tsize>
    _2d_vct(vector<Tvalue> V_val, Tsize h = 220U, Tsize w = 172U){
        if(h == 0 || w == 0 || V_val.size() != w*h) {
            raise_exception("wrong size!");
            return;
        }
        data.resize(h, vector<Tvalue>(w, Tvalue(0)));
        for(Tsize r = 0; r < w; ++r){
            for(Tsize c = 0; c < h; ++c){
                this->data.at(r).at(c) = V_val.at(r * w + c);
            }
        }
    }

    int height(){
        return this->data.size();
        }

    int width(){
        if(this->height() < 1) return 0;
        return this->data.at(0).size();    
    }

    bool is_empty(){
        return (this->height() < 1 || this->width() < 1);
    }

    template<class Tsize>
    Tvalue& at(Tsize row, Tsize col){
        if(row >= height() || col >= width()) {
        	cout << "? " << "Img-col : " << width() << " Img-row : " << height() << endl;
        	cout << "? " << "col : " << col << " row : " << row << endl;
            raise_exception("invalid size!");
            exit(1);
        }
        return data.at(row).at(col);
    }

    template<class Tsize>
    _2d_vct operator = (_2d_vct o){
        this->data.resize(o.height(), vector<Tvalue>(o.width(), 0));
        for(Tsize r = 0; r < o.height(); ++r){
            for(Tsize c = 0; c < o.width(); ++c){
                this->data.at(r).at(c) = o.at(r, c);
            }
        }
        return o;
    }

    template<class Tsize>
    _2d_vct add (_2d_vct o, Tsize row_offset = 0, Tsize col_offset = 0){
        Tsize row_start = row_offset, row_stop = min(height()-1, row_offset+o.height()-1),
              col_start = col_offset, col_stop = min(width()-1,  col_offset+o.width() -1); 
        for(Tsize r = row_start; r <= row_stop; ++r){
            for(Tsize c = col_start; c <= col_stop; ++c){
                this->data.at(r).at(c) += o.at(r, c);
            }
        }
        return *this;
    }

    template<class Tsize>
    _2d_vct operator + (_2d_vct o){
        if(
            this->height() != o.height() 
            || this->width() != o.width()
        ){
            raise_exception("wrong size!");
            return *this;
        }
        for(Tsize r = 0; r < o.height(); ++r){
            for(Tsize c = 0; c < o.width(); ++c){
                o.at(r, c) += this->data.at(r).at(c);
            }
        }
        return o;
    }

    template<class Tsize>
    _2d_vct conv(_2d_vct kernel){
        if(
            this->height() > kernel.height() 
            || this->width() > kernel.width()
        ){
            raise_exception("kernel's size > img's size!");
            return *this;
        }
        _2d_vct result(this->height() - kernel.height() +1, this->width() - kernel.width() + 1);
        for( Tsize r = 0; r < result.height(); ++r){
            for( Tsize c =  0; c < result.height(); ++c){
                uint64_t pixel = 0;
                for( Tsize kr = 0; kr < kernel.height(); ++kr){
                    for(Tsize kc = 0; kc < kernel.width(); ++kc){
                        pixel += this->data.at(r+kr, c+kc)*kernel.at(kr, kc);
                    }
                }
                pixel /= (kernel.height() * kernel.width());
                result.at(r, c) = pixel;
            }
        } 
        return result;
    }

};


class img_16bit{
public:
    template<class Tsize>
    img_16bit(Tsize w = 172U, Tsize h = 220U, uint16_t color = 0){
        _16bit_img.resize(h, vector<uint16_t>(w, color));
    }
    clear(){
        _16bit_img.resize(h, vector<uint16_t>(w, color));
    }
    resize(){

    }
}

uint16_t rgb_to_rgb565(unsigned char r, unsigned char g, unsigned char b) {
    // Extract 5 bits for red, 6 bits for green, and 5 bits for blue
    uint16_t r_5 = (r >> 3) & 0x1F; // Take the upper 5 bits of red
    uint16_t g_6 = (g >> 2) & 0x3F; // Take the upper 6 bits of green
    uint16_t b_5 = (b >> 3) & 0x1F; // Take the upper 5 bits of blue

    // Combine into a single 16-bit value
    return (r_5 << 11) | (g_6 << 5) | b_5;
}

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
    std::cout << "Image dimensions: " << width << "x" << height << ", Channels: " << channels << std::endl;

    // Allocate memory for a row of pixels
    unsigned char* row_buffer = new unsigned char[width * channels];

    uint16_t r,g,b;

    // Read each row
    for (int y = 0; y < height; y++) {
        jpeg_read_scanlines(&cinfo, &row_buffer, 1);
        for (int x = 0; x < width; x++) {
            int index = x * channels;
            unsigned char red = row_buffer[index];
            unsigned char green = row_buffer[index + 1];
            unsigned char blue = row_buffer[index + 2];
            /// RGB -> 565
            uint16_t _565f = rgb_to_rgb565(red, green, blue);
            /// split into byteH byteL
            const char byte_h = (_565f&0xFF00)>>8;
            const char byte_l = _565f & 0x00FF;
            /// write to bin file
            fs.ostream::write(&byte_h, 1);
            fs.ostream::write(&byte_l, 1);

            // std::cout << "Pixel (" << y << ", " << x << "): " \
            //           << "R=" << (int)red << ", G=" << (int)green << ", B=" << (int)blue \
            //           << "\t--> 565f: " << hex << (uint16_t) (byte_h&0xFF) <<(uint16_t) (byte_l&0xFF) \
            //           << dec << std::endl;
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

    for(int i = 0; i < argc; i++)
        cout << "arg[" << i << "] : " << argv[i] << endl;
    if(argc != 3) return 0;
    readJPEG( argv[1], argv[2]);
    return 0;
}