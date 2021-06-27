echo "building C network"
cd C_stuff && git clone https://github.com/adeharo9/cpp-dotenv.git && cmake . && make && cd ..

echo "building node server"

cd node_stuff && yarn && cd ..

echo "building pygame UI"

cd Pygame/Game_Network_Programming && pip3 install -r requirements.txt && mkdir image/enemy/jump && cd ../..


echo "done"
