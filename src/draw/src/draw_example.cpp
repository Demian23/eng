#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <cstdlib>
class An_Image_Window
    :
    public Fl_Window
{
public:
    An_Image_Window(int w, int h)
        :
          Fl_Window(w, h)
    {
    }
    virtual void draw()
    {
        uint8_t data[180 * 180 * 3];
        uint8_t* pixel = data;
        for (int j = 0 ; j < 180 ; ++j)
        {
            for (int i = 0 ; i < 180 ; ++i)
            {
                pixel[0] = (uint8_t)random();
                pixel[1] = (uint8_t)random();
                pixel[2] = (uint8_t)random();
                pixel += 3;
            }
        }
        fl_draw_image(data, 0, 0, 180, 180, 3);
    }
};
int main(int argc, char *argv[])
{
    An_Image_Window window(180, 180);
    window.end();
    window.show(argc, argv);
    return Fl::run();
}
