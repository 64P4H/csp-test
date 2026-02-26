#!/bin/bash

N=1000
c=0

usage() {
  cat <<EOF
  Параметры:
    -n Указание количества тестов (10000 по умолчанию)
EOF
}

while getopts ":сn" opt;do
  case "$opt" in
    n) 
      N=${OPTARG}
      regex="^[0-9]+$"
      if ! [[ $N =~ $regex ]]; then
        N=10000
      elif [ $N -eq 0 ]; then
        N=10000
      fi
      ;;
    \?) usage; exit 2 ;;
  esac
done

gcc main.c dlinked.c thread_magic.c -pthread -o main.out
for i in $(seq 1 $N); do
  ./main.out > /dev/null 2> /dev/null
  if [ $? -ne 0 ]; then
    ((++c))
  fi
done
rm main.out
echo "$c из $N тестов провалено"
