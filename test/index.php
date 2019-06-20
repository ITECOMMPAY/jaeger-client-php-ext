<?php

function inject($context, &$carrier)
{
    if (!method_exists('\Tracer', 'inject')) {
        return;
    }

    $carrier = \Tracer::inject($context, $carrier);
}

$request=[
    'Connection'=>'keep-alive',
    'Content-Length'=>'481',
    'Accept-Encoding'=>'gzip, deflate',
    'Host'=>'gate.test',
    'X-B3-Sampled'=>'4',
    'X-B3-Parentspanid'=>'3',
    'X-B3-Spanid'=>'2',
    'X-B3-Traceid'=>'1',
    'Content-Type'=>'application/json',
    'Cookie'=>'XDEBUG_SESSION=PHPSTORM',
    'User-Agent'=>'vscode-restclient',
];

Tracer::init('gate',
    [
        'enabled' => true,
        'mode' => 0,
        'debug_output' => true,
        'udp_transport' => true,
        'reporter' => [
            'type' => 'udp',
            'options' => [
                'addr' => '127.0.0.1',
                'port' => 6831,
            ],
        ],
        'sampler' => [
            'type' => 'percentage',
            'options' => [
                'percents' => 100
            ]
        ]
    ]
);

$x3 = 
$x4 = 
// $x5 = [
//     'event' => "123",
//     'file' => "123",
//     'line' => "123",
//     'log.level' => "123",
//     ];

$span_1 = Tracer::startSpan("span_1");
echo "getCurrentTraceId:\t\t\t";var_dump(Tracer::getCurrentTraceId());
echo PHP_EOL;

$span_cur = Tracer::getCurrentSpan();

echo "getCurrentSpanId (span_1):\t\t";var_dump(Tracer::getCurrentSpanId($span_1));
echo "getCurrentParentId (span_1):\t\t";var_dump(Tracer::getCurrentParentId($span_1));
echo PHP_EOL;

if($span_1)
{
    $span_2 = Tracer::startSpan('span_2 - childOf', [
        'childOf' => $span_1
    ]);
    echo "getCurrentSpanId (span_2):\t\t";var_dump(Tracer::getCurrentSpanId($span_2));
    echo "getCurrentParentId (span_2):\t\t";var_dump(Tracer::getCurrentParentId($span_2));
    echo PHP_EOL;

    $span_3 = Tracer::startSpan('span_3 - followsFrom', [
        'followsFrom' => $span_1
    ]);
    echo "getCurrentSpanId (span_3):\t\t";var_dump(Tracer::getCurrentSpanId($span_3));
    echo "getCurrentParentId (span_3):\t\t";var_dump(Tracer::getCurrentParentId($span_3));
    echo PHP_EOL;
}

$span_1->addTags([
    'span_1.tag_1' => "tag_1",
]);
$span_1->addTags([
    'span_1.tag_2' => true,
]);
$span_2->addTags([
    'span_2.tag_1' => "tag_1",
]);
Tracer::addTags([
    'tracer_1.tag_1' => "tag_1",
]);

$span_1->addLogs([
    'span_1.log_1' => "log_1",
]);
$span_1->addLogs([
    'span_1.log_2' => "log_2",
]);
$span_2->addLogs([
    'span_2.log_1' => "log_1",
]);
Tracer::addLogs([
    'tracer_1.log_1' => "log_1",
]);

$key = "32adb6254e9f7b89a65b79145cf5d0e0a945e9ba-666c4a71a461eb4f02d3b6788d4ab89944da2d9f";
var_dump($key);
//Tracer::inject($span_2, $key);
inject($span_2,$key);
var_dump($key);
inject($span_2,$key);
//Tracer::inject($span_2, $key);
var_dump($key);
inject($span_2,$key);
//Tracer::inject($span_2, $key);
var_dump($key);
inject($span_2,$key);
//Tracer::inject($span_2, $key);
var_dump($key);

$key_parse = "32adb6254e9f7b89a65b79145cf5d0e0a945e9ba-666c4a71a461eb4f02d3b6788d4ab89944da2d9f|142573106:3223346672:2865357473:1";
$context = Tracer::extract($request);

$x_ref = &$x4;
inject($span_2,$x4);
//$x4=Tracer::inject($span_2, $x_ref);
echo("\n");

 //if ($context)
 //{
 //    $span_new = Tracer::startSpan('handle', [
 //        'followsFrom' => $context
 //    ]);
 //    $span_new->addTags([
 //        'bus.topic' => '2323',
 //        'bus.partition' => '341',
 //        'bus.offset' => '444',
 //    ]);

 //}


Tracer::finishSpan($span_1);
Tracer::finishSpan($span_2);
Tracer::finishSpan($span_3);

Tracer::flush();

Tracer::init('gate','');

?>