#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <vector>
#include "Juego.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsEllipseItem;
class QGraphicsSimpleTextItem;
class QGraphicsPathItem;
class QTimer;

// Parte gráfica: solo DIBUJA el estado de Juego y le pasa las órdenes del usuario.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* e) override;   // teclado

private slots:
    void on_btnDisparar_clicked();    // se conectan solas por el nombre (on_<objeto>_<señal>)
    void on_btnReiniciar_clicked();
    void onTick();                    // lo llama el QTimer: avanza la física y redibuja
    void actualizarCanon();           // gira el cañón según el ángulo elegido

private:
    static constexpr double ESC = 10.0;   // píxeles por metro (el mundo mide 100 x 50 m)

    void construirEscena();    // crea los items gráficos a partir del estado de Juego
    void actualizarEscena();   // sincroniza los items con Juego
    void actualizarHUD();      // textos y botones habilitados

    Ui::MainWindow* ui;
    Juego juego;
    QGraphicsScene* escena;
    QTimer* timer;

    struct ItemBloque {                     // un bloque de infraestructura en pantalla
        QGraphicsRectItem* rect;
        QGraphicsSimpleTextItem* texto;     // número de resistencia
    };
    std::vector<ItemBloque> bloques;        // mismo orden que juego.getObstaculos()
    QGraphicsRectItem* canones[2];
    QGraphicsEllipseItem* itemProyectil;
    QGraphicsPathItem* itemTrayectoria;
    QPainterPath ruta;
};

#endif // MAINWINDOW_H
