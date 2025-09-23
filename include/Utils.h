#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "DocGia.h"

#define SO_NGAY_TOI_DA 7

using namespace std;

int sinhMaTheNgauNhien(PTRDG root);
string layNgayHienTai();
bool quaHan(const MuonTra &mt, int soNgayToiDa = SO_NGAY_TOI_DA);


#endif
