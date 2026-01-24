#ifndef DIEM_MONHOC_H
#define DIEM_MONHOC_H

#include <QMainWindow>
#include "DS_SinhVien.h"
#include "danhsachloptinchi.h"
#include "danhsachdangky.h"
#include "danhsachmonhoc.h"
#include "diem_input.h"

namespace Ui {
class diem_monhoc;
}

class diem_monhoc : public QWidget
{
    Q_OBJECT

public:
    explicit diem_monhoc(QWidget *parent = nullptr);
    ~diem_monhoc();

private slots:

    void on_pushButton_TraBangDiem_clicked();

private:
    Ui::diem_monhoc *ui;
    PTRBangNhapDiem danhSachNhapDiem = nullptr; // lưu danh sách sinh viên
    void inBangDiemMonHoc(QString tenMH, QString nienKhoa, int hocKy, int nhom);
};

#endif // DIEM_MONHOC_H
