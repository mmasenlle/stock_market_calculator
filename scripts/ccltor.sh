#! /bin/sh

[ $CCLTOR_PATH ] || CCLTOR_PATH=`pwd`

export LD_LIBRARY_PATH=$CCLTOR_PATH

case "$1" in
  start)
  	sh -v -c $CCLTOR_PATH/ccltor_feeder &
  	sh -v -c $CCLTOR_PATH/ccltor_cruncher &
  	sleep 1
	;;
  restart)
	$0 stop
	sleep 3
	$0 start
	;;
  stop)
  	killall -v ccltor_feeder
  	killall -v ccltor_cruncher
	;;
  *)
	echo "Usage: $0 start|stop" >&2
	exit 3
	;;
esac

