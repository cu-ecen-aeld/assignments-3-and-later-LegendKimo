#!/bin/sh
### BEGIN INIT INFO
# Provides:          aesdsocket
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start/Stop aesdsocket server
### END INIT INFO

DAEMON=/usr/bin/aesdsocket          # path where aesdsocket binary is installed
DAEMON_ARGS="-d"                    # run in daemon mode
DAEMON_NAME=aesdsocket

case "$1" in
start)
    echo "Starting $DAEMON_NAME..."
    start-stop-daemon -S -n $DAEMON_NAME -a $DAEMON -- $DAEMON_ARGS
    if [ $? -eq 0 ]; then
        echo "$DAEMON_NAME started successfully."
    else
        echo "Failed to start $DAEMON_NAME (already running?)."
    fi
    ;;
stop)
    echo "Stopping $DAEMON_NAME..."
    start-stop-daemon -K -n $DAEMON_NAME -s SIGTERM
    if [ $? -eq 0 ]; then
        echo "$DAEMON_NAME stopped gracefully."
    else
        echo "Failed to stop $DAEMON_NAME (maybe not running?)."
    fi
    ;;
restart)
    echo "Restarting $DAEMON_NAME..."
    $0 stop
    sleep 1
    $0 start
    ;;
*)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
    ;;
esac

exit 0
