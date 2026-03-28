rm statsort_example
#rm statsort_test

g++ -O2 statsort_example.cpp -I./include -o statsort_example
#g++ -O2 statsort_test.cpp -I./include -o statsort_test

echo "------ example -----"
./statsort_example

#echo "----- test -----"
#./statsort_test
