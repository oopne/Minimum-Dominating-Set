for n in $(seq 10 2 100); do
    echo "test G($n, 1)"
    ./../../tests/random_graphs $n 1 1 > test || { echo "gen failed"; exit; }
    time ./../../smart_algo/smart 1 < test > out || { echo "smart failed"; exit; }
done

