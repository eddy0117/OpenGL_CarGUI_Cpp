


data 資料夾  sender資料夾   'json/pts.json'   'json/speeds.txt'  json/vehicle_monitor.json'
 'json/traj.json'


 dummy_imgs/


```
conda create -n carguisender_cpp python=3.9
conda activate carguisender_cpp
```

```
pip install opencv-python-headless
```



```
cd sender
```

```
python DataSender_TCP_vec_occ_cpp.py
```