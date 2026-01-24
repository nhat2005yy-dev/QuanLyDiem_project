#ifndef QUANLYLOPTINCHI_H
#define QUANLYLOPTINCHI_H

#include <QDialog>
#include "danhsachloptinchi.h"
#include <QHeaderView>


namespace Ui {
class QuanLyLopTinChi;
}

class QuanLyLopTinChi : public QDialog
{
    Q_OBJECT

public:
    explicit QuanLyLopTinChi(QWidget *parent = nullptr);
    ~QuanLyLopTinChi();

    void HienThiDanhSach();
    void HienThiDanhSachLoc();

private slots:
    void on_btnThem_clicked();
    void on_btnSua_clicked();
    void on_btnXoa_clicked();
    void on_btnQuayLai_clicked();

    void on_btnIn_clicked();

private:
    Ui::QuanLyLopTinChi *ui;
};

#endif
