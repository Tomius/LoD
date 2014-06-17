#!/bin/bash
while ! mkdir .lockdir 2> /dev/null; do sleep 0.001s; done;
CURRENT=`cat .obj/objs_current`
TOTAL=`cat .obj/objs_total`
echo $(($CURRENT + 1)) > .obj/objs_current
VAR=$(( ($CURRENT + 1) * 100 / ($TOTAL + 1) ))
printf [%3d%%] $VAR
rm -rf .lockdir
