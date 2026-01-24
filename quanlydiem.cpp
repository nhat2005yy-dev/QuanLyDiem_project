#include "quanlydiem.h"
#include "diem_input.h"
#include "diem_monhoc.h"
#include "diem_tongket.h"
#include "diem_trungbinh.h"
#include "mainwindow.h"
#include "ui_quanlydiem.h"

#include <QHBoxLayout>
#include <QList>
#include <QString>


quanlydiem::quanlydiem(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::quanlydiem)
{
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_popup);
    layout->setContentsMargins(0,0,0,0);
//    docFileLopTinChi("LopTinChi.txt");
}

quanlydiem::~quanlydiem()
{
    delete ui;
}
//đọc file
//void quanlydiem::docFileLopTinChi(QString path)
// {
//     QFile file(path);
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

//     QTextStream in(&file);

//     while (!in.atEnd()) {
//         QString line = in.readLine();
//         QStringList parts = line.split("|");

//         if (parts.size() < 9) continue;

//         LopTinChi ltc;
//         ltc.MALOPTC  = parts[0].toInt();
//         ltc.MAMH     = parts[1];
//         ltc.TENMH    = parts[2];
//         ltc.NienKhoa = parts[3];
//         ltc.Hocky    = parts[4].toInt();
//         ltc.Nhom     = parts[5].toInt();
//         ltc.sosvmin  = parts[6].toInt();
//         ltc.sosvmax  = parts[7].toInt();
//         ltc.huylop   = parts[8].toInt();

//         dsLTC.append(ltc);
//     }
//     file.close();
// }


//ấn widget popup
void quanlydiem::anTatCa()
{
    if (formNhap) formNhap->hide();
    if (formMon)  formMon->hide();
    if (formTB)   formTB->hide();
    if (formTK)   formTK->hide();
}

void quanlydiem::on_pushButton_Nhap_clicked()
{
    if (!formNhap) {
        formNhap = new diem_input(ui->widget_popup);
        ui->widget_popup->layout()->addWidget(formNhap);
    }
    anTatCa();
    formNhap->show();
}

void quanlydiem::on_pushButton_Mon_clicked()
{
    if (!formMon) {
        formMon = new diem_monhoc(ui->widget_popup);
        ui->widget_popup->layout()->addWidget(formMon);
    }
    anTatCa();
    formMon->show();
}


void quanlydiem::on_pushButton_TB_clicked()
{
//    diem_trungbinh*event = new diem_trungbinh(this);
  //  event->show();
    if (!formTB) {
        formTB = new diem_trungbinh(ui->widget_popup);
        ui->widget_popup->layout()->addWidget(formTB);
    }
    anTatCa();
    formTB->show();
}


void quanlydiem::on_pushButton_TK_clicked()
{
 //   diem_tongket*event = new diem_tongket(this);
  //  event->show();
    if (!formTK) {
        formTK = new diem_tongket(ui->widget_popup);

        QVBoxLayout *layout = new QVBoxLayout(ui->widget_popup);
        layout->setContentsMargins(0,0,0,0);
        ui->widget_popup->layout()->addWidget(formTK);
    }
    anTatCa();
    formTK->show();

}



void quanlydiem::on_pushButton_Quaylai_clicked()
{
    MainWindow*formwindow = new MainWindow();
    formwindow->show();
    this->hide();
   //parentWidget()->show();
}

