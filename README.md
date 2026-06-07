# Computer Vision Image Processing and Analytics Toolkit

A cross-platform, high-performance C++ desktop application engineered using the Qt Framework and OpenCV core matrix representations. This toolkit implements modular analytical frameworks for spatial-domain and frequency-domain image manipulation, featuring granular additive noise modeling, dynamic local neighborhood filtering, classical edge-detection matrices, statistical histogram transformations, and complex frequency-domain hybrid image synthesis.

---

## Technical Pipeline Architecture

The application design isolates intensive image convolution and fast Fourier transform (FFT) matrix calculations into specialized algorithmic pipelines, preventing graphical interface stuttering during live parameter scaling.

```text
+-------------------------------------------------------------------------+
|                                QT GUI LAYER                             |
|          (Dynamic Combo Boxes, Parameter Sliders, Multi-Canvas Renders) |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                     ALGORITHMIC TOOLKIT CORE (C++)                      |
|  [Noise Generators] -> [Spatial/Frequency Filters] -> [Contrast Engines] |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                          OPENCV MATRIX CORE                             |
|          (cv::Mat Buffers, Discrete Fourier Transforms, Convolutions)   |
+-------------------------------------------------------------------------+

```

---

## Core Algorithmic Capabilities

### 1. Granular Additive Noise Simulation

Implements parametric noise-injection layers controlled dynamically via the graphical interface to simulate physical sensor degradation:

* **Uniform Noise Framework:** introduces bounded random perturbations across specified interval matrix structures utilizing uniform distribution fields.
* **Gaussian Noise Generator:** models thermal sensor noise via normal distribution parameters where standard deviation scales natively using the scalar constraint:

$$\sigma = \sqrt{\text{variance}}$$


* **Salt & Pepper Impulse Noise:** simulates binary dead pixels by evaluating an independent probability matrix generated over the domain $[0.0, 1.0]$. Pixels transform into white (salt) or black (pepper) artifacts based on the condition thresholds relative to the target density parameter ($p$).

---

### 2. Spatial-Domain Local Filtering

Provides scalable multi-kernel smoothing arrays ($3 \times 3$ and $5 \times 5$) optimized to reverse specific localized noise profiles:

* **Linear Average Smoothing:** executes mean neighborhood block transformations to suppress broad pixel variation.
* **Gaussian Weight Filtering:** applies space-variant Gaussian kernel convolution to smooth noise while preserving macro structural edges.
* **Non-Linear Median Filtering:** replaces target pixel metrics with exact neighborhood medians, isolating extreme outlier vectors to wipe out impulse Salt & Pepper noise without inducing edge blur.

---

### 3. Spatial Gradient Edge Detection

Integrates classical spatial-derivative convolution matrices to compute structural image gradients and isolate geometric object boundaries:

* **Sobel Operator Matrix:** applies directional derivative kernels to prioritize horizontal and vertical structural trends.
* **Prewitt Derivative Engine:** computes localized intensity orientation offsets using balanced linear gradient masks.
* **Roberts Cross Operator:** executes fast diagonal differential tracking optimized for localized high-contrast boundary profiles.

---

### 4. Statistical Histogram Analytics & Contrast Enhancement

Exposes raw digital exposure statistics and implements mathematical transformation maps to optimize localized gray-level dynamics:

* **Live Histogram & CDF Mapping:** samples exact intensity counts across the 8-bit allocation space to plot live gray-level distributions and corresponding Cumulative Distribution Functions (CDF).
* **Global Histogram Equalization:** reshapes compressed input distributions to linearize the target CDF, amplifying global contrast states.
* **Linear Min-Max Normalization:** rescales active matrix dynamic ranges cleanly across the absolute boundaries $[0, 255]$ to restore details in underexposed inputs.

---

### 5. Frequency-Domain Filtering & Hybrid Image Synthesis

Transforms 2D spatial layouts into complex frequency planes to handle advanced signal isolation and create perceptual hybrid graphics:

* **Discrete Fourier Transform (DFT):** maps standard matrix channels into the complex frequency domain via fast discrete transformations.
* **Ideal and Gaussian Frequency Masks:** executes crisp Low-Pass Filters (LPF) and High-Pass Filters (HPF) directly within the transformed frequency domain to smoothly isolate structured scale signals.
* **Perceptual Hybrid Fusion:** structurally superimposes the low-frequency component of one scene onto the isolated high-frequency components of another, generating an image whose interpretation transforms as a function of visual distance.

---

## Application Output Gallery

### 1. Additive Noise & Artifact Insertion

* **Uniform Noise Profile:**
  
  <img width="1565" height="914" alt="noise_uniform" src="https://github.com/user-attachments/assets/2a43189b-7a6e-46d8-a8fd-0cee5788732a" />

* **Gaussian Noise Profile:**
  
<img width="1565" height="914" alt="noise_gaussian" src="https://github.com/user-attachments/assets/b3a51ca7-0502-4fd5-8797-d022a3c2f7be" />

* **Salt & Pepper Impulse Profile:**

<img width="1565" height="914" alt="noise_sp" src="https://github.com/user-attachments/assets/e81671e8-d4f3-4494-b90c-b3a515e93151" />


### 2. Spatial Noise-Reduction Subsystems

* **Targeted Neighborhood Filtering Outputs:**
  
<img width="1257" height="422" alt="image" src="https://github.com/user-attachments/assets/0e56ad8b-3350-4665-af6c-fd8d789cd97a" />


### 3. Directional Gradient Tracking

* **Edge Extraction Operator Variations:**
  
<img width="551" height="558" alt="image" src="https://github.com/user-attachments/assets/6dd9ab61-bb3a-41a3-9673-93430783afe3" />


### 4. Exposure Correction & Contrast Enhancement

* **Histogram Transformations and Live CDF Distributions:**

<img width="1565" height="914" alt="hist_gray" src="https://github.com/user-attachments/assets/af56559b-6892-4473-a627-a8b9d72c0cb0" />
<img width="1565" height="914" alt="hist_rgb" src="https://github.com/user-attachments/assets/c19f5f22-82e2-44ac-8a94-3472db856802" />

* **Advanced Contrast Enhancement Frameworks:**

<img width="1092" height="426" alt="image" src="https://github.com/user-attachments/assets/51e9eb6b-1042-477b-b080-107f1dd6825a" />



### 5. Complex Frequency Domain & Hybrid Synthesis

* **Frequency Spectrum Transformations and Final Hybrid Outputs:**

<img width="1565" height="914" alt="hybrid_ui" src="https://github.com/user-attachments/assets/b8a4ce64-ade2-43a3-9472-3f5293a3b9d9" />

---

## Repository Directory Tree

```text
project7-cv-image-processing/
├── CMakeLists.txt                 # Master cross-platform build pipeline configurations
├── .gitignore                     # Excludes local compiler cache and dynamic build folders
├── main.cpp                       # Application setup and primary thread initialization
├── mainwindow.cpp                 # Master GUI layout logic and signal routing configurations
├── mainwindow.h                   # Structural UI slots, view matrix allocations, and actions
├── mainwindow.ui                  # Graphical XML setup for widgets, sliders, and canvases
├── frequencytab.cpp               # Implementation of 2D-DFT, frequency masks, and hybrid fusion
├── frequencytab.h                 # Classes and lookup parameters for frequency processing channels
├── histogramtab.cpp               # Logic loops for intensity count distribution, equalization, and CDF mapping
├── histogramtab.h                 # Declarations for analytical graph generation and mapping arrays
├── utils.h                        # Core matrix transformation buffers and shared conversion utilities
├── Task1_Report_Team4.pdf         # Comprehensive technical report and mathematical evaluations
└── assets/                        # Target output gallery image frames (uniform_noise.png, etc.)

```

---

## Toolchain Setup and Deployment

### Prerequisites

* Build Environment: CMake (Version 3.16 or higher).
* Graphical Framework: Qt Creator / Qt5 or Qt6 core library distributions.
* Matrix Dependency: OpenCV Development Libraries (For `cv::Mat` structures and basic file I/O).
* Compiler Requirements: C++17 compliant compiler environment.

### Build Pipeline

1. Clone the repository including its nested structures:
```bash
git clone git@github.com:lyan2003/Qt-Computer-Vision-Image-Processing-Toolkit.git

```


2. Create and switch to an independent build tracking tree:
```bash
mkdir build && cd build
cmake ..

```


3. Trigger native compiler pipelines to assemble targets:
```bash
cmake --build .

```


4. Execute the resulting desktop toolkit binary:
```bash
./CVImageProcessingToolkitApp

```
```

```
