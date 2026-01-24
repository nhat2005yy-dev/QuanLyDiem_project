#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

//#include "diem_input.h"
#include "danhsachdangky.h"
#include "DS_Lop.h"
#include "DS_SinhVien.h"
#include "danhsachloptinchi.h"
#include "danhsachmonhoc.h"
#include "quanlyloptinchi.h"
#include "quanlydiem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_DSdangky_Button_clicked()
{
    DanhSachDangKy *dsdk = new DanhSachDangKy(this);
    dsdk->show();
    this->hide();

}


void MainWindow::on_DSmonhoc_Button_clicked()
{
    DanhSachMonHoc*dsmh = new DanhSachMonHoc(this);
    dsmh->show();
    this->hide();
}



void MainWindow::on_DSsinhvien_Button_clicked()
{
    DS_SV*dssv = new DS_SV(this);
    dssv->show();
    this->hide();
}
void MainWindow::on_DSlop_Button_clicked()
{
    DS_Lop *dsl = new DS_Lop(this);
    dsl->show();
    this->hide();

}

void MainWindow::on_DSloptinchi_Button_clicked() {
    QuanLyLopTinChi*qlltc = new QuanLyLopTinChi(this);
    qlltc->show();
    this->hide();
}

void MainWindow::on_DiemInfor_Button_clicked()
{
    quanlydiem *formdiem = new quanlydiem(nullptr);
    formdiem->setWindowTitle("Điểm Infor");
    formdiem->show();
    this->hide();
}






