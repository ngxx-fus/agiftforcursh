## ```agiftforcursh``` project
Hi! <br>
This project is made with special name is ```agiftforcursh```. I made it for my ```non-existed crush``` :>. Finally, it just a project.

## Hardware
- ESP-WROOM32 (esp32doit-devkit-v1)
- TFT LCD Screen (2.2")
- DHT11 sensor
- Joystick
- Resistors
- LEDs
  
## Sorfware
- Visualt Studio Code
- VSCode's Extensions: PlatformIO
  
## Programming Language
- C++11
  
## Content
In this project, i has made some classes for do somethings:
- POINT<Tpoint = uint16_t> : point's object; methods: operator=, X(), Y() 
- IMAGE<Timage> : image's object; methods: operator=, image_vector(), height(), width(), fill(), crop(), pixel(), contrustors, ... 
- CANVAS<Tcanvas> : canvas's object; methods: operator=, ), H(), W(), refill(), resize(), show(), clear(), insert_text(), insert_bitmap_image(), insert_color_image(), insert_line(), contrustors, ...

CANVAS class is made for UI design, with my own fucntion for draw, process current canvas... Although I has had ```TFT 22 ILI9225``` lib of **nkawu**, but i want to made my own lib to learn and practice knowledgement about algorithm, and knowledgement related to represent geometry in matrix pixel, IoT's protocols, ... The things what i has implement from  ```TFT 22 ILI9225``` is ```drawPixel(x, y)```, ```begin()```, ```clear()``` method.
p/s: I have modified ```TFT 22 ILI9225``` lib of **nkawu** to add a function for inserting an 565-color image, [here](https://github.com/ngxx-fus/TFT_22_ILI9225_MOD.git) is the mod version.

## Demo
