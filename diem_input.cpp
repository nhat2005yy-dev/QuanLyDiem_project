#include "diem_input.h"
#include "ui_diem_input.h"
#include "QHBoxLayout"
#include "DS_SinhVien.h"


#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QWidget>
#include <QDir>

using namespace std;


diem_input::diem_input(QWidget *parent,const QString &a)
    : QWidget(parent),ui(new Ui::diem_input)
{
    ui->setupUi(this);
    danhSachNhapDiem = nullptr;// tạo ds rỗng
    updateTable();//cập nhật bảng

}


diem_input::~diem_input()
{
    delete ui;
}
// -------------------- Tìm sinh viên  --------------------
SinhVien* diem_input::timSinhVien(QString maSV) {
    QFile file("sinhvien.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return nullptr;

    QTextStream in(&file);
    while(!in.atEnd()) {
        QStringList t = in.readLine().split("|");
        if(t.size()<5) continue;
        if(t[1].trimmed() == maSV.trimmed()) {
            SinhVien* sv = new SinhVien;
            strcpy(sv->MASV, t[1].toUtf8().data());
            strcpy(sv->HO, t[2].toUtf8().data());
            strcpy(sv->TEN, t[3].toUtf8().data());
            return sv;
        }
    }
    return nullptr;
}

// -------------------- Tìm mã lớp tín chỉ --------------------
int diem_input::timMaLopTC(QString tenMH, QString nienKhoa, int hocKy, int nhom)
{
    QFile file("LopTinChi.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList t = in.readLine().split("|");
        if (t.size() < 6) continue;
        if (t[2] == tenMH &&
            t[3] == nienKhoa &&
            t[4].toInt() == hocKy &&
            t[5].toInt() == nhom)

            return t[0].toInt();// trả về mã lớp
    }
    return -1;
}

// -------------------- Tìm con trỏ lớp tín chỉ từ mã lớp --------------------
LopTinChi* diem_input::timLopTinChi(int maLop)
{
    QFile file("LopTinChi.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return nullptr;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList t = in.readLine().split("|");
        if (t.size() < 6) continue;

        if (t[0].toInt() == maLop) {
            LopTinChi* lop = new LopTinChi;
            lop->MALOPTC = t[0].toInt();
            lop->MAMH = t[1];
            lop->TENMH = t[2];
            lop->NienKhoa = t[3];
            lop->Hocky = t[4].toInt();
            lop->Nhom = t[5].toInt();
            return lop;
        }
    }
    return nullptr;
}

// -------------------- Tạo danh sách nhập điểm --------------------
PTRBangNhapDiem diem_input::taoDanhSachNhapDiem(int maloptc)
{
    QFile file("danhsachdangky.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return nullptr;

    QTextStream in(&file);
    PTRBangNhapDiem head = nullptr, tail = nullptr;
    int stt = 1;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList t = line.split("|");
        if (t.size() < 4) continue;

        int maLop = t[0].toInt(); // mã lớp từ file
        QString maSV = t[1];
        float diem = t[2].toFloat(); // điểm có thể là -1, 0, >0

        if (maLop != maloptc) continue;

        SinhVien* sv = timSinhVien(maSV);
        if (!sv) {
            qDebug() << "Khong tim thay sinh vien:" << maSV;
            continue;
        }

        PTRBangNhapDiem node = new NodeBangNhapDiem;
        node->data.STT = stt++;
        node->data.controSV = sv;               // con trỏ sinh viên
        node->data.controMaLTC = timLopTinChi(maLop); // con trỏ lớp tín chỉ
        node->data.DIEM_INPUT = diem;           // giữ -1, 0 hoặc điểm thực
        node->next = nullptr;

        if (!head) head = tail = node;
        else { tail->next = node; tail = node; }
    }

    if (!head)
        qDebug() << "Khong co sinh vien nao trong maloptc:" << maloptc;

    return head;
}


//lưu điểm
void diem_input::luuDiem() {
    QFile file("danhsachdangky.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Lỗi", "Không thể mở file để đọc!");
        return;
    }

    // đọc file cũ vào danh sách
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.close();

    // cập nhật điểm
    for (int i = 0; i < lines.size(); ++i) {
        QStringList t = lines[i].split("|");
        if (t.size() < 4) continue;

        int maLop = t[0].toInt();
        QString maSV = t[1];
        QString trangThaiDangKy = t[3];  // 🔒 giữ nguyên

        NodeBangNhapDiem* p = danhSachNhapDiem;
        while (p) {
            if (p->data.controMaLTC->MALOPTC == maLop &&
                QString::fromUtf8(p->data.controSV->MASV) == maSV) {

                float diem = p->data.DIEM_INPUT;  // -1 hoặc 0-10

                lines[i] = QString("%1|%2|%3|%4")
                               .arg(maLop)
                               .arg(maSV)
                               .arg(diem)
                               .arg(trangThaiDangKy); //  KHÔNG ĐỔI
                break;
            }
            p = p->next;
        }
    }
    // ghi lại file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Lỗi", "Không thể mở file để lưu!");
        return;
    }
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();

    QMessageBox::information(this, "Lưu điểm", "Đã lưu danh sách điểm mới!");
}




//giao diện

void diem_input::on_pushButton_TraBangDiem_clicked()
{// lấy in4 từ lineEdit
    QString tenMH = ui->lineEdit__TenMon->text().trimmed();
    QString nienKhoa = ui->lineEdit__NienKhoa->text().trimmed();
    int hocKy = ui->lineEdit__Hocky->text().toInt();
    int nhom = ui->lineEdit__Nhom->text().toInt();

    int maloptc = timMaLopTC(tenMH, nienKhoa, hocKy, nhom);

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
    if (maloptc == -1) {
        QMessageBox::warning(this, "Loi", "Khong tim thay lop tin chi");
        return;
    }

    danhSachNhapDiem = taoDanhSachNhapDiem(maloptc);
    if (!danhSachNhapDiem) {
        QMessageBox::warning(this, "Lỗi", "Không có sinh viên để hiển thị");
        return;
    }
    updateTable();
}

void diem_input::updateTable() {
    ui->BangDiem_Widget->verticalHeader()->setVisible(false);  // xóa stt mđ
    ui->BangDiem_Widget->setColumnCount(6);
    QStringList tieude = {"STT", "Mã SV", "Họ","Tên", "Điểm", "Chức năng"};
    ui->BangDiem_Widget->setHorizontalHeaderLabels(tieude);
    ui->BangDiem_Widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->BangDiem_Widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->BangDiem_Widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->BangDiem_Widget->clearContents();
    ui->BangDiem_Widget->setRowCount(0);


    NodeBangNhapDiem *p = danhSachNhapDiem;
    int row = 0;

    while (p) {
        ui->BangDiem_Widget->insertRow(row);// chèn hàng
        ui->BangDiem_Widget->setItem(row, 0, new QTableWidgetItem(QString::number(p->data.STT)));
        ui->BangDiem_Widget->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        ui->BangDiem_Widget->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->MASV)));
        ui->BangDiem_Widget->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        ui->BangDiem_Widget->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->HO)));
        ui->BangDiem_Widget->item(row, 2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        ui->BangDiem_Widget->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(p->data.controSV->TEN)));
        ui->BangDiem_Widget->item(row, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        QString diemStr = (p->data.DIEM_INPUT < 0.0f) ? "chưa có": QString::number(p->data.DIEM_INPUT);
        ui->BangDiem_Widget->setItem(row, 4, new QTableWidgetItem(diemStr));
        ui->BangDiem_Widget->item(row, 4)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);       // cho phép chỉnh sửa trực tiếp cột điểm

        // tạo nút hiệu chỉnh
        QPushButton* btnNhap = new QPushButton("Nhậppp");
        QPushButton* btnXoa = new QPushButton("Xóa");
        QWidget* btnWidget = new QWidget;
        QHBoxLayout* layout = new QHBoxLayout(btnWidget);
        layout->addWidget(btnNhap);
        layout->addWidget(btnXoa);
        layout->setContentsMargins(2, 0, 2, 0);
        ui->BangDiem_Widget->setCellWidget(row, 5, btnWidget);

        //int currentRow = row;
        connect(btnNhap, &QPushButton::clicked, this, [=]() { onNhapDiemClicked(row); });
        connect(btnXoa, &QPushButton::clicked, this, [=]() { onXoaDiemClicked(row); });

        p = p->next;
        row++;
    }
    ui->BangDiem_Widget->viewport()->update(); // refresh table
}
void diem_input::onNhapDiemClicked(int row) {
    QString maSV = ui->BangDiem_Widget->item(row, 1)->text(); // lấy in4 từ ô mã sv
    QString diemStr = ui->BangDiem_Widget->item(row, 4)->text(); // lấy in4 từ ô điểm

    // nhận biết ng dùng nhập số hay chữ
    bool ok;
    float diem = diemStr.toFloat(&ok);

    if (!ok) {  // nếu không phải số
        QMessageBox::warning(this, "Lỗi", "Điểm phải là số từ 0 đến 10!");
        return;
    }

    if (diem < 0.0f || diem > 10.0f) { // kiểm tra khoảng
        QMessageBox::warning(this, "Lỗi", "Điểm phải từ 0 đến 10!");
        return;
    }

    NodeBangNhapDiem* p = danhSachNhapDiem;
    while(p) {
        if(QString::fromUtf8(p->data.controSV->MASV) == maSV){
            p->data.DIEM_INPUT = diem;
            break;
        }
        p = p->next;
    }
    QString masv = ui->BangDiem_Widget->item(row, 1)->text(); // giả sử cột 0 là mã SV
    QMessageBox::information(this,
                             "Nhap diem",
                             "Nhap diem cho " + masv + ": " + QString::number(diem));
    luuDiem();
    updateTable();
}

void diem_input::onXoaDiemClicked(int row) {
    QString maSV = ui->BangDiem_Widget->item(row, 1)->text();
    NodeBangNhapDiem* p = danhSachNhapDiem;
    while(p){
        if(QString::fromUtf8(p->data.controSV->MASV) == maSV){
            p->data.DIEM_INPUT = -1;
            break;
        }
        p = p->next;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa",
                                  "Xóa điểm của SV " + maSV + "?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
    ui->BangDiem_Widget->item(row, 4)->setText("chưa có"); // chỉ cập nhật lại cột điểm trong table
    luuDiem();
    updateTable();
    }
}




