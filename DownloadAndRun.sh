IP_ADDRESS='172.18.1.121'
echo "Downloading from $IP_ADDRESS..."
scp -r josh@$IP_ADDRESS:'Robotics/Raspberry\ Pi/Camera\ Module/*' ../ > /dev/null
echo "Compiling..."
sh compile.sh
echo "Running..."
./CameraTest

