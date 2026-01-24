#include "diem_monhoc.h"
#include "ui_diem_monhoc.h"
#include "diem_input.h"

#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QWidget>
#include <QDir>

diem_monhoc::diem_monhoc(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::diem_monhoc)
{
    ui->setupUi(this);
}

diem_monhoc::~diem_monhoc()
{
    delete ui;
}


void diem_monhoc::inBangDiemMonHoc(QString tenMH, QString nienKhoa, int hocKy, int nhom)
{
    diem_input diemInput; // dùng object tạm để gọi hàm public
    int maloptc = diemInput.timMaLopTC(tenMH, nienKhoa, hocKy, nhom);

    if (maloptc == -1) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy lớp tín chỉ!");
        return;
    }

    PTRBangNhapDiem danhSach = diemInput.taoDanhSachNhapDiem(maloptc);
    if (!danhSach) {
        QMessageBox::warning(this, "Lỗi", "Không có sinh viên để hiển thị!");
        return;
    }

    // chuẩn bị QTableWidget
    ui->BangDiem_Widget->verticalHeader()->setVisible(false);
    ui->BangDiem_Widget->clearContents();
    ui->BangDiem_Widget->setRowCount(0);

    QStringList tieude = {"STT", "Mã SV", "Họ","Tên", "Điểm"};
    ui->BangDiem_Widget->setColumnCount(tieude.size());
    ui->BangDiem_Widget->setHorizontalHeaderLabels(tieude);
    ui->BangDiem_Widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->BangDiem_Widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->BangDiem_Widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // duyệt danh sách sinh viên
    NodeBangNhapDiem* p = danhSach;
    int row = 0;
    while(p) {
        ui->BangDiem_Widget->insertRow(row);
        ui->BangDiem_Widget->setItem(row, 0, new QTableWidgetItem(QString::number(p->data.STT)));
        ui->BangDiem_Widget->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->MASV)));
        ui->BangDiem_Widget->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->HO)));
        ui->BangDiem_Widget->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->TEN)));

        QString diemStr = (p->data.DIEM_INPUT < 0.0f) ? "chưa có" : QString::number(p->data.DIEM_INPUT);
        ui->BangDiem_Widget->setItem(row, 4, new QTableWidgetItem(diemStr));

        // KHÔNG CHO SỬA
        for(int col=0; col<5; col++)
            ui->BangDiem_Widget->item(row,col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        row++;
        p = p->next;
    }
}
//GIAO DIỆN
void diem_monhoc::on_pushButton_TraBangDiem_clicked()
{
    // lấy in4 từ lineEdit
    QString tenMH = ui->lineEdit__TenMon->text().trimmed();
    QString nienKhoa = ui->lineEdit__NienKhoa->text().trimmed();
    int hocKy = ui->lineEdit__Hocky->text().toInt();
    int nhom   = ui->lineEdit__Nhom->text().toInt();
    ui->label_topic->setText("BẢNG ĐIỂM MÔN: " + tenMH);
    ui->label_nienkhoa->setText("Niên Khóa: " + nienKhoa);
    ui->label_hocky->setText("Học Kỳ: " + QString::number(hocKy));
    ui->label_nhom->setText("Nhóm: " + QString::number(nhom));

    if(tenMH.isEmpty()) {
        QMessageBox::warning(this,"Lỗi","Chưa nhập tên môn");
        return;
    }
    if(nienKhoa.isEmpty()) {
        QMessageBox::warning(this,"Lỗi","Chưa nhập niên khóa");
        return;
    }
    if(hocKy<= 0) {
        QMessageBox::warning(this,"Lỗi","Chưa nhập học kỳ");
        return;
    }
    if(nhom<= 0) {
        QMessageBox::warning(this,"Lỗi","Chưa nhập nhóm");
        return;
    }

    inBangDiemMonHoc(tenMH,nienKhoa,hocKy,nhom);
}


