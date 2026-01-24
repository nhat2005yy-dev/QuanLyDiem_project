#include "danhsachdangky.h"
#include "ui_danhsachdangky.h"
#include <cstring>
#include <iostream>
#include <QMessageBox>

// Biến toàn cục từ file khác (đã load từ LopTinChi.txt)
extern PTRLTC dsLopTinChi;

DanhSachDangKy::DanhSachDangKy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DanhSachDangKy)
{
    ui->setupUi(this);

    // Khởi tạo các biến
    rootMH = nullptr;
    m_nLop = 0;
    currentSvID = "";

    // Load toàn bộ dữ liệu vào RAM ngay khi mở form
    LoadDataVaoCauTruc();

    // === CẤU HÌNH BẢNG (Behavior) ===
    ui->tableLopTinChi->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableLopTinChi->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableDangKySV->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableDangKySV->setSelectionBehavior(QAbstractItemView::SelectRows);
}

DanhSachDangKy::~DanhSachDangKy()
{
    FreeTree(rootMH);
    // Giải phóng bộ nhớ mảng lớp nội bộ
    for(int i = 0; i < m_nLop; i++) {
        PTRSV p = m_dslop[i].FirstSV;
        while(p) {
            PTRSV temp = p;
            p = p->next;
            delete temp;
        }
    }
    delete ui;
}

// ==================== AVL ALGORITHMS ====================

int DanhSachDangKy::height(TreeMH N) { return (N == nullptr) ? 0 : N->height; }
int DanhSachDangKy::max(int a, int b) { return (a > b) ? a : b; }

TreeMH DanhSachDangKy::newNode(MonHoc mh) {
    TreeMH node = new nodeMH;
    node->mh = mh;
    node->left = node->right = nullptr;
    node->height = 1;
    return node;
}

TreeMH DanhSachDangKy::rightRotate(TreeMH y) {
    TreeMH x = y->left;
    TreeMH T2 = x->right;
    x->right = y; y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

TreeMH DanhSachDangKy::leftRotate(TreeMH x) {
    TreeMH y = x->right;
    TreeMH T2 = y->left;
    y->left = x; x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int DanhSachDangKy::getBalance(TreeMH N) {
    return (N == nullptr) ? 0 : height(N->left) - height(N->right);
}

// Hàm thêm node chuẩn AVL
TreeMH DanhSachDangKy::insertNode(TreeMH node, MonHoc mh) {
    if (node == nullptr) return newNode(mh);

    int cmp = strcmp(mh.MAMH, node->mh.MAMH);
    if (cmp < 0) node->left = insertNode(node->left, mh);
    else if (cmp > 0) node->right = insertNode(node->right, mh);
    else return node; // Không thêm trùng

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    if (balance > 1 && strcmp(mh.MAMH, node->left->mh.MAMH) < 0) return rightRotate(node);
    if (balance < -1 && strcmp(mh.MAMH, node->right->mh.MAMH) > 0) return leftRotate(node);
    if (balance > 1 && strcmp(mh.MAMH, node->left->mh.MAMH) > 0) {
        node->left = leftRotate(node->left); return rightRotate(node);
    }
    if (balance < -1 && strcmp(mh.MAMH, node->right->mh.MAMH) < 0) {
        node->right = rightRotate(node->right); return leftRotate(node);
    }
    return node;
}
// ========================================================================

void DanhSachDangKy::FreeTree(TreeMH &node) {
    if (node) { FreeTree(node->left); FreeTree(node->right); delete node; node = nullptr; }
}

QString DanhSachDangKy::LayTenMH(TreeMH node, const char* mamh) {
    if (!node) return "Unknown";
    int cmp = strcmp(mamh, node->mh.MAMH);
    if (cmp == 0) return QString::fromUtf8(node->mh.TENMH);
    if (cmp < 0) return LayTenMH(node->left, mamh);
    return LayTenMH(node->right, mamh);
}

void DanhSachDangKy::LoadDSLopNoiBo(const char* filename) {
    m_nLop = 0;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd() && m_nLop < 10000) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if(parts.size() < 3) continue;
        strncpy(m_dslop[m_nLop].MALOP, parts[0].trimmed().toStdString().c_str(), 15);
        strncpy(m_dslop[m_nLop].TENLOP, parts[1].trimmed().toStdString().c_str(), 50);
        m_dslop[m_nLop].FirstSV = nullptr;
        m_nLop++;
    }
    file.close();
}


// === LOAD DỮ LIỆU ===
void DanhSachDangKy::LoadDataVaoCauTruc() {
    // 1. Môn học
    FreeTree(rootMH);
    QFile fMH("danhsachmonhoc.txt");
    if(fMH.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fMH);
        while(!in.atEnd()) {
            QStringList parts = in.readLine().split("|");
            if(parts.size() >= 2) {
                MonHoc mh;
                strncpy(mh.MAMH, parts[0].trimmed().toStdString().c_str(), 10);
                strncpy(mh.TENMH, parts[1].trimmed().toStdString().c_str(), 50);
                rootMH = insertNode(rootMH, mh);
            }
        }
        fMH.close();
    }

    // 2. Lớp & Sinh viên
    LoadDSLopNoiBo("dslopsv.txt");
    QFile fSV("sinhvien.txt");
    if(fSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fSV);
        while(!in.atEnd()) {
            QStringList parts = in.readLine().split("|");
            if(parts.size() >= 7) {
                QString maLop = parts[0].trimmed();
                for(int i=0; i<m_nLop; i++) {
                    if(strcmp(m_dslop[i].MALOP, maLop.toStdString().c_str()) == 0) {
                        PTRSV p = new nodeSV;
                        strncpy(p->sv.MASV, parts[1].toStdString().c_str(), 15);
                        strncpy(p->sv.HO, parts[2].toStdString().c_str(), 50);
                        strncpy(p->sv.TEN, parts[3].toStdString().c_str(), 10);
                        strncpy(p->sv.PHAI, parts[4].toStdString().c_str(), 9);
                        strncpy(p->sv.SODT, parts[5].toStdString().c_str(), 15);
                        p->next = m_dslop[i].FirstSV;
                        m_dslop[i].FirstSV = p;
                        break;
                    }
                }
            }
        }
        fSV.close();
    }

    // 3. Lớp Tín Chỉ
    DocDanhSachLopTinChi(dsLopTinChi);

    // 4. Load file đăng ký
    LoadFileDangKy();
}

bool DanhSachDangKy::TimSVTrongCauTruc(char* masv, SinhVien &svOut, char* tenLopOut) {
    for(int i = 0; i < m_nLop; i++) {
        PTRSV p = m_dslop[i].FirstSV;
        while(p) {
            if(strcmp(p->sv.MASV, masv) == 0) {
                svOut = p->sv;
                strcpy(tenLopOut, m_dslop[i].MALOP);
                return true;
            }
            p = p->next;
        }
    }
    return false;
}

// === HIỂN THỊ THÔNG TIN SV ===
void DanhSachDangKy::on_btnLayThongTin_clicked()
{
    QString qMasv = ui->editMASV->text().trimmed().toUpper();
    if (qMasv.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Nhập Mã SV!"); return; }
    char masv[16]; strncpy(masv, qMasv.toStdString().c_str(), 15); masv[15] = '\0';
    SinhVien sv; char tenLop[51];

    if (TimSVTrongCauTruc(masv, sv, tenLop)) {
        currentSvID = QString::fromUtf8(sv.MASV);
        ui->lblMASV->setText("Mã SV: " + currentSvID);
        ui->lblHOTEN->setText("Họ tên: " + QString::fromUtf8(sv.HO) + " " + QString::fromUtf8(sv.TEN));
        ui->lblMALOP->setText("Lớp: " + QString::fromUtf8(tenLop));
        ui->lblPHAI->setText("Phái: " + QString::fromUtf8(sv.PHAI));
        if(ui->lblSoDT) ui->lblSoDT->setText("Số ĐT: " + QString::fromUtf8(sv.SODT));
        ui->editMASV->clear();

        HienThiMonDaDangKy(currentSvID);
    } else {
        QMessageBox::warning(this, "Thông báo", "Sinh viên không tồn tại!");
    }
}

// === HIỂN THỊ CÁC MÔN ĐÃ ĐĂNG KÝ ===
void DanhSachDangKy::HienThiMonDaDangKy(QString masv)
{
    ui->tableDangKySV->setRowCount(0);
    PTRLTC p = dsLopTinChi;
    int row = 0;

    while (p != nullptr) {
        nodeDK* k = p->ltc.dssvdk;
        while (k != nullptr) {
            // Chỉ hiển thị môn của SV này VÀ chưa bị hủy
            if (strcmp(k->dk.MASV, masv.toStdString().c_str()) == 0 && k->dk.huydangky == false) {
                ui->tableDangKySV->insertRow(row);

                QTableWidgetItem* itemMaMH = new QTableWidgetItem(p->ltc.MAMH);
                itemMaMH->setData(Qt::UserRole, p->ltc.MALOPTC);
                ui->tableDangKySV->setItem(row, 0, itemMaMH);

                QString tenMH = p->ltc.TENMH;
                if(tenMH.isEmpty()) tenMH = LayTenMH(rootMH, p->ltc.MAMH.toStdString().c_str());
                ui->tableDangKySV->setItem(row, 1, new QTableWidgetItem(tenMH));

                ui->tableDangKySV->setItem(row, 2, new QTableWidgetItem(QString::number(p->ltc.Nhom)));
                ui->tableDangKySV->setItem(row, 3, new QTableWidgetItem(p->ltc.NienKhoa));
                ui->tableDangKySV->setItem(row, 4, new QTableWidgetItem(QString::number(p->ltc.Hocky)));

                // Kiểm tra nếu điểm < 0 thì ghi "Chưa có"
                if (k->dk.DIEM < 0) {
                    ui->tableDangKySV->setItem(row, 5, new QTableWidgetItem("Chưa có"));
                } else {
                    ui->tableDangKySV->setItem(row, 5, new QTableWidgetItem(QString::number(k->dk.DIEM)));
                }

                QTableWidgetItem* itemCheck = new QTableWidgetItem();
                itemCheck->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                itemCheck->setCheckState(Qt::Unchecked);
                ui->tableDangKySV->setItem(row, 6, itemCheck);

                // Cột 7: Trạng thái lớp (MỚI)
                QString trangThaiLop;
                QTableWidgetItem* itemStatus;

                if (p->ltc.huylop) {
                    trangThaiLop = "Lớp bị hủy";
                    itemStatus = new QTableWidgetItem(trangThaiLop);
                    itemStatus->setForeground(Qt::red); // Tô đỏ báo động
                } else {
                    trangThaiLop = "Mở";
                    itemStatus = new QTableWidgetItem(trangThaiLop);
                }
                ui->tableDangKySV->setItem(row, 7, itemStatus);

                row++;
                break;
            }
            k = k->next;
        }
        p = p->next;
    }
}
// === LỌC LỚP ĐỂ ĐĂNG KÝ ===
void DanhSachDangKy::on_btnLocLop_clicked()
{
    QString nk = ui->editNienKhoa->text().trimmed();
    int hk = ui->editHocKy->text().toInt();

    if (nk.isEmpty() || hk <= 0) {
        QMessageBox::warning(this, "Lỗi", "Nhập Niên khóa và Học kỳ!");
        return;
    }

    ui->tableLopTinChi->setRowCount(0);
    PTRLTC p = dsLopTinChi;
    int row = 0;

    while (p != nullptr) {
        if (p->ltc.NienKhoa.compare(nk, Qt::CaseInsensitive) == 0 &&
            p->ltc.Hocky == hk) {

            // Xóa vòng lặp thừa, chỉ đếm SV chưa hủy
            int count = 0;
            nodeDK* k = p->ltc.dssvdk;
            while(k) {
                if (!k->dk.huydangky) {
                    count++;
                }
                k = k->next;
            }
            // --------------------------------------------------

            int slotCon = p->ltc.sosvmax - count;

            ui->tableLopTinChi->insertRow(row);

            QTableWidgetItem* itemMaMH = new QTableWidgetItem(p->ltc.MAMH);
            itemMaMH->setData(Qt::UserRole, p->ltc.MALOPTC);
            ui->tableLopTinChi->setItem(row, 0, itemMaMH);

            QString tenMH = p->ltc.TENMH;
            if(tenMH.isEmpty()) tenMH = LayTenMH(rootMH, p->ltc.MAMH.toStdString().c_str());
            ui->tableLopTinChi->setItem(row, 1, new QTableWidgetItem(tenMH));

            ui->tableLopTinChi->setItem(row, 2, new QTableWidgetItem(QString::number(p->ltc.Nhom)));
            ui->tableLopTinChi->setItem(row, 3, new QTableWidgetItem(QString::number(count)));
            ui->tableLopTinChi->setItem(row, 4, new QTableWidgetItem(QString::number(slotCon)));

            QTableWidgetItem* itemCheck = new QTableWidgetItem();
            itemCheck->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            itemCheck->setCheckState(Qt::Unchecked);
            // [SỬA 2] Xử lý hiển thị Trạng thái và Khóa Checkbox
            QString trangThaiStr;
            if (p->ltc.huylop) {
                trangThaiStr = "Bị hủy";
                // Nếu lớp bị hủy -> Khóa checkbox, hiện chữ X
                itemCheck->setFlags(Qt::NoItemFlags);
                itemCheck->setText("X");
            } else {
                trangThaiStr = "Mở";
                // Nếu lớp mở nhưng hết chỗ -> Cũng khóa checkbox
                if (slotCon <= 0) {
                    itemCheck->setFlags(Qt::NoItemFlags);
                    itemCheck->setText("Full");
                }
            }

            ui->tableLopTinChi->setItem(row, 5, itemCheck);

            // Cột 6: Trạng thái (Cột mới thêm)
            QTableWidgetItem* itemStatus = new QTableWidgetItem(trangThaiStr);
            if(p->ltc.huylop) itemStatus->setForeground(Qt::red); // Tô đỏ cho dễ nhìn
            ui->tableLopTinChi->setItem(row, 6, itemStatus);

            row++;
        }
        p = p->next;
    }
    if (row == 0) QMessageBox::information(this, "Thông báo", "Không tìm thấy lớp nào.");
}

// === XỬ LÝ NÚT ĐĂNG KÝ ===
void DanhSachDangKy::on_btnDangKy_clicked()
{
    if (currentSvID.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Chưa tìm SV!"); return; }
    bool hasSuccess = false;

    for (int i = 0; i < ui->tableLopTinChi->rowCount(); i++) {
        if (ui->tableLopTinChi->item(i, 5)->checkState() == Qt::Checked) {
            int maLTC = ui->tableLopTinChi->item(i, 0)->data(Qt::UserRole).toInt();
            QString maMH = ui->tableLopTinChi->item(i, 0)->text();

            PTRLTC p = dsLopTinChi;
            while (p && p->ltc.MALOPTC != maLTC) p = p->next;

            if (p) {
                // Kiểm tra lớp hủy trước tiên
                if (p->ltc.huylop) {
                    QMessageBox::critical(this, "Lỗi", "Lớp " + maMH + " đã bị hủy, không thể đăng ký!");
                    ui->tableLopTinChi->item(i, 5)->setCheckState(Qt::Unchecked);
                    continue; // Bỏ qua lớp này
                }
                // --- KIỂM TRA 2: Check trùng môn ở NHÓM KHÁC (Quan trọng) ---
                // Sinh viên không được học cùng 1 môn ở 2 nhóm khác nhau trong cùng HK
                bool trungMonO_NhomKhac = false;
                PTRLTC temp = dsLopTinChi;
                while(temp) {
                    // Cùng Niên khóa, Học kỳ, Mã môn NHƯNG Khác Mã Lớp TC (tức là khác Nhóm)
                    if(temp->ltc.NienKhoa == p->ltc.NienKhoa &&
                        temp->ltc.Hocky == p->ltc.Hocky &&
                        temp->ltc.MAMH == p->ltc.MAMH &&
                        temp->ltc.MALOPTC != p->ltc.MALOPTC) {

                        // Kiểm tra SV có đang học (chưa hủy) bên nhóm kia không
                        nodeDK* sv = temp->ltc.dssvdk;
                        while(sv) {
                            if(strcmp(sv->dk.MASV, currentSvID.toStdString().c_str()) == 0 && sv->dk.huydangky == false) {
                                trungMonO_NhomKhac = true;
                                break;
                            }
                            sv = sv->next;
                        }
                    }
                    if(trungMonO_NhomKhac) break;
                    temp = temp->next;
                }

                if(trungMonO_NhomKhac) {
                    QMessageBox::warning(this, "Lỗi", "Bạn đã đăng ký môn " + maMH + " ở nhóm khác rồi!");
                    ui->tableLopTinChi->item(i, 5)->setCheckState(Qt::Unchecked);
                    continue;
                }

                // --- ĐẾM SỐ LƯỢNG SINH VIÊN HIỆN TẠI (Chỉ đếm những người chưa hủy) ---
                int count = 0;
                nodeDK* k = p->ltc.dssvdk;
                while(k) {
                    if (!k->dk.huydangky) count++;
                    k = k->next;
                }

                if(count >= p->ltc.sosvmax) {
                    QMessageBox::warning(this, "Lỗi", "Lớp " + maMH + " đã đầy!");
                    continue;
                }

                // --- KIỂM TRA 3: SV ĐÃ TỪNG CÓ TÊN TRONG LỚP NÀY CHƯA? ---
                bool daCoTenTrongDanhSach = false;
                k = p->ltc.dssvdk;
                while (k != nullptr) {
                    if (strcmp(k->dk.MASV, currentSvID.toStdString().c_str()) == 0) {
                        daCoTenTrongDanhSach = true;

                        // Trường hợp 3a: Đang học bình thường -> Báo trùng
                        if (k->dk.huydangky == false) {
                            QMessageBox::warning(this, "Thông báo", "Bạn đã có trong danh sách lớp này rồi.");
                        }
                        // Trường hợp 3b: Đã hủy -> Đăng ký lại (RE-ACTIVATE)
                        else {
                            k->dk.huydangky = false;
                            // LƯU Ý: Không gán lại DIEM = -1.0
                            // Để nguyên DIEM cũ (nếu trước đó đã có điểm thì giữ nguyên, nếu chưa có thì nó vẫn là -1)
                            hasSuccess = true;
                        }
                        break;
                    }
                    k = k->next;
                }

                // --- KIỂM TRA 4: NGƯỜI MỚI HOÀN TOÀN -> THÊM NODE MỚI ---
                if (!daCoTenTrongDanhSach) {
                    nodeDK* newNode = new nodeDK;
                    strncpy(newNode->dk.MASV, currentSvID.toStdString().c_str(), 15);
                    newNode->dk.DIEM = -1.0; // Người mới tinh thì điểm là -1 (Chưa có)
                    newNode->dk.huydangky = false;

                    // Chèn vào đầu danh sách
                    newNode->next = p->ltc.dssvdk;
                    p->ltc.dssvdk = newNode;
                    hasSuccess = true;
                }
            }
        }
    }

    if (hasSuccess) {
        LuuFileDangKy(); // Lưu file ngay lập tức
        QMessageBox::information(this, "Thành công", "Đăng ký thành công!");

        // Refresh lại giao diện
        on_btnLocLop_clicked();       // Cập nhật lại số slot trống trên bảng trên
        HienThiMonDaDangKy(currentSvID); // Cập nhật lại bảng dưới
    }
}

// === XỬ LÝ NÚT HỦY ĐĂNG KÝ (SINH VIÊN) ===
void DanhSachDangKy::on_btnHuyDangKy_clicked()
{
    if (currentSvID.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Chưa chọn SV!"); return; }

    bool hasChanged = false;
    for(int i=0; i<ui->tableDangKySV->rowCount(); i++) {
        if(ui->tableDangKySV->item(i, 6)->checkState() == Qt::Checked) {

            int maLTC = ui->tableDangKySV->item(i, 0)->data(Qt::UserRole).toInt();
            PTRLTC p = dsLopTinChi;
            while(p && p->ltc.MALOPTC != maLTC) p=p->next;

            if(p) {
                nodeDK* k = p->ltc.dssvdk;
                while(k) {
                    if(strcmp(k->dk.MASV, currentSvID.toStdString().c_str())==0) {
                        if (k->dk.huydangky == false) {
                            k->dk.huydangky = true;
                            hasChanged = true;
                        }
                        break;
                    }
                    k = k->next;
                }
            }
        }
    }

    if(hasChanged) {
        LuuFileDangKy();
        QMessageBox::information(this, "Thành công", "Đã hủy môn học!");
        on_btnLocLop_clicked();
        HienThiMonDaDangKy(currentSvID);
    } else {
        QMessageBox::information(this, "Thông báo", "Bạn chưa chọn môn nào để hủy.");
    }
}

// === HÀM XỬ LÝ HỌC LẠI / HỌC CẢI THIỆN ===
// Logic: Tìm tất cả các lần học của SV cho 1 môn. Giữ điểm cao nhất. Xóa các điểm thấp hơn.
void DanhSachDangKy::LocVaXoaDiemThapToanBo()
{
    // Duyệt qua tất cả các lớp tín chỉ
    PTRLTC p = dsLopTinChi;
    while(p != nullptr) {
        nodeDK* k = p->ltc.dssvdk;
        while(k != nullptr) {
            // Với mỗi sinh viên k trong lớp p
            // Gọi hàm check riêng cho cặp (MASV, MAMH) này
            if(!k->dk.huydangky) { // Chỉ check nếu chưa hủy
                XoaDiemThapHonCuaSinhVien(k->dk.MASV, p->ltc.MAMH.toStdString().data());
            }
            k = k->next;
        }
        p = p->next;
    }
}

void DanhSachDangKy::XoaDiemThapHonCuaSinhVien(char* masv, char* mamh)
{
    float maxDiem = -2.0;

    // BƯỚC 1: TÌM ĐIỂM CAO NHẤT CỦA SV ĐỐI VỚI MÔN HỌC NÀY
    PTRLTC p = dsLopTinChi;
    while(p != nullptr) {
        // Chỉ xét các lớp có cùng Mã Môn Học
        if(strcmp(p->ltc.MAMH.toStdString().c_str(), mamh) == 0 && !p->ltc.huylop) {
            nodeDK* k = p->ltc.dssvdk;
            while(k != nullptr) {
                if(strcmp(k->dk.MASV, masv) == 0 && !k->dk.huydangky) {
                    // Nếu điểm > maxDiem, cập nhật (Lưu ý: bỏ qua điểm -1 nếu đã có điểm thật)
                    if(k->dk.DIEM > maxDiem) {
                        maxDiem = k->dk.DIEM;
                    }
                }
                k = k->next;
            }
        }
        p = p->next;
    }

    // Nếu SV chưa có điểm nào (toàn -1) hoặc điểm thấp (vd: 0) mà chưa có điểm cao hơn
    // Thì không cần xóa gì cả. Chỉ xóa khi maxDiem là một điểm đậu hoặc điểm cao.
    // Logic của bạn: "nếu như đã có điểm rồi... xóa hết môn trùng thấp hơn"

    if(maxDiem < 0) return; // Chưa có điểm thực (đang học), không xóa.

    // BƯỚC 2: XÓA CÁC NODE CÓ ĐIỂM THẤP HƠN MAXDIEM
    p = dsLopTinChi;
    while(p != nullptr) {
        if(strcmp(p->ltc.MAMH.toStdString().c_str(), mamh) == 0 && !p->ltc.huylop) {

            nodeDK* prev = nullptr;
            nodeDK* curr = p->ltc.dssvdk;

            while(curr != nullptr) {
                bool canDelete = false;

                // Tìm thấy SV trong lớp này
                if(strcmp(curr->dk.MASV, masv) == 0 && !curr->dk.huydangky) {

                    // Logic xóa:
                    // 1. Nếu điểm của node này NHỎ HƠN maxDiem -> Xóa (giữ lại cái cao nhất)
                    // 2. Không xóa nếu điểm là -1.0 (Đang học cải thiện, chưa có kết quả mới)
                    //    Tức là: Nếu tôi được 5.0, tôi đk học lại (điểm -1). Thì giữ cả 2.
                    //    Khi nào điểm -1 cập nhật thành 8.0 -> Thì lúc đó mới gọi hàm này để xóa con 5.0 đi.
                    //    Nếu điểm -1 cập nhật thành 4.0 -> Thì lúc đó xóa con 4.0 đi, giữ con 5.0.
                    if(curr->dk.DIEM >= 0 && curr->dk.DIEM < maxDiem) {
                        canDelete = true;
                    }
                }
                if(canDelete) {
                    // Thực hiện xóa node khỏi danh sách liên kết đơn
                    nodeDK* temp = curr;
                    if(prev == nullptr) {
                        p->ltc.dssvdk = curr->next;
                        curr = p->ltc.dssvdk;
                    } else {
                        prev->next = curr->next;
                        curr = prev->next;
                    }
                    delete temp; // Giải phóng bộ nhớ
                    // Đã xóa xong node này, không cần tăng prev, curr đã trỏ sang cái mới
                } else {
                    prev = curr;
                    curr = curr->next;
                }
            }
        }
        p = p->next;
    }
}

// === FILE I/O ===
void DanhSachDangKy::LuuFileDangKy()
{
    QFile file("danhsachdangky.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);

    PTRLTC p = dsLopTinChi;
    while (p != nullptr) {
        nodeDK* k = p->ltc.dssvdk;
        while (k != nullptr) {
            // Tách lệnh in để tránh lỗi cú pháp
            out << p->ltc.MALOPTC << "|";
            out << k->dk.MASV << "|";
            out << k->dk.DIEM << "|";

            if (k->dk.huydangky == true) {
                out << "1";
            } else {
                out << "0";
            }
            out << "\n";
            k = k->next;
        }
        p = p->next;
    }
    file.close();
}

void DanhSachDangKy::LoadFileDangKy()
{
    QFile file("danhsachdangky.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        // Format: MALOPTC | MASV | DIEM | HUY
        if (parts.size() >= 3) {
            int maLTC = parts[0].toInt();
            QString maSV = parts[1].trimmed();
            float diem = parts[2].toFloat();

            bool isHuy = (parts.size() > 3) ? (parts[3].toInt() == 1) : false;

            PTRLTC p = dsLopTinChi;
            while (p != nullptr) {
                if (p->ltc.MALOPTC == maLTC) {
                    bool exist = false;
                    nodeDK* k = p->ltc.dssvdk;
                    while(k) {
                        if(strcmp(k->dk.MASV, maSV.toStdString().c_str()) == 0) {
                            exist = true; break;
                        }
                        k = k->next;
                    }

                    if(!exist) {
                        nodeDK* nn = new nodeDK;
                        strncpy(nn->dk.MASV, maSV.toStdString().c_str(), 15);
                        nn->dk.DIEM = diem;
                        nn->dk.huydangky = isHuy;
                        nn->next = p->ltc.dssvdk;
                        p->ltc.dssvdk = nn;
                    }
                    break;
                }
                p = p->next;
            }
        }
    }
    file.close();
}

// === HỦY LỚP TỰ ĐỘNG (ADMIN) ===
void DanhSachDangKy::on_btnHuyLop_clicked() {
    QString nk = ui->editNienKhoa->text().trimmed();
    int hk = ui->editHocKy->text().toInt();
    if(nk.isEmpty() || hk<=0) { QMessageBox::warning(this,"Lỗi","Nhập NK, HK"); return; }

    int c = 0;
    PTRLTC p = dsLopTinChi;
    while(p) {
        if(p->ltc.NienKhoa==nk && p->ltc.Hocky==hk && !p->ltc.huylop) {
            int sv=0;
            nodeDK* k=p->ltc.dssvdk;
            while(k){
                if (!k->dk.huydangky) sv++;
                k=k->next;
            }
            if(sv < p->ltc.sosvmin) c++;
        }
        p=p->next;
    }

    if(c>0) {
        if(QMessageBox::question(this,"Hủy",QString("Có %1 lớp thiếu SV. Hủy?").arg(c))==QMessageBox::Yes) {
            XuLyHuyLopTuDong(nk, hk);
            QMessageBox::information(this,"OK","Đã hủy");
            on_btnLocLop_clicked();
        }
    } else QMessageBox::information(this,"Info","Không có lớp cần hủy");
}

void DanhSachDangKy::XuLyHuyLopTuDong(QString nienkhoa, int hocky) {
    PTRLTC p = dsLopTinChi;
    while(p) {
        if(p->ltc.NienKhoa==nienkhoa && p->ltc.Hocky==hocky && !p->ltc.huylop) {
            int sv=0;
            nodeDK* k=p->ltc.dssvdk;
            while(k){
                if (!k->dk.huydangky) sv++;
                k=k->next;
            }
            if(sv < p->ltc.sosvmin) p->ltc.huylop = true;
        }
        p=p->next;
    }
    LuuDanhSachLopTinChi(dsLopTinChi);
}

void DanhSachDangKy::on_btnQuayLai_clicked() {
    this->close();
    if(parentWidget()) parentWidget()->show();
}
