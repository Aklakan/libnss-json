#Name Switch Service (NSS) JSON Binding Module

## You have been warned - don't blame me if you mess up your system!
As of now, this is a prototype implementation in 'works for me' state on my Ubuntu 16.04 machine.
Feedback is very welcome in order to improve and battle-harden this module, however as of now, be aware that bugs in the software could either lock you out from your system, or grant access rights to people who shouldn't have them.


## Its a JSON-in / JSON-out NSS service
In a nutshell, the Name Switch Service is a facility in Linux systems that handles different kinds of data lookup requests.
Most prominently, user, group and password lookups are handled by the NSS.
For details you may want to consult the (https://en.wikipedia.org/wiki/Name_Service_Switch)[Wikipedia arcticle].
There are NSS modules for LDAP, Active Directory, MYSQL, Postgres, and whatnot, which all implement a certain set of C functions.
The exercise of this project is to provide a simple extension point for connecting handers written in arbitrary languages to the NSS.
We do this by simply delegating all NSS requests as *JSON documents* to *function* which is expected to reply with appropriate *JSON* document repsonse.

In short: Its JSON in / Json out!

The module ships with an implementation of that function which delegates to an *executable* - `/etc/nss-json`: The json request is passed an the only argument, and
the output to STDOUT is parsed as JSON again. The process exit code is mapped to the NSS error codes.

So with this module, you can now just plug in a bash, NodeJS, JavaScript, Java or whatever script to handle the requests. For example, you could just
curl a JSON file in a private GitHub repo.


Whether this is a feasibile approach from security and performance perspectives has yet to be determined.

## Building

* `make`: This compiles the files in `src` and creates a `libnss_json.so.2` in the `target` folder.

If make yields an error, please report an issue.

## Installing

*First of all, make sure you have an open terminal where you are root!* You can easily disable the nss-json service as root, but you may not be able to make yourself root if something goes wrong.

* `make install`: This copies the `libnss_json.so.2` to /lib and runs `ldconfig`. At this point, no harm should be caused.

### Enabling the service

* Create an example `nss-json` executable which logs all requests to `/tmp/nss-json-test.txt` by running the following commands:

```bash
echo 'echo "$0 was called $1" >> /tmp/nss-json-test.txt' | sudo tee -a /etc/nss-json
sudo chmod +x /etc/nss-json
```

* Open the file `/etc/nsswitch.conf` with the editor of your choice and add `json` to e.g. passwd. If you open a fresh terminal and type `sudo su`, you should already see some debug output in action.


```
passwd:         json compat
group:          compat
shadow:         compat
```

* When you run
```bash
cat /tmp/nss-json-test.txt
```

You should see output such as:

```
/etc/nss-json got called with {
	"request":	"getwpnam",
	"name":	"myusername"
}
/etc/nss-json got called with {
	"request":	"getwpnam",
	"name":	"root"
}
```


## Files

This project a copy of DaveGamble's (cJSON as of this commit)[https://github.com/DaveGamble/cJSON/tree/05f75e360bf047de359cfce0581a93ea857a0a72].

* `nss-json.h`: Header file only defining the `int _nss_json_handle_request(const *cJSON *const requestJson, cJSON** result)` function for which an implementation must be provided
* `nss-json.c': Core implementation that delegates all call to the handle json request function mentioned above.
* `nss-json-impl`: An implementation of the json handler that invokes an executable at `/etc/nss-json`.
* `nss-json-bindings.h': Function prototypes for converting json to and from passwd, group and shadow structs.
* `nss-json-bindings.c': Implementation of the json bindings
* `cJSON-utils.c`: A few helper functions that made my life easier.
* `cJSON-utils.h`: The corresponding headers.


