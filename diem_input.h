#ifndef DIEM_INPUT_H
#define DIEM_INPUT_H

#include <QWidget>
#include <QString>
#include "danhsachloptinchi.h"
#include "DS_SinhVien.h"

struct BangNhapDiem{
    int STT;
    SinhVien *controSV;
    LopTinChi*controMaLTC;
    float DIEM_INPUT;
};

struct NodeBangNhapDiem{
    BangNhapDiem data;// (STT, sv ,DIEM_INPUT)
    NodeBangNhapDiem * next;// trỏ tới node tiếp
};
typedef NodeBangNhapDiem* PTRBangNhapDiem;

namespace Ui {
class diem_input;
}

class diem_input : public QWidget
{
    Q_OBJECT
public:
    explicit diem_input(QWidget *parent = nullptr, const QString &a="");
    ~diem_input();


    void luuDiem();
    int timMaLopTC(QString tenMH, QString nienKhoa, int hocKy, int nhom);
    PTRBangNhapDiem taoDanhSachNhapDiem(int maloptc);
    LopTinChi* timLopTinChi(int maLop);
    SinhVien* timSinhVien(QString maSV);
    NodeBangNhapDiem* danhSachNhapDiem = nullptr;
    // Tạo các chức năng giao diện
private slots:

    void on_BangDiem_Widget_cellActivated(int row, int column);

    void onNhapDiemClicked(int row);

    void onXoaDiemClicked(int row);

    void on_pushButton_TraBangDiem_clicked();

    void on_pushButton_Quaylai_clicked();

private:
    Ui::diem_input *ui;
    //NodeBangNhapDiem* danhSachNhapDiem = nullptr;
    void updateTable();
    // ===== HAM XU LY FILE =====
    //int timMaLopTC(QString tenMH, QString nienKhoa, int hocKy, int nhom);
    // SinhVien* timSinhVien(QString maSV);
    // PTRBangNhapDiem taoDanhSachNhapDiem(int maloptc);
    // void luuDiem();
    // LopTinChi* timLopTinChi(int lopTC);
};

#endif // DIEM_INPUT_H
