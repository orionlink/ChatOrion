#!/bin/bash

pkill -x StatusServer

echo "StatusServer stop"

sleep 1

pkill -x GateServer

sleep 1

echo "GateServer stop"


pkill -x ChatServer

echo "ChatServer stop"

pkill npm
pkill node

echo "VarifyServer stop"
