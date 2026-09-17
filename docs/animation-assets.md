# Desktop-pet animation assets

The active desktop-pet assets are transparent 128 x 128 PNG frames in
`resources/animations/kitty/`. Each action has four frames and is configured
declaratively in `resources/config/animations.json`.

| Action | Frames | Playback |
| --- | ---: | --- |
| Idle / blink | 4 | 650 ms, loops |
| WalkRight / WalkLeft | 4 shared frames | 165 ms, loops; left is mirrored by `AnimationPlayer` |
| Talking | 4 | 220 ms, loops |
| Happy | 4 | 145 ms, once |
| Reminder | 4 | 220 ms, once |
| Sleep | 4 | 1000 ms, loops |

The blue placeholder sequence remains packaged as `fallbackFrames`. If a new
frame is missing, malformed, or cannot be decoded, `AnimationPlayer` logs a
warning and continues with the safe fallback instead of terminating the app.

## Contact sheet

| Action | Frame 1 | Frame 2 | Frame 3 | Frame 4 |
| --- | --- | --- | --- | --- |
| Idle / blink | ![](../resources/animations/kitty/idle/kitty-idle-01.png) | ![](../resources/animations/kitty/idle/kitty-idle-02.png) | ![](../resources/animations/kitty/idle/kitty-idle-03.png) | ![](../resources/animations/kitty/idle/kitty-idle-04.png) |
| Walk | ![](../resources/animations/kitty/walk/kitty-walk-01.png) | ![](../resources/animations/kitty/walk/kitty-walk-02.png) | ![](../resources/animations/kitty/walk/kitty-walk-03.png) | ![](../resources/animations/kitty/walk/kitty-walk-04.png) |
| Talking | ![](../resources/animations/kitty/talking/kitty-talking-01.png) | ![](../resources/animations/kitty/talking/kitty-talking-02.png) | ![](../resources/animations/kitty/talking/kitty-talking-03.png) | ![](../resources/animations/kitty/talking/kitty-talking-04.png) |
| Happy | ![](../resources/animations/kitty/happy/kitty-happy-01.png) | ![](../resources/animations/kitty/happy/kitty-happy-02.png) | ![](../resources/animations/kitty/happy/kitty-happy-03.png) | ![](../resources/animations/kitty/happy/kitty-happy-04.png) |
| Reminder | ![](../resources/animations/kitty/reminder/kitty-reminder-01.png) | ![](../resources/animations/kitty/reminder/kitty-reminder-02.png) | ![](../resources/animations/kitty/reminder/kitty-reminder-03.png) | ![](../resources/animations/kitty/reminder/kitty-reminder-04.png) |
| Sleep | ![](../resources/animations/kitty/sleep/kitty-sleep-01.png) | ![](../resources/animations/kitty/sleep/kitty-sleep-02.png) | ![](../resources/animations/kitty/sleep/kitty-sleep-03.png) | ![](../resources/animations/kitty/sleep/kitty-sleep-04.png) |

## Attribution and scope

Hello Kitty is a Sanrio character. These reference-based assets are included
only for this course project’s non-commercial learning and demonstration use.
StudyPet does not claim ownership of the character or related marks.

