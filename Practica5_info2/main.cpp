/*

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
*/

#include <iostream>
#include "Actualizacion.h"
using namespace std;

int main() {
    // 1. Configurar la simulación
    Actualizacion sim(800, 600, 0.016, 0.8);

    // 2. Crear y agregar partículas
    sim.agregarParticula(Particula(100, 100, 50, 0, 1.0, 10.0, 1));
    sim.agregarParticula(Particula(200, 200, -30, 40, 2.0, 15.0, 2));

    // 3. Crear obstáculos
    sim.agregarObstaculo(Obstaculo(300, 250, 100, 100));

    // 4. Ejecutar
    sim.iniciarSimulacion();
    sim.ejecutarSimulacion();

    cout << "Listo, revisa simulacion.txt" << endl;
    return 0;
}