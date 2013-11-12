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
 * This example demonstrates usage of the Parser API.
 * The `data` event is emitted for each new statement.
 * You can also attach to the `namespace` event that will be
 * emitted when a new namespace declaration was parsed.
 * The `error` event is emitted for stream errors,
 * while the `message` event contains parser messages (including
 * syntax errors).
 */

var fs     = require('fs'),
    path   = require('path'),
    raptor = require('..');

var fileName   = path.join(__dirname, '..', 'test', 'phil.rdf'),
    fileStream = fs.createReadStream(fileName, { flags: 'r'} ),
    parser     = raptor.createParser();

var counter = 0;

parser
.setBaseURI(fileName)
.addListener('data', function (statement) {
    process.stdout.write('\r' + (++counter) + ' statements parsed ...');
})
.addListener('namespace', function (uri, prefix) {
    console.log('namespace ' + prefix + ' = ' + uri);
})
.addListener('error', function (error) {
    console.error(error.stack);
})
.addListener('message', function (message) {
    console.error(message);
})
.addListener('end', function () {
    console.log('\r' + counter + ' statements parsed (done).');
});

fileStream.pipe(parser);
