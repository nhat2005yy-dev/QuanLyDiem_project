#ifndef DS_SINHVIEN_H
#define DS_SINHVIEN_H

#include "DS_Lop.h"
#include <QDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <cstdlib>
#include <cstring>

struct SinhVien {
    char MASV[16];
    char HO[51];
    char TEN[15];
    char PHAI[10];
    char SODT[16];
    char NAMNHAPHOC[10];
};

struct nodeSV {
    SinhVien sv;
    nodeSV* next;
};
typedef nodeSV* PTRSV;

namespace Ui {
class DS_SV;
}

class DS_SV : public QDialog
{
    Q_OBJECT

public:
    explicit DS_SV(QWidget *parent = nullptr);
    ~DS_SV();

    void ClearDSSV();
    void LoadDSLop(const char* filename);
    void LoadDSSV(const char* filename, const char* malop);  // đọc sinh viên của lớp
    void SaveDSSV(const char* filename, const char* malop);  // lưu sinh viên của lớp
    void RefreshTable();// cập nhật TableWidget

    int TimLop(const char* malop);
    PTRSV TimSV(const char* masv);

private slots:
    void on_btnThem_clicked();
    void on_btnSua_clicked();
    void on_btnXoa_clicked();
    void on_tableSV_cellClicked(int row, int column);
    void onBtnTimKiemClicked();
    void onBtnXemLop_clicked();
    void on_btnBack_clicked();
    void ClearInput();

private:
    Ui::DS_SV *ui;
    LopSV dslop[10000];  // mảng danh sách lớp
    int nLop;    // số lớp hiện có

    PTRSV FirstSV;
    char CurrentMALOP[16];// biến lưu mã lớp đang đc chọn
};

#endif // DS_SINHVIEN_H

