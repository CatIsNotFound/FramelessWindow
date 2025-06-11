
#include "DemoWindow.h"
#include "ui_DemoWindow.h"


DemoWindow::DemoWindow(QWidget *parent) :
        FramelessWidget(parent), ui(new Ui::DemoWindow) {
    your_widget = new QWidget();
    ui->setupUi(your_widget);
    insertWidget(your_widget);
    // setMinimizedButtonVisible(false);
    // setMaximizedButtonVisible(false);
}

DemoWindow::~DemoWindow() {
    delete ui;
    delete your_widget;
}

void DemoWindow::closeEvent(QCloseEvent *event) {
    FramelessWidget::closeEvent(event);
    qDebug() << "已关闭！";
}

void DemoWindow::resizeEvent(QResizeEvent *event) {
    FramelessWidget::resizeEvent(event);
//    qDebug() << "Resized: " << size();
//    qDebug() << "A_Size: " << windowSize();
}
