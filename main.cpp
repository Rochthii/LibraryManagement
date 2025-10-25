#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "include/GiaoDienSFML.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(CHIEU_RONG, CHIEU_CAO), "Quan Ly Thu Vien");
    window.setFramerateLimit(60);
    sf::Font font;

    const char *candidates[] = {"bin/DejaVuSans.ttf", "files/DejaVuSans.ttf", "arial.ttf", "C:/Windows/Fonts/arial.ttf"};
    std::string fontPath;
    for (auto c : candidates)
        if (std::filesystem::exists(c))
        {
            fontPath = c;
            break;
        }
    if (fontPath.empty())
    {
        std::cerr << "Thieu font (dat DejaVuSans.ttf vao thu muc files/ hoac bin/)\n";
        return 1;
    }

    if (!KhoiTaoGiaoDienSFML(window, font, fontPath))
        return 2;

    ChayChuongTrinhSFML(window, font);
    return 0;
}