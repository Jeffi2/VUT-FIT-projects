<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 5.11.17
 * Time: 13:00
 */

namespace App\Presenters;
use Nette,
    App\Model;
class SystemPresenter extends LoginPresenter {
    private $database_handler;

    public function __construct(Model\iis_database $database_handler){
        $this->database_handler = $database_handler;
    }
    public function renderDefault(){

        $this->template->cheese=$this->database_handler->show_cheese();
    }
}