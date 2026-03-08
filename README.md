# Object Detection & Image Classification with TensorFlow Lite

An Android demonstration application showcasing real-time on-device machine learning capabilities using TensorFlow Lite. This project is designed for learning purposes, demonstrating how to integrate pre-trained ML models into an Android environment.

## 🚀 Features

*   **Real-time Object Detection:** Detect and locate multiple objects in the camera feed.
*   **Image Classification:** Identify primary objects in a scene.
*   **On-Device Processing:** All inferences are performed locally using TensorFlow Lite for privacy and offline capability.
*   **GPU Acceleration:** Utilizes the device's GPU (via TFLite GPU delegate) for faster performance where supported.
*   **Image Processing:** Includes examples of basic image manipulation like grayscale conversion.
*   **Modern Android Development:** Built using Kotlin, ViewBinding, and ML Model Binding.

## 🛠 Tech Stack & Libraries

*   **Language:** [Kotlin](https://kotlinlang.org/)
*   **Machine Learning:** [TensorFlow Lite](https://www.tensorflow.org/lite)
    *   `tensorflow-lite-support`
    *   `tensorflow-lite-gpu`
    *   `tensorflow-lite-metadata`
*   **UI Components:** Material Design, ConstraintLayout
*   **Build System:** Gradle (Kotlin DSL) with Version Catalogs

## 📦 Project Structure

*   **`/app/src/main/ml`**: Contains the `.tflite` models used for detection and classification.
*   **`/app/src/main/java`**: Core logic for camera handling and model inference.
*   **`/app/src/main/cpp`**: Native C++ implementations (if applicable for custom processing).

## ⚙️ Setup & Installation

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/ObjectDetectionWithTensorFlowDemo.git
    ```
2.  **Open in Android Studio:**
    - Open Android Studio (Ladybug or newer recommended).
    - Select `File > Open` and navigate to the project folder.
3.  **Build the project:**
    - Allow Gradle to sync and download dependencies.
    - Ensure you have the Android SDK 34+ installed.
4.  **Run on Device:**
    - Connect a physical Android device (API 24 or higher).
    - Note: Camera features require a physical device for the best experience.

## 📖 How it Works

1.  **Camera Stream:** The app captures frames from the camera using the Android Camera APIs.
2.  **Preprocessing:** Frames are resized and normalized to match the input requirements of the TFLite model.
3.  **Inference:** The TFLite model processes the image and returns a list of detections (labels, confidence scores, and bounding boxes).
4.  **Post-processing:** The results are mapped back to the screen coordinates and drawn over the camera preview.

## 📜 Privacy

This app does not collect or store any personal data. All camera data is processed in real-time in the device's volatile memory and is never saved or transmitted. See [PRIVACY_POLICY.md](PRIVACY_POLICY.md) for more details.

## 🎓 Learning Objectives

This project was created to explore:
- Integrating `.tflite` models into Android using **ML Model Binding**.
- Handling Camera permissions and real-time surface drawing.
- Optimizing ML performance with GPU delegates.
- Implementing a multi-activity dashboard for different ML tasks.

## 📄 License

This project is for educational purposes. Feel free to use the code as a reference for your own learning.
