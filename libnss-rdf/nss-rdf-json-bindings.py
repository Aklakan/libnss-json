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

# Ok, using resources for members just makes things ugly - lets skip that
# Post process group member arrayws

#for obj in graphs:
#  if 'member' in obj:
#    arr = obj['member']
#    for i, member in enumerate(arr):
#      s = URIRef(member['@id'])
#      for o in g.objects(s, nssName):
#          arr[i] = "" + o
  

resultStr = json.dumps(graphs)
print(resultStr)


