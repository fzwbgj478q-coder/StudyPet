# Desktop-pet animation assets

The active desktop-pet assets are transparent 512 x 512 PNG frames in
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

![Animation contact sheet](images/kitty-animation-preview.png)

## Attribution and scope

Hello Kitty is a Sanrio character. These reference-based assets are included
only for this course project’s non-commercial learning and demonstration use.
StudyPet does not claim ownership of the character or related marks.
