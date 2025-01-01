#include <bits/stdc++.h>
using namespace std;
#include <jpeglib.h>

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