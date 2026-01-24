#ifndef DIEM_TRUNGBINH_H
#define DIEM_TRUNGBINH_H

#include <QMainWindow>
#include <QWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QDebug>
#include "diem_input.h" // lấy dữ liệu sinh viên và điểm

namespace Ui {
class diem_trungbinh;
}

class diem_trungbinh : public QWidget
{
    Q_OBJECT
    struct BangDiemTB{
        float DIEM_TB;
    };

public:
    explicit diem_trungbinh(QWidget *parent = nullptr);
    ~diem_trungbinh();

private slots:
    void on_pushButton_TraDiemTB_clicked();

private:
    Ui::diem_trungbinh *ui;
    void inBangDiemTBKhoa(QString malop);
    float tinhDiemTrungBinh(const char* maSV);
    diem_input diemInput;// dùng lại hàm của điểm input
};

#endif // DIEM_TRUNGBINH_H
