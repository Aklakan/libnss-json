# Name Switch Service (NSS) JSON Binding Module

## You have been warned - don't blame me if you mess up your system!
As of now, this is a prototype implementation in 'works for me' state on my Ubuntu 16.04 machine.
Feedback is very welcome in order to improve and battle-harden this module, however as of now, be aware that bugs in the software could either lock you out from your system, or grant access rights to people who shouldn't have them.


## Its a JSON-in / JSON-out NSS service
In a nutshell, the Name Switch Service is a facility in Linux systems that handles different kinds of data lookup requests.
Most prominently, user, group and password lookups are handled by the NSS.
For details you may want to consult the [Wikipedia article](https://en.wikipedia.org/wiki/Name_Service_Switch).
There are NSS modules for LDAP, Active Directory, MYSQL, Postgres, and whatnot, which all implement a certain set of C functions.
The exercise of this project is to provide a simple extension point for connecting handlers written in arbitrary languages to the NSS.
This project does that by simply delegating all NSS requests as *JSON documents* to a single *function* which is expected to reply with an appropriate *JSON* document repsonse.

In short: Its JSON in / JSON out!

The module ships with an implementation of that function which delegates to an *executable* - `/etc/nss-json`: The json request is passed as the only argument, and
the output to STDOUT is parsed as JSON again. The process exit code is mapped to the NSS error codes.

So with this module, you can now just plug in a bash, NodeJS, JavaScript, Java or whatever script to handle the requests. For example, you could just
curl a JSON file in a private GitHub repo.


Whether this is a feasibile approach from security and performance perspectives has yet to be determined.

## Building
Enter the `libnss-json` folder.

* Simply run `make`. This builds the project from the `src` folder and puts the artifacts into `target`. It runs the following two sub targets:
* `make test`: Creates the `test-nss-json` executable which runs example user/group requests to your `/etc/nss-json` via the nss api.
* `make lib`: Builds the `libnss_json.so.2` library.
* `make install`: This copies the `libnss_json.so.2` to `/lib` and runs `ldconfig`. At this point, no harm should be caused.

If make yields an error, please report an issue.


### Enabling the service

* Create simple `/etc/nss-json` file which handles the requests. The following example allows the user 'yoda' to log in.
You can also copy `example-nss-json-yoda` file included in this repo.

```python
#!/usr/bin/python

import sys
import json

arg = sys.argv[1]
x = json.loads(arg)

# Lets allow yoda to log in
if (x['request'] == 'getpwnam' and x['name'] == "yoda") or (x['request'] == 'getpwuid' and x['uid'] == 10000):
  print '{"name": "yoda", "passwd": "foobar", "uid":10000, "gid":10000, "gecos": "foobar", "dir": "/home/yoda", "shell": "/bin/bash"}'
  exit(1)
elif(x['request'] == 'getgrnam' and x['name'] == "yoda") or (x['request'] == 'getgrgid' and x['gid'] == 10000):
  print '{"name": "yoda", "passwd": "foobar", "gid": 10000, "members": ["yoda"] }'
  exit(1)
else:
  exit(0)
```

* Make the file executable

```bash
sudo chmod 755 /etc/nss-json
```

* Note, that this file **must** be executable. If this file contains sensitive information (e.g. a password) you probably want to make it `511` (executable but neither read- nor writable), but this will not work for script languages where the interpreter reads the file under the user's rights.

* Run the `test-nss-json` tool created by `make` to make sure everything is in place so far.

* **Now make sure you have an open terminal where you are root!**

* Open the file `/etc/nsswitch.conf` with the editor of your choice and add `json` to e.g. passwd.
Obviously, you can easily disable the nss-json service by removing `json` again, but if something goes wrong, you may have locked yourself out.
In the worst case, you need to start your system for e.g. a USB stick to revert your changes to `/etc/nsswitch.conf`.

```
passwd:         json compat
group:          json compat
shadow:         compat
```

* Try `sudo su yoda` - You should now be logged in as him. May the force be with you.


### JSON Models
The schema of the JSON documents has yet to be documented - please look at the source files for now; essentially the schemas are similar to the system structs, but
are not stable yet.

## Implementation Status

* `_nss_json_getpwnam_r`: Done
* `_nss_json_getpwuid_r`: Done
* `_nss_json_getgrnam_r`: Done
* `_nss_json_getgrgid_r`: Done
* `_nss_json_getpwent_r`: Soon
* `_nss_json_getgrent_r`: Soon
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


### Note: What you should NOT do
Don't create a script that logs to a specific file - whenever you change a user with e.g. `sudo su foo`, the file will be written to as a different user,
most likely provoking an access violation and causing troubles.

* Create an example `nss-json` executable which logs all requests to `/tmp/nss-json-test.txt` by running the following commands:

```bash
echo 'echo "$0 was called $1" >> /tmp/nss-json-test.txt' | sudo tee -a /etc/nss-json
sudo chmod +x /etc/nss-json
```

* When you run
```bash
cat /tmp/nss-json-test.txt
```

You should see output such as:

```
/etc/nss-json got called with {
        "request":      "getwpnam",
        "name": "myusername"
}
/etc/nss-json got called with {
        "request":      "getwpnam",
        "name": "root"
}
```


