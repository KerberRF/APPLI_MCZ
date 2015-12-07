#!/bin/sh

# This file is part of AppliMCZ.
#
# AppliMCZ is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AppliMCZ is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Jeedom. If not, see <http://www.gnu.org/licenses/>.

### BEGIN INIT INFO
# Provides:          AppliMCZ
# Required-Start:    $local_fs $network $remote_fs
# Required-Stop:     $local_fs $network $remote_fs
# Should-Start:      $NetworkManager
# Should-Stop:       $NetworkManager
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: starts instance of APPLIMCZ
# Description:       starts instance of APPLIMCZ using start-stop-daemon
### END INIT INFO

PATH_TO_APPLIMCZ="/home/pi/Dev/APPLIMCZ/src"


####################NODE JS###################################
APPLIMCZ_DAEMON="/home/pi/Dev/APPLIMCZ/src/Appli_Emit" #ligne de commande du programme
APPLIMCZ_DAEMON_NAME="Appli_Emit" #Nom du programme (doit être identique à l'exécutable)


PATH="/sbin:/bin:/usr/sbin:/usr/bin:/home/pi/Dev/APPLIMCZ/src" #Ne pas toucher


. /lib/lsb/init-functions

d_start () {
        log_daemon_msg "Starting system $APPLIMCZ_DAEMON_NAME Daemon"
        start-stop-daemon --name $APPLIMCZ_DAEMON_NAME --background --start --user "root" --exec $APPLIMCZ_DAEMON
        log_end_msg $?
}

d_stop () {
        log_daemon_msg "Stopping system $APPLIMCZ_DAEMON_NAME Daemon"
        start-stop-daemon --name $APPLIMCZ_DAEMON_NAME --stop --retry 5 --quiet
        log_end_msg $?
}

case "$1" in

        start|stop)
                d_${1}
                ;;

        restart|reload|force-reload)
                        d_stop
                        d_start
                ;;

        force-stop)
               d_stop
                killall -q $APPLIMCZ_DAEMON_NAME || true
                sleep 2
                killall -q -9 $APPLIMCZ_DAEMON_NAME || true
                ;;

        status)
                status_of_proc "$APPLIMCZ_DAEMON_NAME" "$APPLIMCZ_DAEMON" "system-wide $APPLIMCZ_DAEMON_NAME" && exit 0 || exit $?
                ;;
        *)
                echo "Usage: service APPLIMCZ {start|stop|force-stop|restart|reload|force-reload|status}"
                exit 1
                ;;

esac
exit 0

