all:
	#mkdir target
	gcc -fPIC -shared -o target/libnss_json.so.2 -Wl,-soname,libnss_json.so.2 -Isrc nss-json.c nss-json-impl.c nss-json-bindings.c cJSON.c cJSON-utils.c -lm
install:
	sudo install -m 0644 target/libnss_json.so.2 /lib
	sudo /sbin/ldconfig -n /lib /usr/lib
#clean:
#	/bin/rf -rf libnss-json.so.2
