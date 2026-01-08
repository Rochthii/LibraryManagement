#ifndef MANHINHQUANLYSACH_H
#define MANHINHQUANLYSACH_H

#include <SFML/Graphics.hpp>

// Khai báo các hàm public cho màn hình sách
void VeManHinhQuanLySach(sf::RenderWindow &window, const sf::Font &font, PTRDS dsDauSach[], int soLuongDauSach, bool &duLieuDaThayDoi);
void XuLySuKienManHinhSach(sf::RenderWindow& window, sf::Event event, PTRDS dsDauSach[], int &soLuongDauSach, bool &duLieuDaThayDoi);
void KhoiTaoManHinhSach(); 

#endif // MANHINHQUANLYSACH_H