# EegStreamer

脑电采集系统控制软件，适用HDU脑机接口实验室自研脑电采集主板BrainWave。

# Prerequistites

## Install packages

```bash
pacman -S qt6-base qt6-charts qt6-serialport
```

## Install brainflow

下载Brainflow，编译安装。

# Build

```bash
mkdir build
cd build
qmake6 ..
make -j4
```

# Start Application

```bash
./EegSreamer
```
