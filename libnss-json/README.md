SPARQL-based Name Switch Service (NSS) Module

This module simply forwards lookp requests to a command line script, which is expected to yield an RDF graph containing the information needed by the NSS system.
The module just maps the RDF graph to the system structures.


* `nss-json.h`: Header file only defining the `int _nss_json_handle_request(const *cJSON *const requestJson, cJSON** result)` function for which an implementation must be provided
* `nss-json.c': Core implementation that delegates all call to the handle json request method mentioned above.
* `nss-json-impl`: An implementation of the json handler
* `pwd-json-bindings.h': Function prototypes for converting json to and from passwd, group and shadow structs.
* `pwd-json-bindings.c': Implementation of the json bindings


This project a copy of DaveGamble's (cJSON as of this commit)[https://github.com/DaveGamble/cJSON/tree/05f75e360bf047de359cfce0581a93ea857a0a72].


