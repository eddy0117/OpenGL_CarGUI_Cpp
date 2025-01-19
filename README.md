### 1. Install packages
```
sudo apt install libgl1-mesa-dev
sudo apt install libglfw3-dev
sudo apt install libglew-dev
sudo apt install libglm-dev
```

### 2. Install cmake
```
sudo apt update
sudo apt install cmake
```

### 3. Create a build directory (usually separate from the source code):
``` bash
mkdir build
cd build
```

### 4. Run CMake to generate build system files:
```
cmake ..
```

### 5. Run the GUI
Switch to the main directory
```
cd ..
```
run GUI
```
./build/OpenGL_CarGUI
```


## Optional
1.If you want to configure a simulated data sender.[HERE](docs/datasender.md)



## performance test
### Run the CPU performance test script
```
chmod +x run_test.sh
```
```
./run_test.sh
```
### Generate a visualization chart
```
gnuplot plot_cpu_usage.gp
xdg-open cpu_usage_plot.png
```




