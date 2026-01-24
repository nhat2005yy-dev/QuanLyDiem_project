#ifndef DANHSACHDANGKY_H
#define DANHSACHDANGKY_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "DS_Lop.h"
#include "DS_SinhVien.h"
#include "danhsachmonhoc.h"
#include "danhsachloptinchi.h"

//extern DS_Lop* dsLop;

struct DangKy {
    char MASV[16];
    float DIEM;
    bool huydangky = false;
};

struct nodeDK {
    DangKy dk;
    nodeDK *next;
};

namespace Ui {
class DanhSachDangKy;
}

class DanhSachDangKy : public QDialog
{
    Q_OBJECT

public:
    explicit DanhSachDangKy(QWidget *parent = nullptr);
    ~DanhSachDangKy();

    TreeMH rootMH;
    //DS_Lop* quanLyLop;
    LopSV m_dslop[10000];
    int m_nLop;
    QString currentSvID;

    // === CÁC HÀM XỬ LÝ ===
    void LoadDataVaoCauTruc();
    void LoadDSLopNoiBo(const char* filename);

    // === CÁC HÀM AVL (Copy logic từ danh sách môn học sang) ===
    int height(TreeMH N);
    int max(int a, int b);
    TreeMH newNode(MonHoc mh);
    TreeMH rightRotate(TreeMH y);
    TreeMH leftRotate(TreeMH x);
    int getBalance(TreeMH N);
    TreeMH insertNode(TreeMH node, MonHoc mh);

    QString LayTenMH(TreeMH node, const char* mamh);
    void FreeTree(TreeMH &node);

    // Xử lý tìm kiếm SV (Duyệt Mảng Lớp -> List SV)
    bool TimSVTrongCauTruc(char* masv, SinhVien &svOut, char* tenLopOut);
    // Hiển thị & Lưu file
    void HienThiMonDaDangKy(QString masv);
    void LuuFileDangKy();
    void LoadFileDangKy();
    void XuLyHuyLopTuDong(QString nienkhoa, int hocky);
    // Hàm dọn dẹp điểm thấp
    void XoaDiemThapHonCuaSinhVien(char* masv, char* mamh);
    // Hàm quét toàn bộ hệ thống để dọn dẹp (Gọi khi load)
    void LocVaXoaDiemThapToanBo();

private slots:
    void on_btnLayThongTin_clicked();
    void on_btnLocLop_clicked();
    void on_btnDangKy_clicked();
    void on_btnHuyLop_clicked();
    void on_btnQuayLai_clicked();
    void on_btnHuyDangKy_clicked();

private:
    Ui::DanhSachDangKy *ui;
};

#endif // DANHSACHDANGKY_H
