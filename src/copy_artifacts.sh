#!/bin/bash

  scp src/simple_bash/cat/s21_cat liquorib@10.10.0.3:~/
  scp src/simple_bash/grep/s21_grep liquorib@10.10.0.3:~/
  ssh liquorib@10.10.0.3 "echo "1337" | sudo -S mv s21_cat s21_grep /usr/local/bin"
