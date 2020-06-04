# cd ./SmartPillow/server
# chmod +x run.sh
# ./run.sh
cd ./SmartPillow/server
pip install -r requirements.txt
nohup python server.py > server.out 2>&1 &
nohup python backend.py > backend.out 2>&1 &
# nohup gunicorn -b 172.17.0.13:12345 backend:app > backend.out 2>&1 &

# check status
# ps -x|grep python
# lsof -i:12345 -i:54321