package org.aksw.jnss.api;

import org.aksw.jnss.model.Passwd;

public interface PassWdApi {
    Passwd getpwnam(String name);
    Passwd getpwuid(int uid);
}
