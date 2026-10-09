#include "mainwindow.h"

#include "chaoscanvas.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <QtMath>

namespace {

constexpr int DefaultIterations = 3000000;
constexpr int MaxIterations = 100000000;

// VCL colours from the original program: clRed, clGreen, clYellow, clSilver,
// clLime, clMaroon, clAqua, clWhite, clFuchsia, clPurple, then clBlue.
const QList<QColor> &vclColors()
{
    static const QList<QColor> colors = {
        QColor(0xFF, 0x00, 0x00), QColor(0x00, 0x80, 0x00), QColor(0xFF, 0xFF, 0x00),
        QColor(0xC0, 0xC0, 0xC0), QColor(0x00, 0xFF, 0x00), QColor(0x80, 0x00, 0x00),
        QColor(0x00, 0xFF, 0xFF), QColor(0xFF, 0xFF, 0xFF), QColor(0xFF, 0x00, 0xFF),
        QColor(0x80, 0x00, 0x80), QColor(0x00, 0x00, 0xFF),
    };
    // "Your Figure" colours every user vertex from this palette.
    Q_ASSERT(colors.size() >= ChaosCanvas::MaxUserVertices);
    return colors;
}

// The fixed figures used the first colours of the palette, with the last
// vertex always blue (the "default:" branch of the original switch).
QList<QColor> figureColors(int count)
{
    QList<QColor> colors = vclColors().mid(0, count - 1);
    colors.append(QColor(0x00, 0x00, 0xFF));
    return colors;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Weird Math - Sierpinski Triangle - W.Potoczek design"));
    resize(640, 520);

    m_canvas = new ChaosCanvas(this);

    m_log = new QPlainTextEdit(this);
    m_log->setReadOnly(true);
    m_log->setMaximumBlockCount(100);
    m_log->setFixedHeight(m_log->fontMetrics().lineSpacing() * 3 + 10);

    auto makeButton = [this](const QString &text, auto slot) {
        auto *button = new QPushButton(text, this);
        connect(button, &QPushButton::clicked, this, slot);
        return button;
    };

    // Two rows of preset figures; "Clear Screen" spans both rows.
    auto *figures = new QGridLayout;
    figures->addWidget(makeButton(tr("Random Hexagon"), &MainWindow::drawRandomHexagon), 0, 0);
    figures->addWidget(makeButton(tr("Random Pentagon"), &MainWindow::drawRandomPentagon), 0, 1);
    figures->addWidget(makeButton(tr("Random Quadrangle"), &MainWindow::drawRandomQuadrangle), 0, 2);
    figures->addWidget(makeButton(tr("Random Triangle"), &MainWindow::drawRandomTriangle), 0, 3);
    figures->addWidget(makeButton(tr("Regular Hexagon"), [this] { drawRegularPolygon(6); }), 1, 0);
    figures->addWidget(makeButton(tr("Regular Pentagon"), [this] { drawRegularPolygon(5); }), 1, 1);
    figures->addWidget(makeButton(tr("Regular Quadrangle"), [this] { drawRegularPolygon(4); }), 1, 2);
    figures->addWidget(makeButton(tr("Regular Triangle"), [this] { drawRegularPolygon(3); }), 1, 3);
    auto *clear = makeButton(tr("Clear Screen"), &MainWindow::clearScreen);
    clear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    figures->addWidget(clear, 0, 4, 2, 1);

    m_iterations = new QSpinBox(this);
    m_iterations->setRange(1, MaxIterations);
    m_iterations->setSingleStep(100000);
    m_iterations->setGroupSeparatorShown(true);
    m_iterations->setValue(DefaultIterations);
    m_iterations->setToolTip(tr("Number of points (iterations) for every figure"));

    auto *userRow = new QHBoxLayout;
    userRow->addWidget(makeButton(tr("Your Figure"), &MainWindow::drawUserFigure));
    userRow->addWidget(new QLabel(tr("Points:"), this));
    userRow->addWidget(m_iterations);
    userRow->addWidget(m_log, 1);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 6);
    layout->addWidget(m_canvas, 1);
    auto *controls = new QVBoxLayout;
    controls->setContentsMargins(6, 0, 6, 0);
    controls->addLayout(userRow);
    controls->addLayout(figures);
    layout->addLayout(controls);

    connect(m_canvas, &ChaosCanvas::userVertexAdded, this, [this](int count) {
        log(tr("Vertex %1 added").arg(count));
    });
    connect(m_canvas, &ChaosCanvas::userVertexRejected, this, [this] {
        log(tr("At most %1 vertices").arg(ChaosCanvas::MaxUserVertices));
    });

    log(tr("Click on the black area to place vertices, then press \"Your Figure\"."));
}

void MainWindow::drawRandomTriangle()
{
    drawPolygon(m_canvas->randomVertices(3));
}

void MainWindow::drawRandomQuadrangle()
{
    drawPolygon(m_canvas->randomVertices(4));
}

void MainWindow::drawRandomPentagon()
{
    drawPolygon(m_canvas->randomVertices(5));
}

void MainWindow::drawRandomHexagon()
{
    drawPolygon(m_canvas->randomVertices(6));
}

void MainWindow::drawRegularPolygon(int count)
{
    // Unit polygon with a flat bottom edge (a vertex on top when count is odd),
    // then scaled and centred so it fills about 90% of the canvas.
    const double start = -M_PI / 2 + ((count % 2 == 0) ? M_PI / count : 0.0);
    QList<QPointF> unit;
    for (int i = 0; i < count; ++i) {
        const double a = start + 2 * M_PI * i / count;
        unit.append(QPointF(qCos(a), qSin(a)));
    }
    // Bounding box by hand: QRectF::united() ignores the zero-size rects of single points.
    double left = unit.first().x(), right = left, top = unit.first().y(), bottom = top;
    for (const QPointF &p : unit) {
        left = qMin(left, p.x());
        right = qMax(right, p.x());
        top = qMin(top, p.y());
        bottom = qMax(bottom, p.y());
    }
    const QRectF box(QPointF(left, top), QPointF(right, bottom));

    const double scale = 0.9 * qMin(m_canvas->width() / box.width(),
                                     m_canvas->height() / box.height());
    const QPointF offset = QPointF(m_canvas->width(), m_canvas->height()) / 2
                           - box.center() * scale;
    QList<QPoint> vertices;
    for (const QPointF &p : unit)
        vertices.append((p * scale + offset).toPoint());
    drawPolygon(vertices);
}

void MainWindow::drawUserFigure()
{
    const QList<QPoint> vertices = m_canvas->userVertices();
    if (vertices.size() < 3) {
        log(tr("Place at least 3 vertices first (%1 so far)").arg(vertices.size()));
        return;
    }
    const int plotted = m_canvas->runChaosGame(vertices, vclColors(), m_iterations->value());
    log(tr("No. of points plotted %1").arg(plotted));
    m_canvas->clearUserVertices();
}

void MainWindow::clearScreen()
{
    m_canvas->clear();
}

void MainWindow::drawPolygon(const QList<QPoint> &vertices)
{
    const int plotted = m_canvas->runChaosGame(vertices, figureColors(int(vertices.size())),
                                               m_iterations->value());
    log(tr("No. of points plotted %1").arg(plotted));
}

void MainWindow::log(const QString &message)
{
    m_log->appendPlainText(message);
}
