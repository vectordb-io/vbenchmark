
grep "myproject" -rn *
grep "VBENCHMARK" -rn *

find . -type f -exec sed -i 's/myproject/vbenchmark/g' {} +
find . -type f -exec sed -i 's/VBENCHMARK/VBENCHMARK/g' {} +


