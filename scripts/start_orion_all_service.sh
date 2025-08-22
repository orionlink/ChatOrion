#!/bin/bash

cd ../ChatOrionServer/bin/

./StatusServer &

echo "StatusServer start"

sleep 1

./GateServer &

sleep 1

echo "GateServer start"


./ChatServer 1 &

echo "ChatServer start"

sleep 1

cd ../VarifyServer

npm run server
echo "VarifyServer start"


