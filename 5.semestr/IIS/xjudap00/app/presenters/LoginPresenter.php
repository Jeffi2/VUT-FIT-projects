<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 5.11.17
 * Time: 11:18
 */
namespace App\Presenters;
use Nette,
    App\Model;
class LoginPresenter extends Nette\Application\UI\Presenter{

    public function renderDefault(){

        if ($this->user->isLoggedIn()) {
            $this->redirect('Homepage:');
        }
    }

    protected function createComponentSignInForm(){
        $form = new Nette\Application\UI\Form;
        $form->addText('email', 'Login:')
            ->setRequired('Zadejte vaši e-mailovou adresu.')
            ->setType('email') // Nastaveni typu pro HTML5 - mobilni zarizeni
            ->setAttribute('placeholder', 'E-mailova adresa')
            ->setAttribute('class', 'form-control')
            ->setAttribute('id', 'email');

        $form->addPassword('password', 'Heslo:')
            ->setRequired('Zadejte vaše heslo.')
            ->setAttribute('placeholder', 'Heslo')
            ->setAttribute('class', 'form-control')
            ->setAttribute('id', 'password');

        $form->addSubmit('send', 'Přihlásit')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->onSuccess[] = [$this, 'signInFormSucceeded'];
        return $form;
    }
    public function signInFormSucceeded($form, $values){


        try {

            $this->getUser()->login($values->email, $values->password);
            $this->getUser()->setExpiration('60 minutes', TRUE);
            $this->redirect("Homepage:");

        } catch (Nette\Security\AuthenticationException $e) {
            $form->addError($e->getMessage());
        }

    }
    public function handleLogout(){
        $this->getUser()->logout();
        $this->flashMessage('Byl jste odhlasen.', 'alert-success');
        $this->redirect("Homepage:");
    }


}