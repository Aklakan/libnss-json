
package org.aksw.jnss.model;

import org.aksw.jena_sparql_api.mapper.annotation.DefaultIri;
import org.aksw.jena_sparql_api.mapper.annotation.IriNs;
import org.aksw.jena_sparql_api.mapper.annotation.RdfType;

//@Namespace("nss", "http://example.org/nss/")
@DefaultIri("http://example.org/")
@RdfType("http://example.org/nss/Passwd")
public class Passwd {

    @IriNs("nss")
    protected String name;

    @IriNs("nss")
    protected String passwd;

    @IriNs("nss")
    protected int uid;

    @IriNs("nss")
    protected String gecos;

    @IriNs("nss")
    protected String dir;

    @IriNs("nss")
    protected String shell;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPasswd() {
        return passwd;
    }

    public void setPasswd(String passwd) {
        this.passwd = passwd;
    }

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public String getGecos() {
        return gecos;
    }

    public void setGecos(String gecos) {
        this.gecos = gecos;
    }

    public String getDir() {
        return dir;
    }

    public void setDir(String dir) {
        this.dir = dir;
    }

    public String getShell() {
        return shell;
    }

    public void setShell(String shell) {
        this.shell = shell;
    }

    @Override
    public String toString() {
        return "Passwd [name=" + name + ", passwd=" + passwd + ", uid=" + uid + ", gecos=" + gecos + ", dir=" + dir
                + ", shell=" + shell + "]";
    }

}
