#!/usr/bin/python

from rdflib import Graph, plugin, URIRef
from rdflib.serializer import Serializer
import json

with open('yoda.ttl', 'r') as fp:
    rdfStr=fp.read().replace('\n', '')

g = Graph().parse(data=rdfStr, format='turtle')

contextNss = { "@vocab": "http://example.org/nss/", "member": { "@container": "@set" } }

jsonDocStr = g.serialize(format='json-ld', context=contextNss, indent=4)
jsonDoc=json.loads(jsonDocStr)

graphs = jsonDoc['@graph']
for graph in graphs:
    graph['@context'] = contextNss

resultStr = json.dumps(graphs)
print(resultStr)


