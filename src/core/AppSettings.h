#pragma once

struct AppSettings {
    double petScale = 1.0;
    double movementSpeed = 1.0;
    double animationSpeed = 1.0;
    int volume = 70;
    bool alwaysOnTop = true;
    bool clickThrough = false;
    bool autoActivity = true;
    bool proactiveBubbles = true;
    bool autoStart = false;
    int focusMinutes = 25;
    int breakMinutes = 5;
    bool waterReminder = true;
    int waterIntervalMinutes = 45;
    bool restReminder = true;
    int restIntervalMinutes = 60;

    void sanitize() {
        petScale = petScale < 0.5 || petScale > 2.0 ? 1.0 : petScale;
        movementSpeed = movementSpeed < 0.25 || movementSpeed > 4.0 ? 1.0 : movementSpeed;
        animationSpeed = animationSpeed < 0.25 || animationSpeed > 4.0 ? 1.0 : animationSpeed;
        volume = volume < 0 || volume > 100 ? 70 : volume;
        focusMinutes = focusMinutes < 1 || focusMinutes > 180 ? 25 : focusMinutes;
        breakMinutes = breakMinutes < 1 || breakMinutes > 60 ? 5 : breakMinutes;
        waterIntervalMinutes = waterIntervalMinutes < 5 || waterIntervalMinutes > 360 ? 45 : waterIntervalMinutes;
        restIntervalMinutes = restIntervalMinutes < 10 || restIntervalMinutes > 360 ? 60 : restIntervalMinutes;
    }
};

