export RML=/home/sokirko/ddc-2.0.37
while [ 1 ]; do
    $RML/bin/ddc_daemon start_cli
    sleep 5
done
