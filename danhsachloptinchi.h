#ifndef DANHSACHLOPTINCHI_H
#define DANHSACHLOPTINCHI_H

#include "danhsachmonhoc.h"

#include <QString>
#include <cstdlib>
#include <QMap>

struct nodeDK;

struct LopTinChi {
    int MALOPTC;
    QString  MAMH;
    QString TENMH;
    QString NienKhoa;
    int Hocky;
    int Nhom;
    int sosvmin;
    int sosvmax;
    bool huylop = false;
    nodeDK* dssvdk = nullptr;
};

struct nodeLopTinChi {
    LopTinChi ltc;
    nodeLopTinChi *next;
};
typedef nodeLopTinChi* PTRLTC;
//typedef nodeDK* PTRLTC;

extern PTRLTC dsLopTinChi;
extern QMap<QString, QString> dsMonHoc; // MAMH -> TENMH

void ThemLopTinChiVaoDanhSach(PTRLTC &ds, LopTinChi ltc);
void LuuDanhSachLopTinChi(PTRLTC ds, const QString &filename = "LopTinChi.txt");
void DocDanhSachLopTinChi(PTRLTC &ds, const QString &filename = "LopTinChi.txt");
void XoaDanhSachLopTinChi(PTRLTC &ds);
void DocDanhSachMonHoc(const QString &filename = "danhsachmonhoc.txt");
bool KiemTraNhomTrung(PTRLTC ds, const QString &maMH, const QString &nienKhoa, int hocKy, int nhom);
QString LayTenMonHoc(TreeMH root, const QString &maMH);
int SoLuongSinhVienDK(nodeDK* dssvdk);

#endif
