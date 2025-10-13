## fatal error: 'iostream' file not found::
clang++ or g++ by default looks for headers like <iostream> under:
                       /Library/Developer/CommandLineTools/usr/include/c++/v1
 but they are under:: /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1
export CPATH=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1
echo $CPATH