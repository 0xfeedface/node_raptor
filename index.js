var events = require('events');
var bindings = require(__dirname + '/build/Release/bindings');


function inherits(target, source) {
    for (var k in source.prototype)
        target[k] = source.prototype[k];
}

exports.newParser = function(mimeType, cb) {
    if (undefined == cb) {
        var parser = bindings.newParser(mimeType);
        inherits(parser, events.EventEmitter);
        return parser;
    } else {
        var res = bindings.newParser(mimeType, function(parser) {
            inherits(parser, events.EventEmitter);
            cb(parser);
        });
    }
}


exports.newSerializer = function(mimeType) {
    var serializer = null;

    if (undefined == mimeType) {
        serializer = bindings.newSerializer();
    } else {
        serializer = bindings.newSerializer(mimeType);
    }

    inherits(serializer, events.EventEmitter);

    return serializer;
}
