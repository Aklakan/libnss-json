package org.aksw.jnss.model;

import java.util.List;

import org.aksw.jena_sparql_api.mapper.annotation.DefaultIri;
import org.aksw.jena_sparql_api.mapper.annotation.Iri;
import org.aksw.jena_sparql_api.mapper.annotation.IriNs;

//@Namespace("nss", "http://example.org/nss/")
@DefaultIri("http://example.org/")
public class Group {
    @IriNs("nss")
    public String name;

    @IriNs("nss")
    public String passwd;

    @IriNs("nss")
    public int gid;

    @Iri("nss:member")
    public List<String> members;
}
