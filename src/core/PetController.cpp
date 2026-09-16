#include "PetController.h"

#include "window/PetWindow.h"

PetController::PetController(QObject *parent)
    : QObject(parent)
    , m_petWindow(new PetWindow)
{
    connect(m_petWindow, &PetWindow::exitRequested, this, [] {
        QCoreApplication::quit();
    });
}

void PetController::start()
{
    m_petWindow->show();
}

