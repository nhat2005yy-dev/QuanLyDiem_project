#ifndef QUANLYDIEM_H
#define QUANLYDIEM_H

#include <QMainWindow>
#include "diem_input.h"
#include "diem_monhoc.h"
#include "diem_tongket.h"
#include "diem_trungbinh.h"
#include "danhsachloptinchi.h"

namespace Ui {
class quanlydiem;
}

class quanlydiem : public QMainWindow
{
    Q_OBJECT

public:
    explicit quanlydiem(QWidget *parent = nullptr);
    ~quanlydiem();

private slots:
    void on_pushButton_Nhap_clicked();

    void on_pushButton_Mon_clicked();

    void on_pushButton_TB_clicked();

    void on_pushButton_TK_clicked();

    void on_pushButton_Quaylai_clicked();

private:
    Ui::quanlydiem *ui;
    diem_input*formNhap = nullptr;
    diem_monhoc*formMon = nullptr;
    diem_tongket*formTK = nullptr;
    diem_trungbinh*formTB = nullptr;
    PTRSV dsSV = nullptr;       // danh sách sinh viên
    PTRLTC dsLTC = nullptr;     // danh sách lớp tín chỉ
    void anTatCa();
    void docFileLopTinChi();
};

#endif // QUANLYDIEM_H
