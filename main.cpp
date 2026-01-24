#include "mainwindow.h"
//#include "monhoc.h"
//#include "sinhvien.h"
//#include "quanlyloptinchi.h"
//#include "danhsachlopsinhvien.h"
//#include "danhsachloptinchi.h"


#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QuanLyLopTinChi Q;
    //Q.show();
    //return a.exec();

    //PTRLTC dsltc = NULL;
    //TreeMH dsmh = NULL;
    //DS_LOPSV dslopsv;

    //Hiển thị giao diện chính
    MainWindow w;
    w.show();
    return a.exec();

}
