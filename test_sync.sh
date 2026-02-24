#!/bin/bash

V=
M=
N=1000
c=0

usage() {
  cat <<EOF
  Параметры:
    -v Verbose
    -h Режим на истории
    -c Режим на критической зоне
    -n Указание количества тестов (10000 по умолчанию)
  По умолчанию запускается режим на флагах. Комбинация hc дает первый упомянутый режим.
EOF
}

is_decimal() {
  local regex="^[0-9]+$"
  if [ "$1" =~ $regex ]; then
    return 0
  else
    return 1
  fi
}

while getopts ":vhcn:" opt;do
  case "$opt" in
    v) V='-DVERB' ;;
    h) M='-DHIST' ;;
    c) M='-DCRIT' ;;
    n) 
      N=${OPTARG}
      regex="^[0-9]+$"
      if ! [[ $N =~ $regex ]]; then
        N=1000
      elif [ $N -eq 0 ]; then
        N=1000
      fi
      ;;
    \?) usage; exit 2 ;;
  esac
done

rm t*.txt
g++ main.cpp DLinked.cpp -fopenmp -o main.out $V $M
for i in $(seq 1 $N); do
  ./main.out > t$i.txt 2> t$i.txt
  if [ $? -eq 0 ]; then
    rm t$i.txt
  else
    ((++c))
  fi
done
rm main.out
echo "$c из $N тестов провалено"
