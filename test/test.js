const MAX_STATEMENTS = 10000;

var util    = require('util'), 
    raptor  = require('./../build/default/raptor');

var parser = raptor.newParser('rdfxml', function (p) {
    // start parsing when all other callbacks are in place
    process.nextTick(function () {
        if (process.argv.length > 2) {
            p.parse(process.argv[2]);
        }
    });
});

var subjectsMap = {};
var subjectCount = 0;
var tripleCount = 0;
parser.on('statement', function (statement) {
    if (statement.subject.value.match(/NormanHeino/)) {
        util.puts(String(statement));
    }
    
    if (++tripleCount >= MAX_STATEMENTS) {
        parser.abort();
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
    
    if (subjectsMap && subjectCount) {
        util.puts(subjectCount + ' distinct predicates found.');
    }
    
    // util.puts('Namespaces: ' + util.inspect(namespaces));
});

/*
parser.on('description', function (description) {
    util.puts('New description: ' + util.inspect(description));
});
*/
