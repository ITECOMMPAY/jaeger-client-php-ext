<?php

use \Tracer as Tracer;

function inject($context, &$carrier)
{
    if (!method_exists('\Tracer', 'inject')) {
        return;
    }

    $carrier = \Tracer::inject($context, $carrier);
}


function Start()
{
   //Tracer::getTracer();
   echo("***\n");
   $q = getTracer();
      echo("***\n");
   $q = getTracer();
}

$arr1=[
    'enabled'=>true,
    'mode'=>0,
    'reporter'=>[
        'type'=>'udp',
        'options'=>[
            'addr'=>'localhost',
            'port'=>6831,
            ]
        ],
    'sampler'=>[
        'type'=>'percentage',
        'options'=>[
            'percents'=>100,
            ]
        ]
    ];

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
                                    echo("\n***init***\n");
Tracer::init('gate',$arr1);

$method = "GET";
$path = "/";
$x1 = $method . ' ' . $path;
$x2=[];
$x3 = [
    'http.method' => "GET",
    'http.url' => '/',
    'ase' => '/',
];
$x4 = [
    'error' => true,
    ];
$x5 = [
    'event' => "123",
    'file' => "123",
    'line' => "123",
    'log.level' => "123",
    ];

                                    echo("\n***startSpan***\n");
$span = Tracer::startSpan($x1);
$span_cur = Tracer::getCurrentSpan();

var_dump($span);
if($span)
{
    //var_dump($span1);
    $publishSpan = Tracer::startSpan('produce', [
        'childOf' => $span
            ]);
}

$span->addTags($x3);
$span->addTags($x4);
$publishSpan->addTags($x3);
Tracer::addTags($x3);

                                    echo("\n***addLogs***\n");
$span->addLogs($x3);
$span->addLogs($x4);
$span->addLogs($x5);
Tracer::addLogs($x3);
Tracer::addLogs($x4);
$publishSpan->addLogs($x5);

                                    echo("\n***inject***\n");
$key = "32adb6254e9f7b89a65b79145cf5d0e0a945e9ba-666c4a71a461eb4f02d3b6788d4ab89944da2d9f";
var_dump($key);
//Tracer::inject($publishSpan, $key);
inject($publishSpan,$key);
var_dump($key);
inject($publishSpan,$key);
//Tracer::inject($publishSpan, $key);
var_dump($key);
inject($publishSpan,$key);
//Tracer::inject($publishSpan, $key);
var_dump($key);
inject($publishSpan,$key);
//Tracer::inject($publishSpan, $key);
var_dump($key);
die();

$key_parse = "32adb6254e9f7b89a65b79145cf5d0e0a945e9ba-666c4a71a461eb4f02d3b6788d4ab89944da2d9f|142573106:3223346672:2865357473:1";
$context = Tracer::extract($request);

echo("\n");
echo("\nTest_work_with_array\n");

$x_ref = &$x4;
inject($publishSpan,$x4);
//$x4=Tracer::inject($publishSpan, $x_ref);
echo("\n");

var_dump($x4);
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

                                    echo("\n***finishSpan***\n");
Tracer::finishSpan($span);
Tracer::finishSpan($publishSpan);
if (isset($span_new)) {
    Tracer::finishSpan($span_new);
}
Tracer::flush();

                                    echo("\n***Destruct Tracer***\n");
//$tra = Tracer::getTracer();
//Tracer::init('gate',$arr1);
                                    echo("\n***Destruct finish***\n");

?>