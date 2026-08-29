# Tungsten CAD Photogrammetry Scanner

Native Android app for capturing photogrammetry data. Guides the user through capturing photos around an object, records device orientation (IMU) for each shot, calibrates the phone camera, and exports everything as a zip file consumable by the Tungsten CAD desktop application.

## Build

```
./gradlew assembleDebug
```

Requires Android Studio Iguana (2023.2.1) or later with SDK 34.

## Export Zip Format

The exported zip contains:

```
project.zip
├── images/
│   ├── IMG_0001.jpg
│   ├── IMG_0002.jpg
│   └── ...
├── poses.json
├── calibration.json
└── project.json
```

- **images/** — JPEG photos in capture order.
- **poses.json** — Per-image camera pose (position + quaternion rotation) derived from IMU data.
- **calibration.json** — Camera intrinsics (3x3 matrix, 5 distortion coefficients, image size).
- **project.json** — Device info, capture settings, date, and image count.

## Permissions

- Camera — required for photo capture.
- Write External Storage — legacy fallback for saving to shared storage on Android 9–10.
