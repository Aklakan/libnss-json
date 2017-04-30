
package org.aksw.jnss.model;

import org.aksw.jena_sparql_api.mapper.annotation.DefaultIri;
import org.aksw.jena_sparql_api.mapper.annotation.IriNs;

//@Namespace("nss", "http://example.org/nss/")
@DefaultIri("http://example.org/")
public class Passwd {

    @IriNs("nss")
    public String name;

    @IriNs("nss")
    public String passwd;

    @IriNs("nss")
    public int uid;

    @IriNs("nss")
    public String gecos;

    @IriNs("nss")
    public String dir;

    @IriNs("nss")
    public String shell;

    @Override
    public String toString() {
        return "Passwd [name=" + name + ", passwd=" + passwd + ", uid=" + uid + ", gecos=" + gecos + ", dir=" + dir
                + ", shell=" + shell + "]";
    }

}
