/*
 * Copyright 2010–2013 Norman Heino <norman.heino@gmail.com>
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

/*
 * This example demonstrates usage of the new streams2-based API.
 */

var fs     = require('fs'),
    path   = require('path'),
    raptor = require('..');

var fileName   = path.join(__dirname, '..', 'test', 'phil.rdf'),
    fileStream = fs.createReadStream(fileName, { flags: 'r'} ),
    serializer = raptor.createSerializer('turtle');

var serialization = '';

serializer
.setBaseURI('http://example.com/')
.addListener('data', function (data) {
    // console.log(data);
    serialization += data;
})
.addListener('end', function () {
    console.log(serialization);
});

serializer
.serializeStatement({
    subject:   { type: 'uri', value: 'http://example.com/r1' },
    predicate: { type: 'uri', value: 'http://example.com/p1' },
    object:    { type: 'uri', value: 'http://example.com/o1' }
})
.serializeStatement({
    subject:   { type: 'uri', value: 'http://example.com/r2' },
    predicate: { type: 'uri', value: 'http://example.com/p2' },
    object:    { type: 'typed-literal', value: 'ttt', dtype: 'http://example.com/string' }
})
.serializeStatement({
    subject:   { type: 'uri', value: 'http://example.com/r2' },
    predicate: { type: 'uri', value: 'http://example.com/p2' },
    object:    { type: 'literal', value: 'Cördobó', lang: 'de' }
})
.serializeEnd();
