#include "chaoscanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>

ChaosCanvas::ChaosCanvas(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMinimumSize(200, 150);
    setCursor(Qt::CrossCursor);
    setToolTip(tr("Click to add vertices for \"Your Figure\""));
}

int ChaosCanvas::runChaosGame(const QList<QPoint> &vertices, const QList<QColor> &colors,
                              int iterations)
{
    const int n = int(vertices.size());
    Q_ASSERT_X(colors.size() >= n, "ChaosCanvas::runChaosGame", "fewer colours than vertices");
    if (n < 1 || colors.size() < n || m_image.isNull())
        return 0;

    QList<QRgb> rgbs;
    for (int i = 0; i < n; ++i) {
        drawMarker(vertices[i], colors[i]);
        rgbs.append(colors[i].rgb());
    }

    QPoint p = randomPoint();
    drawMarker(p, Qt::red);

    // Integer midpoints, as in the original. Pixels are written straight into
    // the image bits: setPixelColor() and the shared global generator are
    // too slow for millions of iterations.
    QRandomGenerator rng(QRandomGenerator::global()->generate());
    const int w = m_image.width();
    const int h = m_image.height();
    const qsizetype stride = m_image.bytesPerLine();
    uchar *bits = m_image.bits();
    int x = p.x();
    int y = p.y();
    int plotted = 0;
    for (int i = 0; i < iterations; ++i) {
        const int v = rng.bounded(n);
        x = (x + vertices[v].x()) / 2;
        y = (y + vertices[v].y()) / 2;
        if (x >= 0 && x < w && y >= 0 && y < h) {
            reinterpret_cast<QRgb *>(bits + y * stride)[x] = rgbs[v];
            ++plotted;
        }
    }

    update();
    return plotted;
}

QPoint ChaosCanvas::randomPoint() const
{
    auto *rng = QRandomGenerator::global();
    return QPoint(rng->bounded(qMax(1, width())), rng->bounded(qMax(1, height())));
}

QList<QPoint> ChaosCanvas::randomVertices(int count) const
{
    QList<QPoint> result;
    for (int i = 0; i < count; ++i)
        result.append(randomPoint());
    return result;
}

void ChaosCanvas::clear()
{
    m_image.fill(Qt::black);
    m_userVertices.clear();
    update();
}

void ChaosCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(event->rect(), m_image, event->rect());
}

void ChaosCanvas::resizeEvent(QResizeEvent *event)
{
    // Keep what has been drawn so far when the window grows or shrinks.
    QImage resized(event->size(), QImage::Format_RGB32);
    resized.fill(Qt::black);
    if (!m_image.isNull()) {
        QPainter painter(&resized);
        painter.drawImage(0, 0, m_image);
    }
    m_image = resized;
    QWidget::resizeEvent(event);
}

void ChaosCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    if (m_userVertices.size() >= MaxUserVertices) {
        emit userVertexRejected();
        return;
    }
    const QPoint p = event->position().toPoint();
    m_userVertices.append(p);
    drawMarker(p, Qt::white);
    emit userVertexAdded(int(m_userVertices.size()));
}

void ChaosCanvas::drawMarker(const QPoint &p, const QColor &color)
{
    // Equivalent of VCL Canvas->Ellipse(x-2, y-2, x+2, y+2) with the default
    // white brush and a coloured pen.
    QPainter painter(&m_image);
    painter.setPen(color);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QRect(p.x() - 2, p.y() - 2, 4, 4));
    update(QRect(p.x() - 3, p.y() - 3, 7, 7));
}
