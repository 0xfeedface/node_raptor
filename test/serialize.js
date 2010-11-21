var util    = require('util'), 
    raptor  = require('./../build/default/raptor'), 
    fs      = require('fs');

var s = {
    subject:    {value: 'http://example.com/resource1', type: 'uri'}, 
    predicate:  {value: 'http://xmlns.com/foaf/0.1/name', type: 'uri'}, 
    object:     {value: 'Resource One', type: 'literal'}
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
    serializer.serializeStatement(s);
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

