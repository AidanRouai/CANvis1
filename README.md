# CAN Visualizer

A lightweight, cross-platform GUI tool for visualizing Controller Area Network (CAN) traffic from `.log` and `.dbc` files. Built using **C++20** and the **Qt framework**, this tool helps embedded systems engineers, automotive developers, and hobbyists parse and analyze CAN bus data interactively and efficiently.

## 🚗 Project Overview

Modern vehicles rely on a network of Electronic Control Units (ECUs) that communicate using the CAN bus protocol. This tool allows users to:
- Load and parse CAN log files (SocketCAN format)
- Step through CAN messages in timestamp order
- Visually track message activity by ID
- Decode message IDs using DBC files
- Playback data at multiple speeds
- Filter messages by CAN ID
- Analyze activity with a clean and intuitive UI

## 🛠️ Features

- **Responsive GUI**: Qt-based interface for real-time interaction.
- **Playback Controls**: Play, pause, fast-forward CAN traffic at up to 32x.
- **ID Visualization**: CAN IDs light up as messages are received.
- **DBC Parsing**: Uses [mireo/can-utils](https://github.com/mireo/can-utils) for decoding message names.
- **Data Filtering**: Filter by individual CAN IDs for focused debugging.
- **SocketCAN Support**: Designed around the Linux-native SocketCAN `.log` format.

## 📂 File Inputs

- **CAN Log File** (`.log` or `.csv`): SocketCAN style logs (e.g. `can0 152#F08C000000000082`)
- **DBC File** (`.dbc`): Describes how to decode raw CAN data into human-readable signals

## 📦 Dependencies

- [Qt 6 or higher](https://www.qt.io/)
- C++20 compiler (tested with GCC and Clang)
- [mireo/can-utils](https://github.com/mireo/can-utils) for DBC parsing

## 🚀 Getting Started

1. Clone the repository:
    ```bash
    git clone https://github.com/AidanRouai/CANvis1.git
    cd CANvis1
    ```

2. Install Qt and can-utils.

3. Build using CMake:
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

4. Run the application:
    ```bash
    ./CANVisualizer
    ```

## 🔮 Future Work

- Real-time CAN stream support from live interfaces
- Richer visual analytics (e.g., signal graphs, gauges)
- Support for more formats (BLF, MF4, ASC)
- Web dashboard for remote data access

## 📜 License

This project uses publicly available CAN log and DBC files under permissive licenses. See the [ElDominio CANBUSlogs](https://github.com/ElDominio/CANBUSlogs) and [CSS Electronics DBC files](https://www.csselectronics.com) for more details.

---

Made with 🚗 and C++ by [Aidan Rouai](https://github.com/AidanRouai)
