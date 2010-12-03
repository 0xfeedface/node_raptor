var util    = require('util'), 
    raptor  = require('./../build/default/raptor'), 
    fs      = require('fs');

var s = {
    subject:    {value: 'http://example.com/resource1', type: 'uri'}, 
    predicate:  {value: 'http://xmlns.com/foaf/0.1/name', type: 'uri'}, 
    object:     {value: 'Resource One', type: 'literal'}
}

var s2 = {
    subject:   {value: '_:123', type: 'bnode'}, 
    predicate: {value: 'http://ns.aksw.org/scms#beginIndex', type: 'uri'}, 
    object:    {value: '31', datatype: 'http://www.w3.org/2001/XMLSchema#int', lang: 'de'}
}

/*
var serializer = raptor.newSerializer('rdfxml');

// set syntax options
// serializer.setOption('foo', 'bar');

serializer.start(__dirname + '/ser_test.rdf');

serializer.setNamespace('foaf', 'http://xmlns.com/foaf/0.1/');

// serialize statements
serializer.serializeStatement(s);

// close serializer
serializer.end();

var s = fs.createReadStream(__dirname + '/ser_test.rdf');
s.on('data', function (d) {
    util.print(d);
});
*/

// ----------------------------------------------------------------------------

// the file we will be writing to
var serializer = raptor.newSerializer('turtle');


process.nextTick(function () {
    serializer.serializeStart();
    // serializer.serializeStatement(s);
    serializer.serializeStatement(s2);
    serializer.serializeEnd();
});

var triples = '';
serializer.on('data', function (data) {
    triples += data;
});

serializer.on('end', function () {
    util.puts(triples);
});

serializer.on('error', function (type, message, code) {
    util.puts(message);
});

