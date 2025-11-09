#define STB_IMAGE_WRITE_IMPLEMENTATION

#if defined(__clang__) || defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 26495) // champs non initialisés signalés par MSVC
#endif

#include "stb_image_write.h"

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#if defined(__clang__) || defined(__GNUC__)
  #pragma GCC diagnostic pop
#endif



#include <iostream>
#include <vector>
#include <cstdint>

#include "progressbar.h" 

#include "color.h"



int main() {
    // --- Image ---
    const int image_width  = 256;
    const int image_height = 256;
    const int channels     = 3;
    const int stride       = image_width * channels;

    std::vector<unsigned char> image_data(static_cast<size_t>(image_width) * image_height * channels);

    // --- ProgressBar : 1 update par ligne ---
    ProgressBar pb(static_cast<size_t>(image_height), "Rendering", 60);

    // --- Render ---
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {

            const double r = double(i) / (image_width  - 1);
            const double g = double(j) / (image_height - 1);
            const double b = 0.0;
            color c(
                static_cast<COLOR_TYPE>(r),
                static_cast<COLOR_TYPE>(g),
                static_cast<COLOR_TYPE>(b)
            );

            const size_t idx = static_cast<size_t>(j) * image_width * channels + static_cast<size_t>(i) * channels;
            write_color(&image_data[idx], c /*, COLOR_TYPE(2.2)*/);
        }
        // update par ligne (j va de 0 à H-1, on affiche j+1)
        pb.update(static_cast<size_t>(j + 1));
    }

    // --- Write PNG ---
    const char* out_path = "./res/output.png";
    const int ok = stbi_write_png(out_path, image_width, image_height, channels,
                                  image_data.data(), stride);

    if (!ok) {
        std::cerr << "\nErreur: écriture PNG échouée vers " << out_path
                  << " (le dossier existe ?)\n";
        return 1;
    }

    std::cout << "\nImage écrite: " << out_path << '\n';
    return 0;
}
