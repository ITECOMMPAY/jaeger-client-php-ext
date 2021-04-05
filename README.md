# PHP bindings for Jaeger tracer

This is a PHP extension that can be used to instrument php apps for distributed trace collection, and to send those traces to **[Jaeger](https://github.com/jaegertracing/jaeger)**. 

## Installation

Extension relies on **[PHP-CPP](https://github.com/CopernicaMarketingSoftware/PHP-CPP)** library. 
So, install it firstly. You can do this manually or using provided submodule and commands:

```bash
make fetch-php-cpp
sudo make install-php-cpp
```

Then, install the extension.

```bash
sudo make
sudo make install
```

And restart php service.
```bash
sudo make restart 
```

## Initialization

Tracer must be initialized before any further usage. 
Otherwise all consequent calls will operate as noop.
Let's assume that we have jaeger-agent listening on port 6831: 

```php
\Tracer::init('serviceName', [
    'enabled' => true,
    'reporter' => [
        'type' => 'udp',
        'options' => [
            'addr' => '192.168.10.10',
            'port' => 6831,
        ],
    ],
    'sampler' => [
        'type' => 'percentage',
        'options' => [
            'percents' => 100
        ]
    ]
]);
```

Some tags (host, ip) are attached to service automatically. This is done via internal `hostname` call.
Check also reporter and sampling settings format in the example above.

## Start span

```php
// starting root span
$span = \Tracer::startSpan('spanName');

// starting sub-span with `child Of` relation
$span2 = \Tracer::startSpan('subSpanName', [
    'childOf' => $span
]);

// starting sub-span with `follows From` relation
$span3 = \Tracer::startSpan('subSpanName', [
    'followsFrom' => $span2
]);
```

New span will be treated as root span if no `context` or `span` is passed in the options. 
Actually, we need context only, and extension will get context from a provided span under the hood.

## Logs, tags

You can add multiple tags or logs to known span.

```php
$span->addTags([
    'tagKey' => 'tagValue',
]);

$span->addLogs([
    'logKey' => 'logValue',
]);
```

But sometimes it's useful to attach them to "current span" - opened and not finished one.
You can do this as follows:

```php
\Tracer::addTags([
    'tagKey' => 'tagValue',
]);

\Tracer::addLogs([
    'logKey' => 'logValue',
]);
``` 

## Get current span

You always can get current span without passing its object everywhere, just use this command:

```php
$span = \Tracer::getCurrentSpan();
``` 

Null will be returned if there are no opened spans.

## Finish and flush

You can finish span explicitly:

```php
\Tracer::finishSpan($span);
``` 

But there is no need to do so, for example, for root span - all opened spans will be finished (and flushed) automatically when php process handling your request is done.

For long running scripts you should flush the trace yourself in order to avoid memory burst:

```php
\Tracer::flush();
``` 

## IDs

If you need span/trace identifiers, you can get them this way:

```php
$spanId = \Tracer::getCurrentSpanId($span);
$parentId = \Tracer::getCurrentParentId($span);
$traceId = \Tracer::getCurrentTraceId($span);
``` 

## Inject and extract

Extension supports map and string propagation. Just check this snippet out for more information:

```php
$span = \Tracer::startSpan('spanName');

$array = [];
$array = \Tracer::inject($span, $array);
echo print_r($array, 1);
// e.g. Array ( [X-B3-Traceid] => 2452943018168689885 [X-B3-Spanid] => 2452943018168689885 [X-B3-Parentspanid] => 0 [X-B3-Sampled] => 1 )

$context = \Tracer::extract($array);
if ($context) {
    $subSpan = \Tracer::startSpan('subSpanName', [
        'childOf' => $context
    ]);

    $string = 'yourString';
    $string = \Tracer::inject($subSpan, $string);
    echo $string;
    // e.g. yourString|2452943018168689885:2701778407571494680:2452943018168689885:1
}
``` 

## Other info

Extension tries its best to deliver trace to the agent.
And if the trace's size overflows UDP package - we split spans into several copies and flush them one by one so that you'll get multiple identical spans with
a sequential set of logs.

## Building for PHP 7.3

1. clone from master (php-cpp will have tag 2.1.4)

```git clone git@github.com:ITECOMMPAY/jaeger-client-php-ext.git
cd jaeger-client-php-ext/
git submodule update --init
```

2. `PHP-CPP` compile as static lib, do not `make install` 

```cd PHP-CPP/
sed -i "s/^STATIC_COMPILER_FLAGS.*/STATIC_COMPILER_FLAGS	=	-fpic/" Makefile
scl enable devtoolset-8 php74 "make clean"
scl enable devtoolset-8 php74 "make"
cp -r include/ phpcpp/
cp libphpcpp.a.2.2.0 libphpcpp.a
```

3. `jaeger-client` compile

```cd ..
sed -i "s/^COMPILER_FLAGS.*/COMPILER_FLAGS           = -Wall -c -I\"src\" -I\"PHP-CPP\" -I\"\/opt\/remi\/php74\/root\/usr\/include\" -O2 -std=c++11 -fpic -o/" Makefile
sed -i "s/^LINKER_FLAGS.*/LINKER_FLAGS               = -shared -L\"PHP-CPP\" -L\"\/opt\/remi\/php74\/root\/usr\/lib64\" -fpic -Wl,--whole-archive/" Makefile
sed -i "s/^LINKER_DEPENDENCIES.*/LINKER_DEPENDENCIES = -l:libphpcpp.a -Wl,--no-whole-archive/" Makefile
scl enable devtoolset-8 php74 "make clean"
scl enable devtoolset-8 php74 "make"
sudo scl enable devtoolset-8 php74 "make install"
echo 'extension=jaeger-client.so' | sudo tee /etc/opt/remi/php74/php.d/50-jaeger-client.ini
sudo systemctl restart php74-php-fpm.service
```

## License

[Apache 2.0 License](./LICENSE).