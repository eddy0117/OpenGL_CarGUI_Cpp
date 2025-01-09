# Simulated Sender Program

> **Note:** The `sender` folder contains all the necessary objects for simulating the sender. If the sender simulation is not required, you can delete the `sender` folder without affecting the operation of the GUI program.

### Create a Python Environment
```
conda create -n carguisender_cpp python=3.9
conda activate carguisender_cpp
```

### Install Dependencies for Simulated Sender
```
pip install opencv-python-headless
```

### Run the GUI Program
```
./build/OpenGL_CarGUI
```

### Run the Simulated Sender (Recommended to open a new terminal)
```
cd ./sender
```
```
python DataSender_TCP_vec_occ_cpp.py
