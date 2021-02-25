#!/usr/bin/env bash

cd $(dirname "$0")
echo "Generating service file"
cat stagecast-scenes.service.template | sed "s:{{PATH}}:$(pwd):g" > stagecast-scenes.service
echo "Copying service file to ~/.config/systemd/user"
cp stagecast-scenes.service ~/.config/systemd/user/stagecast-scenes.service

echo "Reloading systemd units"
systemctl --user daemon-reload
echo "Starting service"
systemctl start --user stagecast-scenes.service
echo "run <systemctl enable --user stagecast-scenes.service> to start on boot"
