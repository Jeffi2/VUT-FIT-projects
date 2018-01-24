<?php
/**
 * Created by PhpStorm.
 * User: juda
 * Date: 10.11.17
 * Time: 20:48
 */

namespace App\Presenters;
use Nette,
    App\Model;

class HomeDodPresenter extends Nette\Application\UI\Presenter
{
    private $database_handler;
    private $price;
    private $id_syr;
    private $login;

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
        if (!$this->user->isInRole("supplier")) {
            $this->redirect('Homepage:');
        }
        $cheese = $this->database_handler->get_offered_cheese($this->getUser()->id[0]->login);
        if (!is_null($cheese)){
            foreach ($cheese as $ch){
            $temp = $this->database_handler->get_country_name($ch["id_zem"]);
            $ch["zeme_nazev"] = $temp[0]["nazev"];
            }
        }
        if (!is_null($cheese))
            $this->template->nabizeneSyry = $cheese;
        else
            $this->template->noCheese = true;
    }

    public function handleDelete($id_syr, $login) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $succ = false;
        try {
            $this->database_handler->delete_offered_cheese($id_syr,$login);
            $this->flashMessage('Nabídka odstraněna.', 'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
        if ($succ == true)
            $this->redirect("HomeDod:default");
    }

    public function actionUpdatePrice($id_syr, $login, $price) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $this->login = $login;
        $this->id_syr = $id_syr;
        $this->price = $price;
    }

    public function actionOrders($login) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $orders = $this->database_handler->get_supp_orders($login);
        if (!is_null($orders)) {
            $this->template->orders = array_reverse($orders);
        }
        else {
            $this->template->noOrders = true;
        }
    }

    public function handleUpdate($id_syr, $login, $price) {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $this->redirect(":updatePrice",$id_syr, $login, $price);
    }

    protected function createComponentUptCheeseForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addSubmit('send', 'Uptavit')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('cena', 'Cena v Kč/Kg')
            ->setRequired("Zadejte cenu za Kg.")
            ->addRule($form::NUMERIC, 'Zadejte cenu za Kg.')
            ->setAttribute('placeholder', 'Cena Kč/Kg')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Cena musí být větší jak 0.', 1)
             ->setDefaultValue($this->price);

        $form->onSuccess[] = [$this, 'uptCheeseFormsucc'];
        return $form;
    }

    public function uptCheeseFormsucc($form, $values)
    { if (!$this->user->isLoggedIn()) {
        $this->redirect('Login:');
    }
        $succ = false;
        try {
          //  $this->database_handler->add_cheese($values, $this->getUser()->id[0]->login);
            $this->database_handler->update_cheese_price($values, $this->login, $this->id_syr);
            $this->flashMessage("Cena upravena.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
            }
        }
        if ($succ == true)
            $this->redirect("HomeDod:default");
    }


    protected function createComponentRegCheeseForm()
    {
        $form = new Nette\Application\UI\Form;
        $form->addSubmit('send', 'Přidat')
            ->setAttribute('class', 'form-control btn btn-login')
            ->setAttribute('id', 'login-submit');

        $form->addText('nazev', 'Název:')
            ->setRequired('Zadejte název sýru.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka názvu je 100 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'Název musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Název')
            ->setAttribute('class', 'form-control');

        $form->addText('syr_trvanlivost', 'Trvanlivost:')
            ->setRequired('Zadejte dny trvalivosti sýru.')
            ->addRule($form::NUMERIC, 'Zadejte počet dní trvanlivosti.')
            ->addRule($form::RANGE, 'Trvanlivost musí být mezi 1-365.', [1, 365])
            ->setAttribute('placeholder', 'Dny trvanlivosti')
            ->setAttribute('class', 'form-control');

        $form->addText('zivocich', 'Živočich:')
            ->setRequired('Zadejte z jakého živočicha sýr pochází.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka zivocicha je 40 znaků!', 100)
            ->addRule($form::MIN_LENGTH, 'živočich musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Živočich')
            ->setAttribute('class', 'form-control');

        $form->addText('tuk', 'Procento tuku:')
            ->setRequired('Zadejte procento tuku sýru.')
            ->addRule($form::NUMERIC, 'Zadejte číslo tuku.')
            ->setAttribute('placeholder', 'Procento tuku')
            ->setAttribute('class', 'form-control')
            ->addRule($form::RANGE, 'Procento tuku musí být mezi 0-100', [0, 100]);

        $form->addText('typ', 'Typ sýru:')
            ->setRequired('Zadejte typ sýru.')
            ->addRule($form::MAX_LENGTH, 'Maximální délka typu je 40 znaků!', 40)
            ->addRule($form::MIN_LENGTH, 'Typ musí obsahovat alespoň 1 znak!', 1)
            ->setAttribute('placeholder', 'Typ sýru')
            ->setAttribute('class', 'form-control');

        $form->addSelect('id_zem', 'Země',$this->database_handler->get_country())
            ->setAttribute('class', 'form-control')
            ->setDefaultValue(6);

        $form->addText('cena', 'Cena v Kč/Kg')
            ->setRequired("Zadejte cenu za Kg.")
            ->addRule($form::NUMERIC, 'Zadejte cenu za Kg.')
            ->setAttribute('placeholder', 'Cena Kč/Kg')
            ->setAttribute('class', 'form-control')
            ->addRule($form::MIN, 'Cena musí být větší jak 0.', 1);

        $form->onSuccess[] = [$this, 'regCheeseFormsucc'];
        return $form;
    }

    public function regCheeseFormsucc($form, $values)
    { if (!$this->user->isLoggedIn()) {
        $this->redirect('Login:');
    }

        $succ = false;
        try {
            $this->database_handler->add_cheese($values, $this->getUser()->id[0]->login);
            $this->flashMessage("Sýr přidán.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
            }
        }
        if ($succ == true)
            $this->redirect("HomeDod:default");

    }
}