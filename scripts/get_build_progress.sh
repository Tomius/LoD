#!/bin/bash
while ! mkdir .lockdir 2> /dev/null; do sleep 0.01s; done;
CURRENT=`cat .obj/objs_current`
TOTAL=`cat .obj/objs_total`
echo $(($CURRENT + 1)) > .obj/objs_current
ROUNDING_BIAS=$(( ($TOTAL + 1) / 2 ))
VAR=$(( (100*($CURRENT + 1) + $ROUNDING_BIAS) / ($TOTAL + 1) ))
printf [%3d%%] $VAR
rm -rf .lockdir
