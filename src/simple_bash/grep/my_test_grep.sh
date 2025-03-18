#!/bin/bash

SUCCESS=0
FAIL=0

# Функция для сравнения вывода s21_grep и grep
compare_output() {
    echo "Testing flag: $1"
    ./s21_grep $1 $2 $3 > s21_grep_output.txt
    grep $1 $2 $3 > grep_output.txt
    diff s21_grep_output.txt grep_output.txt
    if [ $? -eq 0 ]; then
        echo "Test passed for flag $1"
        ((SUCCESS++))
    else
        echo "Test failed for flag $1"
        ((FAIL++))
    fi
    rm s21_grep_output.txt grep_output.txt
}

# Тестирование флагов

# 1. -e pattern
compare_output "-e to" to_be.txt

# 2. -i Ignore uppercase vs. lowercase
compare_output "-i to" to_be.txt

# 3. -v Invert match
compare_output "-v to" to_be.txt

# 4. -c Output count of matching lines only
compare_output "-c to" to_be.txt

# 5. -l Output matching files only
compare_output "-l to" to_be.txt

# 6. -n Precede each matching line with a line number
compare_output "-n to" to_be.txt

# 7. -h Output matching lines without preceding them by file names
compare_output "-h to" to_be.txt to_be_1.txt

# 8. -s Suppress error messages about nonexistent or unreadable files
compare_output "-s to non_existent_file.txt"

# 9. -f file Take regexes from a file
echo "to" > pattern_file.txt
compare_output "-f pattern_file.txt" to_be.txt
rm pattern_file.txt

# 10. -o Output the matched parts of a matching line
compare_output "-o to" to_be.txt

echo "All tests completed."
echo "SUCCESS: $SUCCESS"
echo "FAIL: $FAIL"
exit $(( $FAIL > 0 ))