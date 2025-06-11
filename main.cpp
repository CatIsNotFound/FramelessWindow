#include <QApplication>
#include "DemoWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    DemoWindow framelessWidget;
    framelessWidget.setWindowTitle("无边框窗口示例");
    framelessWidget.setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
    framelessWidget.setPadding(10);
    framelessWidget.setWindowSize(1024, 768);
    framelessWidget.setShadow(15, QColor::fromRgb(0, 0, 0));
    framelessWidget.setShadowVisible(true);
    framelessWidget.setResizable(true);
    framelessWidget.setAnimationEnabled(true);
//    framelessWidget.setWindowTitleBarStyle();
//    framelessWidget.setContextStyle("background-color: #ffffff; color: #000000; ");
    framelessWidget.show();
    return QApplication::exec();
}
