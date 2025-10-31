#ifndef GIAO_DIEN_MUON_TRA_H
#define GIAO_DIEN_MUON_TRA_H

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "DocGia.h"
#include "NgayThang.h"

// Ma UI rieng cho man hinh muon tra
enum MaUI_MuonTra {
    INPUT_MATHE = 600,
    INPUT_MASACH,
    NUT_MUON,
    NUT_TRA,
    NUT_QUAYLAI_MT,
};

// Ham khoi tao va ve
void KhoiTaoManHinhMuonTra();
void VeManHinhMuonTra(sf::RenderWindow& window, const sf::Font& font);
void XuLySuKienManHinhMuonTra(sf::RenderWindow& window, sf::Event event);
void VeBangMuonTra(sf::RenderWindow& window, const sf::Font& font, float yStart);

#endif
