package org.aksw.jnss.impl.jpa;

import javax.annotation.Resource;
import javax.persistence.EntityManager;
import javax.persistence.criteria.Root;

import org.aksw.jnss.api.PassWdApi;
import org.aksw.jnss.model.Passwd;

public class PassWdApiImplJpa
    implements PassWdApi
{
    @Resource
    protected EntityManager em;

    public PassWdApiImplJpa() {
        this(null);
    }

    public PassWdApiImplJpa(EntityManager em) {
        super();
        this.em = em;
    }

    public EntityManager getEntityManager() {
        return em;
    }

    public void setEntityManager(EntityManager em) {
        this.em = em;
    }

    @Override
    public Passwd getpwnam(final String name) {
        Passwd result = JpaUtils.getSingleResult(em, Passwd.class, (cb, cq) -> {
            Root<Passwd> r = cq.from(Passwd.class);
            cq.select(r).where(cb.equal(r.get("name"), name));
        });
        return result;
    }

    @Override
    public Passwd getpwuid(final int uid) {
        Passwd result = JpaUtils.getSingleResult(em, Passwd.class, (cb, cq) -> {
            Root<Passwd> r = cq.from(Passwd.class);
            cq.select(r).where(cb.equal(r.get("uid"), uid));
        });
        return result;
    }
}
