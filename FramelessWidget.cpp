
#include "FramelessWidget.h"
#include "ui_FramelessWidget.h"


FramelessWidget::FramelessWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::FramelessWidget) {
    ui->setupUi(this);
    setMinimumSize(QSize(180, 180));
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    mouseEventFilter = new MouseEventFilter();
    connect(mouseEventFilter, &MouseEventFilter::onMouseMovingOnWidget, [&](const int x, const int y, const QString& obj_name) {
        if (obj_name == "root_widget" || obj_name == "title_widget") {
            setCursor(Qt::ArrowCursor);
            qDebug() << "CH";
        }
    });

    ui->verticalLayout->setContentsMargins(4, 4, 4, 4);
    ui->verticalLayout->setStretch(1, 1);
    ui->root_widget->setProperty("class", "root");
    connect(this, &QWidget::windowTitleChanged, [&](const QString& title) { ui->label_title->setText(title); });
    connect(this, &QWidget::windowIconChanged, [&](const QIcon& icon) {
        if (icon.isNull()) ui->toolButton_icon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::Computer));
        else ui->toolButton_icon->setIcon(icon);
    });
    connect(ui->toolButton_min, &QToolButton::clicked, this, &FramelessWidget::setWindowMinimized);
    connect(ui->toolButton_max, &QToolButton::clicked, this, &FramelessWidget::setWindowMaximized);
    connect(ui->toolButton_close, &QToolButton::clicked, this, &FramelessWidget::closeWindow);
    // 添加控件
    app_menu = new QMenu();
    act_min = new QAction("最小化");
    act_max = new QAction("最大化/还原");
    act_close = new QAction("关闭");
    connect(act_min, &QAction::triggered, this, &FramelessWidget::setWindowMinimized);
    connect(act_max, &QAction::triggered, this, &FramelessWidget::setWindowMaximized);
    connect(act_close, &QAction::triggered, this, &FramelessWidget::closeWindow);
    app_menu->addAction(act_min);
    app_menu->addAction(act_max);
    app_menu->addAction(act_close);
    connect(ui->toolButton_icon, &QToolButton::clicked, [&] {
        QPoint p = {x() + padding(), y() + ui->toolButton_icon->height() + padding()};
        if (windowState() == Qt::WindowMaximized) {
            p.setX(0);
            p.setY(ui->toolButton_icon->height());
        }
        app_menu->move(p);
        app_menu->show();
    });
}

FramelessWidget::~FramelessWidget() {
    delete ui;
    delete mouseEventFilter;
}

void FramelessWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (windowState() == Qt::WindowNoState) {
        ui->root_widget->setGeometry(root_padding, root_padding, width() - root_padding - 4,
                                     height() - root_padding - 4);
    } else {
        ui->root_widget->setGeometry(0, 0, width(),height());
    }
    ui->verticalLayoutWidget->setGeometry(0, 0, ui->root_widget->width(), ui->root_widget->height());
    ui->layoutWidget->setGeometry(0, 0, ui->title_widget->width(), ui->title_widget->height());
}

void FramelessWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && windowState() != Qt::WindowMaximized) {
        is_moved = true;
        cur = QCursor::pos();
        win_pos = pos();
        setCursor(Qt::SizeAllCursor);
    }
}

void FramelessWidget::mouseMoveEvent(QMouseEvent *event) {
    if (is_moved) {
        QPoint new_cur_pos = QCursor::pos();
        QPoint new_win_pos = win_pos + new_cur_pos - cur;
        move(new_win_pos);
    } else {
        QPoint pt = event->pos();
        if ((pt.x() < padding() && pt.y() < padding()) ||
            (pt.x() > width() - padding() && pt.y() > height() - padding())) {
            setCursor(Qt::SizeFDiagCursor);
        } else if ((pt.x() > width() - padding() && pt.y() < padding()) ||
                   (pt.x() < padding() && pt.y() > height() - padding())) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (pt.x() < padding() || pt.x() > width() - padding()) {
            setCursor(Qt::SizeHorCursor);
        } else if (pt.y() < padding() || pt.y() > height() - padding()) {
            setCursor(Qt::SizeVerCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }

}

void FramelessWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (is_moved) {
        is_moved = false;
        setCursor(Qt::ArrowCursor);
        if (pos().y() <= 0 - padding()) {
            if (is_animation_enabled) playMoveWindowAnimation();
            else move(x(), 0 - padding());
        }
    }
}

void FramelessWidget::setWindowSize(const int width, const int height) {
    win_width = width; win_height = height;
}

const QSize FramelessWidget::windowSize() const {
    return QSize(win_width, win_height);
}

void FramelessWidget::setPadding(const int& new_padding) {
    root_padding = new_padding;
}

const int &FramelessWidget::padding() const {
    return root_padding;
}

void FramelessWidget::showEvent(QShowEvent *event) {
    setWindowState(Qt::WindowNoState);
    resize(win_width, win_height);
    is_closed = false;
    setMouseTracking(is_resizable);
    ui->root_widget->setMouseTracking(is_resizable);
    ui->title_widget->setMouseTracking(is_resizable);
    ui->root_widget->installEventFilter(mouseEventFilter);
    ui->title_widget->installEventFilter(mouseEventFilter);
    // qDebug() << "Insert Widget Address: " << ui->verticalLayout->itemAt(2)->widget()->objectName();
//    if (ui->verticalLayout->count() > 2) {
//        ui->verticalLayout->itemAt(2)->widget()->setMouseTracking(is_resizable);
//        ui->verticalLayout->itemAt(2)->widget()->installEventFilter(mouseEventFilter);
//    }
    if (!is_once) return;
    move(0, 0);
    if (is_animation_enabled) {
        QParallelAnimationGroup *parallelAnimationGroup = new QParallelAnimationGroup(this);
        connect(parallelAnimationGroup, &QParallelAnimationGroup::finished, [&] { is_once = false; });
        QPropertyAnimation *op_animation = new QPropertyAnimation(this, "windowOpacity", this);
        op_animation->setStartValue(0);
        op_animation->setEndValue(1);
        op_animation->setDuration(500);
        op_animation->setEasingCurve(QEasingCurve::OutCubic);

        int origin_width = width(), origin_height = height();
        double scale = 0.7;
        int st_width = origin_width * scale, st_height = origin_height * scale;
        QPropertyAnimation *sz_animation = new QPropertyAnimation(this, "size", this);
        sz_animation->setStartValue(QSize(st_width, st_height));
        sz_animation->setEndValue(QSize(origin_width, origin_height));
        sz_animation->setEasingCurve(QEasingCurve::OutCubic);
        sz_animation->setDuration(500);

        int origin_x = x(), origin_y = y();
        QPropertyAnimation *ps_animation = new QPropertyAnimation(this, "pos", this);
        ps_animation->setStartValue(QPoint(origin_x + origin_width * (1 - scale) / 2,
                                           origin_y + origin_height * (1 - scale) / 2));
        ps_animation->setEndValue(QPoint(origin_x, origin_y));
        ps_animation->setEasingCurve(QEasingCurve::OutCubic);
        ps_animation->setDuration(500);

        parallelAnimationGroup->addAnimation(op_animation);
        parallelAnimationGroup->addAnimation(sz_animation);
        parallelAnimationGroup->addAnimation(ps_animation);
        parallelAnimationGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
    }
}

void FramelessWidget::hideEvent(QHideEvent *event) {
    
}

void FramelessWidget::closeEvent(QCloseEvent *event) {
    if (is_closed) return;
    event->ignore();
}

void FramelessWidget::setShadow(const int size, const QColor &color) {
    shadow_size = size;
    shadow_color = color;
}

const bool &FramelessWidget::shadowVisible() const {
    return has_shadow;
}

void FramelessWidget::setResizable(const bool &b) {
    is_resizable = b;
}

const bool& FramelessWidget::isResizable() const {
    return is_resizable;
}

void FramelessWidget::setAnimationEnabled(const bool &b) {
    is_animation_enabled = b;
}

const bool& FramelessWidget::animationEnabled() const {
    return is_animation_enabled;
}

void FramelessWidget::setShadowVisible(const bool &visible) {
    if (shadow_size - 4 >= root_padding) {
        has_shadow = visible;
        qDebug() << "Success!";
    } else qCritical() << "Error: Can't show the window shadow! The padding value is not enough!";
}

void FramelessWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    if (has_shadow && windowState() != Qt::WindowMaximized) {
        win_shadow = renderShadow(true, shadow_color, shadow_size);
        ui->root_widget->setGraphicsEffect(win_shadow);
    }
}

void FramelessWidget::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
    if (has_shadow && windowState() != Qt::WindowMaximized) {
        win_shadow = renderShadow(false, shadow_color, shadow_size);
        ui->root_widget->setGraphicsEffect(win_shadow);
    }
}


void FramelessWidget::insertWidget(QWidget *widget) {
    ui->empty_widget->setVisible(false);
    widget->installEventFilter(mouseEventFilter);
    widget->setMouseTracking(is_resizable);
    widget->setObjectName("insertObject");
    ui->verticalLayout->addWidget(widget, 1);
}

void FramelessWidget::playMoveWindowAnimation() {
    QPropertyAnimation* mv_animation = new QPropertyAnimation(this, "pos", this);
    mv_animation->setStartValue(pos());
    mv_animation->setEndValue(QPoint(x(), 0 - padding()));
    mv_animation->setEasingCurve(QEasingCurve::OutBack);
    mv_animation->setDuration(250);

    mv_animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void FramelessWidget::setMinimizedButtonClassStyle(const QString &class_name) {
    ui->toolButton_min->setProperty("class", class_name);
}

void FramelessWidget::setMinimizedButtonStyle(const QString &style_sheet) {
    ui->toolButton_min->setStyleSheet(style_sheet);
}

void FramelessWidget::setMinimizedButtonVisible(const bool &b) {
    ui->toolButton_min->setVisible(b);
}

void FramelessWidget::setMaximizedButtonClassStyle(const QString &class_name) {
    ui->toolButton_max->setProperty("class", class_name);
}

void FramelessWidget::setMaximizedButtonStyle(const QString &style_sheet) {
    ui->toolButton_max->setStyleSheet(style_sheet);
}

void FramelessWidget::setMaximizedButtonVisible(const bool &b) {
    ui->toolButton_max->setVisible(b);
}

void FramelessWidget::setCloseButtonClassStyle(const QString &class_name) {
    ui->toolButton_close->setProperty("class", class_name);
}

void FramelessWidget::setCloseButtonStyle(const QString &style_sheet) {
    ui->toolButton_close->setStyleSheet(style_sheet);
}

void FramelessWidget::setCloseButtonVisible(const bool &b) {
    ui->toolButton_close->setVisible(b);
}

void FramelessWidget::setWindowTitleBarClassStyle(const QString &class_name) {
    ui->title_widget->setProperty("class", class_name);
}

void FramelessWidget::setWindowTitleBarStyle(const QString &style_sheet) {
    // ui->verticalLayoutWidget->setStyleSheet(style_sheet);
    ui->title_widget->setStyleSheet(style_sheet);
}

void FramelessWidget::setContextClassStyle(const QString &class_name) {
    ui->root_widget->setProperty("class", class_name);
}

void FramelessWidget::setContextStyle(const QString &style_sheet) {
    ui->root_widget->setStyleSheet(style_sheet);
}

void FramelessWidget::setWindowMinimized() {
    setWindowState(Qt::WindowMinimized);
}

void FramelessWidget::setWindowMaximized() {
    setWindowState((windowState() == Qt::WindowMaximized ? Qt::WindowNoState : Qt::WindowMaximized));
}

void FramelessWidget::closeWindow() {
    if (is_animation_enabled) {
        QParallelAnimationGroup *parallelAnimationGroup = new QParallelAnimationGroup(this);
        connect(parallelAnimationGroup, &QParallelAnimationGroup::finished, [&] {
            is_closed = true;
            this->close();
        });
        QPropertyAnimation *op_animation = new QPropertyAnimation(this, "windowOpacity", this);
        op_animation->setStartValue(1);
        op_animation->setEndValue(0);
        op_animation->setDuration(500);
        op_animation->setEasingCurve(QEasingCurve::OutCubic);

        int origin_width = width(), origin_height = height();
        double scale = 0.7;
        int st_width = origin_width * scale, st_height = origin_height * scale;
        QPropertyAnimation *sz_animation = new QPropertyAnimation(this, "size", this);
        sz_animation->setStartValue(QSize(origin_width, origin_height));
        sz_animation->setEndValue(QSize(st_width, st_height));
        sz_animation->setEasingCurve(QEasingCurve::OutCubic);
        sz_animation->setDuration(500);

        int origin_x = x(), origin_y = y();
        QPropertyAnimation *ps_animation = new QPropertyAnimation(this, "pos", this);
        ps_animation->setStartValue(QPoint(origin_x, origin_y));
        ps_animation->setEndValue(QPoint(origin_x + origin_width * (1 - scale) / 2,
                                         origin_y + origin_height * (1 - scale) / 2));
        ps_animation->setEasingCurve(QEasingCurve::OutCubic);
        ps_animation->setDuration(500);

        parallelAnimationGroup->addAnimation(op_animation);
        parallelAnimationGroup->addAnimation(sz_animation);
        parallelAnimationGroup->addAnimation(ps_animation);
        parallelAnimationGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
    } else {
        is_closed = true;
        close();
    }
}


MouseEventFilter::MouseEventFilter(QObject *parent) : QObject(parent) {}

bool MouseEventFilter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        auto mouse_event = dynamic_cast<QMouseEvent*>(event);
        emit onMouseMovingOnWidget(mouse_event->pos().x(), mouse_event->pos().y(), watched->objectName());
    }
    return QObject::eventFilter(watched, event);
}
