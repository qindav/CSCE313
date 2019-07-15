PA2 Shell

To run this, do:

g++ -std=c++11 *.cpp
./a.out <flag>

Only one flag can be active, modifies "Shell>":
-t replaces the "Shell>" with nothing
-p prints username, date, and file path alongside "Shell>"
no flags leaves "Shell>" unmodified


Commands that DON'T work:

echo -e "<<<<< This message contains a line feed >>>>>\n"

<<<<< 4 pipes with I/O redirection >>>>>
ls -l /proc/sys > test.txt
awk '{print $8$9}' <test.txt | head -10 | head -8 | head -7 | sort > output.txt
cat output.txt

<<<<< Optional... >>>>>
cat /proc/$(ps|grep bash|head -1|awk '{print $1}')/status
