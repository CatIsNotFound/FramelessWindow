
#ifndef FRAMELESSWINDOWHINT_FRAMELESSWIDGET_H
#define FRAMELESSWINDOWHINT_FRAMELESSWIDGET_H

#include <QWidget>
#include <QCursor>
#include <QPoint>
#include <QPointer>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QAnimationGroup>
#include <QPropertyAnimation>
#include <QSizeGrip>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class FramelessWidget; }
QT_END_NAMESPACE

class MouseEventFilter;

class FramelessWidget : public QWidget {
Q_OBJECT

public:
    explicit FramelessWidget(QWidget *parent = nullptr);

    ~FramelessWidget() override;

    void setWindowSize(const int width, const int height);
    const QSize windowSize() const;
    void setPadding(const int& new_padding);
    const int& padding() const;

    void setShadow(const int size, const QColor& color);
    void setShadowVisible(const bool& visible);
    const bool& shadowVisible() const;
    void setResizable(const bool& b);
    const bool& isResizable() const;
    void setAnimationEnabled(const bool& b);
    const bool& animationEnabled() const;

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    void insertWidget(QWidget* widget);
    void playMoveWindowAnimation();

public:
    void setMinimizedButtonClassStyle(const QString& class_name);
    void setMinimizedButtonStyle(const QString& style_sheet);
    void setMinimizedButtonVisible(const bool& b);

    void setMaximizedButtonClassStyle(const QString& class_name);
    void setMaximizedButtonStyle(const QString& style_sheet);
    void setMaximizedButtonVisible(const bool& b);

    void setCloseButtonClassStyle(const QString& class_name);
    void setCloseButtonStyle(const QString& style_sheet);
    void setCloseButtonVisible(const bool& b);

    void setWindowTitleBarClassStyle(const QString& class_name);
    void setWindowTitleBarStyle(const QString& style_sheet);

    void setContextClassStyle(const QString& class_name);
    void setContextStyle(const QString& style_sheet);

public slots:
    void setWindowMinimized();
    void setWindowMaximized();
    void closeWindow();

private:
    enum Direction {
        None,
        NorthWest,
        NorthEast,
        SouthWest,
        SouthEast,
        North,
        South,
        West,
        East
    };
    Ui::FramelessWidget *ui;
    bool is_moved{false}, has_shadow{false};
    QPoint cur;
    QPoint win_pos;
    QPoint cur_in_win_pos;
    int root_padding{0}, shadow_size{0};
    int win_width, win_height;
    int ori_win_width, ori_win_height;
    QColor shadow_color;
    QPointer<QGraphicsDropShadowEffect> win_shadow;
    bool is_once{true};
    bool is_closed{false};
    bool is_resizable{false};
    bool is_resizing{false};
    bool is_animation_enabled{true};
    Direction direction;
    QMenu* app_menu;
    QPointer<QAction> act_min, act_max, act_close;
    MouseEventFilter* mouseEventFilter;
};

inline QGraphicsDropShadowEffect* renderShadow(const bool& trigger, const QColor& color, const int& size) {
    QGraphicsDropShadowEffect* new_shadow = new QGraphicsDropShadowEffect();
    new_shadow->setColor(color);
    new_shadow->setOffset(0);
    new_shadow->setBlurRadius(trigger ? size : size / 2);
    new_shadow->setEnabled(true);
    return new_shadow;
}

class MouseEventFilter : public QObject {
    Q_OBJECT
public:
    MouseEventFilter(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void onMouseMovingOnWidget(const int x, const int y, const QString& object_name);
};

#endif //FRAMELESSWINDOWHINT_FRAMELESSWIDGET_H
