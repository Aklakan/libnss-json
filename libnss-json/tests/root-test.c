#include <unistd.h>
#include <stdlib.h>

// gcc root-test.c
// sudo chown root:root a.out
// sudo chmod u+s a.out
int main() {
    char *name[10];
    setuid(0);

if(0) {
    name[0] = "bash";
    name[1] = "-c";
    name[2] = "/etc/nss-json-impl";
    name[3] = NULL;
    execvp("/bin/bash", name);
} else {
    //name[0] = "{}";
    //name[1] = NULL;
    //execvp("/etc/nss-json-impl", name);
    system("/etc/nss-json-impl");
}

//    name[1] = NULL;
}
