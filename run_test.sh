#!/bin/bash

# 測試次數
TEST_ROUNDS=10
# GUI 程式啟動等待時間（秒）→ 改為 3 秒
GUI_WARMUP_TIME=3
# CPU 監控時間（秒）→ 改為 20 秒
DURATION=20
# 假設 GUI 程式使用的 PORT
PORT=65432

# 確認 Conda 可用
if ! command -v conda &> /dev/null; then
    echo "Conda 未安裝或未正確設定環境變數。"
    exit 1
fi

for i in $(seq 1 $TEST_ROUNDS); do
    echo "第 $i 次測試開始"

    # 釋放佔用的端口
    fuser -k ${PORT}/tcp 2>/dev/null

    # 啟動 GUI 程式（背景執行）
    ./build/OpenGL_CarGUI &
    APP_PID=$!

    # 確認 GUI 程式是否正常啟動
    sleep 3
    if ! ps -p $APP_PID > /dev/null; then
        echo "GUI 程式啟動失敗，跳過第 $i 次測試。"
        continue
    fi

    echo "GUI 程式 PID: $APP_PID，開始暖機 $GUI_WARMUP_TIME 秒..."
    sleep $GUI_WARMUP_TIME

    echo "開始記錄 GUI 程式 CPU 使用率..."
    # 開始記錄 GUI 程式 CPU 使用率
    pidstat -p $APP_PID 1 $DURATION > result_$i.txt &
    PIDSTAT_PID=$!

    # 啟動 Python 資料發送程式（背景執行）
    eval "$(conda shell.bash hook)"
    conda activate carguisender_cpp
    cd ./sender
    python DataSender_TCP_vec_occ_cpp.py &
    PYTHON_PID=$!
    cd ..

    # 確認 Python 程式是否正常啟動
    sleep 3
    if ! ps -p $PYTHON_PID > /dev/null; then
        echo "Python 程式啟動失敗，結束 GUI 程式與監控。"
        kill $PIDSTAT_PID
        kill $APP_PID
        wait $APP_PID 2>/dev/null
        continue
    fi

    echo "Python 程式 PID: $PYTHON_PID"

    # 等待 CPU 監控結束
    wait $PIDSTAT_PID

    # 測試結束，終止 Python 與 GUI 程式
    kill $PYTHON_PID
    wait $PYTHON_PID 2>/dev/null

    kill $APP_PID
    wait $APP_PID 2>/dev/null

    echo "第 $i 次測試結束"
    sleep 5  # 減少測試間的干擾
done

echo "所有測試完成"
