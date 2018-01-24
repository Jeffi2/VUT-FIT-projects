<?php
ini_set('session.save_path',getcwd(). '/tmp');
ini_set('session.gc_probability', 1);

$container = require __DIR__ . '/../app/bootstrap.php';

$container->getByType(Nette\Application\Application::class)
	->run();
