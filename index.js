var stream   = require('stream'),
    // bindings = require(__dirname + '/build/Debug/bindings');
    bindings = require(__dirname + '/build/Release/bindings');

exports.createParser = function (syntax) {
    syntax = syntax || 'guess';
    var parser = new bindings.Parser(syntax);
    return new StreamParser(parser);
};

function StreamParser(parser) {
    stream.Transform.call(this, { objectMode: true });
    this._parser  = parser;
    this._started = false;

    var self = this;
    this._parser.setStatementHandler(function (statement) {
        self.emit('data', statement);
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

exports.createSerializer = function (syntax) {
    var serializer = new bindings.Serializer(syntax);
    return new StreamSerializer(serializer);
};

function StreamSerializer(serializer) {
    stream.Transform.call(this, { objectMode: true });

    this._serializer = serializer;
    this._started = false;

    var self = this;
    this._serializer.setDataHandler(function (data) {
        self.push(data);
    });
    this._serializer.setEndHandler(function () {
        self.push();
    });
}

StreamSerializer.prototype = Object.create(stream.Transform.prototype);

StreamSerializer.prototype.setBaseURI = function (baseURI) {
    this._baseURI = baseURI;
    return this;
};

StreamSerializer.prototype.serializeStatement = function (statement) {
    if (!this._started) {
        if (!this._baseURI) {
            throw RangeError('base URI not set');
        }
        this._serializer.serializeStart(this._baseURI);
        this._started = true;
    }
    this._serializer.serializeStatement(statement);
    return this;
};

StreamSerializer.prototype.serializeEnd = function (statement) {
    this._serializer.serializeEnd();
    return this;
};

StreamSerializer.prototype._transform = function (chunk, encoding, cb) {
    console.log('transform');
    StreamSerializer.prototype.serializeStatement.call(this, chunk);
    cb();
};

StreamSerializer.prototype._flush = function (cb) {
    console.log('flush');
    StreamSerializer.prototype.serializeEnd.call(this);
    cb();
};
