#include "danhsachloptinchi.h"
#include "danhsachdangky.h" // Để hiểu nodeDK

#include <QFile>
#include <QTextStream>

PTRLTC dsLopTinChi = nullptr;
QMap<QString, QString> dsMonHoc;
int MALOPTC_TIEPTHEO = 1;

//ktra xem lớp đó có trùng nhóm không, 1 lớp không thể có 2 nhóm 1
bool KiemTraNhomTrung(PTRLTC ds, const QString &maMH, const QString &nienKhoa, int hocKy, int nhom) {
    PTRLTC p = ds;
    while (p != nullptr) {//dùng vòng while đi qua từng lớp 1 ktra điều kiện dưới
        if (p->ltc.MAMH == maMH &&
            p->ltc.NienKhoa == nienKhoa &&
            p->ltc.Hocky == hocKy &&
            p->ltc.Nhom == nhom &&
            !p->ltc.huylop) {
            return true;
        }
        p = p->next;
    }
    return false;
}

void ThemLopTinChiVaoDanhSach(PTRLTC &ds, const LopTinChi ltc) {
    PTRLTC p = new nodeLopTinChi;//cấp phát vùng nhớ mới
    p->ltc = ltc;//đổ toàn bộ dữ liệu vào vùng nhớ vừa cấp
    p->next = ds;//node mới trỏ đến người đang đứng đầu ds hiện tại
    ds = p;// bây giờ ds bắt đầu bằng node vừa thêm
}

//hàm lưu file vào txt
void LuuDanhSachLopTinChi(PTRLTC ds, const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    PTRLTC p = ds;
    while (p) {
        out << p->ltc.MALOPTC << "|"
            << p->ltc.MAMH << "|"
            << p->ltc.TENMH << "|"
            << p->ltc.NienKhoa << "|"
            << p->ltc.Hocky << "|"
            << p->ltc.Nhom << "|"
            << p->ltc.sosvmin << "|"
            << p->ltc.sosvmax << "|"
            << p->ltc.huylop << "\n";
        p = p->next;
    }
    file.close();
}

//hàm đọc file txt
void DocDanhSachLopTinChi(PTRLTC &ds, const QString &filename) {
    XoaDanhSachLopTinChi(ds);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if (parts.size() < 9) continue;

        LopTinChi ltc;
        ltc.MALOPTC = parts[0].toInt();
        ltc.MAMH = parts[1].trimmed();
        ltc.TENMH = parts[2].trimmed();
        ltc.NienKhoa = parts[3].trimmed();
        ltc.Hocky = parts[4].trimmed().toInt();
        ltc.Nhom = parts[5].toInt();
        ltc.sosvmin = parts[6].toInt();
        ltc.sosvmax = parts[7].toInt();
        ltc.huylop = (parts[8].toInt() == 1);
        ltc.dssvdk = nullptr;

        if (ltc.MALOPTC >= MALOPTC_TIEPTHEO) MALOPTC_TIEPTHEO = ltc.MALOPTC + 1;
        ThemLopTinChiVaoDanhSach(ds, ltc);
    }
    file.close();
}

void XoaDanhSachLopTinChi(PTRLTC &ds) {
    while (ds != nullptr) {
        PTRLTC temp = ds; //giữ lại địa chỉ node đầu tiên
        ds = ds->next;    //cho ds nhảy sang node tiếp theo
        delete temp;      //xóa node đầu tiên vừa giữ lại
    }
}

//đọc ds môn học để thêm lớp
void DocDanhSachMonHoc(const QString &filename) {
    dsMonHoc.clear();
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if(parts.size() < 2) continue;
        dsMonHoc[parts[0].trimmed()] = parts[1].trimmed();// sử dụng mã môn làm key, tên môn là value
    }
    file.close();
}

//tìm tên môn học dựa trên mã môn
QString LayTenMonHoc(TreeMH root, const QString &maMH) {
    if (!root) return "";
    QString ma = QString(root->mh.MAMH);
    if (ma == maMH) return QString(root->mh.TENMH);
    if (maMH < ma) return LayTenMonHoc(root->left, maMH);
    return LayTenMonHoc(root->right, maMH);
}

//đếm số lượng sv đki
int SoLuongSinhVienDK(nodeDK* dssvdk) {
    int count = 0;
    nodeDK* p = dssvdk;
    while (p) { count++; p = p->next; }
    return count;
}
