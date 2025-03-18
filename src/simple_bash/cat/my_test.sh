SUCCESS=0
FAIL=0
DIFF_RES=""
OPTIONS="b e n s t v E T"

declare -a tests=(
    "VAR tests/test_1_cat.txt"
    "VAR tests/test_2_cat.txt"
    "VAR tests/test_3_cat.txt"
    "VAR tests/test_4_cat.txt"
    "VAR tests/test_5_cat.txt"
    "VAR tests/test_case_cat.txt"
)

testing() {
    t=$(echo "$@" | sed "s/VAR/$var/")
    ./s21_cat $t > test_s21_cat.log
    cat $t > test_sys_cat.log
    DIFF_RES=$(diff test_s21_cat.log test_sys_cat.log -q)
    if [ -z "$DIFF_RES" ]; then
      (( SUCCESS++ ))
      echo "SUCCESS cat $t"
    else
      (( FAIL++ ))
      echo "FAIL cat $t"
    fi
    rm test_s21_cat.log test_sys_cat.log
}

for var1 in $OPTIONS; do
    for i in "${tests[@]}"; do
        var="-$var1"
        testing "$i"
    done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
exit $(( $FAIL > 0 ))