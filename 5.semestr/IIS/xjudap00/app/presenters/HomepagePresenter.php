<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 5.11.17
 * Time: 10:46
 */

namespace App\Presenters;
use Nette,
    App\Model;
/**
 * Homepage presenter.
 */
class HomepagePresenter extends Nette\Application\UI\Presenter{
    private $database;
    private $database_handler;


    public function __construct(Nette\Database\Context $database,Model\iis_database $database_handler){
        $this->database = $database;
        $this->database_handler=$database_handler;
    }
    public function renderDefault(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if ($this->user->roles[0] == "staff"){
            $this->redirect("HomeUser:");
        }
        if ($this->user->isInRole("admin")) {
            $this->redirect("HomeAdmin:");
        }
        if ($this->user->isInRole("supplier")) {
            $this->redirect("HomeDod:");
        }
    }

    public function handleLogout(){
        $this->getUser()->logout();
        $this->flashMessage('Byl jste odhlasen.', 'alert-success');
        $this->redirect("Homepage:");
    }
}