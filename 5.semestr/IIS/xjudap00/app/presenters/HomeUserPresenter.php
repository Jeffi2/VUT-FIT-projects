<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 6.11.17
 * Time: 0:04
 */
namespace App\Presenters;
use Nette,
    App\Model;
/**
 * Homepage presenter.
 */
class HomeUserPresenter extends Nette\Application\UI\Presenter{
    private $database_handler;
    private $id;
    private $id_obj;


    public function __construct(Model\iis_database $database_handler,Nette\Http\Session $session){
        $this->database_handler=$database_handler;
    }

    public function renderDefault(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $result=$this->database_handler->get_shops();
        $this->template->data=$result;
    }

    public function handleLogout(){
        $this->getUser()->logout();
        unset($_SESSION['card']);
        $this->flashMessage('Byl jste odhlasen.', 'alert-success');
        $this->redirect("Homepage:");
    }

    public function handlechooseShop($id){
        $_SESSION['shop']=$id;
        if (isset($_SESSION['card']))
            unset($_SESSION['card']);
        $this->redirect(":shopDetail",$id);

    }

    public function handleDelete($id_boch, $id_pro){
        $succ = false;
        try {
                $this->database_handler->remove_boch($id_boch);
                $this->flashMessage('Bochnik odstraněn.', 'alert-success');
                $succ = true;
            }
        catch (\Exception $e) {
            $this->flashMessage($e->getMessage(), 'alert-danger');
        }
        if ($succ == true)
            $this->redirect(":shopDetail", $id_pro);
    }


    /*
     * id je bud id prodejny nebo si posílam pole s objednavkou
     */
    public function actionShopDetail($id){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        try {
            $this->template->showOrders = true;
            $this->template->id_prodejny = $id;
            $old_cheeses = $this->database_handler->get_old_cheeses($id);
            if ($old_cheeses != NULL){
                $this->template->oldCheese = $old_cheeses;
            }
            $this->template->cena = "";
            $result = $this->database_handler->cheese_in_shop($id);
            $this->template->data = $result;
            $this->template->id = $id;
            $this->template->info = $this->database_handler->cheese_graph($id);
            if (isset($_SESSION['send']))
                if ($_SESSION['send']) {
                    $this->flashMessage("Objednavka odeslána", 'alert-success');
                    $_SESSION['send'] = false;
                }
            $this->template->draw = 1;
        } catch (\Exception $e) {
            //$this->flashMessage($e->getMessage(),"alert-danger");
            $this->template->prod_name = $this->database_handler->get_prod_name($id);
            $this->template->draw = 0;
        }
    }

    public function handleFirstChange($value){

        if ($value) {
            $_SESSION['ico']=$value;


            $this['selectForm']['second']->setPrompt('Sýr')
                ->setItems($this->database_handler->cheese_form_supplier($value))
            ->setRequired("Zadejte sýr");


        } else {
            $this['selectForm']['second']->setPrompt('Sýr')
                ->setItems([]);
        }

        $this->redrawControl('wrapper');
        $this->redrawControl('secondSnippet');
    }



    public function handleSecondChange($value){
        try {
            $this->template->cena = $this->database_handler->get_price(array($value, $_SESSION['ico']))[0] . " Kč / Kg";
        }catch (\Exception $e){
            $this->template->cena="";
        }


        $this->redrawControl('wrapper');
        $this->redrawControl('thirdSnippet');

    }
    /********************** form **********************/


    protected function createComponentSelectForm(){

        if ( (!isset($_SESSION['card'])) || empty($_SESSION['card']) ){
        $form = new Nette\Application\UI\Form;
        $form->addSelect('first', 'Dodavatelé', $this->database_handler->suppliers())
            ->setAttribute('class', 'form-control')
            ->setPrompt("Vyberte dodavatele")
            ->setRequired("Vyberte dodavatele!");


        $form->addSelect('second', 'Sýr')
            ->setAttribute('class', 'form-control')
            ->setPrompt('Sýry');


            $form->addSubmit('send', 'Přidat do objednávky')
        ->setAttribute('class', 'form-control btn btn-login');
        $form->addText('mnozstvi', 'Množství (v Kg)')
            ->setRequired("Zadejte množství!")
            ->addRule($form::FLOAT, 'Zadejte množství.')
            ->setAttribute('placeholder', 'Množství')
            ->setAttribute('class', 'form-control');


        $form->onSuccess[] = [$this, 'processSelectForm'];

        return $form;
    }
    else{
        $form = new Nette\Application\UI\Form;
        $form->addSelect('first', 'Dodavatelé', array($_SESSION['card'][0]['first'] => $_SESSION['card'][0]['nazev']))
            ->setAttribute('class', 'form-control')
            ->setPrompt("Vyberte dodavatele")
            ->setRequired("Zadejte dodavatele");


        $form->addSelect('second', 'Sýr')
            ->setAttribute('class', 'form-control')
            ->setPrompt('Sýry');


        $form->addSubmit('send', 'Přidat do objednávky')
            ->setAttribute('class', 'form-control btn btn-login');
        $form->addText('mnozstvi', 'Množství (v Kg)')
            ->setRequired("Zadejte množství!")
            ->addRule($form::FLOAT, 'Zadejte množství.')
            ->addRule($form::MIN, 'Množství musí být kladné',0)

        ->setAttribute('placeholder', 'Množství')
            ->setAttribute('class', 'form-control');


        $form->onSuccess[] = [$this, 'processSelectForm'];

        return $form;
    }
    }


    /**
     * @param form
     */
    public function processSelectForm(Nette\Application\UI\Form $form){
        // $form->getValues() ignores invalidated input's values
        $values = $form->getHttpData();
        unset($values['send']);
        try {
            $values['cena'] = $this->database_handler->get_price(array(0 => $values['second'], 1 => $values['first']))[0] * $values['mnozstvi'];
        }catch (\Exception $e){
            $this->flashMessage("Vyber sýr",'alert-danger');
       //     $this->redirect(":shopDetail",$_SESSION['shop']);
        }
        $_SESSION['card'][]=$values;


        unset($values['send']);
        $this->redirect(":card");

    }

    public function actionShowOrders($id_pro)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $this->template->showOrders = true;
        $this->template->id_prodejny = $id_pro;
        $orders = $this->database_handler->get_shop_orders($id_pro);
        if (!is_null($orders)) {
            $this->template->orders = array_reverse($orders);
        }
        else {
            $this->template->noOrders = true;
        }
    }

    public function actionCard(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $this->template->showOrders = true;
        $this->template->id_prodejny = $_SESSION['shop'];
        if (!isset($_SESSION['card']))
            $this->redirect("shopDetail",$_SESSION['shop']);
        $card=$_SESSION['card'];
        $this->template->cena="";
        try {
            for ($i = 0; $i < count($card); $i++) {
                $card[$i]["nazev"] = $this->database_handler->supplier_name($card[$i]['first'])[0];
                $card[$i]["syr"] = $this->database_handler->cheese_name($card[$i]['second'])[0];
            }
            $this->template->card = $card;
            $total_price = 0;
            for ($i = 0; $i < count($_SESSION['card']); $i++) {
                $tmp = array(0 => $_SESSION['card'][$i]['second'], 1 => $_SESSION['card'][0]['first']);
                $total_price += ($this->database_handler->get_price($tmp)[0] * $_SESSION['card'][$i]['mnozstvi']);
            }
            $this->template->celkem = $total_price;
            $_SESSION['card'] = $card;
        }catch (\Exception $e){
            $this->flashMessage("vypln všechna polička",'alert-danger'); //TODO overit co chytne vyjimka
        }
    }

    public function handleremove($id){
        if (!$this->user->isLoggedIn()) {
            unset($_SESSION['card']);
            $this->redirect('Login:');
        }
        unset($_SESSION['card'][$id]);
        $_SESSION['card']=array_values($_SESSION['card']);
        $this->redirect(":card");
    }

    public function handlesend(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if ((!isset($_SESSION['card'])) || empty($_SESSION['card'])) {
            $this->redirect(":shopDetail",$_SESSION['shop']);
        }

    $price=0;
        //dump($_SESSION['card']);
    for ($i = 0; $i<count($_SESSION['card']);$i++) {
        $tmp=array( 0 => $_SESSION['card'][$i]['second'],1=> $_SESSION['card'][0]['first']);
       // dump($tmp);
        $price += ($this->database_handler->get_price($tmp)[0]*$_SESSION['card'][$i]['mnozstvi']);
        }
       try {
           $id_zam = $this->database_handler->get_staff($this->user->id[0][0])[0]['id_zam'];
           $order_data = array("celkova_cena" => $price, "datum_vytvoreni" => date('Y-m-d H:i:s'), "dodano" => 0,
               "ico" => $_SESSION['card'][0]['first'], "id_zam" => $id_zam, "id_pro" => $_SESSION['shop'], "zaplacena" =>0);


           $id_result = $this->database_handler->insert_order($order_data);


           $result=0;
           for ($i = 0; $i<count($_SESSION['card']);$i++) {
                $result+=$this->database_handler->insert_order_detail(array("id_obj"=>$id_result,
                    "id_syr"=>$_SESSION['card'][$i]['second'],"hmotnost" =>$_SESSION['card'][$i]['mnozstvi']));
           }
          // if ($result!==count($_SESSION['card']))
            //   throw new Nette\Neon\Exception("nejaka chyba pri pridavani objednavky ",2);




       }catch (\Exception $e) {
         //   dump($e);
           // die();
       }
        unset($_SESSION['card']);
        $_SESSION['send']=true;
        $this->redirect(":shopDetail",$_SESSION['shop']);

    }

    protected function createComponentChoosePayment()
    {
        $form = new Nette\Application\UI\Form;
        $form->addSelect('druh_platby', 'Platba',[
            '0' => 'HOTOVĚ',
            '1' => "FAKTURA",
        ])
            ->setAttribute('class', 'form-control')
            ->setDefaultValue('0');

        $form->addSubmit('prevzit', 'Potvrdit převzetí bochníku')
            ->setAttribute('class', 'form-control btn btn-login');

        $form->onSuccess[] = [$this, 'processChoosePayment'];
        return $form;
    }

    public function processChoosePayment($form, $values)
    {
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        //var_dump($_SESSION["obj"]);die();
        $succ = false;
        try {
            $this->database_handler->add_cheeses_to_shop($values, $this->id_obj);
            $this->flashMessage("Bochníky přidány na prodejnu.",'alert-success');
            $succ = true;
        } catch (\Exception $e) {
            switch ($e->getCode()) {
                default:
                    $this->flashMessage($e->getMessage(),'alert-danger');
            }
        }
        if ($succ == true)
            $this->redirect("HomeUser:shopDetail", $_SESSION['shop']);
    }


    protected function createComponentFindCheese()
    {
        $form = new Nette\Application\UI\Form;



        $form->addSelect('id_zem', 'Země',$this->database_handler->get_country())
            ->setAttribute('class', 'form-control')
            ->setPrompt("Vyberte zemi");

        $form->addText('typ', 'Typ:')
            ->setRequired(false)
            ->setAttribute('placeholder', 'Typ')
            ->setAttribute('class', 'form-control');

        $form->addText('zivocich', 'Živočich:')
            ->setRequired(false)
            ->setAttribute('placeholder', 'Živočich')
            ->setAttribute('class', 'form-control');
        $form->addSubmit('vyhledat', 'Vyhledat')
            ->setAttribute('class', 'form-control btn btn-login');



        $form->onSuccess[] = [$this, 'processFindCheese'];
        return $form;
    }

    public function processFindCheese(Nette\Application\UI\Form $form){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        unset($_SESSION['cheese']);

        $result=$form->getValues();
        $data=array();
        if ($result['id_zem'] === null)
            unset($result['id_zem']);
        else
            $data['id_zem']=$result['id_zem'];

        if($result['typ'] == '')
            unset($result['typ']);
        else
            $data['typ']=$result['typ'];

        if($result['zivocich'] == '')
            unset($result['zivocich']);
        else
            $data['zivocich']=$result['zivocich'];

        if (empty($data)) {
            $this->flashMessage("Zadejte alepnoň jeden atribut",'alert-danger');
            $this->redirect(":findCheese");

        }

        $_SESSION['cheese']=$data;
        $this->redirect(":findCheese");
    }

    public function actionChoosePayment($id_obj){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        $this->template->showOrders = true;
        $this->template->id_prodejny = $_SESSION['shop'];
        $this->id_obj = $id_obj;
    }

    public function actionFindCheese(){
        if (!$this->user->isLoggedIn()) {
            $this->redirect('Login:');
        }
        if (!isset($_SESSION['cheese']) || empty($_SESSION['cheese']) )
        {}
        else {

            try {
                $result = $this->database_handler->find_cheese($_SESSION['cheese'], "*");
                for ($i=0; $i<count($result); $i++){
                    $result[$i]['zeme']=$this->database_handler->get_country_name($result[$i]['id_zem'])[0][0];
                }
                $this->template->cheese = $result;
            }catch (\Exception $e) {
                if ($e->getCode() === 4) {
                    unset($_SESSION['cheese']);
                    $this->redirect(":findCheese");
                }
            }
            unset($_SESSION['cheese']);
        }
    }
}