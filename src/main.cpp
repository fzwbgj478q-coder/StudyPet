#include <QApplication>

#include "core/PetController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("StudyPet"));
    app.setOrganizationName(QStringLiteral("StudyPet Team"));
    app.setApplicationVersion(QStringLiteral(STUDYPET_VERSION));

    PetController controller;
    controller.start();

    return app.exec();
}

