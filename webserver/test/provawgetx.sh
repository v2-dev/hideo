while true; do
mkdir prova$2
cd prova$2
wget --mirror 127.0.0.1:$1 
sleep 1
cd ..
rm -rf prova$2
done
