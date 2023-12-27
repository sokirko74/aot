while [ 1 ]; do
    echo "start synan"
    date
    $RML/Bin/SynanDaemon --host 63.250.59.257 --port 8081
    sleep 5
done
