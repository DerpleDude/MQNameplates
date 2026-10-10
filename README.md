## MQNameplates

MQNameplates replaces the stock EverQuest name sprites with animated, ImGui-drawn nameplates. Each nameplate is anchored to the spawn's head bone and shows the name, level, class, an animated HP bar, an optional guild or purpose line, and (for your target) the spawn's buff icons with a yellow indicator on buffs you cast. Nameplates are drawn for six spawn categories (Self, Target, Group, Auto Haters / XTargets, PCs, NPCs), each of which can be toggled independently and assigned its own visual style. Line-of-sight and distance culling, distance scaling, and a smooth HP interpolation keep the overlay readable without cluttering the screen. All settings are editable live from the MacroQuest settings window and persist to a YAML file.

### Known Bugs

EQ Nameplates are not correctly disabled.

### Getting Started

Load the plugin from the MacroQuest chat window, then open the settings panel under Settings > Plugins > Nameplates to turn categories on and off and tweak styles. By default Self, Target and Auto Hater nameplates are enabled; Group, PC and NPC nameplates are off.

[CODE=txt]/plugin Nameplates
[/CODE]

### Commands

Almost everything is driven from the settings panel, which has four tabs: Look and Feel, Style Editor, Rendering Options, and Dev and Debug.

[CODE=txt]/nameplates reload - Re-read MQNameplates.yaml from disk and apply it (useful after hand-editing the file)
[/CODE]

### Other Notes

- Nameplates for spawns that haven't been drawn in 30 seconds are discarded; Target, Group and Auto Hater nameplates are also dropped as soon as the spawn leaves that role. Toggling a category off is instant.
- Line-of-sight is tested from the camera first, then from your character's head, so a mob behind a pillar relative to the camera can still show if you can see it.
- Nameplates are sorted so the furthest draw first and your current target always draws on top.
- Invisible and dynamic-object spawns are supported.
- HP bars animate smoothly toward the real value rather than snapping. The Color Range style blends through an automatically computed mid color to avoid the flicker you get when lerping between very different hues.
- The Dev and Debug tab includes a test bar, fake HP percentages, and a bounding box overlay for tuning positioning without needing a live target.
- Blinking buff icons are on the TODO list.

### Screenshots
<img width="471" height="431" alt="image" src="https://github.com/user-attachments/assets/7eb5d9b8-3c3c-4498-bf66-9229d1768d32" />
<img width="2868" height="1085" alt="image" src="https://github.com/user-attachments/assets/4c2c8008-682a-487c-aedc-8a7b0a1462e5" />
<img width="988" height="1068" alt="image" src="https://github.com/user-attachments/assets/f54e78b0-f539-46a3-b0a0-e280ef838bed" />
<img width="989" height="1240" alt="image" src="https://github.com/user-attachments/assets/e59a36c1-20e7-49e0-b0d6-d9cf4999a258" />
<img width="985" height="583" alt="image" src="https://github.com/user-attachments/assets/68191ad2-63fd-4e94-814a-057170c0fe8e" />
<img width="994" height="662" alt="image" src="https://github.com/user-attachments/assets/68c1864c-0e23-4451-9c25-aad72c62bd59" />

### Authors

- Derple - Initial work
- brainiac - Nameplate positioning, LOS, and config variable system
