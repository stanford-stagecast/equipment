#!/usr/bin/env bash
cd backend
source env/bin/activate
python3 main.py ${PORT:-8001} ../frontend/public
