n=$1
p=$2
test_count=$3
heuristic=$4
res_file=$5

echo "$test_count instances of G($n, $p), heuristic=$heuristic"
./../../tests/random_graphs $n $p $test_count > test || { echo "gen failed"; exit; }
./../../smart_algo/smart $heuristic < test > out 2> $res_file || { echo "smart failed"; exit; }

echo "times saved into $res_file"
