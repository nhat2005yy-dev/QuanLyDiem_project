#ifndef DIEM_TONGKET_H
#define DIEM_TONGKET_H

#include <QMainWindow>
#include <QString>

#include "diem_input.h"
#include "danhsachmonhoc.h"
#include "danhsachdangky.h"

namespace Ui {
class diem_tongket;
}

class diem_tongket : public QWidget
{
    Q_OBJECT
    struct BangDiemTK{
        float DIEM_TK;
    };

public:
    explicit diem_tongket(QWidget *parent = nullptr);
    ~diem_tongket();

private slots:
    void on_pushButton_TraTongKet_clicked();  // nút tra bảng tổng kết


private:
    Ui::diem_tongket *ui;
    void inBangDiemTongKet(QString maLop);

};

#endif // DIEM_TONGKET_H
