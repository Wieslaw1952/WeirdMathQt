#ifndef CHAOSCANVAS_H
#define CHAOSCANVAS_H

#include <QColor>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QWidget>

// Drawing area for the "chaos game": start from a random point and keep
// jumping halfway towards a randomly chosen vertex, plotting every landing
// point. Everything is drawn into an off-screen image, so the picture
// survives repaints (unlike the original, which drew straight on the form).
class ChaosCanvas : public QWidget
{
    Q_OBJECT

public:
    static constexpr int MaxUserVertices = 11;

    explicit ChaosCanvas(QWidget *parent = nullptr);

    // Draws the vertices and runs the chaos game. colors[i] is the colour of
    // vertex i and of every point that jumped towards it. Returns the number
    // of points plotted.
    int runChaosGame(const QList<QPoint> &vertices, const QList<QColor> &colors,
                     int iterations);

    QPoint randomPoint() const;
    QList<QPoint> randomVertices(int count) const;

    const QList<QPoint> &userVertices() const { return m_userVertices; }
    void clearUserVertices() { m_userVertices.clear(); }

public slots:
    void clear();

signals:
    void userVertexAdded(int count);
    void userVertexRejected();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void drawMarker(const QPoint &p, const QColor &color);

    QImage m_image;
    QList<QPoint> m_userVertices;
};

#endif // CHAOSCANVAS_H
