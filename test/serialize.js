var util    = require('util'), 
    raptor  = require('./../build/default/raptor'), 
    fs      = require('fs');

/*{
    subject:    {value: 'http://example.com/resource1', type: 'uri'}, 
    predcate:   {value: 'http://xmlns.com/foaf/0.1/name', type: 'uri'}, 
    object:     {value: 'Resource One', type: 'literal'}
}*/

var serializer = raptor.newSerializer('rdfxml');

// set syntax options
serializer.setOption('foo', 'bar');

serializer.start('ser_test.rdf');

// serializer.setNamespace(prefix, uri);

// serialize statements
serializer.serializeStatement(/* statement */);
serializer.serializeStatement(/* statement */);
serializer.serializeStatement(/* statement */);

// close serializer
serializer.end();
