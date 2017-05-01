# Name Switch Service (NSS) JSON Binding Module

LDAP, Active Directory, MYSQL, Postgres, ... Why not just JSON / JSON-LD? Its just data after all.

## What is this project about it and why?
In a nutshell, the Name Switch Service is a facility in Linux systems that handles different kinds of data lookup requests.
Most prominently, user, group and password lookups are handled by the NSS.
For details you may want to consult the [Wikipedia article](https://en.wikipedia.org/wiki/Name_Service_Switch).
There are NSS modules for LDAP, Active Directory, MYSQL, Postgres, and whatnot, which all implement a certain set of C functions.
The exercise of this project is to provide a simple extension point for connecting handlers written in arbitrary languages to the NSS.
This project does that by simply delegating all NSS requests as *JSON documents* to a single *C function* whose implementation is is expected to reply with an appropriate *JSON* document repsonse. The default backend sends to request to a specified URL.

In fact, you can just return an arbitary JSON array, and nss-json will pick the appropriate ones.

## You have been warned - don't blame me if you mess up your system!
As of now, this implementation has only been tested on Ubuntu 16.04.
Feedback is very welcome in order to improve and battle-harden this module, however as of now, be aware that bugs in the software could either lock you out from your system, or grant access rights to people who shouldn't have them.


## How to use
The module ships with an implementation which delegates all requests to a URL configured in `/etc/nss-json.conf` via curl.
The response is expected to be an appropriate JSON document.


## Building
Enter the `libnss-json` folder.

* Simply run `make`. This builds the project from the `src` folder and puts the artifacts into `target`. It runs the following two sub targets:
* `make test`: Creates the `test-nss-json` executable which runs example user/group requests to your `/etc/nss-json` via the nss api.
* `make lib`: Builds the `libnss_json.so.2` library.

* `make install`: This copies the `libnss_json.so.2` to `/lib` and runs `ldconfig`. At this point, no harm should be caused.

If make yields an error, please report an issue.


### Enabling the service

* Create a simple `/etc/nss-json.conf` configuration. The following example allows the user 'yoda' to log in based on this publicly hosted json file.
**Obviously, this is just an example, and you should change the config quickly**. The point is, you can now easily set up a web server which serves that url.


```bash
# The only mandatory parameter
url = https://raw.githubusercontent.com/Aklakan/libnss-json/master/yoda.json

# Enable debug output, such as that of the curl library.
# Default: false
verbose = true

# Use false for serving a static JSON document. Use true for url requests to include a POST argument of the form request=<the request as json>
# Default: false
includeRequestArg = false
```

* Run the `target/main-test-nss-json` tool created by `make` to make sure everything is in place so far. **TODO** The test script assumes above setup, and half of the tests actually succeed if a fail is reported.

* **Now make sure you have an open terminal where you are root!**

* Open the file `/etc/nsswitch.conf` with the editor of your choice and add `json` to e.g. passwd.
Obviously, you can easily disable the nss-json service by removing `json` again, but if something goes wrong, you may have locked yourself out.
In the worst case, you need to start your system for e.g. a USB drive to revert your changes to `/etc/nsswitch.conf`.

```
passwd:         json compat
group:          json compat
shadow:         compat
```

* Try `sudo su yoda` - You should now be logged in as him. May the force be with you.

## Caching
* Output of `/etc/nss-json` is within libnss-json by default cached for 10 seconds, controlled by `CACHE_INTERVAL_IN_SEC` in [nss-json-impl.c](libnss-json/src/nss-json-impl.c). However, the system usually relaunches `libnss-json` for its request in *much* shorter intervals.

* Installing the _Name Service Caching Daemon_ significantly reduces the number of invocations to `/etc/nss-json`.

```bash
sudo apt-get install nscd
```
* You will notice the differences immediately with the examples above.


### JSON Models
The schema of the JSON documents has yet to be documented - please look at the source files for now; essentially the schemas are similar to the system structs, but
are not stable yet.

The following rules are used to identify the corresponding nss record type by a JSON document (subject to change).

* passwd records must have the `uid` and `name` keys.
* group records are identified by having both `gid` and `members` keys. This means even if there are no members, the JSON must still define that key.


## Implementation Status

* `_nss_json_getpwnam_r`: Done
* `_nss_json_getpwuid_r`: Done
* `_nss_json_getpwent_r`: Done

* `_nss_json_getgrnam_r`: Done
* `_nss_json_getgrgid_r`: Done
* `_nss_json_getgrent_r`: Done

* `the stuff for shadow`: Later


## Files

This project includes a copy of DaveGamble's [cJSON as of this commit](https://github.com/DaveGamble/cJSON/tree/05f75e360bf047de359cfce0581a93ea857a0a72).

* `nss-json-impl.h`: Header file only defining the `int _nss_json_handle_request(const *cJSON *const requestJson, cJSON** responseJson)` function for which an implementation must be provided
* `nss-json.c`: Core implementation that delegates all call to the handle json request function mentioned above.
* `nss-json.h`: Function prototypes of the core implementation.
* `nss-json-impl.c`: An implementation of the json request handler that invokes an executable at `/etc/nss-json`.
* `nss-json-bindings.h`: Function prototypes for converting json to and from passwd, group and shadow structs.
* `nss-json-bindings.c`: Implementation of the json bindings
* `cJSON-utils.c`: A few helper functions that made my life easier.
* `cJSON-utils.h`: The corresponding headers.

## Acknowledgements

Source code is largely derived from this [nss-pgsql module](https://github.com/jandd/libnss-pgsql), whose code is according to an comment partly inspired by an nss-ldap module.


## License
To be stated; will be something very liberal that just asks for an acknowledgement.


## Performance
* Login performance of `nss-json` under 50 logins: disabled vs uncached vs 10 second cache.

```bash
time for x in {0..50}; do sudo su yoda -c 'echo "test"'; done
```

|      | disabled | uncached | cache10  |
| ---- | -------- | -------- | -------- |
| real | 0m0.331s | 0m3.624s | 0m0.836s |
| user | 0m0.052s | 0m0.440s | 0m0.164s |
| sys  | 0m0.040s | 0m0.480s | 0m0.260s |


