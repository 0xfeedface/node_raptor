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

var s = fs.createReadStream(__dirname + '/ser_test.rdf');
s.on('data', function (d) {
    util.print(d);
})

// ----------------------------------------------------------------------------

// the file we will be writing to
var file = fs.createWriteStream('./ser2_test.ttl');
var serializer = raptor.newSerializer('turtle');

serializer.start('http://example.com/graph/');
var numStmt = 1000;
while (numStmt--) {
    serializer.serializeStatement(s);
}
serializer.end();

serializer.on('data', function (data) {
    file.write(data);
});

serializer.on('end', function () {
    file.end();
});

serializer.on('error', function (type, message, code) {
    util.puts(message);
});

