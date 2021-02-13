#!/usr/bin/env bash

cd $(dirname "$0")
echo "Generating service file"
cat stagecast-lighting.service.template | sed "s:{{PATH}}:$(pwd):g" > stagecast-lighting.service
echo "Copying service file to ~/.config/systemd/user"
cp stagecast-lighting.service ~/.config/systemd/user/stagecast-lighting.service

echo "Reloading systemd units"
systemctl --user daemon-reload
echo "Starting service"
systemctl start --user stagecast-lighting.service
echo "run <systemctl enable --user stagecast-lighting.service> to start on boot"
