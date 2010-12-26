Java JNI API currently only supports Russian morphology extraction from input words.

This file is about Source/JNIMorphAPI and Source/JNIMorphAPITest folders. They contain
Java JNI C++ files. The accompanying .java files for this JNI interface are at 
https://sourceforge.net/projects/seman-java/ . 

JNIMorphAPI:     Core C/C++ JNI files.
JNIMorphAPITest: Files for testing of files in JNIMorphAPI.

The following instructions apply for Unix-like operating systems.

To build JNIMorphAPI.so, do the following:

- cd Source/JNIMorphAPI
- edit build.sh to set the correct values for your environment;
- run build.sh by typing:
    sh build.sh
	
To build JNIMorphAPITest application, do the following:

- build JNIMorphAPI first (see above);
- cd ../JNIMorphAPITest
- edit build.sh to set the correct values for your environment;
- run build.sh by typing:
    sh build.sh

To run JNIMorphAPITest application, do the following:
- build JNIMorphAPITest application (see above);
- cd ../../Bin
- run the test app by typing:
		LD_LIBRARY_PATH=. ./JNIMorphTest
- if everything is valid, it will output "TEST PASSED".