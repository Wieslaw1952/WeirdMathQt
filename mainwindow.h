#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QPoint>
#include <QWidget>

class ChaosCanvas;
class QPlainTextEdit;
class QSpinBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void drawRandomTriangle();
    void drawRandomQuadrangle();
    void drawRandomPentagon();
    void drawRandomHexagon();
    void drawUserFigure();
    void clearScreen();

private:
    void drawPolygon(const QList<QPoint> &vertices);
    void drawRegularPolygon(int count);
    void log(const QString &message);

    ChaosCanvas *m_canvas = nullptr;
    QPlainTextEdit *m_log = nullptr;
    QSpinBox *m_iterations = nullptr;
};

#endif // MAINWINDOW_H
