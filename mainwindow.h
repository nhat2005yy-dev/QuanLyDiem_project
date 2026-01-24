#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "danhsachdangky.h"
#include "danhsachloptinchi.h"
#include "danhsachmonhoc.h"
#include "DS_Lop.h"
#include "DS_SinhVien.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_DSdangky_Button_clicked();
    void on_DSmonhoc_Button_clicked();
    void on_DSlop_Button_clicked();
    void on_DSsinhvien_Button_clicked();
    void on_DSloptinchi_Button_clicked();
    void on_DiemInfor_Button_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
