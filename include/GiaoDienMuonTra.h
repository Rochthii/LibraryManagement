#ifndef GIAO_DIEN_MUON_TRA_H
#define GIAO_DIEN_MUON_TRA_H

#include "Constants.h"
#include "DocGia.h"
#include "NgayThang.h"
#include <SFML/Graphics.hpp>

// All UI IDs are now defined in GiaoDienSFML.h
// No need for additional enum here

// Ham khoi tao va ve
void KhoiTaoManHinhMuonTra();
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font);
void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event);
void VeModalMuonTra(sf::RenderWindow &window, const sf::Font &font);

#endif
