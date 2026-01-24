#include "danhsachmonhoc.h"
#include "ui_danhsachmonhoc.h"
#include "danhsachloptinchi.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

extern PTRLTC dsLopTinChi;

DanhSachMonHoc::DanhSachMonHoc(QWidget *parent) :
    QDialog(parent), // [SỬA] Khớp với header là QDialog
    ui(new Ui::DanhSachMonHoc)
{
    ui->setupUi(this);
    root = nullptr;

    // Cấu hình bảng
    ui->tblMonHoc->setColumnCount(4);
    QStringList headers = {"Mã MH", "Tên MH", "STCLT", "STCTH"};
    ui->tblMonHoc->setHorizontalHeaderLabels(headers);
    ui->tblMonHoc->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMonHoc->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblMonHoc->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblMonHoc->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Giới hạn nhập liệu
    ui->spnSTCLT->setMaximum(10);
    ui->spnSTCTH->setMaximum(10);

    loadFromFile();
}

DanhSachMonHoc::~DanhSachMonHoc() {
    FreeTree(root);
    delete ui;
}

// ==================== AVL ALGORITHMS ====================

int DanhSachMonHoc::height(TreeMH N) { return (N == nullptr) ? 0 : N->height; }
int DanhSachMonHoc::max(int a, int b) { return (a > b) ? a : b; }

TreeMH DanhSachMonHoc::newNode(MonHoc mh) {
    TreeMH node = new nodeMH;
    node->mh = mh;
    node->left = node->right = nullptr;//dfs
    node->height = 1;
    return node;
}

TreeMH DanhSachMonHoc::rightRotate(TreeMH y) {
    TreeMH x = y->left;
    TreeMH T2 = x->right;
    x->right = y; y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

TreeMH DanhSachMonHoc::leftRotate(TreeMH x) {
    TreeMH y = x->right;
    TreeMH T2 = y->left;
    y->left = x; x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int DanhSachMonHoc::getBalance(TreeMH N) {
    return (N == nullptr) ? 0 : height(N->left) - height(N->right);
}

TreeMH DanhSachMonHoc::insertNode(TreeMH node, MonHoc mh) {
    if (node == nullptr) return newNode(mh);

    int cmp = strcmp(mh.MAMH, node->mh.MAMH);
    if (cmp < 0) node->left = insertNode(node->left, mh);
    else if (cmp > 0) node->right = insertNode(node->right, mh);
    else return node;

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

TreeMH DanhSachMonHoc::minValueNode(TreeMH node) {
    TreeMH current = node;
    while (current->left != nullptr) current = current->left;
    return current;
}

// Đổi tham số thành const char* để khớp với .c_str()
TreeMH DanhSachMonHoc::deleteNode(TreeMH root, const char* mamh) {
    if (root == nullptr) return root;

    int cmp = strcmp(mamh, root->mh.MAMH);
    if (cmp < 0) root->left = deleteNode(root->left, mamh);
    else if (cmp > 0) root->right = deleteNode(root->right, mamh);
    else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            TreeMH temp = root->left ? root->left : root->right;
            if (temp == nullptr) { temp = root; root = nullptr; }
            else *root = *temp;
            delete temp;
        } else {
            TreeMH temp = minValueNode(root->right);
            root->mh = temp->mh;
            root->right = deleteNode(root->right, temp->mh.MAMH);
        }
    }

    if (root == nullptr) return root;

    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left); return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right); return leftRotate(root);
    }
    return root;
}

TreeMH DanhSachMonHoc::searchNode(TreeMH node, const char* mamh) {
    if (node == nullptr || strcmp(node->mh.MAMH, mamh) == 0) return node;
    if (strcmp(mamh, node->mh.MAMH) < 0) return searchNode(node->left, mamh);
    return searchNode(node->right, mamh);
}

// ==================== DISPLAY & EVENTS ====================

int DanhSachMonHoc::countNodes(TreeMH node) {
    if (!node) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

void DanhSachMonHoc::treeToArray(TreeMH node, MonHoc* arr, int &index) {
    if (!node) return;
    treeToArray(node->left, arr, index);
    arr[index++] = node->mh;
    treeToArray(node->right, arr, index);
}

void DanhSachMonHoc::HienThiDanhSach() {
    int n = countNodes(root);
    if (n == 0) { ui->tblMonHoc->setRowCount(0); return; }

    MonHoc* arr = new MonHoc[n];
    int index = 0;
    treeToArray(root, arr, index);

    // Selection Sort theo Tên
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (strcmp(arr[j].TENMH, arr[minIdx].TENMH) < 0) minIdx = j;
        }
        if (minIdx != i) { MonHoc temp = arr[i]; arr[i] = arr[minIdx]; arr[minIdx] = temp; }
    }

    ui->tblMonHoc->setRowCount(0);
    for (int i = 0; i < n; i++) {
        ui->tblMonHoc->insertRow(i);
        ui->tblMonHoc->setItem(i, 0, new QTableWidgetItem(QString::fromUtf8(arr[i].MAMH)));
        ui->tblMonHoc->setItem(i, 1, new QTableWidgetItem(QString::fromUtf8(arr[i].TENMH)));
        ui->tblMonHoc->setItem(i, 2, new QTableWidgetItem(QString::number(arr[i].STCLT)));
        ui->tblMonHoc->setItem(i, 3, new QTableWidgetItem(QString::number(arr[i].STCTH)));
    }
    delete[] arr;
}

void DanhSachMonHoc::FreeTree(TreeMH &node) {
    if (node) { FreeTree(node->left); FreeTree(node->right); delete node; node = nullptr; }
}

// ==================== AUTO SAVE LOGIC ====================

void DanhSachMonHoc::saveToFileRecursive(TreeMH node, QTextStream &out) {
    if (node != nullptr) {
        out << node->mh.MAMH << "|" << node->mh.TENMH << "|"
            << node->mh.STCLT << "|" << node->mh.STCTH << "\n";
        saveToFileRecursive(node->left, out);
        saveToFileRecursive(node->right, out);
    }
}

// Hàm lưu dữ liệu chung
void DanhSachMonHoc::SaveData() {
    QFile file("danhsachmonhoc.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    saveToFileRecursive(root, out);
    file.close();
}

void DanhSachMonHoc::loadFromFile() {
    QFile file("danhsachmonhoc.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    FreeTree(root); root = nullptr;
    while (!in.atEnd()) {
        QStringList p = in.readLine().split("|");
        if (p.size() >= 4) {
            MonHoc mh;
            strncpy(mh.MAMH, p[0].trimmed().toStdString().c_str(), 10); mh.MAMH[10] = '\0';
            strncpy(mh.TENMH, p[1].trimmed().toStdString().c_str(), 50); mh.TENMH[50] = '\0';
            mh.STCLT = p[2].toInt(); mh.STCTH = p[3].toInt();
            root = insertNode(root, mh);
        }
    }
    file.close();
    HienThiDanhSach();
}

void DanhSachMonHoc::on_btnThemMH_clicked() {
    // [SỬA] Đổi tên biến ui->txt... thành ui->edt...
    QString ma = ui->edtMaMH->text().trimmed().toUpper();
    QString ten = ui->edtTenMH->text().trimmed();
    int lt = ui->spnSTCLT->value();
    int th = ui->spnSTCTH->value();

    if (ma.isEmpty() || ten.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Nhập đủ thông tin!"); return;
    }
    if (searchNode(root, ma.toStdString().c_str())) {
        QMessageBox::warning(this, "Lỗi", "Mã đã tồn tại!"); return;
    }

    MonHoc mh;
    strncpy(mh.MAMH, ma.toStdString().c_str(), 10); mh.MAMH[10] = '\0';
    strncpy(mh.TENMH, ten.toStdString().c_str(), 50); mh.TENMH[50] = '\0';
    mh.STCLT = lt; mh.STCTH = th;

    root = insertNode(root, mh);
    HienThiDanhSach();
    SaveData();
    ui->edtMaMH->clear();
    ui->edtTenMH->clear();
    ui->spnSTCLT->setValue(0);
    ui->spnSTCTH->setValue(0);
}

void DanhSachMonHoc::on_btnSuaMH_clicked() {
    // 1. Lấy dữ liệu từ giao diện (Mã mới có thể khác Mã cũ)
    QString maMoi = ui->edtMaMH->text().trimmed().toUpper();
    QString tenMoi = ui->edtTenMH->text().trimmed();
    int lt = ui->spnSTCLT->value();
    int th = ui->spnSTCTH->value();

    if (maMoi.isEmpty() || tenMoi.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Thông tin không được để trống!");
        return;
    }

    // Nếu chưa chọn môn nào từ bảng (m_maMhDangChon rỗng)
    if (m_maMhDangChon.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một môn trong bảng để sửa!");
        return;
    }

    // 2. Kiểm tra xem người dùng có thay đổi Mã Môn Học không?
    if (maMoi != m_maMhDangChon) {
        // === TRƯỜNG HỢP: CÓ SỬA MÃ (Phức tạp) ===

        // A. Kiểm tra mã mới có bị trùng với môn khác không?
        if (searchNode(root, maMoi.toStdString().c_str())) {
            QMessageBox::warning(this, "Lỗi", "Mã môn học mới (" + maMoi + ") đã tồn tại!");
            return;
        }

        // B. Kiểm tra mã cũ có đang bị ràng buộc khóa ngoại không?
        // (Nếu môn cũ đang được mở lớp tín chỉ thì không cho đổi mã, vì sẽ làm hỏng dữ liệu lớp)
        PTRLTC p = dsLopTinChi;
        while(p) {
            if(p->ltc.MAMH == m_maMhDangChon) {
                QMessageBox::warning(this, "Không thể sửa Mã",
                                     "Môn học cũ (" + m_maMhDangChon + ") đang có lớp tín chỉ hoạt động.\nKhông thể đổi Mã môn, chỉ có thể đổi Tên/Số TC!");
                return;
            }
            p = p->next;
        }

        // C. Thực hiện đổi mã: XÓA CŨ -> THÊM MỚI
        if (QMessageBox::question(this, "Xác nhận",
                                  "Bạn đang đổi mã từ " + m_maMhDangChon + " sang " + maMoi + ".\nHành động này sẽ xóa môn cũ và tạo môn mới. Tiếp tục?") == QMessageBox::Yes)
        {
            // Xóa node mã cũ
            root = deleteNode(root, m_maMhDangChon.toStdString().c_str());

            // Thêm node mã mới
            MonHoc mh;
            strncpy(mh.MAMH, maMoi.toStdString().c_str(), 10); mh.MAMH[10] = '\0';
            strncpy(mh.TENMH, tenMoi.toStdString().c_str(), 50); mh.TENMH[50] = '\0';
            mh.STCLT = lt;
            mh.STCTH = th;
            root = insertNode(root, mh);

            // Cập nhật lại biến đang chọn
            m_maMhDangChon = maMoi;
        } else {
            return; // Người dùng hủy bỏ
        }

    } else {
        // === TRƯỜNG HỢP: KHÔNG SỬA MÃ (Chỉ sửa Tên/Tín chỉ) ===
        // Tìm node dựa trên mã cũ (chính là mã hiện tại)
        TreeMH node = searchNode(root, m_maMhDangChon.toStdString().c_str());

        if (node) {
            strncpy(node->mh.TENMH, tenMoi.toStdString().c_str(), 50); node->mh.TENMH[50] = '\0';
            node->mh.STCLT = lt;
            node->mh.STCTH = th;
        } else {
            // Trường hợp hy hữu: Node bị mất tích
            QMessageBox::warning(this, "Lỗi", "Dữ liệu không đồng bộ, vui lòng tải lại!");
            return;
        }
    }

    // 3. Hoàn tất
    HienThiDanhSach();
    SaveData(); // Lưu file tự động
    QMessageBox::information(this, "Thành công", "Cập nhật môn học thành công!");

    // Reset form
    ui->edtMaMH->clear();
    ui->edtTenMH->clear();
    m_maMhDangChon = ""; // Reset biến chọn
}

void DanhSachMonHoc::on_btnXoaMH_clicked() {
    QString ma = ui->edtMaMH->text().trimmed().toUpper();
    if (ma.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Nhập mã để xóa!"); return; }

    // Check khóa ngoại
    PTRLTC p = dsLopTinChi;
    while(p) {
        if(p->ltc.MAMH == ma) { QMessageBox::warning(this, "Lỗi", "Môn này đang mở lớp!"); return; }
        p = p->next;
    }

    if (QMessageBox::question(this, "Xóa", "Xóa môn " + ma + "?") == QMessageBox::Yes) {
        // [SỬA] Truyền tham số đúng kiểu const char*
        root = deleteNode(root, ma.toStdString().c_str());
        HienThiDanhSach();
        SaveData();
        ui->edtMaMH->clear();
        ui->edtTenMH->clear();
    }
}

void DanhSachMonHoc::on_tblMonHoc_cellClicked(int row, int column) {
    Q_UNUSED(column);
    ui->edtMaMH->setText(ui->tblMonHoc->item(row, 0)->text());
    ui->edtTenMH->setText(ui->tblMonHoc->item(row, 1)->text());
    ui->spnSTCLT->setValue(ui->tblMonHoc->item(row, 2)->text().toInt());
    ui->spnSTCTH->setValue(ui->tblMonHoc->item(row, 3)->text().toInt());
    m_maMhDangChon = ui->tblMonHoc->item(row, 0)->text();
}

void DanhSachMonHoc::on_btnQuayLai_clicked() {
    this->close();
    if(parentWidget()) parentWidget()->show();
}
