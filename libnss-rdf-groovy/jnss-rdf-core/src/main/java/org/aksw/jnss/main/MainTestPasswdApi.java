package org.aksw.jnss.main;

import javax.persistence.EntityManager;

import org.aksw.jena_sparql_api.core.SparqlService;
import org.aksw.jena_sparql_api.mapper.impl.engine.RdfMapperEngineImpl;
import org.aksw.jena_sparql_api.mapper.impl.type.RdfTypeFactoryImpl;
import org.aksw.jena_sparql_api.mapper.jpa.core.EntityManagerImpl;
import org.aksw.jena_sparql_api.mapper.model.TypeConversionServiceImpl;
import org.aksw.jena_sparql_api.mapper.model.TypeConverterBase;
import org.aksw.jena_sparql_api.mapper.model.TypeDeciderImpl;
import org.aksw.jena_sparql_api.mapper.test.TestMapperDBpedia;
import org.aksw.jena_sparql_api.stmt.SparqlQueryParserImpl;
import org.aksw.jena_sparql_api.update.FluentSparqlService;
import org.aksw.jena_sparql_api.utils.transform.F_QueryTransformDatasetDescription;
import org.aksw.jena_sparql_api.utils.transform.F_QueryTransformLimit;
import org.aksw.jnss.api.PassWdApi;
import org.aksw.jnss.impl.jpa.PassWdApiImplJpa;
import org.aksw.jnss.model.Passwd;
import org.apache.jena.datatypes.xsd.XSDDatatype;
import org.apache.jena.graph.Node;
import org.apache.jena.graph.NodeFactory;
import org.apache.jena.rdf.model.Model;
import org.apache.jena.riot.RDFDataMgr;
import org.apache.jena.shared.PrefixMapping;
import org.apache.jena.sparql.core.Prologue;
import org.apache.jena.sparql.expr.E_DateTimeYear;
import org.apache.jena.sparql.expr.Expr;
import org.apache.jena.sparql.util.PrefixMapping2;
import org.apache.jena.vocabulary.XSD;

public class MainTestPasswdApi {
    public static void main(String[] args) {
        Prologue prologue = new Prologue(new PrefixMapping2(PrefixMapping.Extended));
        prologue.setPrefix("schema", "http://schema.org/");
        prologue.setPrefix("dbo", "http://dbpedia.org/ontology/");
        prologue.setPrefix("dbr", "http://dbpedia.org/resource/");

        Model model = RDFDataMgr.loadModel("/home/raven/Projects/Eclipse/libnss-json/libnss-rdf-groovy/yoda.ttl");

        SparqlService sparqlService = FluentSparqlService.from(model)
                .config()
                    .configQuery()
                        .withParser(SparqlQueryParserImpl.create())
                    .end()
//                    .withDatasetDescription(dd, graphName)
                    .configQuery()
                        .withQueryTransform(F_QueryTransformDatasetDescription.fn)
                        .withQueryTransform(F_QueryTransformLimit.create(1000))
                    .end()
                .end()
                .create();


        RdfMapperEngineImpl mapperEngine = new RdfMapperEngineImpl(sparqlService, prologue);
        ((TypeDeciderImpl)mapperEngine.getTypeDecider()).putAll(TypeDeciderImpl.scan("org.aksw.jnss", prologue));
        //NodeFactory.createURI("http://dbpedia.org/ontology/Company"), Company.class);


        RdfTypeFactoryImpl tf = (RdfTypeFactoryImpl)mapperEngine.getRdfTypeFactory();
        TypeConversionServiceImpl tcs = (TypeConversionServiceImpl)tf.getTypeConversionService();

        tcs.put(new TypeConverterBase(XSD.gYear.toString(), int.class) {
            @Override
            public Expr toJava(Expr expr) {
                return new E_DateTimeYear(expr);
            }

            @Override
            public Node toRdf(Object o) {
                Node node = NodeFactory.createLiteral("" + o, XSDDatatype.XSDgYear);
                return node;
            }
        });


//        tcs.put(new TypeConverterBase(XSD.xstring.toString(), String.class) {
//            @Override
//            public Expr toJava(Expr expr) {
//                return new E_Str(expr);
//            }
//
//            @Override
//            public Node toRdf(Object o) {
//                Node node = NodeFactory.createLiteral("" + o, XSDDatatype.XSDstring);
//                return node;
//            }
//        });


        EntityManager entityManager = new EntityManagerImpl(mapperEngine);

        PassWdApi api = new PassWdApiImplJpa(entityManager);

        Passwd x = api.getpwnam("yoda");
        System.out.println(x);

    }
}
