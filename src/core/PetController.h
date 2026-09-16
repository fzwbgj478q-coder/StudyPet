#pragma once

#include <QObject>

class PetWindow;

/// Coordinates application-level lifecycle without putting it in the window class.
class PetController final : public QObject
{
    Q_OBJECT

public:
    explicit PetController(QObject *parent = nullptr);
    void start();

private:
    PetWindow *m_petWindow = nullptr;
};

