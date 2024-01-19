for n in $(seq 2 6); do
    echo "test $n"
    ./../../tests/small_graphs $n > test || { echo "gen failed"; exit; }
    ./../../stupid_algo/stupid < test > ok || { echo "stupid failed"; exit; }
    ./../../smart_algo/smart 1 < test > out || { echo "smart failed"; exit; }
    diff ok out || exit
done

