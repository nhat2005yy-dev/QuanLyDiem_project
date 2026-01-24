#include "diem_trungbinh.h"
#include "ui_diem_trungbinh.h"
//ĐTB = (Điểmx tổng tín)/
diem_trungbinh::diem_trungbinh(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::diem_trungbinh)
{
    ui->setupUi(this);
}

diem_trungbinh::~diem_trungbinh()
{
    delete ui;
}

void diem_trungbinh::on_pushButton_TraDiemTB_clicked()
{
    QString maLop = ui->lineEdit__MaLop->text().trimmed().toUpper();
    if(maLop.isEmpty()){
        QMessageBox::warning(this, "Lỗi", "Chưa nhập mã lớp!");
        return;
    }

    inBangDiemTBKhoa(maLop);
}

float diem_trungbinh::tinhDiemTrungBinh(const char* maSV)
{
    float tongDiem = 0.0f;
    int tongTC = 0;

    QFile fileDK("danhsachdangky.txt");
    if(!fileDK.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QTextStream in(&fileDK);
    while(!in.atEnd()){
        QStringList t = in.readLine().split("|");
        if(t.size() < 4) continue;

        // dung MASV
        if(t[1].trimmed() != maSV) continue;

        float diem = t[2].toFloat();
        if(diem < 0) continue; // bo mon chua co diem

        QString maloptc = t[0].trimmed();
        QString mamh;

        // tim ma mon hoc
        QFile fileLTC("LopTinChi.txt");
        if(fileLTC.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream inLTC(&fileLTC);
            while(!inLTC.atEnd()){
                QStringList l = inLTC.readLine().split("|");
                if(l.size() < 2) continue;
                if(l[0].trimmed() == maloptc){
                    mamh = l[1].trimmed();
                    break;
                }
            }
            fileLTC.close();
        }

        if(mamh.isEmpty()) continue;

        int tcLT = 0, tcTH = 0;
        QFile fileMH("danhsachmonhoc.txt");
        if(fileMH.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream inMH(&fileMH);
            while(!inMH.atEnd()){
                QStringList m = inMH.readLine().split("|");
                if(m.size() < 4) continue;
                if(m[0].trimmed() == mamh){
                    tcLT = m[2].toInt();
                    tcTH = m[3].toInt();
                    break;
                }
            }
            fileMH.close();
        }

        int tc = tcLT + tcTH;
        if(tc == 0) continue;

        tongDiem += diem * tc;
        tongTC += tc;
    }
    fileDK.close();

    return (tongTC > 0) ? tongDiem / tongTC : 0;
}

void diem_trungbinh::inBangDiemTBKhoa(QString maLop)
{
    diem_input diemInput; // để dùng hàm timSinhVien

    // 1. Lọc danh sách sinh viên theo lớp
    QList<SinhVien*> danhSachSV;
    QFile fileSV("sinhvien.txt");
    if(fileSV.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&fileSV);
        while(!in.atEnd()){
            QStringList t = in.readLine().split("|");
            if(t.size() < 7) continue;
            if(t[0].trimmed() == maLop){ // lọc theo lớp
                SinhVien* sv = new SinhVien;
                strcpy(sv->MASV, t[1].toUtf8().data());
                strcpy(sv->HO, t[2].toUtf8().data());
                strcpy(sv->TEN, t[3].toUtf8().data());
                danhSachSV.append(sv);
            }
        }       
        fileSV.close();
        // sau khi đọc xong file sinhvien.txt
        if(danhSachSV.isEmpty()){
            ui->BangDiem_Widget->setRowCount(0);
            ui->BangDiem_Widget->clearContents();
            ui->label_malop->setText("Lớp: không tìm thấy");
            QMessageBox::warning(this, "Lỗi", "Không tìm thấy lớp " + maLop);
            return;
        }else{
        ui->label_malop->setText("Lớp: " + maLop);
        }

    }

    //  Setup QTableWidget
    ui->BangDiem_Widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->BangDiem_Widget->verticalHeader()->setVisible(false);
    ui->BangDiem_Widget->clearContents();
    ui->BangDiem_Widget->setRowCount(0);
    QStringList tieude = {"STT","Mã SV","Họ","Tên","Điểm TB"};
    ui->BangDiem_Widget->setColumnCount(tieude.size());
    ui->BangDiem_Widget->setHorizontalHeaderLabels(tieude);
    ui->BangDiem_Widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->BangDiem_Widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->BangDiem_Widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    int stt = 1;

    //duyệt từng sv
    for(SinhVien* sv : danhSachSV){
        float diemTB = tinhDiemTrungBinh(sv->MASV);
        // Hiển thị lên bảng
        int row = ui->BangDiem_Widget->rowCount();
        ui->BangDiem_Widget->insertRow(row);
        ui->BangDiem_Widget->setItem(row, 0, new QTableWidgetItem(QString::number(stt)));
        ui->BangDiem_Widget->setItem(row, 1, new QTableWidgetItem(sv->MASV));
        ui->BangDiem_Widget->setItem(row, 2, new QTableWidgetItem(sv->HO));
        ui->BangDiem_Widget->setItem(row, 3, new QTableWidgetItem(sv->TEN));
        ui->BangDiem_Widget->setItem(row, 4, new QTableWidgetItem(QString::number(diemTB,'f',1)));

        for(int col=0; col<5; col++)
            ui->BangDiem_Widget->item(row,col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        stt++;
        delete sv; // giải phóng bộ nhớ
    }

//    QMessageBox::information(this,"Thông báo","Đã hiển thị bảng điểm trung bình lớp " + maLop);
}
