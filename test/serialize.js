/*
 * Copyright 2010 Norman Heino <norman.heino@gmail.com>
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

var util    = require('util'), 
    raptor  = require('..'), 
    fs      = require('fs');

var s = {
    subject:    {value: 'http://example.com/resource1', type: 'uri'}, 
    predicate:  {value: 'http://xmlns.com/foaf/0.1/name', type: 'uri'}, 
    object:     {value: 'Resource One', language: 'en', type: 'literal'}
}

var s2 = {
    subject:   {value: '_:123', type: 'bnode'}, 
    predicate: {value: 'http://ns.aksw.org/scms#beginIndex', type: 'uri'}, 
    object:    {value: '31', datatype: 'http://www.w3.org/2001/XMLSchema#int', type:'typed-literal'}
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
    //serializer.serializeStatement(s);
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

