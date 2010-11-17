var util    = require('util'), 
    raptor  = require('./../build/default/raptor'), 
    fs      = require('fs');

var s = {
    subject:    {value: 'http://example.com/resource1', type: 'uri'}, 
    predicate:  {value: 'http://xmlns.com/foaf/0.1/name', type: 'uri'}, 
    object:     {value: 'Resource One', type: 'literal'}
}

var serializer = raptor.newSerializer('rdfxml');

// set syntax options
// serializer.setOption('foo', 'bar');

serializer.start(__dirname + '/ser_test.rdf');

serializer.setNamespace('foaf', 'http://xmlns.com/foaf/0.1/');

// serialize statements
serializer.serializeStatement(s);

// close serializer
serializer.end();
