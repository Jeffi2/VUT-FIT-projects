<?php
/**
 * Created by PhpStorm.
 * User: juda
 * Date: 06.11.17
 * Time: 18:14
 */

namespace App\Presenters;
use Nette,
    App\Model;

class HomeAdminPresenter extends Nette\Application\UI\Presenter
{
    private $database_handler;
    private $editUser;
    private $login;
    private $editUser2;
    private $prod;

    public function __construct(Model\iis_database $database_handler){
        $this->database_handler=$database_handler;
    }

    public function handleLogout(){
        $this->getUser()->logout();
        $this->flashMessage('Byl jste odhlasen.', 'alert-success');
        $this->redirect("Homepage:");
    }

    public function renderDefault(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
        $this->template->uzivatele = $this->database_handler->get_users();
    }

    public function renderRegisterUser(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
    }

    public function renderPrehledProdejny(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }

        $prods = $this->database_handler->get_prod();
        foreach ($prods as $prod){
            $fromDB =$this->database_handler->get_zam_on_prod($prod['id_pro']);
            if (!is_null($fromDB)){
                $prod["zamestnanci"] = $fromDB->fetchAll();
            }
        }
        $this->template->prodejny = $prods;
    }

    public function renderRegisterProd(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
    }

    public function handleUpdate($login, $type) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if ($type == 2)
            $this->redirect(":updateUser", $login, $type);
        if ($type == 3)
            $this->redirect(":updateSup", $login, $type);
    }

    public function actionUpdateSup($login, $type){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
        $this->login = $login;
        try {
            $this->editUser = $this->database_handler->get_supp($login);
        } catch (\Exception $e ) {
        }
    }

    public function actionUpdateUser($login, $type){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
        $this->login = $login;
        try {
            $this->editUser = $this->database_handler->get_staff($login);
            $this->editUser2 = $this->database_handler->get_id_pro($this->editUser[0]['id_zam']);
        } catch (\Exception $e ) {
        }
    }

    public function renderRegisterAdmin(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
    }
    public function handleDelete($login, $type) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            if ($type != 1) {
                // THIS ONLY BLOCK flag $this->database_handler->remove_user($login);
                //THIS IS CASCADE delete
                $this->database_handler->remove_admin($login, $type);
                $this->flashMessage('Uživatel odstraněn.', 'alert-success');
                $succ = true;
            }
            else {
                $this->database_handler->remove_admin($login,$type);
                $this->flashMessage('Administrátor odstraněn.', 'alert-success');
            }
        } catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");
    }

    public function handleBlock($login) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $this->database_handler->block_user($login);
            $this->flashMessage('Uživatel zablokován.', 'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");
    }

    public function handleUnblock($login) {
        $succ = false;
        try {
            $this->database_handler->unblock_user($login);
            $this->flashMessage('Uživatel odblokován.', 'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");
    }


    public function handleUpdateProd($id_pro) {
        $this->redirect(":updateProd", $id_pro);
    }

    public function actionUpdateProd($id_pro){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!$this->user->isInRole("admin")) {
            $this->redirect('Homepage:');
        }
        try {
            $this->prod = $this->database_handler->get_specific_prod($id_pro);
            $this->prod['id_pro'] = $id_pro;
            }
        catch (\Exception $e) {}
    }



    public function handleDeleteProd($id_pro) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        try {
            if ($this->database_handler->is_prod_free($id_pro)) {
                $this->database_handler->delte_prod($id_pro);
                $this->flashMessage('Prodejna odstraněna.', 'alert-success');
                }
            else {
                $this->flashMessage('Na prodejně stále pracují zaměstnanci, nelze ji odstranit.', 'alert-danger');
            }
        } catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
    }

    protected function createComponentUptProdForm()
    {
        $form = new Nette\Application\UI\Form;
        $form->addSubmit('send', 'Upravit')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('nazev', 'Název:')
            ->setRequired('Zadejte název prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka názvu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Název musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Název')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->prod[0]["nazev"]);

        $form->addText('ulice', 'Ulice:')
            ->setRequired('Zadejte ulici prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Ulice')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->prod[0]["ulice"]);

        $form->addText('cislo_popisne', 'Číslo popisné:')
            ->setRequired('Zadejte číslo popisné prodejny.')
            ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
            ->setAttribute('placeholder', 'Číslo popisné')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->prod[0]["cislo_popisne"])
            ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);

        $form->addText('mesto', 'Město:')
            ->setRequired('Zadejte město prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Město')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->prod[0]["mesto"]);

        $form->addText('psc', 'PSČ:')
            ->setRequired('Zadejte PSČ prodejny.')
            ->addRule($form::NUMERIC, 'Zadejte psč.')
            ->setAttribute('placeholder', 'PSČ')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->prod[0]["psc"])
            ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');

        $form->onSuccess[] = [$this, 'uptProdSucceeded'];
        return $form;
    }

    public function uptProdSucceeded($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $values['id_pro'] = $this->prod[0]['id_pro'];
        $succ = false;
        try {
            $this->database_handler->update_shop($values);
            $this->flashMessage("Prodejna upravena.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
            }
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:prehledProdejny");

    }

    protected function createComponentRegProdForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addSubmit('send', 'Přidat')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('nazev', 'Název:')
            ->setRequired('Zadejte název prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka názvu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Název musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Název')
            ->setAttribute('class', 'form-control');

        $form->addText('ulice', 'Ulice:')
            ->setRequired('Zadejte ulici prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Ulice')
            ->setAttribute('class', 'form-control');

        $form->addText('cislo_popisne', 'Číslo popisné:')
            ->setRequired('Zadejte číslo popisné prodejny.')
            ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
            ->setAttribute('placeholder', 'Číslo popisné')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);

        $form->addText('mesto', 'Město:')
            ->setRequired('Zadejte město prodejny.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Město')
            ->setAttribute('class', 'form-control');

        $form->addText('psc', 'PSČ:')
            ->setRequired('Zadejte PSČ prodejny.')
            ->addRule($form::NUMERIC, 'Zadejte psč.')
            ->setAttribute('placeholder', 'PSČ')
            ->setAttribute('class', 'form-control')
            ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');

        $form->onSuccess[] = [$this, 'regProdSucceeded'];
        return $form;
    }

    public function regProdSucceeded($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $this->database_handler->insert_shop($values);
            $this->flashMessage("Prodejna přidána.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
                    $this->template->dbERR = $e->getMessage();
            }
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:prehledProdejny");

    }

    protected function createComponentUpdateSupForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addSubmit('send', 'Upravit')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('nazev', 'Název:')
            ->setRequired('Zadejte název společnosti.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka názvu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Název musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Název')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->editUser[0]['nazev']);

        $form->addText('cislo_uctu', 'Číslo účtu:')
            ->setRequired('Zadejte číslo účtu.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka uctu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ucet musí obsahovat alespoň 10 znaků!', 10)
            ->setAttribute('placeholder', 'Číslo účtu')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->editUser[0]['cislo_uctu']);

        $form->addSelect('id_zem', 'Země',$this->database_handler->get_country())
            ->setAttribute('class', 'form-control')
            ->setDefaultValue(6);

        if ($this->editUser[0]['telefon'] == 0) {
            $form->addText('telefon', 'Telefon:')
                ->setRequired(false)
                ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
                ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
                ->setAttribute('placeholder', 'Telefon')
                ->setAttribute('class', 'form-control');
        }
        else {
            $form->addText('telefon', 'Telefon:')
                ->setRequired(false)
                ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
                ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
                ->setAttribute('placeholder', 'Telefon')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['telefon']);
        }
        if (is_null($this->editUser[0]['ulice'])) {
            $form->addText('ulice', 'Ulice:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Ulice')
                ->setAttribute('class', 'form-control');
        } else {
            $form->addText('ulice', 'Ulice:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Ulice')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['ulice']);
        }
        if ($this->editUser[0]['cislo_popisne'] == 0) {
            $form->addText('cislo_popisne', 'Číslo popisné:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
                ->setAttribute('placeholder', 'Číslo popisné')
                ->setAttribute('class', 'form-control')
                ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);
        } else {
            $form->addText('cislo_popisne', 'Číslo popisné:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
                ->setAttribute('placeholder', 'Číslo popisné')
                ->setAttribute('class', 'form-control')
                ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1)
                ->setDefaultValue($this->editUser[0]['cislo_popisne']);
        }
        if (is_null($this->editUser[0]['mesto'])) {
            $form->addText('mesto', 'Město:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Město')
                ->setAttribute('class', 'form-control');
        } else{
            $form->addText('mesto', 'Město:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Město')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['mesto']);
        }
        if ($this->editUser[0]['psc'] == 0) {
            $form->addText('psc', 'PSČ:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte psč.')
                ->setAttribute('placeholder', 'PSČ')
                ->setAttribute('class', 'form-control')
                ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');
        } else {
            $form->addText('psc', 'PSČ:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte psč.')
                ->setAttribute('placeholder', 'PSČ')
                ->setAttribute('class', 'form-control')
                ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}')
                ->setDefaultValue($this->editUser[0]['psc']);
        }
        $form->onSuccess[] = [$this, 'updateSupSucceeded'];
        return $form;
    }

    public function updateSupSucceeded($form, $values)
    { if (!$this->user->isLoggedIn()) {
        $this->redirect('Login:');
    }
        $succ = false;
        try {
            $values['login'] = $this->login;
            $this->database_handler->update_sup($values);
            $this->flashMessage("Dodavatel upraven.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
                    $this->template->dbERR = $e->getMessage();
            }
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");

    }

    protected function createComponentUpdateUsrForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addSubmit('send', 'Upravit')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('jmeno', 'Jméno:')
            ->setRequired('Zadejte jméno zaměstnance.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka jména je 50 znaků!', 50)
            ->addRule($form::MIN_LENGTH, 'Jméno musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Jméno')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->editUser[0]['jmeno']);

        $form->addText('prijmeni', 'Příjmení:')
            ->setRequired('Zadejte příjmení zaměstnance.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka příjmení je 50 znaků!', 50)
            ->addRule($form::MIN_LENGTH, 'Příjmení musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Příjmení')
            ->setAttribute('class', 'form-control')
            ->setDefaultValue($this->editUser[0]['prijmeni']);

        $form->addText('plat', 'Plat:')
            ->setRequired('Zadejte plat zaměstnance.')
            ->addRule($form::NUMERIC, 'Zadejte plat zaměstnance.')
            ->setAttribute('placeholder', 'Plat')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Plat musí být větší jak 0.', 1)
            ->setDefaultValue($this->editUser[0]['plat']);

        $prodejny = array();
        foreach ($this->editUser2 as $item) {
            $prodejny[] = $item["id_pro"];
        }

        $form->addMultiSelect('id_pro', 'Prodejna',$this->database_handler->get_shop())
            ->setAttribute('class', 'form-control')
            ->setRequired('Vyberte prodejnu zaměstnance.')
            ->setDefaultValue($prodejny);

        if ($this->editUser[0]['telefon'] == 0) {
            $form->addText('telefon', 'Telefon:')
                ->setRequired(false)
                ->addRule($form::LENGTH, 'Zadejte 9ti místné číslo.', 9)
                ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
                ->setAttribute('placeholder', 'Telefon')
                ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
                ->setAttribute('class', 'form-control');
        }
        else {
            $form->addText('telefon', 'Telefon:')
                ->setRequired(false)
                ->addRule($form::LENGTH, 'Zadejte 9ti místné číslo.', 9)
                ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
                ->setAttribute('placeholder', 'Telefon')
                ->setAttribute('class', 'form-control')
                ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
                ->setDefaultValue($this->editUser[0]['telefon']);
        }
        if (is_null($this->editUser[0]['ulice'])) {
            $form->addText('ulice', 'Ulice:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Ulice')
                ->setAttribute('class', 'form-control');
        } else {
            $form->addText('ulice', 'Ulice:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Ulice')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['ulice']);
        }
        if ($this->editUser[0]['cislo_popisne'] == 0) {
            $form->addText('cislo_popisne', 'Číslo popisné:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
                ->setAttribute('placeholder', 'Číslo popisné')
                ->setAttribute('class', 'form-control')
                ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);
        } else {
            $form->addText('cislo_popisne', 'Číslo popisné:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte číslo popisné.')
                ->setAttribute('placeholder', 'Číslo popisné')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['cislo_popisne'])
                ->addRule($form::MIN, 'Číslo popisné musí být větší jak 0.', 1);
        }
        if (is_null($this->editUser[0]['mesto'])) {
            $form->addText('mesto', 'Město:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Město')
                ->setAttribute('class', 'form-control');
        } else{
            $form->addText('mesto', 'Město:')
                ->setRequired(false)
                ->addRule($form::MAX_LENGTH, 'Maximální délka města je 100 znaků!', 100)
                ->addRule($form::MIN_LENGTH, 'Město musí obsahovat alespoň 1 znak!', 1)
                ->setAttribute('placeholder', 'Město')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['mesto']);
        }
        if ($this->editUser[0]['psc'] == 0) {
            $form->addText('psc', 'PSČ:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte psč.')
                ->setAttribute('placeholder', 'PSČ')
                ->setAttribute('class', 'form-control')
                ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');
        } else {
            $form->addText('psc', 'PSČ:')
                ->setRequired(false)
                ->addRule($form::NUMERIC, 'Zadejte psč.')
                ->setAttribute('placeholder', 'PSČ')
                ->setAttribute('class', 'form-control')
                ->setDefaultValue($this->editUser[0]['psc'])
                ->addRule($form::PATTERN, 'PSČ: zadejte 5ti místné číslo.', '[0-9]{5}');
        }

        $form->onSuccess[] = [$this, 'updateUsrSucceeded'];
        return $form;
    }

    public function updateUsrSucceeded($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $values['login'] = $this->login;
            $this->database_handler->update_staff($values);
            $this->flashMessage("Zaměstnanec upraven.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
                    $this->template->dbERR = $e->getMessage();
            }
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");

    }

    protected function createComponentRegUsrForm()
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

        $form->addSubmit('send', 'Registrovat')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('jmeno', 'Jméno:')
            ->setRequired('Zadejte jméno zaměstnance.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka jména je 50 znaků!', 50)
            ->addRule($form::MIN_LENGTH, 'Jméno musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Jméno')
            ->setAttribute('class', 'form-control');

        $form->addText('prijmeni', 'Příjmení:')
            ->setRequired('Zadejte příjmení zaměstnance.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka příjmení je 50 znaků!', 50)
            ->addRule($form::MIN_LENGTH, 'Příjmení musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Příjmení')
            ->setAttribute('class', 'form-control');

        $form->addText('plat', 'Plat:')
            ->setRequired('Zadejte plat zaměstnance.')
            ->addRule($form::NUMERIC, 'Zadejte plat zaměstnance.')
            ->setAttribute('placeholder', 'Plat')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Plat musí být větší jak 0.', 1);

        $form->addMultiSelect('id_pro', 'Prodejna',$this->database_handler->get_shop())
            ->setAttribute('class', 'form-control')
            ->setRequired('Vyberte prodejnu zaměstnance.');

        $form->addText('telefon', 'Telefon:')
            ->setRequired(false)
            ->addRule($form::PATTERN, 'Telefon: zadejte 9ti místné číslo.', '[0-9]{9}')
            ->addRule($form::NUMERIC, 'Zadejte 9ti místné číslo.')
            ->setAttribute('placeholder', 'Telefon')
            ->setAttribute('class', 'form-control');

        $form->addText('ulice', 'Ulice:')
            ->setRequired(false)
            ->addRule($form::MAX_LENGTH, 'Maximální délka ulice je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Ulice musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Ulice')
            ->setAttribute('class', 'form-control');

        $form->addText('cislo_popisne', 'Ulice:')
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


        $form->onSuccess[] = [$this, 'registrationUsrSucceeded'];
        return $form;
    }

    public function registrationUsrSucceeded($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $this->database_handler->insert_staff($values);
            $this->flashMessage("Zaměstnanec přidán.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                case 1:
                    $this->template->loginERR = $e->getMessage();
                    break;
                default:
                    $this->template->dbERR = $e->getMessage();
            }
        }
        if ($succ == true)
            $this->redirect("HomeAdmin:default");

    }

    protected function createComponentRegAdmForm()
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

        $form->addSubmit('send', 'Registrovat')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->onSuccess[] = [$this, 'registrationAdmSucceeded'];
        return $form;
    }

    public function registrationAdmSucceeded($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $this->database_handler->insert_admin($values);
            $this->flashMessage("Administrátor přidán.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                case 1:
                    $this->template->loginERR = $e->getMessage();
                    break;
                default:
                    $this->template->dbERR = $e->getMessage();
            }
        }
       if ($succ == true)
           $this->redirect("HomeAdmin:default");

    }
}