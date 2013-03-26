var events = require('events'),
    util = require('util'),
    bindings = require(__dirname + '/build/Release/bindings');

function inherits(target, source) {
    for (var key in source.prototype) {
        target[key] = source.prototype[key];
    }
}

exports.newParser = function (mimeType, cb) {
    if (undefined === cb) {
        var parser = bindings.newParser(mimeType);
        inherits(parser, events.EventEmitter);
        return parser;
    } else {
        var res = bindings.newParser(mimeType, function (parser) {
            inherits(parser, events.EventEmitter);
            cb(parser);
        });
    }
};


exports.newSerializer = function (mimeType) {
    var serializer = null;

    if (undefined === mimeType) {
        serializer = bindings.newSerializer();
    } else {
        serializer = bindings.newSerializer(mimeType);
    }

    inherits(serializer, events.EventEmitter);

    return serializer;
};
