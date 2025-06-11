
#ifndef FRAMELESSWINDOWHINT_DEMOWINDOW_H
#define FRAMELESSWINDOWHINT_DEMOWINDOW_H

#include <QWidget>
#include "FramelessWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DemoWindow; }
QT_END_NAMESPACE

class DemoWindow : public FramelessWidget {
Q_OBJECT

public:
    explicit DemoWindow(QWidget *parent = nullptr);

    ~DemoWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget* your_widget;
    Ui::DemoWindow *ui;
};


#endif //FRAMELESSWINDOWHINT_DEMOWINDOW_H
