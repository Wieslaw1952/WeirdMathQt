#include "mainwindow.h"

#include "chaoscanvas.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

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

    auto addButton = [this](QHBoxLayout *row, const QString &text, void (MainWindow::*slot)()) {
        auto *button = new QPushButton(text, this);
        connect(button, &QPushButton::clicked, this, slot);
        row->addWidget(button);
    };

    auto *figures = new QHBoxLayout;
    addButton(figures, tr("Random Pentagon"), &MainWindow::drawRandomPentagon);
    addButton(figures, tr("Random Quadrangle"), &MainWindow::drawRandomQuadrangle);
    addButton(figures, tr("Random Triangle"), &MainWindow::drawRandomTriangle);
    addButton(figures, tr("Triangle"), &MainWindow::drawTriangle);
    addButton(figures, tr("Clear Screen"), &MainWindow::clearScreen);

    m_iterations = new QSpinBox(this);
    m_iterations->setRange(1, MaxIterations);
    m_iterations->setSingleStep(100000);
    m_iterations->setGroupSeparatorShown(true);
    m_iterations->setValue(DefaultIterations);
    m_iterations->setToolTip(tr("Number of points (iterations) for every figure"));

    auto *userRow = new QHBoxLayout;
    addButton(userRow, tr("Your Figure"), &MainWindow::drawUserFigure);
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

void MainWindow::drawTriangle()
{
    const int w = m_canvas->width();
    const int h = m_canvas->height();
    drawPolygon({QPoint(10, 10), QPoint(w - 10, 10), QPoint(w / 2, h - 10)});
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
