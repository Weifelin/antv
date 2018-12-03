# antv

...Note this is an exceprt taken from our document, which you can read elsewhere...

Our project is meant to be run on Ubuntu 16.04.5 LTS 32 bit(ISO in appendix 3). Our implementation of kernel hooking sometimes doesn’t work on other versions of linux so do be careful when running this on a separate virtual machine. Since our program comes pre-compiled, there is no other setup that should be necessary to run our code. We are using the curl.h library for our database update implementation (documentation in appendix 1).

If for whatever reason you need to compile the project again, there will be a couple steps to follow. This step can and should be skipped, unless something is wrong (i.e. skip to the “Scanning” section of the document). Since we are using the curl library, we need to make sure curl is installed on the machine. To install curl.h on our linux machine simply use the following command.

	sudo apt-get install libcurl4-openssl-dev

After installing curl.h we can then compile our program as “anti”, with the following line. This should be done from the ./antv directory

	make

Now we have successfully built the project and can start execution.

Also, if you need to recompile the modules for on access scan, go to on_access/ and type

	make
