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

const MAX_STATEMENTS = Number.MAX_VALUE;

var util    = require('util'), 
    fs      = require('fs'), 
    raptor  = require('..');

var time;
/*
 * var filename = process.argv[2];
 * 
 * var fileStream = fs.createReadStream(filename);
 */
var parser = raptor.newParser('turtle');

var time = Date.now();

process.nextTick(function () {
    parser.parseStart('http://example.com/');
    parser.parseBuffer(new Buffer('<http://example.com/me> a <http://example.com/Person> ,<http://example.com/PhDStudent> .'));
    parser.parseBuffer();
});

var subjectsMap = {};
var subjectCount = 0;
var tripleCount = 0;
parser.on('statement', function (statement) {
    // if (statement.subject.type == 'bnode') {
        util.puts(statement);
    // }
    
    if (++tripleCount >= MAX_STATEMENTS) {
        parser.abort();
    }
});

parser.on('message', function (type, message, code) {
    if (type == 'error' || type == 'warning') {
        util.log(type.toUpperCase() + ': ' + message + ' (' + code + ')');
    }
});

var namespaces = {};
parser.on('namespace', function (prefix, uri) {
    if (!(prefix in namespaces)) {
        namespaces[prefix] = uri;
    }
});

parser.on('end', function () {
    util.puts(tripleCount + ' statements parsed.');
    util.puts('Parsing took ' + (Date.now() - time) + ' ms.');
    
    // if (subjectsMap && subjectCount) {
    //     util.puts(subjectCount + ' distinct predicates found.');
    // }
    
    // util.puts('Namespaces: ' + util.inspect(namespaces));
});

/*
parser.on('description', function (description) {
    util.puts('New description: ' + util.inspect(description));
});
*/
