while [ 1 ]; do
    echo "start synan"
    date
    $RML/Bin/SynanDaemon start_cli
    sleep 5
done
