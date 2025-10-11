#ifndef THONGBAO_H
#define THONGBAO_H

#include <string>
#include <iostream>

#include "Constants.h"

enum LoaiThongBao { LOI, THONG_TIN, CANH_BAO };

void thongBao(std::ostream& out, const std::string& msg, LoaiThongBao loai);
std::string thongBaoLoi(int soDong, const std::string& chiTiet, bool canhBao = false);
std::string thongBaoLoiDMS(int soDong, const std::string& chiTiet, bool canhBao = false);
void thongBaoSoLanThuConLai(int soLanDaThu, std::ostream& out, int maxLanThu = SO_LAN_THU_TOI_DA);

#endif // THONGBAO_H