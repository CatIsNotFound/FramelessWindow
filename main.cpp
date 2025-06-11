#include <QApplication>
#include "DemoWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    DemoWindow framelessWidget;
    framelessWidget.setWindowTitle("无边框窗口示例");
    framelessWidget.setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
    framelessWidget.setPadding(10);
    framelessWidget.setWindowSize(1024, 768);
    framelessWidget.setShadow(15, QColor::fromRgb(255, 255, 255));
    framelessWidget.setShadowVisible(true);
    framelessWidget.setResizable(true);
    framelessWidget.setAnimationEnabled(true);
//    framelessWidget.setWindowTitleBarStyle("background-color: #000000; color: #ffffff");
//    framelessWidget.setContextStyle("background-color: #ffffff; color: #000000; ");
    framelessWidget.show();
    return QApplication::exec();
}
