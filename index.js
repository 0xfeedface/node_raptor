var stream   = require('stream'),
    bindings;

bindings = require(__dirname + '/build/Release/bindings');
// bindings = require(__dirname + '/build/Debug/bindings');

exports.createParser = function (syntax) {
    syntax = syntax || 'guess';
    var parser = new bindings.Parser(syntax);
    return new StreamParser(parser);
};

function StreamParser(parser) {
    stream.Transform.call(this);
    this._parser  = parser;
    this._started = false;

    var self = this;
    this._parser.setStatementHandler(function (statement) {
        self.emit('statement', statement);
    });
    this._parser.setNamespaceHandler(function (namespaceURI, prefix) {
        self.emit('namespace', namespaceURI, prefix);
    });
    this._parser.setMessageHandler(function (type, message) {
        self.emit('message', {
            type: type,
            text: message
        });
    });
}

StreamParser.prototype = Object.create(stream.Transform.prototype);

StreamParser.prototype.setBaseURI = function (baseURI) {
    this._baseURI = baseURI;
    return this;
};

StreamParser.prototype._transform = function (chunk, encoding, cb) {
    try {
        if (!this._started) {
            if (!this._baseURI) {
                throw RangeError('base URI not set');
            }
            this._parser.parseStart(this._baseURI);
            this._started = true;
        }
        this._parser.parseBuffer(chunk);
    } catch (e) {
        this.emit('error', e);
    }
    cb();
};

StreamParser.prototype._flush = function (cb) {
    try {
        this._parser.parseEnd();
    } catch (e) {
        this.emit('error', e);
    }
    this.emit('end');
    cb();
};
