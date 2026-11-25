#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPen>
#include <QTimer>
#include <cmath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), escena(nullptr), timer(nullptr),
    itemProyectil(nullptr), itemTrayectoria(nullptr) {
    ui->setupUi(this);
    canones[0] = canones[1] = nullptr;

    // Escena (el "lienzo") asociada a la vista que se diseñó en el .ui
    escena = new QGraphicsScene(0, 0, Juego::ANCHO * ESC, Juego::ALTO * ESC, this);
    ui->graphicsView->setScene(escena);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Para que las flechas/espacio las reciba la ventana y no un botón
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->btnDisparar->setFocusPolicy(Qt::NoFocus);
    ui->btnReiniciar->setFocusPolicy(Qt::NoFocus);

    connect(ui->spinAngulo, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::actualizarCanon);

    construirEscena();
    actualizarHUD();

    // Bucle de animación: cada 16 ms (~60 FPS) se llama onTick()
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTick);
    timer->start(16);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Crea (o recrea) todos los items gráficos leyendo el estado inicial de Juego.
void MainWindow::construirEscena() {
    escena->clear();          // borra y destruye todos los items anteriores
    bloques.clear();
    ruta = QPainterPath();
    escena->setBackgroundBrush(QColor(205, 232, 255));

    // Marco de la caja (las paredes contra las que rebota el proyectil)
    QGraphicsRectItem* marco = escena->addRect(0, 0, Juego::ANCHO * ESC, Juego::ALTO * ESC, QPen(Qt::black, 4));
    marco->setZValue(10);

    // Bloques de infraestructura: color según resistencia máxima (100 = techo gris, 200 = pilar naranja)
    for (const Obstaculo& o : juego.getObstaculos()) {
        QColor color = (o.getResistenciaMax() >= 200) ? QColor(255, 220, 180) : QColor(225, 225, 225);
        QGraphicsRectItem* r = escena->addRect(o.getPosX() * ESC, o.getPosY() * ESC,
                                               o.getAncho() * ESC, o.getAlto() * ESC, QPen(QColor(200, 130, 40), 2), QBrush(color));
        QGraphicsSimpleTextItem* t = escena->addSimpleText("");
        t->setBrush(Qt::black);
        t->setZValue(2);
        bloques.push_back({r, t});
    }

    // Representantes (muñeco) y cañones
    const std::vector<Jugador>& js = juego.getJugadores();
    for (int i = 0; i < 2; ++i) {
        const Obstaculo& rep = js[i].getRepresentante();
        double cx = (rep.getPosX() + rep.getAncho() / 2) * ESC;
        double y0 = rep.getPosY() * ESC, y1 = (rep.getPosY() + rep.getAlto()) * ESC;
        QGraphicsSimpleTextItem* etq = escena->addSimpleText(QString("Rep. J%1").arg(i + 1));
        etq->setScale(0.75);
        etq->setPos(cx - 19, y0 + 2);
        escena->addEllipse(cx - 8, y0 + 22, 16, 16, QPen(Qt::black, 2), QBrush(Qt::white));   // cabeza
        escena->addLine(cx, y0 + 38, cx, y1 - 18, QPen(Qt::black, 2));                        // cuerpo
        escena->addLine(cx, y1 - 18, cx - 8, y1, QPen(Qt::black, 2));                         // piernas
        escena->addLine(cx, y1 - 18, cx + 8, y1, QPen(Qt::black, 2));

        double px = js[i].getCanonX() * ESC, py = js[i].getCanonY() * ESC;
        escena->addRect(px - 25, py + 8, 50, 6, QPen(Qt::black), QBrush(Qt::darkGray));       // plataforma
        escena->addEllipse(px - 10, py - 10, 20, 20, QPen(Qt::black), QBrush(Qt::darkBlue));  // base
        // Cañón: rectángulo que nace en el pivote (0,0) y gira alrededor de él
        canones[i] = escena->addRect(0, -3, Juego::LARGO_CANON * ESC, 6, QPen(Qt::black), QBrush(Qt::darkBlue));
        canones[i]->setPos(px, py);
        canones[i]->setTransformOriginPoint(0, 0);
    }

    // Trayectoria del último disparo y proyectil
    itemTrayectoria = escena->addPath(QPainterPath(), QPen(QColor(220, 40, 40), 1.5, Qt::DashLine));
    double r = Juego::RADIO_PROY * ESC;
    itemProyectil = escena->addEllipse(-r, -r, 2 * r, 2 * r, QPen(Qt::black), QBrush(Qt::red));
    itemProyectil->setZValue(5);
    itemProyectil->setVisible(false);

    actualizarEscena();
    actualizarCanon();
}

// Sincroniza lo dibujado con el estado de Juego (se llama cada tick).
void MainWindow::actualizarEscena() {
    const std::vector<Obstaculo>& obs = juego.getObstaculos();
    for (size_t i = 0; i < obs.size() && i < bloques.size(); ++i) {
        const Obstaculo& o = obs[i];
        bool visible = !o.estaDestruido();
        bloques[i].rect->setVisible(visible);
        bloques[i].texto->setVisible(visible);
        if (!visible) continue;

        // Se va volviendo transparente a medida que pierde resistencia
        double fraccion = o.getResistencia() / o.getResistenciaMax();
        bloques[i].rect->setOpacity(0.35 + 0.65 * fraccion);
        bloques[i].texto->setText(QString::number(static_cast<int>(std::ceil(o.getResistencia()))));
        QRectF br = bloques[i].texto->boundingRect();
        bloques[i].texto->setPos((o.getPosX() + o.getAncho() / 2) * ESC - br.width() / 2,
                                 (o.getPosY() + o.getAlto() / 2) * ESC - br.height() / 2);
    }

    const Proyectil* p = juego.getProyectil();
    if (p) {
        QPointF pos(p->getPosX() * ESC, p->getPosY() * ESC);
        itemProyectil->setPos(pos);
        itemProyectil->setVisible(true);
        if (ruta.elementCount() == 0) ruta.moveTo(pos); else ruta.lineTo(pos);
        itemTrayectoria->setPath(ruta);
    } else {
        itemProyectil->setVisible(false);
    }
}

// Gira el cañón del jugador en turno. Qt mide la rotación en sentido horario con y hacia abajo:
//   jugador 1 (apunta a la derecha): rotación = -ángulo
//   jugador 2 (apunta a la izquierda): rotación = 180 + ángulo
void MainWindow::actualizarCanon() {
    if (!canones[0] || !canones[1]) return;
    int t = juego.getTurno();
    canones[0]->setRotation(t == 0 ? -ui->spinAngulo->value() : -45);
    canones[1]->setRotation(t == 1 ? 180 + ui->spinAngulo->value() : 180 + 45);
}

void MainWindow::actualizarHUD() {
    bool apuntando = juego.getEstado() == EstadoJuego::APUNTANDO;
    ui->lblTurno->setText(QString::fromStdString(juego.getJugadores()[juego.getTurno()].getNombre()));
    ui->lblMensaje->setText(QString::fromStdString(juego.getMensaje()));
    ui->spinAngulo->setEnabled(apuntando);
    ui->spinVelocidad->setEnabled(apuntando);
    ui->btnDisparar->setEnabled(apuntando);
}

void MainWindow::on_btnDisparar_clicked() {
    if (juego.getEstado() != EstadoJuego::APUNTANDO) return;
    ruta = QPainterPath();                       // borra la trayectoria anterior
    itemTrayectoria->setPath(ruta);
    juego.disparar(ui->spinAngulo->value(), ui->spinVelocidad->value());
    actualizarHUD();
}

// Cada tick (16 ms ~ 1/60 s) se simulan 4 pasos físicos de 1/240 s.
// Pasos pequeños evitan que un proyectil rápido "atraviese" un bloque entre dos pasos.
void MainWindow::onTick() {
    if (juego.getEstado() != EstadoJuego::VOLANDO) return;

    for (int i = 0; i < 4 && juego.getEstado() == EstadoJuego::VOLANDO; ++i)
        juego.avanzar(1.0f / 240.0f);

    actualizarEscena();
    if (juego.getEstado() != EstadoJuego::VOLANDO) {   // terminó el vuelo: cambió el turno o hubo ganador
        actualizarHUD();
        actualizarCanon();
        if (juego.getEstado() == EstadoJuego::TERMINADO)
            QMessageBox::information(this, "Fin del juego", QString::fromStdString(juego.getMensaje()));
    }
}

void MainWindow::on_btnReiniciar_clicked() {
    juego.reiniciar();
    construirEscena();
    actualizarHUD();
}

// Teclado: ↑/↓ ángulo, ←/→ velocidad, Espacio/Enter dispara
void MainWindow::keyPressEvent(QKeyEvent* e) {
    if (juego.getEstado() != EstadoJuego::APUNTANDO) { QMainWindow::keyPressEvent(e); return; }
    switch (e->key()) {
    case Qt::Key_Up:    ui->spinAngulo->setValue(ui->spinAngulo->value() + 1); break;
    case Qt::Key_Down:  ui->spinAngulo->setValue(ui->spinAngulo->value() - 1); break;
    case Qt::Key_Right: ui->spinVelocidad->setValue(ui->spinVelocidad->value() + 1); break;
    case Qt::Key_Left:  ui->spinVelocidad->setValue(ui->spinVelocidad->value() - 1); break;
    case Qt::Key_Space:
    case Qt::Key_Return: on_btnDisparar_clicked(); break;
    default: QMainWindow::keyPressEvent(e);
    }
}
