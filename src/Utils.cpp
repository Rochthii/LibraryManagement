#include "Utils.h"

#include <ctime>
#include <cstdlib>
#include <string>

#define MAX 9000
#define MIN 1000

int sinhMaTheNgauNhien(PTRDG root){
    srand((unsigned int)time(NULL));
    int ma;
    do{
        ma = rand() % MAX + MIN;
    }while(timDocGia(root, ma) != NULL);
    return ma;
}
string layNgayHienTai(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int ngay = ltm->tm_mday;
    int thang = ltm->tm_mon + 1;
    int nam = ltm->tm_year + 1900;
    
    return to_string(ngay)+ "/" +to_string(thang) + "/" + to_string(nam);
}
void tachngay(const string& date, int &year, int &month, int &day ){
    size_t pos1 = date.find("/");
    size_t pos2 = date.find("/", pos1 + 1);
    
    day = stoi(date.substr(0, pos1));
    month = stoi(date.substr(pos1 + 1, pos2 - pos1 - 1));
    year = stoi(date.substr(pos2 + 1));
}

time_t chuyenSangTimeT(int year, int month, int day){
    tm time_in = {};
    time_in.tm_year = year - 1900;
    time_in.tm_mon = month - 1;
    time_in.tm_mday = day;
    return mktime(&time_in);
}

bool quaHan(const MuonTra &mt, int soNgayToiDa){
    int d, m, y;
    tachngay(mt.NgayMuon, y, m, d);
    
    time_t tMuon = chuyenSangTimeT(y, m, d);
    time_t now = time(0);
    double soNgay = difftime(now, tMuon)/(60 * 60 * 24);
    
    return (soNgay > SO_NGAY_TOI_DA);
}
