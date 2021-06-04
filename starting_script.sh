trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

for arg in "$@"
do
    case $arg in
        -i|--initialize)
        SHOULD_SERVER=1
        shift # Remove --initialize from processing
        ;;
        *)
        OTHER_ARGUMENTS+=("$1")
        shift # Remove generic argument from processing
        ;;
    esac
done

if [[ $SHOULD_SERVER -eq 1 ]]; then
	echo "starting server"
	cd node_stuff && (yarn start &) && cd ..
fi

echo "starting network client "
cd C_stuff && ( ./network_client &) && cd ..

echo "starting game client"
cd Pygame/Game_Network_Programming && (python3 test_game_integrated.py) && cd ../..
