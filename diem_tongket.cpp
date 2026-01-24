#include "diem_tongket.h"
#include "ui_diem_tongket.h"
#include "diem_input.h"
#include "danhsachmonhoc.h"
#include "DS_SinhVien.h"
#include "danhsachloptinchi.h"
#include "DS_Lop.h"

#include<QString>
#include <cstring>


diem_tongket::diem_tongket(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::diem_tongket)
{
    ui->setupUi(this);
}

diem_tongket::~diem_tongket()
{
    delete ui;
}

QStringList layDSSVTheoLop(QString maLop)
{
    QStringList ds;

    QFile file("sinhvien.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ds;

    QTextStream in(&file);
    while(!in.atEnd()){
        QStringList t = in.readLine().split("|");
        if(t.size() < 4) continue;

        if(t[0].trimmed() == maLop){
            ds << t[1].trimmed(); // masv
        }
    }
    file.close();
    return ds;
}
QString layHoTen(QString maSV)
{
    QFile file("sinhvien.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    while(!in.atEnd()){
        QStringList t = in.readLine().split("|");
        if(t.size() < 4) continue;

        if(t[1].trimmed() == maSV){
            return t[2].trimmed() + " " + t[3].trimmed();
        }
    }
    file.close();
    return "";
}
QStringList layDSMAMH()
{
    QStringList ds;

    QFile file("LopTinChi.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ds;

    QTextStream in(&file);
    while(!in.atEnd()){
        QStringList t = in.readLine().split("|");
        if(t.size() < 2) continue;

        if(!ds.contains(t[1].trimmed()))
            ds << t[1].trimmed();
    }
    file.close();
    return ds;
}
float layDiem(QString maSV, QString maMH)
{
    QString maLTC = "";

    QFile fileLTC("LopTinChi.txt");
    if(fileLTC.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&fileLTC);
        while(!in.atEnd()){
            QStringList t = in.readLine().split("|");
            if(t.size() < 2) continue;

            if(t[1].trimmed() == maMH){
                maLTC = t[0].trimmed();
                break;
            }
        }
        fileLTC.close();
    }

    if(maLTC.isEmpty())
        return -1;

    QFile fileDK("danhsachdangky.txt");
    if(fileDK.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&fileDK);
        while(!in.atEnd()){
            QStringList t = in.readLine().split("|");
            if(t.size() < 3) continue;

            if(t[0].trimmed() == maLTC &&
                t[1].trimmed() == maSV)
            {
                return t[2].toFloat();
            }
        }
        fileDK.close();
    }
    return -1;
}
void diem_tongket::inBangDiemTongKet(QString maLop)
{
    ui->BangDiemTongKet->clear();
    ui->BangDiemTongKet->setRowCount(0);
    ui->BangDiemTongKet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->BangDiemTongKet->verticalHeader()->setVisible(false);

    QStringList dsSV = layDSSVTheoLop(maLop);
    QStringList dsMH = layDSMAMH();

    if(dsSV.isEmpty()) {
        QMessageBox::information(this, "Thông báo", "Không tìm thấy lớp " + maLop);
        return; // dừng hàm, không in bảng
        ui->label_malop->setText("Lớp: không tìm thấy");
    }else{
        ui->label_malop->setText("Lớp: " + maLop);
    }

    ui->BangDiemTongKet->setColumnCount(3 + dsMH.size());
    ui->BangDiemTongKet->setColumnWidth(2, 200);  // 200 px, tùy chỉnh theo ý bạn

    ui->BangDiemTongKet->setHorizontalHeaderItem(0,new QTableWidgetItem("STT"));
    ui->BangDiemTongKet->setHorizontalHeaderItem(1,new QTableWidgetItem("Mã SV"));
    ui->BangDiemTongKet->setHorizontalHeaderItem(2,new QTableWidgetItem("Họ Tên"));

    for(int i=0;i<dsMH.size();i++)
        ui->BangDiemTongKet->setHorizontalHeaderItem(3+i,new QTableWidgetItem(dsMH[i]));

    int stt = 1;
    for(QString maSV : dsSV){
        int row = ui->BangDiemTongKet->rowCount();
        ui->BangDiemTongKet->insertRow(row);

        ui->BangDiemTongKet->setItem(row,0,new QTableWidgetItem(QString::number(stt)));
        ui->BangDiemTongKet->setItem(row,1,new QTableWidgetItem(maSV));
        ui->BangDiemTongKet->setItem(row,2,new QTableWidgetItem(layHoTen(maSV)));

        for(int i=0;i<dsMH.size();i++){
            float d = layDiem(maSV, dsMH[i]);
            ui->BangDiemTongKet->setItem(
                row, 3+i,
                new QTableWidgetItem(d<0 ? "" : QString::number(d))
                );
        }
        stt++;
    }
}

void diem_tongket::on_pushButton_TraTongKet_clicked()
{
    QString maLop = ui->lineEdit__MaLop->text().trimmed().toUpper();
    ui->label_malop->setText("Lớp: " + maLop);

    if(maLop.isEmpty()) {
        QMessageBox::warning(this,"Lỗi","Chưa nhập mã lớp");
        return;
    }
    inBangDiemTongKet(maLop);
}



