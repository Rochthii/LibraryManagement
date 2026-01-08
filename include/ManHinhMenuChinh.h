#ifndef MANHINHMENUCHINH_H
#define MANHINHMENUCHINH_H

#include <SFML/Graphics.hpp>
#include <DocGia.h>

void VeMenuChinhSFML(sf::RenderWindow& window, const sf::Font& font);
void XuLySuKienMenuChinh(sf::RenderWindow &window, sf::Event event, PTRDG rootDocGia, PTRDS dsDauSach[], int soLuongDauSach);

#endif // MANHINHMENUCHINH_H