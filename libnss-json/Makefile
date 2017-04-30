all: test lib

lib:
	mkdir -p target
	gcc -fPIC -shared -o target/libnss_json.so.2 -Wl,-soname,libnss_json.so.2 src/nss-json.c src/nss-json-impl.c src/nss-json-bindings.c src/cJSON.c src/cJSON-utils.c -lm
test:
	mkdir -p target
	gcc -o target/main-test-nss-json src/nss-json.c src/nss-json-impl.c src/nss-json-bindings.c src/cJSON.c src/cJSON-utils.c src/main-test-nss-json.c -lm

install:
	sudo install -m 0644 target/libnss_json.so.2 /lib
	sudo /sbin/ldconfig -n /lib /usr/lib
#clean:
#	/bin/rf -rf libnss-json.so.2
