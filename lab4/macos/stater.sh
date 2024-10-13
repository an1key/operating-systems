#!/bin/bash

CURRENT_DIR=$(pwd)

osascript -e "tell application \"Terminal\" to do script \"cd $CURRENT_DIR; ./lab4\""
sleep 3
osascript -e "tell application \"Terminal\" to do script \"cd $CURRENT_DIR; ./lab4\""
sleep 4
osascript -e "tell application \"Terminal\" to do script \"cd $CURRENT_DIR; ./lab4\""

