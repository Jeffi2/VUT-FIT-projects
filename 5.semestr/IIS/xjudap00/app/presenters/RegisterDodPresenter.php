<?php
/**
 * Created by PhpStorm.
 * User: juda
 * Date: 05.11.17
 * Time: 18:51
 */
namespace App\Presenters;
use Nette,
    App\Model;
use PMA\libraries\config\Form;

class RegisterDodPresenter extends Nette\Application\UI\Presenter{

    private $database_handler;
    public function __construct(Model\iis_database $database_handler){
        $this->database_handler=$database_handler;
    }

    protected function createComponentRegistrationForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addEmail('login', 'Email:')
            ->setRequired('Zadejte vaši e-mailovou adresu.')
            ->setType('email') // Nastaveni typu pro HTML5 - mobilni zarizeni
            ->addRule($form::EMAIL, 'Prosím zadaj platný email.')
            ->setAttribute('placeholder', 'E-mailova adresa')
            ->setAttribute('class', 'form-control');

        $form->addPassword('password', 'Heslo:')
            ->setRequired('Zvolte heslo')
            ->addRule($form::MIN_LENGTH, 'Heslo musí obsahovat alespoň 3 znaky', 3)
            ->setAttribute('placeholder', 'Heslo')
            ->setAttribute('class', 'form-control');

        $form->addPassword('passwordCheck', 'Heslo pro kontrolu:')
            ->setRequired('Zadejte prosím heslo ještě jednou pro kontrolu')
            ->addRule($form::EQUAL, 'Hesla se neshodují', $form['password'])
            ->setAttribute('placeholder', 'Heslo')
            ->setAttribute('class', 'form-control');

        $form->addText('ico', 'Ičo:')
            ->setRequired('Zadejte ičo.')
            ->addRule($form::PATTERN, 'IČO: zadejte 12ti místné číslo.', '[0-9]{12}')
            ->addRule($form::NUMERIC, 'IČO: zadejte 12ti místné číslo.')
            ->setAttribute('placeholder', 'IČO')
            ->setAttribute('class', 'form-control');

        $form->addText('nazev', 'Název:')
            ->setRequired('Zadejte název společnosti.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka názvu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Název musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Název')
            ->setAttribute('class', 'form-control');

        $form->addText('cislo_uctu', 'Číslo účtu:')
            ->setRequired('Zadejte číslo účtu.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka uctu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ucet musí obsahovat alespoň 10 znaků!', 10)
            ->setAttribute('placeholder', 'Číslo účtu')
            ->setAttribute('class', 'form-control');

        $form->addSelect('id_zem', 'Země',$this->database_handler->get_country())
            ->setAttribute('class', 'form-control')
            ->setDefaultValue(6);

        $form->addText('ulice', 'Ulice:')
            ->setRequired(false)
            ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Ulice')
            ->setAttribute('class', 'form-control');

        $form->addText('cislo_popisne', 'Číslo popisné:')
            ->setRequired(false)
            ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
            ->setAttribute('placeholder', 'Číslo popisné')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);

        $form->addText('mesto', 'Město:')
            ->setRequired(false)
            ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Město')
            ->setAttribute('class', 'form-control');

        $form->addText('psc', 'PSČ:')
            ->setRequired(false)
            ->addRule($form::NUMERIC, 'Zadejte psč.')
            ->setAttribute('placeholder', 'PSČ')
            ->setAttribute('class', 'form-control')
            ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');

        $form->addText('telefon', 'Telefon:')
            ->setRequired(false)
            ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
            ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
            ->setAttribute('placeholder', 'Telefon')
            ->setAttribute('class', 'form-control');

        $form->addSubmit('send', 'Registrovat')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->onSuccess[] = [$this, 'registrationSucceeded'];
        return $form;
    }

    public function handleLogout(){
        $this->getUser()->logout();
        $this->flashMessage('Byl jste odhlasen.', 'alert-success');
        $this->redirect("Homepage:");
    }

    public function registrationSucceeded($form, $values)
    {
        $succ = false;
        try {
            $this->database_handler->insert_supplier($values);
            $this->flashMessage('Registrace provedena. Nyní se můžete přihlásit.','alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                case 1:
                    $this->template->loginERR = $e->getMessage();
                    break;
                case 2:
                    $this->template->icoERR = $e->getMessage();
                    break;
                default:
                    $this->template->dbERR = $e->getMessage();
            }
        }
        if ($succ)
            $this->redirect("Login:");
    }
}

