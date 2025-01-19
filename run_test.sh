#!/bin/bash

# 測試次數
TEST_ROUNDS=10
# GUI 程式啟動等待時間（秒）
GUI_WARMUP_TIME=3
# CPU 監控時間（秒）
DURATION=20
# 統一結果檔案名稱
PLOT_DATA_FILE="cpu_usage_plot_data.txt"

# 清空舊的資料
> $PLOT_DATA_FILE

for i in $(seq 1 $TEST_ROUNDS); do
    echo "第 $i 次測試開始"

    # 釋放 PORT
    fuser -k 65432/tcp 2>/dev/null

    # 啟動 GUI 程式
    ./build/OpenGL_CarGUI &
    APP_PID=$!

    # 暖機
    sleep $GUI_WARMUP_TIME

    # 記錄 CPU 使用率
    pidstat -p $APP_PID 1 $DURATION > temp_pidstat_$i.txt &
    PIDSTAT_PID=$!

    # 執行 Python 程式
    eval "$(conda shell.bash hook)"
    conda activate carguisender_cpp
    cd ./sender
    python DataSender_TCP_vec_occ_cpp.py &
    PYTHON_PID=$!
    cd ..

    # 等待 CPU 記錄結束
    wait $PIDSTAT_PID

    # 轉換 CPU 使用率（從 0 開始編號）
    awk -v round=$i '
    BEGIN { idx = 0 }
    /^[0-9]/ && $9 ~ /^[0-9.]+$/ {
        print idx, $9
        idx++
    }
    END { print "# 測試輪次 " round " 結束\n" }
    ' temp_pidstat_$i.txt >> $PLOT_DATA_FILE

    # 結束程序
    kill $PYTHON_PID
    kill $APP_PID
    wait $APP_PID 2>/dev/null

    echo "第 $i 次測試結束"
    sleep 5
done

echo "所有測試完成"
