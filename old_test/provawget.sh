while true; do
mkdir prova
cd prova
wget --mirror 127.0.0.1:$1 
sleep 1
cd ..
rm -rf prova
done
