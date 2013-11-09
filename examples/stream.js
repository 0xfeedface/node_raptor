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
 * This example demonstrates usage of the new Streams2-based API.
 * You can simply pipe a ReadStream into the parser and attach to 
 * the `statement` event.
 * Keep in mind that this is a minimal example with no error handling.
 * For a more complete example see `examples/parser.js`.
 */

var fs     = require('fs'),
    path   = require('path'),
    raptor = require('..');

var fileName = path.join(__dirname, '..', 'test', 'phil.rdf'),
    parser   = raptor.createParser();

parser
.setBaseURI(fileName)
.addListener('statement', function (statement) {
    console.log(statement);
});

fs.createReadStream(fileName).pipe(parser);
