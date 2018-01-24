<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 4.11.17
 * Time: 10:35
 */
namespace App\Model;
use Nette\Neon\Exception;
use Nette\Object;
use DateTime;
use Nette\Utils\Strings;
use Nette\Security\Passwords;
use Nette;

class iis_database extends Object{
    private $database;
    private $result;
    private $user;

    /**
     * iis_database constructor.
     * @param \Nette\Database\Connection $database
     */
    public function __construct(\Nette\Database\Connection $database,\Nette\Security\User $MyUser){
        $this->database=$database;
        $this->user=$MyUser;
    }

    /**
     * debugging function
     */
    public function print_database(){

        foreach ($this->result as $row) {
            print_r($row)  ;

        }

    }



    private function select($query,$params){
        $this->result=$this->database->queryArgs($query,$params);
        if ($this->result->getRowCount() === 0){
            return null;
        } else{
            return $this->result;
        }
    }

    private function select_hack($data,$table){
        //$keys=array_keys($data);
        //var_dump($data);
        //die();
        $query="SELECT * FROM $table WHERE ";
        if (count($data)>1) {
            for ($i = 0; $i < count($data) - 1; $i++) {
                $query .= "id_pro=". $data[$i] ." OR ";
            }
        }
        $query .="id_pro=". $data[count($data)-1] ;
        $result=$this->database->query($query);
        return $result;
    }

    private function find($data,$table,$key){
        $keys=array_keys($data);
        $query="SELECT $key FROM $table WHERE ";
        if (count($keys)>1) {
            for ($i = 0; $i < count($keys) - 1; $i++) {
                $query .= $keys[$i] . "=? AND ";
            }
        }
        $query .= $keys[count($keys)-1]."=? ";
        //echo $query; //TODO remove after testing

        $data=array_values($data);
        $result=self::select($query,$data);
        if($result === null) {
            throw new Exception("neexistujici syr", 4);
        }
        else
            return $result->fetchAll();
    }
    private function remove_person($login,$table){
       // if($this->user->roles[0] !== "admin")
         //   throw new Exception("JEn admin muže mazat!!",3);
       // else {
            $result = $this->database->query("DELETE FROM $table WHERE login=?", $login);
            //TODO tohle hazi exception nelze porovnavat Nette\Database\ResultSet  s int!
            if ($result < 1)
                throw new Exception("Nelze odebrat $login", 1);
            else{
                $result=self::select("SELECT * FROM users WHERE login=?",$login);
                if ($result>0)
                    $result=$this->database->query("DELETE FROM users WHERE login=?",$login);
                    if($result<1)
                        throw new Exception("Chyba při odebirani loginu $login",2);
         //   }
            return $result;
        }
    }

    public function add_cheese_to_offer($login,$cheese){

    }



    public function show_cheese(){
        return self::select("SELECT * FROM syr",[]);
    }

    public function find_cheese($cheese ,$key="*"){
        return self::find($cheese,"syr",$key);
    }
    public function find_staff($staff,$key="*"){
        return self::find($staff,"zamestnanec",$key);
    }
    public function find_supplier($supplier,$key ="*"){
        return self::find($supplier,"dodavatel",$key);
    }
    public function remove_staff($login){
        return self::remove_person($login,"zamestnanec");
    }
    public function remove_supplier($login){
        return self::remove_person($login,"dodavatel");
    }
    public function delte_prod($id_pro) {
        $result=$this->database->query("DELETE FROM prodejna WHERE id_pro=?",$id_pro);
        return $result;
    }

    public function is_prod_free($id_pro) {
        $result=$this->database->query("SELECT * FROM pracuje WHERE id_pro=?",$id_pro);
        if ($result->getRowCount() == 0)
            return true;
        else
            return false;
    }

    public function block_user($login){
        $result=$this->database->query("UPDATE users SET can_log=0 WHERE login=?",$login);
        if($result->getRowCount()<1)
            throw new Exception("Chyba blokování uživatele: $login",1);
        return $result;
    }

    public function unblock_user($login){
        $result=$this->database->query("UPDATE users SET can_log=1 WHERE login=?",$login);
        if($result->getRowCount()<1)
            throw new Exception("Chyba odblokování uživatele: $login",1);
        return $result;
    }
    public function remove_admin($login, $type){
        if ($type == 3) {
            $test = $this->database->query("SELECT bochnik.id_boch FROM bochnik,objednavka,dodavatel WHERE dodavatel.ico = objednavka.ico AND bochnik.id_obj = objednavka.id_obj AND dodavatel.login =?", $login);
            if ($test->getRowCount() != 0)
                throw new Exception("Na prodejně jsou dosud nezpracované bochníky od dodavatele. Nelze jej smazat, pouze zablokovat!");
        }
        $result=$this->database->query("DELETE FROM users WHERE login=?",$login);
        if($result->getRowCount()<1)
            throw new Exception("Chyba mazání uživatele: $login",1);
        return $result;
    }

    public function remove_boch($id_boch){
       // var_dump($id_boch);
        $result = $this->database->query("DELETE FROM bochnik WHERE id_boch=?", $id_boch);
        if($result->getRowCount()<1)
            throw new Exception("Chyba mazání bochníku.",2);
        return $result;
    }


    public function delete_cheese_from_offer($cheese,$login){
        $cheese_id=$this->find_cheese($cheese,"id_syr")[0][0];
        $user_ico=$this->select("SELECT ico FROM dodavatel WHERE login=?",array($login["login"]))->fetchAll()[0]["ico"];
        $result=$this->database->query("DELETE FROM nabidka WHERE ico=? AND id_syr=?",$user_ico,$cheese_id);
        echo $result->getRowCount();
    }

    public function update_cheese_in_offer($cheese,$login,$new_price){
        $cheese_id=$this->find_cheese($cheese,"id_syr")[0][0];
        $user_ico=$this->select("SELECT ico FROM dodavatel WHERE login=?",array($login["login"]))->fetchAll()[0]["ico"];
        $result=$this->database->query("UPDATE nabidka SET ",array("cena" =>$new_price)," WHERE ico=? AND id_syr=?",$user_ico,$cheese_id);
        echo $result->getRowCount();
    }

    public function insert_cheese_in_offer($cheese,$login,$price){
        if (!is_numeric($price))
            throw new Exception("not a number");
        $cheese_id=$this->find_cheese($cheese,"id_syr")[0][0];
        $user_ico=$this->select("SELECT ico FROM dodavatel WHERE login=?",array($login["login"]))->fetchAll()[0]["ico"];
        $result=$this->database->query("INSERT INTO nabidka ?",array("ico" =>$user_ico,"id_syr"=>$cheese_id,"cena"=>$price));
        if($result->getRowCount()<=0)
            throw new Exception("some error during insert");
    }

    public function add_cheeses_to_shop($data, $id_obj){
        $order = $this->select("SELECT * FROM objednavka WHERE id_obj=?", array($id_obj));
        if (!is_null($order)) {
            $order =$order->fetchAll();
            $order["obsah"] = $this->select("SELECT syr.id_syr, syr.syr_trvanlivost, obsah.hmotnost FROM syr, obsah WHERE obsah.id_syr=syr.id_syr AND obsah.id_obj=?",array($id_obj));
            if (!is_null($order["obsah"])) {
                $order["obsah"] = $order["obsah"]->fetchAll();
            }
            foreach ($order["obsah"] as $bochnik) {
                if ($bochnik["syr_trvanlivost"] == 1) {
                    $text = "+ 1 day";
                }
                else {
                    $text = "+ " . $bochnik["syr_trvanlivost"] ." days";
                }
                $trv = new DateTime("now");
                $trv->modify($text);
                $this->database->query("INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_syr, id_obj) VALUES (?,?,?,?,?,?,?)",$bochnik["hmotnost"], $bochnik["hmotnost"], new DateTime("now"), $trv, $order[0]["id_pro"],$bochnik["id_syr"], $id_obj);
            }
        }
        //var_dump($data);die;
        $this->database->query("UPDATE objednavka SET dodano=1, zaplacena=1, druh_platby=?, datum_platby=? WHERE id_obj=?", $data["druh_platby"], new DateTime("now"), $id_obj);
    }

    public function get_shop_orders($id_pro) {
        $orders = $this->select("SELECT objednavka.*, zamestnanec.jmeno, zamestnanec.prijmeni, dodavatel.login,dodavatel.nazev FROM objednavka, zamestnanec,dodavatel WHERE objednavka.id_zam = zamestnanec.id_zam AND dodavatel.ico = objednavka.ico AND objednavka.id_pro = ?",array($id_pro));
        if (!is_null($orders)){
            $orders = $orders->fetchAll();
            foreach ($orders as $order){
                $order["obsah"] = $this->select("SELECT syr.nazev, obsah.hmotnost, nabidka.cena FROM syr, obsah, nabidka, objednavka WHERE syr.id_syr=obsah.id_syr AND obsah.id_obj = objednavka.id_obj AND nabidka.ico = objednavka.ico AND nabidka.id_syr = obsah.id_syr AND objednavka.id_obj=", array($order['id_obj']))->fetchAll();
                foreach ($order["obsah"] as $syr) {
                    $syr["syr_cena"] = $syr["cena"] * $syr["hmotnost"];
                }
                $order["jmeno_zam"] = $order["jmeno"] . " " . $order["prijmeni"];
                $order["dodavatel"] = $order["nazev"] . " " ."(". $order["login"] .")";
                $order["datum_vytvoreni"] = $order["datum_vytvoreni"]->__toString();
                if (!is_null($order["datum_platby"])) {
                    $order["datum_platby"] = $order["datum_platby"]->__toString();
                }
                else {
                    $order["datum_platby"] = "---";
                }
            }
        }
        return $orders;
    }

    public function get_supp_orders($login) {
        $ico=$this->select("SELECT ico FROM dodavatel WHERE login=?",array($login))->fetchAll()[0]["ico"];
        $orders = $this->select("SELECT objednavka.*, zamestnanec.jmeno, zamestnanec.prijmeni, zamestnanec.login, prodejna.nazev, prodejna.ulice, prodejna.mesto,prodejna.psc, prodejna.cislo_popisne FROM objednavka, zamestnanec, prodejna WHERE objednavka.id_pro=prodejna.id_pro AND objednavka.id_zam=zamestnanec.id_zam AND objednavka.ico = ?", array($ico));
        if (!is_null($orders)) {
            $orders = $orders->fetchAll();
            foreach ($orders as $order) {
                $order["obsah"] = $this->select("SELECT syr.nazev, obsah.hmotnost, nabidka.cena FROM syr, obsah, nabidka, objednavka WHERE syr.id_syr=obsah.id_syr AND obsah.id_obj = objednavka.id_obj AND nabidka.ico = objednavka.ico AND nabidka.id_syr = obsah.id_syr AND objednavka.id_obj=", array($order['id_obj']))->fetchAll();
                foreach ($order["obsah"] as $syr) {
                    $syr["syr_cena"] = $syr["cena"] * $syr["hmotnost"];
                }
                $order["jmeno_zam"] = $order["jmeno"] . " " . $order["prijmeni"] . " " . $order["login"];
                $order["prodejna"] = $order["nazev"] . ", " . $order["ulice"] . " " . $order["cislo_popisne"] . ", ". $order["psc"]. " " . $order["mesto"] ;
                $order["datum_vytvoreni"] = $order["datum_vytvoreni"]->__toString();
                if (!is_null($order["datum_platby"])) {
                    $order["datum_platby"] = $order["datum_platby"]->__toString();
                }
                else {
                    $order["datum_platby"] = "---";
                }
                if ($order["druh_platby"] == 1) {
                    if ($order["zaplacena"] == 0) {
                        $order["druh_platby"] = "FAKTURA - nezaplacena!";
                    }
                    else{
                        $order["druh_platby"] = "FAKTURA - zaplacena";
                    }
                }
                else {
                    $order["druh_platby"] = "HOTOVĚ";
                }
            }
        }
        return $orders;
    }


    public function insert_supplier($data){
        if(self::select("SELECT login FROM users WHERE login=?",array($data["login"])) !== null) {
            throw new Exception("Toto uživatelské jméno je obsazeno.",1);
        }
        if(self::select("SELECT ico FROM dodavatel WHERE ico=?",array($data["ico"])) !== null) {
            throw new Exception("Toto IČO je již registrováno.",2);
        }

        $result = $this->database->query("INSERT INTO users ?", array("heslo" => Passwords::hash($data["password"]), "login" => $data["login"], "type" => 3, "can_log" => 1)); //TODO HASH
        if ($result->getRowCount() < 1)
            throw new Exception("Problém s připojením k databázi.", 3);

        unset($data["password"]);
        unset($data["passwordCheck"]);
        if ($data["ulice"] == '') unset($data["ulice"]);
        if ($data["cislo_popisne"] == '') unset($data["cislo_popisne"]);
        if ($data["mesto"] == '') unset($data["mesto"]);
        if ($data["psc"] == '') unset($data["psc"]);
        if ($data["telefon"] == '') unset($data["telefon"]);

        $result = $this->database->query("INSERT INTO dodavatel ?", $data);
        if ($result->getRowCount() < 1)
            throw new Exception("Problém s připojením k databázi.", 3);

        return $result;

    }
    public function get_country(){
        $result=self::select("SELECT id_zem,nazev FROM zeme",[])->fetchAll();
        $data = array();
        foreach ($result as $res) {
            $data[$res->id_zem] = $res->nazev;
        }
        return $data;
    }


    public function get_shop(){
        $result=self::select("SELECT id_pro,nazev FROM prodejna",[])->fetchAll();
        $data = array();
        foreach ($result as $res) {
            $data[$res->id_pro] = $res->nazev;
        }
        return $data;
    }

    public function get_id_pro($id_zam){
        $result=self::select("SELECT id_pro FROM pracuje WHERE id_zam=?",array($id_zam));
        if($result->getRowCount() === null)
            throw new Exception("Chyba při čteni v databazi",2);
        else
            return $result->fetchAll();
    }

    public function get_staff($login){
        $result=self::select("SELECT * FROM zamestnanec WHERE login=?",array($login));
        if($result->getRowCount() === null)
            throw new Exception("Chyba při čteni v databazi",2);
        else
            return $result->fetchAll();
    }

    public function get_supp($login){
        $result=self::select("SELECT * FROM dodavatel WHERE login=?",array($login));
        if($result->getRowCount() === null)
            throw new Exception("Chyba při čteni v databazi",2);
        else
            return $result->fetchAll();
    }

    public function update_sup($data){
        $result = $this->database->query("UPDATE dodavatel SET nazev=?, cislo_uctu=?, id_zem=?, telefon=?, ulice=?, cislo_popisne=?, mesto=?, psc=? WHERE login=?",$data['nazev'] ,$data['cislo_uctu'],$data['id_zem'],$data['telefon'],$data['ulice'],$data['cislo_popisne'],$data['mesto'],$data['psc'],$data['login']);
        if ($result->getRowCount() < 0)
            throw new Exception("Nemuzu zmenit udaje zamestnance", 2);
    }

    public function update_staff($data){
        $id_zam = $this->database->query("SELECT id_zam FROM zamestnanec WHERE login=?",$data["login"]);
        if ($id_zam->getRowCount() < 1)
            throw new Exception("Uzivatel neexistuje", 2);
        $id_zam = $id_zam->fetchAll();
        $pole = array();
        $pole['id_zam'] = $id_zam[0]['id_zam'];
        $pole['id_pro'] =  $data["id_pro"];
        $result = $this->database->query("DELETE FROM pracuje  WHERE id_zam=?", $id_zam[0]['id_zam']);
        if ($result->getRowCount() < 0)
            throw new Exception("Nemůžu změnit prodejnu zaměstnance.", 2);
        foreach ($data['id_pro'] as $prod)
            $result=$this->database->query("INSERT INTO pracuje ?", array("id_zam" => $id_zam[0]->id_zam, "id_pro" => $prod));
        if($result->getRowCount() <1)
            throw new Exception("Problem with inserting into prodava",2);

        unset($data['id_pro']);
        unset($data['login']);
        $data['id_zam'] = $id_zam[0]["id_zam"];
        $result = $this->database->query("UPDATE zamestnanec SET jmeno=?, prijmeni=?, plat=?, telefon=?, ulice=?, cislo_popisne=?, mesto=?, psc=? WHERE id_zam=?",$data['jmeno'], $data['prijmeni'] ,$data['plat'],$data['telefon'],$data['ulice'],$data['cislo_popisne'],$data['mesto'],$data['psc'],$data['id_zam']);
        if ($result->getRowCount() < 0)
            throw new Exception("Nemuzu zmenit udaje zamestnance", 2);
    }

    public function insert_staff($data){
        if(self::select("SELECT login FROM users WHERE login=?",array($data["login"])) !== null) {
            throw new Exception("Toto uživatelské jméno je obsazeno.",1);
        }
        else {
            $result = $this->database->query("INSERT INTO users ?", array("heslo" => Passwords::hash($data["password"]), "login" => $data["login"], "type" => 2, "can_log" => 1)); //TODO HASH
            if ($result->getRowCount() < 1)
                throw new Exception("Problem with inserting into user", 2);
        }
        unset($data["password"]);
        unset($data["passwordCheck"]);
        if ($data["ulice"] == '') unset($data["ulice"]);
        if ($data["cislo_popisne"] == '') unset($data["cislo_popisne"]);
        if ($data["mesto"] == '') unset($data["mesto"]);
        if ($data["psc"] == '') unset($data["psc"]);
        if ($data["telefon"] == '') unset($data["telefon"]);
        $id_pro = array();
        $id_pro = $data["id_pro"];
        unset($data['id_pro']);
        $result=$this->database->query("INSERT INTO zamestnanec ?",$data);
        if($result->getRowCount() <1)
            throw new Exception("Problem with inserting into zamestnanec",2);
        $id_zam = $this->database->query("SELECT id_zam FROM zamestnanec WHERE login= ?",$data["login"]);
        if ($id_zam->getRowCount() < 1)
            throw new Exception("User does not exist.", 2);
        $id_zam = $id_zam->fetchAll();
        foreach ($id_pro as $prod)
            $result=$this->database->query("INSERT INTO pracuje ?", array("id_zam" => $id_zam[0]->id_zam, "id_pro" => $prod));
        if($result->getRowCount() <1)
            throw new Exception("Problem with inserting into prodava",2);

        return $result;
    }

    public function insert_admin($data){
        if(self::select("SELECT login FROM users WHERE login=?",array($data["login"])) !== null) {
            throw new Exception("Toto uživatelské jméno je obsazeno.",1);
        }
        $result = $this->database->query("INSERT INTO users ?", array("heslo" => Passwords::hash($data["password"]), "login" => $data["login"], "type" => 1, "can_log" => 1)); //TODO HASH
        if ($result->getRowCount() < 1)
            throw new Exception("Problém s připojením k databázi.", 2);
        return $result;
    }

    public function insert_shop($data){
        $result=$this->database->query("INSERT INTO prodejna ?",$data);
        if($result->getRowCount() <1)
            throw new Exception("Problem s přidáváním prodejny.",2);
    }

    public function delete_offered_cheese($id_syr,$login){;
        $ico = $this->database->query("SELECT ico FROM dodavatel WHERE login=?", array($login))->fetchAll();
        $result = $this->database->query("DELETE FROM nabidka WHERE ico=? AND id_syr=?",  $ico[0]['ico'], $id_syr );
        if ($result->getRowCount() < 0)
            throw new Exception("Problem s mazanim nabidky.",2);
    }

    public function update_cheese_price($values, $login, $id_syr){
        $ico = $this->database->query("SELECT ico FROM dodavatel WHERE login=?", array($login))->fetchAll();
        $result = $this->database->query("UPDATE nabidka SET cena=? WHERE ico=? AND id_syr=?", $values['cena'] ,$ico[0]['ico'], $id_syr );
        if ($result->getRowCount() < 0)
            throw new Exception("Problem se změnou ceny.",2);
    }

    public function update_shop($data){
        $result=$this->database->query("UPDATE prodejna SET nazev=?, ulice=?, cislo_popisne=?, mesto=?, psc=? WHERE id_pro=?",$data['nazev'],$data['ulice'],$data['cislo_popisne'],$data['mesto'],$data['psc'],$data['id_pro']);
        if($result->getRowCount() <1)
            throw new Exception("Problem s upravováním prodejny.",2);

    }

    public function add_cheese($data, $login) {
        $result = $this->database->query("SELECT id_syr FROM syr WHERE nazev=? AND zivocich=? AND tuk=? AND typ=? AND id_zem=? AND syr_trvanlivost=?", $data['nazev'], $data['zivocich'], $data['tuk'], $data['typ'], $data['id_zem'],$data['syr_trvanlivost']);
        $cena = $data['cena'];
        unset($data['cena']);
        if ($result->getRowCount() == 0) {
            $check = $this->database->query("INSERT INTO syr ?", $data);
            if ($check->getRowCount() < 1 )
                throw new Exception("Nelze zadat stejny syr s rozdílnou cenou.", 2);
            $result = $this->database->query("SELECT id_syr FROM syr WHERE nazev=? AND zivocich=? AND tuk=? AND typ=? AND id_zem=? AND syr_trvanlivost=?", $data['nazev'], $data['zivocich'], $data['tuk'], $data['typ'], $data['id_zem'],$data['syr_trvanlivost']);}
        $result = $result->fetchAll();
        $ico = $this->database->query("SELECT ico FROM dodavatel WHERE login=?", array($login))->fetchAll();
        try {
            $this->database->query("INSERT INTO nabidka", array("ico" => $ico[0]['ico'], "id_syr" => $result[0]["id_syr"], "cena" => $cena));
        } catch (\Exception $e) {
            throw new Exception("Nelze zadat stejny syr s rozdílnou cenou.", 2);
        }
    }


    public function cheese_in_shop($shop_id){

        $result=$this->database->query("SELECT syr.id_syr ,syr.nazev AS nazev_syr,syr.typ,prodejna.*,sum(bochnik.aktualni_hmotnost)AS celkem FROM bochnik NATURAL JOIN syr INNER JOIN prodejna ON bochnik.ID_PRO = prodejna.id_pro WHERE prodejna.id_pro =?  GROUP BY syr.nazev",$shop_id);
       //TODO check it (zmenil jsem z 1 na 0 aby mi nepadalo)
        if ($result->getRowCount()<0)
            throw new Exception("Žádné bochniky na prodejně /chyba připojeni k databazi");

        return $result->fetchAll();

    }
    public function get_shops(){
        $id_zam=0;
        if ($this->user->roles[0] === "staff") {
            $login= $this->user->identity->getId()[0][0];
            $id_zam = $this->database->query("SELECT id_zam FROM zamestnanec WHERE login= ?",$login);
            if ($id_zam->getRowCount() < 1)
                throw new Exception("User does not exist.", 2);
            $id_zam = $id_zam->fetchAll();

            $result = $this->select("SELECT id_pro FROM pracuje WHERE id_zam=?", array($id_zam[0]['id_zam']));
            if ($result->getRowCount() > 0) {
                $result = $result->fetchAll();
             //   $id_zam=$result[0][0];
            }else
                throw new Exception("neexistujici zaměstnanec",2);

      //  $result=self::select("SELECT * FROM prodejna WHERE id_pro=? OR id_pro=?",array($result[1]['id_pro'], $result[0]['id_pro']));
       // $result=$result->fetchAll();
        $data = array();
        foreach ($result as $res) {
            $data[] = $res['id_pro'];
        }
        $result=self::select_hack( $data,'prodejna');
        if($result->getRowCount()<1)
            throw new Exception("nepracujici zaměstnanec",3);
        else
            return $result->fetchAll();
        }
    }

    public function get_users(){
        $result=self::select("SELECT login, type, can_log FROM users",[])->fetchAll();;
        return $result;
    }

    public function get_prod(){
        $result=self::select("SELECT * FROM prodejna",[])->fetchAll();;
        return $result;
    }

    public function get_specific_prod($id_pro){
        $result=self::select("SELECT * FROM prodejna WHERE id_pro=?",array($id_pro))->fetchAll();;
        return $result;
    }

    public function get_zam_on_prod($id_pro){
        $result=self::select("SELECT * FROM zamestnanec NATURAL JOIN pracuje WHERE pracuje.id_pro =?",array($id_pro));
        if (!is_null($result))
            return $result;
        else
            return NULL;
    }

    public function get_offered_cheese($login){
        $result=self::select("SELECT syr.id_syr, nabidka.cena, syr.nazev, syr.zivocich, syr.tuk, syr.typ, syr.id_zem FROM syr, nabidka, dodavatel WHERE  nabidka.ico = dodavatel.ico AND dodavatel.login=? AND nabidka.id_syr = syr.id_syr",array($login));
        if (!is_null($result))
            return $result->fetchAll();
        else
            return NULL;
    }

    public function get_country_name($id_zem) {
        $result=self::select("SELECT nazev FROM zeme WHERE id_zem =?",array($id_zem));
        if ($result->getRowCount() < 1)
            throw new Exception("Země neexistuje",2);
        return $result->fetchAll();
    }


    public function suppliers(){
        $result=self::select("SELECT dodavatel.nazev,nabidka.ico FROM nabidka NATURAL JOIN dodavatel GROUP BY ico",[]);
        if ($result === null)
            throw new Exception("žadny dodavatel",2);
        else{
            $data = array();
            $result=$result->fetchAll();
            foreach ($result as $res) {
                $data[$res->ico] = $res->nazev;
            }
            return $data;
        }

    }
    public function cheese_form_supplier($id_supply){
        $result=self::select("SELECT * FROM nabidka NATURAL JOIN syr WHERE nabidka.ico=?",array($id_supply));
        if ($result === null)
            throw new Exception("žadní dodavatele sýru",2);
        else{
            $data = array();
            $result=$result->fetchAll();
            foreach ($result as $res) {
                $data[$res->id_syr] = $res->nazev;
            }
            return $data;
        }

    }
    public function cheese_graph($id_pro){
        $result=self::select("SELECT dodavatel.nazev AS dodavatel, bochnik .datum_dodani,bochnik.aktualni_hmotnost,bochnik.pocatecni_hmotnost,bochnik.trvanlivost , syr . *  FROM dodavatel, bochnik NATURAL JOIN syr NATURAL JOIN objednavka WHERE dodavatel.ico = objednavka.ico AND id_pro=?",array($id_pro));
        if ($result === null)
            throw new Exception("Žadní dodavatele sýru",2);
        else{
            $result=$result->fetchAll();
            return $result;
        }
    }

    public function get_prod_name($id_pro){
        $result = self::select("SELECT prodejna.nazev FROM prodejna WHERE id_pro=?", array($id_pro));
        if ($result === null)
            throw new Exception("Prodejna neexituje!",2);
        else{
            $result=$result->fetchAll();
            return $result[0]["nazev"];
        }
    }

    public function get_old_cheeses($id_pro){
        $result = self::select("SELECT bochnik.id_pro, bochnik.id_boch, bochnik.pocatecni_hmotnost, bochnik.aktualni_hmotnost, bochnik.trvanlivost, bochnik.datum_dodani, syr.nazev FROM bochnik, syr WHERE bochnik.id_syr = syr.id_syr AND id_pro=?", array($id_pro));
        if (!is_null($result)) {
            $result = $result->fetchAll();
            $bad_cheese = array();
            $today = new DateTime("now");
            foreach ($result as $boch) {
                if ($boch["trvanlivost"] < $today) {
                    $boch["datum_trv"] = $boch["trvanlivost"]->__toString();
                    $boch["datum_dod"] = $boch["datum_dodani"]->__toString();
                    $bad_cheese[] = $boch;
                }
            }
            return $bad_cheese;
        }
        else
            return NULL;
    }

    public function supplier_name($ico){
        $result=self::select("SELECT nazev FROM dodavatel WHERE ico=?",array($ico));
        if($result === null)
            throw new Exception("dodavatek s $ico neexistuje",2);
        else if ($result->getRowCount() === 1)
            return $result->fetch();
        else
            throw new Exception("vice dodavatelu s $ico",2);

    }
    public function cheese_name($id_syr){
        $result=self::select("SELECT nazev FROM syr WHERE id_syr=?",array($id_syr));
        if($result === null)
            throw new Exception("syr s  $id_syr neexistuje",2);
        else if ($result->getRowCount() === 1)
            return $result->fetch();
        else
            throw new Exception("vice syru  s id  $id_syr",2);

    }
    public function insert_order($data){
        $result=$this->database->query("INSERT INTO objednavka ?",$data);
        if ($result->getRowCount() !== 1)
            throw new Exception("nepodarilo se pridat objednavku",2);
        else
            return $this->database->getInsertId();
    }
    public function get_price($data){
        $result=self::select("SELECT cena FROM nabidka WHERE id_syr=? AND ico=?",$data);
        if ($result == null  )
            throw new Exception("neexistujici nabidka",2);
        else if($result->getRowCount() > 1 )
            throw new Exception("vice stejnych nabidek",2); //TODO ověřit v zadaní vice stejnych syru od stejneho dodavatele
        else
            return $result->fetch();
    }

    public function insert_order_detail($data){
        $result=$this->database->query("INSERT INTO obsah ?",$data);
        if ($result->getRowCount() < 1)
            throw new Exception("nepodarilo se pridat objednavku",2);
        else
            $result->getRowCount();
    }

}
